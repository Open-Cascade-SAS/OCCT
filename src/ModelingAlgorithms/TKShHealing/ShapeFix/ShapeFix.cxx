// Created on: 2000-01-21
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <ShapeFix.hxx>
//: k2 abv 16.12.98: eliminating code duplication
// pdn     18.12.98: checking deviation for SP edges
//:   abv 22.02.99: method FillFace() removed since PRO13123 is fixed
// szv#4 S4163
// szv#9:S4244:19Aug99: Added method FixWireGaps
// szv#10:S4244:23Aug99: Added method FixFaceGaps

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>

#include <Precision.hxx>

#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Surface.hxx>

//: i2
#include <gp_Pnt.hxx>
#include <Geom_Plane.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <BRepLib.hxx>

#include <ShapeBuild_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopExp.hxx>

#include <Message_ProgressScope.hxx>
#include <Message_Msg.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>

//=================================================================================================

bool ShapeFix::SameParameter(const TopoDS_Shape&                                 shape,
                             const bool                                          enforce,
                             const double                                        preci,
                             const Message_ProgressRange&                        theProgress,
                             const occ::handle<ShapeExtend_BasicMsgRegistrator>& theMsgReg)
{
  // Calculate number of edges
  int aNbEdges = 0;
  for (TopExp_Explorer anEdgeExp(shape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    ++aNbEdges;

  // Calculate number of faces
  int aNbFaces = 0;
  for (TopExp_Explorer anEdgeExp(shape, TopAbs_FACE); anEdgeExp.More(); anEdgeExp.Next())
    ++aNbFaces;

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMapEF;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, aMapEF);

  BRep_Builder B;
#ifdef OCCT_DEBUG
  int nbfail = 0, numedge = 0;
#endif
  bool                       status = true;
  double                     tol    = preci;
  bool                       iatol  = (tol > 0);
  occ::handle<ShapeFix_Edge> sfe    = new ShapeFix_Edge;
  TopExp_Explorer            ex(shape, TopAbs_EDGE);
  Message_Msg                doneMsg("FixEdge.SameParameter.MSG0");

  // Start progress scope (no need to check if progress exists -- it is safe)
  Message_ProgressScope aPSForSameParam(theProgress, "Fixing same parameter problem", 2);

  {
    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressScope aPS(aPSForSameParam.Next(), "Fixing edge", aNbEdges);

    while (ex.More())
    {
      TopoDS_Edge E;
      while (ex.More() && aPS.More())
      {
#ifdef OCCT_DEBUG
        numedge++;
#endif
        int             ierr = 0;
        TopLoc_Location loc;
        E = TopoDS::Edge(ex.Current());
        ex.Next();

        if (!iatol)
          tol = BRep_Tool::Tolerance(E);
        if (enforce)
        {
          B.SameRange(E, false);
          B.SameParameter(E, false);
        }

        NCollection_List<TopoDS_Shape> aListOfFaces;
        aMapEF.FindFromKey(E, aListOfFaces);
        if (aListOfFaces.Extent() != 0)
        {
          NCollection_List<TopoDS_Shape>::Iterator aListOfFacesIt(aListOfFaces);
          for (; aListOfFacesIt.More(); aListOfFacesIt.Next())
          {
            TopoDS_Face aF = TopoDS::Face(aListOfFacesIt.Value());
            sfe->FixSameParameter(E, aF);
          }
        }
        else
        {
          sfe->FixSameParameter(E); // K2-SEP97
        }

        if (!BRep_Tool::SameParameter(E))
        {
          ierr = 1;
#ifdef OCCT_DEBUG
          nbfail++;
#endif
        }

        if (ierr)
        {
          status = false;
          B.SameRange(E, false);
          B.SameParameter(E, false);
        }
        else if (!theMsgReg.IsNull() && !sfe->Status(ShapeExtend_OK))
        {
          theMsgReg->Send(E, doneMsg, Message_Warning);
        }

        // Complete step in current progress scope
        aPS.Next();
      } // -- end while

      // Halt algorithm in case of user's abort
      if (!aPS.More())
        return false;
    }
  }

  {
    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressScope aPS(aPSForSameParam.Next(), "Update tolerances", aNbFaces);

    //: i2 abv 21 Aug 98: ProSTEP TR8 Motor.rle face 710:
    // Update tolerance of edges on planes (no pcurves are stored)
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More() && aPS.More(); exp.Next(), aPS.Next())
    {
      TopoDS_Face               face = TopoDS::Face(exp.Current());
      occ::handle<Geom_Surface> Surf = BRep_Tool::Surface(face);

      occ::handle<Geom_Plane> plane = occ::down_cast<Geom_Plane>(Surf);
      if (plane.IsNull())
      {
        occ::handle<Geom_RectangularTrimmedSurface> GRTS =
          occ::down_cast<Geom_RectangularTrimmedSurface>(Surf);
        if (!GRTS.IsNull())
          plane = occ::down_cast<Geom_Plane>(GRTS->BasisSurface());
        if (plane.IsNull())
          continue;
      }

      occ::handle<GeomAdaptor_Surface> AS = new GeomAdaptor_Surface(plane);
      for (TopExp_Explorer ed(face, TopAbs_EDGE); ed.More(); ed.Next())
      {
        TopoDS_Edge             edge = TopoDS::Edge(ed.Current());
        double                  f, l;
        occ::handle<Geom_Curve> crv = BRep_Tool::Curve(edge, f, l);
        if (crv.IsNull())
          continue;

        occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(edge, face, f, l);
        if (c2d.IsNull())
          continue;
        occ::handle<Geom2dAdaptor_Curve> GHPC = new Geom2dAdaptor_Curve(c2d, f, l);
        Adaptor3d_CurveOnSurface         ACS(GHPC, AS);

        double tol0         = BRep_Tool::Tolerance(edge);
        tol                 = tol0;
        double    tol2      = tol * tol;
        bool      isChanged = false;
        const int NCONTROL  = 23;
        for (int i = 0; i < NCONTROL; i++)
        {
          double par  = (f * (NCONTROL - 1 - i) + l * i) / (NCONTROL - 1);
          gp_Pnt pnt  = crv->Value(par);
          gp_Pnt prj  = ACS.Value(par);
          double dist = pnt.SquareDistance(prj);
          if (tol2 < dist)
          {
            tol2      = dist;
            isChanged = true;
          }
        }
        if (isChanged)
        {
          tol = 1.00005 * sqrt(tol2); // coeff: see trj3_pm1-ct-203.stp #19681, edge 10
          if (tol >= tol0)
          {
            B.UpdateEdge(edge, tol);
            for (TopoDS_Iterator itV(edge); itV.More(); itV.Next())
            {
              TopoDS_Shape S = itV.Value();
              B.UpdateVertex(TopoDS::Vertex(S), tol);
            }
          }
        }
      }
    }
    // Halt algorithm in case of user's abort
    if (!aPS.More())
      return false;
  }

  if (!status)
  {
#ifdef OCCT_DEBUG
    std::cout << "** SameParameter not complete. On " << numedge << " Edges:";
    if (nbfail > 0)
      std::cout << "  " << nbfail << " Failed";
    std::cout << std::endl;
#endif
  }
  return status;
}

//=================================================================================================

void ShapeFix::EncodeRegularity(const TopoDS_Shape& shape, const double tolang)
{
  BRepLib::EncodeRegularity(shape, tolang);
}

//=================================================================================================

TopoDS_Shape ShapeFix::RemoveSmallEdges(TopoDS_Shape&                    Shape,
                                        const double                     Tolerance,
                                        occ::handle<ShapeBuild_ReShape>& context)
{
  occ::handle<ShapeFix_Shape> sfs = new ShapeFix_Shape;
  sfs->Init(Shape);
  sfs->SetPrecision(Tolerance);
  sfs->FixFaceTool()->FixMissingSeamMode()   = false;
  sfs->FixFaceTool()->FixOrientationMode()   = false;
  sfs->FixFaceTool()->FixSmallAreaWireMode() = false;
  sfs->FixWireTool()->ModifyTopologyMode()   = true;
  // sfs.FixWireTool().FixReorderMode() = false;
  sfs->FixWireTool()->FixConnectedMode()        = false;
  sfs->FixWireTool()->FixEdgeCurvesMode()       = false;
  sfs->FixWireTool()->FixDegeneratedMode()      = false;
  sfs->FixWireTool()->FixSelfIntersectionMode() = false;
  sfs->FixWireTool()->FixLackingMode()          = false;
  sfs->FixWireTool()->FixSmallMode()            = true;
  sfs->Perform();
  TopoDS_Shape result = sfs->Shape();
  context             = sfs->Context();
  return result;
}

//=======================================================================
// function : ReplaceVertex
// purpose  : auxiliary for FixVertexPosition
//=======================================================================
static TopoDS_Edge ReplaceVertex(const TopoDS_Edge& theEdge, const gp_Pnt& theP, const bool theFwd)
{
  TopoDS_Vertex aNewVertex;
  BRep_Builder  aB;
  aB.MakeVertex(aNewVertex, theP, Precision::Confusion());
  TopoDS_Vertex aV1, aV2;
  if (theFwd)
  {
    aV1 = aNewVertex;
    aV1.Orientation(TopAbs_FORWARD);
  }
  else
  {
    aV2 = aNewVertex;
    aV2.Orientation(TopAbs_REVERSED);
  }
  ShapeBuild_Edge    aSbe;
  TopoDS_Edge        e1  = theEdge;
  TopAbs_Orientation Ori = e1.Orientation();
  e1.Orientation(TopAbs_FORWARD);
  TopoDS_Edge aNewEdge = aSbe.CopyReplaceVertices(e1, aV1, aV2);
  aNewEdge.Orientation(Ori);
  return aNewEdge;
}

//=======================================================================
// function : getNearPoint
// purpose  : auxiliary for FixVertexPosition
//=======================================================================
static double getNearPoint(const NCollection_Sequence<gp_Pnt>& aSeq1,
                           const NCollection_Sequence<gp_Pnt>& aSeq2,
                           gp_XYZ&                             acent)
{
  int    i    = 1;
  int    ind1 = 0, ind2 = 0;
  double mindist = RealLast();
  for (; i <= aSeq1.Length(); i++)
  {
    gp_Pnt p1 = aSeq1.Value(i);
    int    j  = 1;
    for (; j <= aSeq2.Length(); j++)
    {
      gp_Pnt p2 = aSeq2.Value(j);
      double d  = p1.Distance(p2);
      if (fabs(d - mindist) <= Precision::Confusion())
        continue;
      if (d < mindist)
      {
        mindist = d;
        ind1    = i;
        ind2    = j;
      }
    }
  }
  if (ind1 && ind2)
    acent = (aSeq1.Value(ind1).XYZ() + aSeq2.Value(ind2).XYZ()) / 2.0;
  return mindist;
}

//=======================================================================
// function : getNearestEdges
// purpose  : auxiliary for FixVertexPosition
//=======================================================================
static bool getNearestEdges(NCollection_List<TopoDS_Shape>&     theLEdges,
                            const TopoDS_Vertex&                theVert,
                            NCollection_Sequence<TopoDS_Shape>& theSuitEdges,
                            NCollection_Sequence<TopoDS_Shape>& theRejectEdges,
                            const double                        theTolerance,
                            gp_XYZ&                             thecentersuit,
                            gp_XYZ&                             thecenterreject)
{
  if (theLEdges.IsEmpty())
    return false;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapEdges;

  NCollection_List<TopoDS_Shape> atempList;
  atempList = theLEdges;
  NCollection_List<TopoDS_Shape>::Iterator alIter(atempList);

  TopoDS_Edge   aEdge1 = TopoDS::Edge(alIter.Value());
  TopoDS_Vertex aVert11, aVert12;
  TopExp::Vertices(aEdge1, aVert11, aVert12);
  aMapEdges.Add(aEdge1);
  double                  aFirst1, aLast1;
  occ::handle<Geom_Curve> aCurve1 = BRep_Tool::Curve(aEdge1, aFirst1, aLast1);
  gp_Pnt                  p11;
  gp_Pnt                  p12;
  bool                    isFirst1 = theVert.IsSame(aVert11);
  bool                    isSame1  = aVert11.IsSame(aVert12);
  if (!aCurve1.IsNull())
  {
    if (isFirst1)
      p11 = aCurve1->Value(aFirst1);
    else if (!isSame1)
      p11 = aCurve1->Value(aLast1);
    if (isSame1)
      p12 = aCurve1->Value(aLast1);
  }
  else
    return false;
  alIter.Next();
  NCollection_Sequence<TopoDS_Shape> aseqreject;
  NCollection_Sequence<TopoDS_Shape> aseqsuit;

  int anumLoop = 0;
  for (; alIter.More();)
  {
    TopoDS_Edge aEdge = TopoDS::Edge(alIter.Value());
    if (aMapEdges.Contains(aEdge))
    {
      atempList.Remove(alIter);
      continue;
    }

    TopoDS_Vertex aVert1, aVert2;
    TopExp::Vertices(aEdge, aVert1, aVert2);
    double isFirst = theVert.IsSame(aVert1);
    bool   isSame  = aVert1.IsSame(aVert2);

    bool isLoop = ((aVert1.IsSame(aVert11) && aVert2.IsSame(aVert12))
                   || (aVert1.IsSame(aVert12) && aVert2.IsSame(aVert11)));
    if (isLoop /*&& !aseqsuit.Length()*/ && (atempList.Extent() > anumLoop))
    {
      atempList.Append(aEdge);
      atempList.Remove(alIter);
      anumLoop++;
      continue;
    }
    aMapEdges.Add(aEdge);
    double                  aFirst, aLast;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(aEdge, aFirst, aLast);
    if (!aCurve.IsNull())
    {
      gp_Pnt p1;
      gp_Pnt p2;
      if (isFirst)
        p1 = aCurve->Value(aFirst);
      else
        p1 = aCurve->Value(aLast);
      if (isSame)
        p2 = aCurve->Value(aLast);
      double aMinDist = RealLast();
      gp_XYZ acent;
      if (!isSame && !isSame1)
      {
        aMinDist = p1.Distance(p11);
        acent    = (p1.XYZ() + p11.XYZ()) / 2.0;
      }
      else
      {
        NCollection_Sequence<gp_Pnt> aSeq1;
        NCollection_Sequence<gp_Pnt> aSeq2;
        aSeq1.Append(p11);
        if (isSame1)
          aSeq1.Append(p12);
        aSeq2.Append(p1);
        if (isSame)
          aSeq2.Append(p2);
        aMinDist = getNearPoint(aSeq1, aSeq2, acent);
      }

      if (aMinDist > theTolerance)
      {
        if (!aseqreject.Length())
          thecenterreject = acent;
        aseqreject.Append(aEdge);
      }
      else
      {
        if (!aseqsuit.Length())
        {
          thecentersuit = acent;
          aseqsuit.Append(aEdge);
        }
        else if (!isSame1)
          aseqsuit.Append(aEdge);
        else if ((thecentersuit - acent).Modulus() < theTolerance)
          aseqsuit.Append(aEdge);
        else
          aseqreject.Append(aEdge);
      }
    }
    atempList.Remove(alIter);
  }

  bool isDone = (!aseqsuit.IsEmpty() || !aseqreject.IsEmpty());
  if (isDone)
  {
    if (aseqsuit.IsEmpty())
    {
      theRejectEdges.Append(aEdge1);
      theLEdges.RemoveFirst();

      getNearestEdges(theLEdges,
                      theVert,
                      theSuitEdges,
                      theRejectEdges,
                      theTolerance,
                      thecentersuit,
                      thecenterreject);
    }
    else
    {
      theSuitEdges.Append(aEdge1);
      theSuitEdges.Append(aseqsuit);
      theRejectEdges.Append(aseqreject);
    }
  }
  else
    theRejectEdges.Append(aEdge1);

  return isDone;
}

//=================================================================================================

bool ShapeFix::FixVertexPosition(TopoDS_Shape&                          theshape,
                                 const double                           theTolerance,
                                 const occ::handle<ShapeBuild_ReShape>& thecontext)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                  aMapVertEdges;
  TopExp_Explorer aExp1(theshape, TopAbs_EDGE);
  for (; aExp1.More(); aExp1.Next())
  {
    TopoDS_Shape    aVert1;
    int             nV = 1;
    TopoDS_Iterator aExp3(aExp1.Current());
    for (; aExp3.More(); aExp3.Next(), nV++)
    {
      const TopoDS_Shape& aVert = aExp3.Value();
      if (nV == 1)
        aVert1 = aVert;
      else if (aVert1.IsSame(aVert))
        continue;
      aMapVertEdges.Bound(aVert, NCollection_List<TopoDS_Shape>())->Append(aExp1.Current());
    }
  }
  bool isDone = false;
  int  i      = 1;
  for (; i <= aMapVertEdges.Extent(); i++)
  {
    TopoDS_Vertex aVert    = TopoDS::Vertex(aMapVertEdges.FindKey(i));
    double        aTolVert = BRep_Tool::Tolerance(aVert);
    if (aTolVert <= theTolerance)
      continue;

    BRep_Builder aB1;
    aB1.UpdateVertex(aVert, theTolerance);
    gp_Pnt aPvert = BRep_Tool::Pnt(aVert);
    gp_XYZ acenter(aPvert.XYZ()), acenterreject(aPvert.XYZ());

    NCollection_Sequence<TopoDS_Shape> aSuitEdges;
    NCollection_Sequence<TopoDS_Shape> aRejectEdges;
    NCollection_List<TopoDS_Shape>     aledges;
    aledges = aMapVertEdges.FindFromIndex(i);
    if (aledges.Extent() == 1)
      continue;
    // if tolerance of vertex is more than specified tolerance
    //  check distance between curves and vertex

    if (!getNearestEdges(aledges,
                         aVert,
                         aSuitEdges,
                         aRejectEdges,
                         theTolerance,
                         acenter,
                         acenterreject))
      continue;

    // update vertex by nearest point
    bool isAdd = false;
    int  k     = 1;
    for (; k <= aSuitEdges.Length(); k++)
    {

      TopoDS_Edge   aEdgeOld = TopoDS::Edge(aSuitEdges.Value(k));
      TopoDS_Vertex aVert1, aVert2;
      TopExp::Vertices(aEdgeOld, aVert1, aVert2);

      bool isFirst = (aVert1.IsSame(aVert));
      bool isLast  = (aVert2.IsSame(aVert));
      if (!isFirst && !isLast)
        continue;
      double                  aFirst, aLast;
      occ::handle<Geom_Curve> aCurve;
      TopoDS_Edge             aEdge = TopoDS::Edge(thecontext->Apply(aEdgeOld));

      TopoDS_Vertex aVert1n, aVert2n;
      TopExp::Vertices(aEdge, aVert1n, aVert2n);
      aCurve = BRep_Tool::Curve(aEdge, aFirst, aLast);
      if (!aCurve.IsNull())
      {
        gp_Pnt p1 = aCurve->Value(aFirst);
        gp_Pnt p2 = aCurve->Value(aLast);

        // if distance between ends of curve more than specified tolerance
        // but vertices are the same that one of the vertex will be replaced.

        bool isReplace = (aVert1n.IsSame(aVert2n) && p1.Distance(p2) > theTolerance);

        // double dd1 = (acenter - p1.XYZ()).Modulus();
        // double dd2 = (acenter - p2.XYZ()).Modulus();
        if (isFirst)
        {
          if (k > 2)
          {
            acenter += p1.XYZ();
            acenter /= 2.0;
          }
          if (isReplace)
          {
            TopoDS_Edge enew;
            if (p1.Distance(acenter) < p2.Distance(acenter))
              enew = ReplaceVertex(aEdge, p2, false);
            else
              enew = ReplaceVertex(aEdge, p1, true);
            thecontext->Replace(aEdge, enew);
            isDone = true;
          }
        }
        else
        {
          if (k > 2)
          {
            acenter += p2.XYZ();
            acenter /= 2.0;
          }
          if (isReplace)
          {
            TopoDS_Edge enew;
            if (p1.Distance(acenter) < p2.Distance(acenter))
              enew = ReplaceVertex(aEdge, p2, false);
            else
              enew = ReplaceVertex(aEdge, p1, true);
            thecontext->Replace(aEdge, enew);
            isDone = true;
          }
        }

        isAdd = true;
      }
    }

    if (isAdd && aPvert.Distance(acenter) > theTolerance)
    {

      BRep_Builder aB;

      //  aB.UpdateVertex(aVert,Precision::Confusion());
      // else {
      isDone = true;
      TopoDS_Vertex aNewVertex;
      aB.MakeVertex(aNewVertex, acenter, Precision::Confusion());
      aNewVertex.Orientation(aVert.Orientation());
      thecontext->Replace(aVert, aNewVertex);
    }

    for (k = 1; k <= aRejectEdges.Length(); k++)
    {
      TopoDS_Edge   aEdgeOld = TopoDS::Edge(aRejectEdges.Value(k));
      TopoDS_Vertex aVert1, aVert2;
      TopExp::Vertices(aEdgeOld, aVert1, aVert2);

      bool isFirst = (aVert1.IsSame(aVert));
      bool isLast  = (aVert2.IsSame(aVert));
      if (!isFirst && !isLast)
        continue;
      bool                    isSame = aVert1.IsSame(aVert2);
      occ::handle<Geom_Curve> aCurve;
      TopoDS_Edge             aEdge = TopoDS::Edge(thecontext->Apply(aEdgeOld));

      TopoDS_Vertex aVert1n, aVert2n;
      TopExp::Vertices(aEdge, aVert1n, aVert2n);

      double aFirst, aLast;
      aCurve = BRep_Tool::Curve(aEdge, aFirst, aLast);
      if (!aCurve.IsNull())
      {
        gp_Pnt      p1 = aCurve->Value(aFirst);
        gp_Pnt      p2 = aCurve->Value(aLast);
        TopoDS_Edge enew;
        if (isFirst)
        {
          enew = ReplaceVertex(aEdge, p1, true);
          if (isSame)
            enew = ReplaceVertex(enew, p2, false);
        }
        else
        {
          enew = ReplaceVertex(aEdge, p2, false);
          if (isSame)
            enew = ReplaceVertex(enew, p1, true);
        }

        thecontext->Replace(aEdge, enew);
        isDone = true;
      }
    }
  }
  if (isDone)
    theshape = thecontext->Apply(theshape);
  return isDone;
}

//=================================================================================================

double ShapeFix::LeastEdgeSize(TopoDS_Shape& theShape)
{
  double aRes = RealLast();
  for (TopExp_Explorer exp(theShape, TopAbs_EDGE); exp.More(); exp.Next())
  {
    TopoDS_Edge             edge = TopoDS::Edge(exp.Current());
    double                  first, last;
    occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(edge, first, last);
    if (!c3d.IsNull())
    {
      Bnd_Box bb;
      bb.Add(c3d->Value(first));
      bb.Add(c3d->Value(last));
      bb.Add(c3d->Value((last + first) / 2.));
      double x1, x2, y1, y2, z1, z2, size;
      bb.Get(x1, y1, z1, x2, y2, z2);
      size = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
      if (size < aRes)
        aRes = size;
    }
  }
  aRes = sqrt(aRes);
  return aRes;
}
