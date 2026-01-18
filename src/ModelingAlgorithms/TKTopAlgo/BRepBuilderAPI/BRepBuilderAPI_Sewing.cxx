// Created on: 1995-03-24
// Created by: Jing Cheng MEI
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// dcl          CCI60011 : Correction of degeneratedSection
//              Improvement of SameParameter Edge to treat case of failure in BRepLib::SameParameter
// dcl          Thu Aug 20 09:24:49 1998
//              Suppression of little faces.
// dcl          Fri Aug  7 15:27:46 1998
//                Refection of function SameParameter Edge.
//              Merge on the edge which has the less of poles.
//              Suppression of the Connected Edge function.
// dcl          Tue Jun  9 14:21:53 1998
//              Do not merge edge if they belong the same face
//              Tolerance management in VerticesAssembling
//              Tolerance management in Cutting
// dcl          Thu May 14 15:51:46 1998
//              optimization of cutting
// dcl          Thu May 7  15:51:46 1998
//              Add of cutting option
//              Add of SameParameter call

//-- lbr April 1 97
//-- dpf December 10 1997 Processing of pcurve collections

// rln 02.02.99 BUC60449 Making compilable on NT in DEB mode
// rln 02.02.99 BUC60449 Protection against exception on NT

#define TEST 1

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TVertex.hxx>
#include <BRepBuilderAPI_BndBoxTreeSelector.hxx>
#include <BRepBuilderAPI_VertexInspector.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepTools_ReShape.hxx>
#include <Extrema_ExtPC.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(BRepBuilderAPI_Sewing, Standard_Transient)

// #include <LocalAnalysis_SurfaceContinuity.hxx>
//=================================================================================================

occ::handle<Geom2d_Curve> BRepBuilderAPI_Sewing::SameRange(
  const occ::handle<Geom2d_Curve>& CurvePtr,
  const double                     FirstOnCurve,
  const double                     LastOnCurve,
  const double                     RequestedFirst,
  const double                     RequestedLast) const
{
  occ::handle<Geom2d_Curve> NewCurvePtr;
  try
  {

    GeomLib::SameRange(Precision::PConfusion(),
                       CurvePtr,
                       FirstOnCurve,
                       LastOnCurve,
                       RequestedFirst,
                       RequestedLast,
                       NewCurvePtr);
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Exception in BRepBuilderAPI_Sewing::SameRange: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return NewCurvePtr;
}

//=======================================================================
// function : WhichFace
// purpose  : Give the face whose edge is the border
//=======================================================================

TopoDS_Face BRepBuilderAPI_Sewing::WhichFace(const TopoDS_Edge& theEdg, const int index) const
{
  TopoDS_Shape bound = theEdg;
  if (mySectionBound.IsBound(bound))
    bound = mySectionBound(bound);
  if (myBoundFaces.Contains(bound))
  {
    int                                      i = 1;
    NCollection_List<TopoDS_Shape>::Iterator itf(myBoundFaces.FindFromKey(bound));
    for (; itf.More(); itf.Next(), i++)
      if (i == index)
        return TopoDS::Face(itf.Value());
  }
  return TopoDS_Face();
}

//=================================================================================================

static bool IsClosedShape(const TopoDS_Shape& theshape,
                          const TopoDS_Shape& v1,
                          const TopoDS_Shape& v2)
{
  double          TotLength = 0.0;
  TopExp_Explorer aexp;
  for (aexp.Init(theshape, TopAbs_EDGE); aexp.More(); aexp.Next())
  {
    TopoDS_Edge aedge = TopoDS::Edge(aexp.Current());
    if (aedge.IsNull())
      continue;
    TopoDS_Vertex ve1, ve2;
    TopExp::Vertices(aedge, ve1, ve2);
    if (!ve1.IsSame(v1) && !ve1.IsSame(v2))
      continue;
    if (BRep_Tool::Degenerated(aedge))
      continue;
    double                  first, last;
    occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(TopoDS::Edge(aedge), first, last);
    if (!c3d.IsNull())
    {
      GeomAdaptor_Curve cAdapt(c3d);
      double            length = GCPnts_AbscissaPoint::Length(cAdapt, first, last);
      TotLength += length;
      if (ve2.IsSame(v1) || ve2.IsSame(v2))
        break;
    }
  }
  if (TotLength > 0.0)
  {
    gp_Pnt p1 = BRep_Tool::Pnt(TopoDS::Vertex(v1));
    gp_Pnt p2 = BRep_Tool::Pnt(TopoDS::Vertex(v2));
    return (p1.Distance(p2) < TotLength / (1.2 * M_PI));
  }
  return false;
}

//=================================================================================================

static bool IsClosedByIsos(const occ::handle<Geom_Surface>& thesurf,
                           const occ::handle<Geom2d_Curve>& acrv2d,
                           const double                     f2d,
                           const double                     l2d,
                           const bool                       isUIsos)
{
  bool isClosed = false;

  gp_Pnt2d                psurf1 = (acrv2d->IsPeriodic() ? acrv2d->Value(f2d)
                                                         : acrv2d->Value(std::max(f2d, acrv2d->FirstParameter())));
  gp_Pnt2d                psurf2 = (acrv2d->IsPeriodic() ? acrv2d->Value(l2d)
                                                         : acrv2d->Value(std::min(l2d, acrv2d->LastParameter())));
  occ::handle<Geom_Curve> aCrv1;
  occ::handle<Geom_Curve> aCrv2;
  if (isUIsos)
  {
    aCrv1 = thesurf->UIso(psurf1.X());
    aCrv2 = thesurf->UIso(psurf2.X());
  }
  else
  {
    aCrv1 = thesurf->VIso(psurf1.Y());
    aCrv2 = thesurf->VIso(psurf2.Y());
  }
  gp_Pnt p11, p1m, p12, p21, p2m, p22;
  double af1 = aCrv1->FirstParameter();
  double al1 = aCrv1->LastParameter();
  double af2 = aCrv2->FirstParameter();
  double al2 = aCrv2->LastParameter();
  aCrv1->D0(af1, p11);
  aCrv1->D0((af1 + al1) * 0.5, p1m);
  aCrv1->D0(al1, p12);
  aCrv2->D0(af2, p21);
  aCrv2->D0((af2 + al2) * 0.5, p2m);
  aCrv2->D0(al2, p22);
  isClosed = (((p11.XYZ() - p12.XYZ()).Modulus()
               < (p11.XYZ() - p1m.XYZ()).Modulus() - Precision::Confusion())
              && ((p21.XYZ() - p22.XYZ()).Modulus()
                  < (p21.XYZ() - p2m.XYZ()).Modulus() - Precision::Confusion()));
  return isClosed;
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsUClosedSurface(const occ::handle<Geom_Surface>& surf,
                                             const TopoDS_Shape&              theEdge,
                                             const TopLoc_Location&           theloc) const
{
  occ::handle<Geom_Surface> tmpsurf = surf;
  if (tmpsurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    tmpsurf = occ::down_cast<Geom_RectangularTrimmedSurface>(surf)->BasisSurface();
  else if (tmpsurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    tmpsurf = occ::down_cast<Geom_OffsetSurface>(surf)->BasisSurface();
  else
  {
    bool isClosed = tmpsurf->IsUClosed();
    if (!isClosed)
    {
      double                    f2d, l2d;
      occ::handle<Geom2d_Curve> acrv2d =
        BRep_Tool::CurveOnSurface(TopoDS::Edge(theEdge), surf, theloc, f2d, l2d);
      if (!acrv2d.IsNull())
        isClosed = IsClosedByIsos(tmpsurf, acrv2d, f2d, l2d, false);
    }
    return isClosed;
  }
  return IsUClosedSurface(tmpsurf, theEdge, theloc);
  // return surf->IsUClosed();
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsVClosedSurface(const occ::handle<Geom_Surface>& surf,
                                             const TopoDS_Shape&              theEdge,
                                             const TopLoc_Location&           theloc) const
{
  occ::handle<Geom_Surface> tmpsurf = surf;
  if (tmpsurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    tmpsurf = occ::down_cast<Geom_RectangularTrimmedSurface>(surf)->BasisSurface();
  else if (tmpsurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    tmpsurf = occ::down_cast<Geom_OffsetSurface>(surf)->BasisSurface();
  else
  {
    bool isClosed = tmpsurf->IsVClosed();
    if (!isClosed)
    {
      double                    f2d, l2d;
      occ::handle<Geom2d_Curve> acrv2d =
        BRep_Tool::CurveOnSurface(TopoDS::Edge(theEdge), surf, theloc, f2d, l2d);
      if (!acrv2d.IsNull())
        isClosed = IsClosedByIsos(tmpsurf, acrv2d, f2d, l2d, true);
    }
    return isClosed;
  }
  return IsVClosedSurface(tmpsurf, theEdge, theloc);
  // return surf->IsVClosed();
}

//=================================================================================================

void BRepBuilderAPI_Sewing::SameParameter(const TopoDS_Edge& edge) const
{
  try
  {

    BRepLib::SameParameter(edge);
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Exception in BRepBuilderAPI_Sewing::SameParameter: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
}

//=======================================================================
// function : SameParameterEdge
// purpose  : internal use
//           Merge the Sequence Of Section on one edge.
//           This function keep the curve3d,curve2d,range and parametrization
//           from the first section, and report and made sameparameter the
//           pcurves of the other function.
//           This function works when the are not more than two Pcurves
//           on a same face.
//=======================================================================

TopoDS_Edge BRepBuilderAPI_Sewing::SameParameterEdge(
  const TopoDS_Shape&                                     edge,
  const NCollection_Sequence<TopoDS_Shape>&               seqEdges,
  const NCollection_Sequence<bool>&                       seqForward,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& mapMerged,
  const occ::handle<BRepTools_ReShape>&                   locReShape)
{
  // Retrieve reference section
  TopoDS_Shape aTmpShape = myReShape->Apply(edge); // for porting
  TopoDS_Edge  Edge1     = TopoDS::Edge(aTmpShape);
  aTmpShape              = locReShape->Apply(Edge1);
  if (locReShape != myReShape)
    Edge1 = TopoDS::Edge(aTmpShape);
  bool isDone = false;

  // Create data structures for temporary merged edges
  NCollection_List<TopoDS_Shape>                         listFaces1;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MergedFaces;

  if (mySewing)
  {

    // Fill MergedFaces with faces of Edge1
    TopoDS_Shape bnd1 = edge;
    if (mySectionBound.IsBound(bnd1))
      bnd1 = mySectionBound(bnd1);
    if (myBoundFaces.Contains(bnd1))
    {
      NCollection_List<TopoDS_Shape>::Iterator itf(myBoundFaces.FindFromKey(bnd1));
      for (; itf.More(); itf.Next())
        if (MergedFaces.Add(itf.Value()))
          listFaces1.Append(itf.Value());
    }
  }
  else
  {

    // Create presentation edge
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(Edge1, V1, V2);
    if (const TopoDS_Shape* pV1 = myVertexNode.Seek(V1))
      V1 = TopoDS::Vertex(*pV1);
    if (const TopoDS_Shape* pV2 = myVertexNode.Seek(V2))
      V2 = TopoDS::Vertex(*pV2);

    TopoDS_Edge NewEdge = Edge1;
    NewEdge.EmptyCopy();

    // Add the vertices
    BRep_Builder aBuilder;
    TopoDS_Shape anEdge = NewEdge.Oriented(TopAbs_FORWARD);
    aBuilder.Add(anEdge, V1.Oriented(TopAbs_FORWARD));
    aBuilder.Add(anEdge, V2.Oriented(TopAbs_REVERSED));

    Edge1 = NewEdge;
  }

  bool isForward = true;

  // Merge candidate sections
  for (int i = 1; i <= seqEdges.Length(); i++)
  {

    // Retrieve candidate section
    const TopoDS_Shape& oedge2 = seqEdges(i);

    if (mySewing)
    {

      aTmpShape         = myReShape->Apply(oedge2); // for porting
      TopoDS_Edge Edge2 = TopoDS::Edge(aTmpShape);
      aTmpShape         = locReShape->Apply(Edge2);
      if (locReShape != myReShape)
        Edge2 = TopoDS::Edge(aTmpShape);

      // Calculate relative orientation
      bool Orientation = seqForward(i);
      if (!isForward)
        Orientation = !Orientation;

      // Retrieve faces information for the second edge
      TopoDS_Shape bnd2 = oedge2;
      if (mySectionBound.IsBound(bnd2))
        bnd2 = mySectionBound(bnd2);
      if (!myBoundFaces.Contains(bnd2))
        continue; // Skip floating edge
      const NCollection_List<TopoDS_Shape>& listFaces2 = myBoundFaces.FindFromKey(bnd2);

      int         whichSec = 1; // Indicates on which edge the pCurve has been reported
      TopoDS_Edge NewEdge =
        SameParameterEdge(Edge1, Edge2, listFaces1, listFaces2, Orientation, whichSec);
      if (NewEdge.IsNull())
        continue;

      // Record faces information for the temporary merged edge
      NCollection_List<TopoDS_Shape>::Iterator itf(listFaces2);
      for (; itf.More(); itf.Next())
        if (MergedFaces.Add(itf.Value()))
          listFaces1.Append(itf.Value());

      // Record merged section orientation
      if (!Orientation && whichSec != 1)
        isForward = !isForward;
      Edge1 = NewEdge;
    }

    // Append actually merged edge
    mapMerged.Add(oedge2);
    isDone = true;

    if (!myNonmanifold)
      break;
  }

  if (isDone)
  {
    // Change result orientation
    Edge1.Orientation(isForward ? TopAbs_FORWARD : TopAbs_REVERSED);
  }
  else
    Edge1.Nullify();

  return Edge1;
}

//=================================================================================================

static bool findNMVertices(const TopoDS_Edge&                  theEdge,
                           NCollection_Sequence<TopoDS_Shape>& theSeqNMVert,
                           NCollection_Sequence<double>&       theSeqPars)
{
  TopoDS_Iterator aItV(theEdge, false);
  for (; aItV.More(); aItV.Next())
  {
    if (aItV.Value().Orientation() == TopAbs_INTERNAL
        || aItV.Value().Orientation() == TopAbs_EXTERNAL)
      theSeqNMVert.Append(aItV.Value());
  }
  int nbV = theSeqNMVert.Length();
  if (!nbV)
    return false;
  double                  first, last;
  occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(theEdge, first, last);
  GeomAdaptor_Curve       GAC(c3d);
  Extrema_ExtPC           locProj;
  locProj.Initialize(GAC, first, last);
  gp_Pnt pfirst = GAC.Value(first), plast = GAC.Value(last);

  for (int i = 1; i <= nbV; i++)
  {
    TopoDS_Vertex aV = TopoDS::Vertex(theSeqNMVert.Value(i));
    gp_Pnt        pt = BRep_Tool::Pnt(aV);

    double distF2 = pfirst.SquareDistance(pt);
    double distL2 = plast.SquareDistance(pt);
    double apar   = (distF2 > distL2 ? last : first);
    // Project current point on curve
    locProj.Perform(pt);
    if (locProj.IsDone() && locProj.NbExt() > 0)
    {
      double dist2Min = std::min(distF2, distL2);
      int    ind, indMin = 0;
      for (ind = 1; ind <= locProj.NbExt(); ind++)
      {
        double dProj2 = locProj.SquareDistance(ind);
        if (dProj2 < dist2Min)
        {
          indMin   = ind;
          dist2Min = dProj2;
        }
      }
      if (indMin)
        apar = locProj.Point(indMin).Parameter();

      theSeqPars.Append(apar);
    }
  }
  return true;
}

static void ComputeToleranceVertex(TopoDS_Vertex theV1, TopoDS_Vertex theV2, TopoDS_Vertex& theNewV)
{
  int           m, n;
  double        aR[2], dR, aD, aEps;
  TopoDS_Vertex aV[2];
  gp_Pnt        aP[2];
  BRep_Builder  aBB;
  //
  aEps  = RealEpsilon();
  aV[0] = std::move(theV1);
  aV[1] = std::move(theV2);
  for (m = 0; m < 2; ++m)
  {
    aP[m] = BRep_Tool::Pnt(aV[m]);
    aR[m] = BRep_Tool::Tolerance(aV[m]);
  }
  //
  m = 0; // max R
  n = 1; // min R
  if (aR[0] < aR[1])
  {
    m = 1;
    n = 0;
  }
  //
  dR = aR[m] - aR[n]; // dR >= 0.
  gp_Vec aVD(aP[m], aP[n]);
  aD = aVD.Magnitude();
  //
  if (aD <= dR || aD < aEps)
  {
    aBB.MakeVertex(theNewV, aP[m], aR[m]);
  }
  else
  {
    double aRr;
    gp_XYZ aXYZr;
    gp_Pnt aPr;
    //
    aRr   = 0.5 * (aR[m] + aR[n] + aD);
    aXYZr = 0.5 * (aP[m].XYZ() + aP[n].XYZ() - aVD.XYZ() * (dR / aD));
    aPr.SetXYZ(aXYZr);
    //
    aBB.MakeVertex(theNewV, aPr, aRr);
  }
  return;
}

static void ComputeToleranceVertex(TopoDS_Vertex  theV1,
                                   TopoDS_Vertex  theV2,
                                   TopoDS_Vertex  theV3,
                                   TopoDS_Vertex& theNewV)
{
  double        aDi, aDmax;
  gp_Pnt        aCenter;
  gp_Pnt        aP[3];
  double        aR[3];
  TopoDS_Vertex aV[3];
  gp_XYZ        aXYZ(0., 0., 0.);
  aV[0] = std::move(theV1);
  aV[1] = std::move(theV2);
  aV[2] = std::move(theV3);
  for (int i = 0; i < 3; ++i)
  {
    aP[i] = BRep_Tool::Pnt(aV[i]);
    aR[i] = BRep_Tool::Tolerance(aV[i]);
    aXYZ  = aXYZ + aP[i].XYZ();
  }
  //
  aXYZ.Divide(3.0);
  aCenter.SetXYZ(aXYZ);
  //
  aDmax = -1.;
  for (int i = 0; i < 3; ++i)
  {
    aDi = aCenter.Distance(aP[i]);
    aDi += aR[i];
    if (aDi > aDmax)
      aDmax = aDi;
  }

  BRep_Builder aBB;
  aBB.MakeVertex(theNewV, aCenter, aDmax);
  return;
}

TopoDS_Edge BRepBuilderAPI_Sewing::SameParameterEdge(
  const TopoDS_Edge&                    edgeFirst,
  const TopoDS_Edge&                    edgeLast,
  const NCollection_List<TopoDS_Shape>& listFacesFirst,
  const NCollection_List<TopoDS_Shape>& listFacesLast,
  const bool                            secForward,
  int&                                  whichSec,
  const bool                            firstCall)
{
  // Do not process floating edges
  if (!listFacesFirst.Extent() || !listFacesLast.Extent())
    return TopoDS_Edge();

  // Sort input edges
  TopoDS_Edge edge1, edge2;
  if (firstCall)
  {
    // Take the longest edge as first
    double                  f, l;
    occ::handle<Geom_Curve> c3d1 = BRep_Tool::Curve(TopoDS::Edge(edgeFirst), f, l);
    GeomAdaptor_Curve       cAdapt1(c3d1);
    double                  len1 = GCPnts_AbscissaPoint::Length(cAdapt1, f, l);
    occ::handle<Geom_Curve> c3d2 = BRep_Tool::Curve(TopoDS::Edge(edgeLast), f, l);
    GeomAdaptor_Curve       cAdapt2(c3d2);
    double                  len2 = GCPnts_AbscissaPoint::Length(cAdapt2, f, l);
    if (len1 < len2)
    {
      edge1    = edgeLast;
      edge2    = edgeFirst;
      whichSec = 2;
    }
    else
    {
      edge1    = edgeFirst;
      edge2    = edgeLast;
      whichSec = 1;
    }
  }
  else
  {
    if (whichSec == 1)
    {
      edge1    = edgeLast;
      edge2    = edgeFirst;
      whichSec = 2;
    }
    else
    {
      edge1    = edgeFirst;
      edge2    = edgeLast;
      whichSec = 1;
    }
  }

  double first, last;
  BRep_Tool::Range(edge1, first, last);
  BRep_Builder aBuilder;

  // To keep NM vertices on edge
  NCollection_Sequence<TopoDS_Shape> aSeqNMVert;
  NCollection_Sequence<double>       aSeqNMPars;
  findNMVertices(edge1, aSeqNMVert, aSeqNMPars);
  findNMVertices(edge2, aSeqNMVert, aSeqNMPars);

  // Create new edge
  TopoDS_Edge edge;
  aBuilder.MakeEdge(edge);
  edge.Orientation(edge1.Orientation());

  // Retrieve edge curve
  TopLoc_Location         loc3d;
  double                  first3d, last3d;
  occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(edge1, loc3d, first3d, last3d);
  if (!loc3d.IsIdentity())
  {
    c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
    c3d->Transform(loc3d.Transformation());
  }
  aBuilder.UpdateEdge(edge, c3d, BRep_Tool::Tolerance(edge1));
  aBuilder.Range(edge, first, last);
  aBuilder.SameRange(edge, false); // true
  aBuilder.SameParameter(edge, false);
  // Create and add new vertices
  {
    TopoDS_Vertex V1New, V2New;

    // Retrieve original vertices from edges
    TopoDS_Vertex V11, V12, V21, V22;
    TopExp::Vertices(edge1, V11, V12);
    TopExp::Vertices(edge2, V21, V22);

    // check that edges merged valid way (for edges having length less than specified
    // tolerance
    //  Check if edges are closed
    bool isClosed1 = V11.IsSame(V12);
    bool isClosed2 = V21.IsSame(V22);
    if (!isClosed1 && !isClosed2)
    {
      if (secForward)
      {
        if (V11.IsSame(V22) || V12.IsSame(V21))
          return TopoDS_Edge();
      }
      else
      {
        if (V11.IsSame(V21) || V12.IsSame(V22))
          return TopoDS_Edge();
      }
    }

    // szv: do not reshape here!!!
    // V11 = TopoDS::Vertex(myReShape->Apply(V11));
    // V12 = TopoDS::Vertex(myReShape->Apply(V12));
    // V21 = TopoDS::Vertex(myReShape->Apply(V21));
    // V22 = TopoDS::Vertex(myReShape->Apply(V22));

    // bool isRev = false;
    if (isClosed1 || isClosed2)
    {
      // at least one of the edges is closed
      if (isClosed1 && isClosed2)
      {
        // both edges are closed
        ComputeToleranceVertex(V11, V21, V1New);
      }
      else if (isClosed1)
      {
        // only first edge is closed
        ComputeToleranceVertex(V22, V21, V11, V1New);
      }
      else
      {
        // only second edge is closed
        ComputeToleranceVertex(V11, V12, V21, V1New);
      }
      V2New = V1New;
    }
    else
    {
      // both edges are open
      bool isOldFirst = (secForward ? V11.IsSame(V21) : V11.IsSame(V22));
      bool isOldLast  = (secForward ? V12.IsSame(V22) : V12.IsSame(V21));
      if (secForward)
      {
        // case if vertices already sewed
        if (!isOldFirst)
        {
          ComputeToleranceVertex(V11, V21, V1New);
        }
        if (!isOldLast)
        {
          ComputeToleranceVertex(V12, V22, V2New);
        }
      }
      else
      {
        if (!isOldFirst)
        {
          ComputeToleranceVertex(V11, V22, V1New);
        }
        if (!isOldLast)
        {
          ComputeToleranceVertex(V12, V21, V2New);
        }
      }
      if (isOldFirst)
        V1New = V11;

      if (isOldLast)
        V2New = V12;
    }
    // Add the vertices in the good sense
    TopoDS_Shape anEdge = edge.Oriented(TopAbs_FORWARD);
    // clang-format off
    TopoDS_Shape aLocalEdge = V1New.Oriented(TopAbs_FORWARD); //(listNode.First()).Oriented(TopAbs_FORWARD);
    // clang-format on
    aBuilder.Add(anEdge, aLocalEdge);
    aLocalEdge = V2New.Oriented(TopAbs_REVERSED); //(listNode.Last()).Oriented(TopAbs_REVERSED);
    aBuilder.Add(anEdge, aLocalEdge);

    int k = 1;
    for (; k <= aSeqNMVert.Length(); k++)
      aBuilder.Add(anEdge, aSeqNMVert.Value(k));
  }

  // Retrieve second PCurves
  TopLoc_Location           loc2;
  occ::handle<Geom_Surface> surf2;

  // occ::handle<Geom2d_Curve> c2d2, c2d21;
  //   double firstOld, lastOld;

  NCollection_List<TopoDS_Shape>::Iterator itf2;
  if (whichSec == 1)
    itf2.Initialize(listFacesLast);
  else
    itf2.Initialize(listFacesFirst);
  bool        isResEdge = false;
  TopoDS_Face fac2;
  for (; itf2.More(); itf2.Next())
  {
    occ::handle<Geom2d_Curve> c2d2, c2d21;
    double                    firstOld, lastOld;
    fac2 = TopoDS::Face(itf2.Value());

    surf2        = BRep_Tool::Surface(fac2, loc2);
    bool isSeam2 = ((IsUClosedSurface(surf2, edge2, loc2) || IsVClosedSurface(surf2, edge2, loc2))
                    && BRep_Tool::IsClosed(TopoDS::Edge(edge2), fac2));
    if (isSeam2)
    {
      if (!myNonmanifold)
        return TopoDS_Edge();
      TopoDS_Shape aTmpShape = edge2.Reversed(); // for porting
      c2d21 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aTmpShape), fac2, firstOld, lastOld);
    }
    c2d2 = BRep_Tool::CurveOnSurface(edge2, fac2, firstOld, lastOld);
    if (c2d2.IsNull() && c2d21.IsNull())
      continue;

    if (!c2d21.IsNull())
    {
      c2d21 = occ::down_cast<Geom2d_Curve>(c2d21->Copy());
      if (!secForward)
      {
        if (c2d21->IsKind(STANDARD_TYPE(Geom2d_Line)))
          c2d21 = new Geom2d_TrimmedCurve(c2d21, firstOld, lastOld);
        double first2d = firstOld; // c2dTmp->FirstParameter(); BUG USA60321
        double last2d  = lastOld;  // c2dTmp->LastParameter();
        firstOld       = c2d21->ReversedParameter(last2d);
        lastOld        = c2d21->ReversedParameter(first2d);
        c2d21->Reverse();
      }
      c2d21 = SameRange(c2d21, firstOld, lastOld, first, last);
    }

    // Make second PCurve sameRange with the 3d curve
    c2d2 = occ::down_cast<Geom2d_Curve>(c2d2->Copy());

    if (!secForward)
    {
      if (c2d2->IsKind(STANDARD_TYPE(Geom2d_Line)))
        c2d2 = new Geom2d_TrimmedCurve(c2d2, firstOld, lastOld);
      double first2d = firstOld;
      double last2d  = lastOld;
      firstOld       = c2d2->ReversedParameter(last2d);
      lastOld        = c2d2->ReversedParameter(first2d);
      c2d2->Reverse();
    }

    c2d2 = SameRange(c2d2, firstOld, lastOld, first, last);
    if (c2d2.IsNull())
      continue;

    // Add second PCurve
    bool               isSeam = false;
    TopAbs_Orientation Ori    = TopAbs_FORWARD;
    // occ::handle<Geom2d_Curve> c2d1, c2d11;

    NCollection_List<TopoDS_Shape>::Iterator itf1;
    if (whichSec == 1)
      itf1.Initialize(listFacesFirst);
    else
      itf1.Initialize(listFacesLast);
    for (; itf1.More() && !isSeam; itf1.Next())
    {
      occ::handle<Geom2d_Curve> c2d1, c2d11;
      const TopoDS_Face&        fac1 = TopoDS::Face(itf1.Value());

      TopLoc_Location           loc1;
      occ::handle<Geom_Surface> surf1 = BRep_Tool::Surface(fac1, loc1);

      double first2d, last2d;
      bool isSeam1 = ((IsUClosedSurface(surf1, edge1, loc1) || IsVClosedSurface(surf1, edge1, loc1))
                      && BRep_Tool::IsClosed(TopoDS::Edge(edge1), fac1));
      c2d1         = BRep_Tool::CurveOnSurface(edge1, fac1, first2d, last2d);
      Ori          = edge1.Orientation();
      if (fac1.Orientation() == TopAbs_REVERSED)
        Ori = TopAbs::Reverse(Ori);

      if (isSeam1)
      {
        if (!myNonmanifold)
          return TopoDS_Edge();
        TopoDS_Shape aTmpShape = edge1.Reversed(); // for porting
        c2d11 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aTmpShape), fac1, first2d, last2d);
        // if(fac1.Orientation() == TopAbs_REVERSED) //
        if (Ori == TopAbs_FORWARD)
          aBuilder.UpdateEdge(edge, c2d1, c2d11, fac1, 0);
        else
          aBuilder.UpdateEdge(edge, c2d11, c2d1, fac1, 0);
      }
      else
        aBuilder.UpdateEdge(edge, c2d1, fac1, 0);

      if (c2d1.IsNull() && c2d11.IsNull())
        continue;

      if (surf2 == surf1)
      {
        // Merge sections which are on the same face
        if (!loc2.IsDifferent(loc1))
        {
          bool uclosed = IsUClosedSurface(surf2, edge2, loc2);
          bool vclosed = IsVClosedSurface(surf2, edge2, loc2);
          if (uclosed || vclosed)
          {
            double pf = c2d1->FirstParameter();
            //	    double pl = c2d1->LastParameter();
            gp_Pnt2d p1n   = c2d1->Value(std::max(first, pf));
            gp_Pnt2d p21n  = c2d2->Value(std::max(first, c2d2->FirstParameter()));
            gp_Pnt2d p22n  = c2d2->Value(std::min(last, c2d2->LastParameter()));
            double   aDist = std::min(p1n.Distance(p21n), p1n.Distance(p22n));
            double   U1, U2, V1, V2;
            surf2->Bounds(U1, U2, V1, V2);
            isSeam = ((uclosed && aDist > 0.75 * (fabs(U2 - U1)))
                      || (vclosed && aDist > 0.75 * (fabs(V2 - V1))));
            if (!isSeam && BRep_Tool::IsClosed(TopoDS::Edge(edge), fac1))
              continue;
          }
        }
      }

      isResEdge = true;
      if (isSeam)
      {
        if (Ori == TopAbs_FORWARD)
          aBuilder.UpdateEdge(edge, c2d1, c2d2, surf2, loc2, Precision::Confusion());
        else
          aBuilder.UpdateEdge(edge, c2d2, c2d1, surf2, loc2, Precision::Confusion());
      }
      else if (isSeam2)
      {
        TopAbs_Orientation InitOri = edge2.Orientation();
        TopAbs_Orientation SecOri  = edge.Orientation();
        if (fac2.Orientation() == TopAbs_REVERSED)
        {

          InitOri = TopAbs::Reverse(InitOri);
          SecOri  = TopAbs::Reverse(SecOri);
        }
        if (!secForward)
          InitOri = TopAbs::Reverse(InitOri);

        if (InitOri == TopAbs_FORWARD)
          aBuilder.UpdateEdge(edge, c2d2, c2d21, surf2, loc2, Precision::Confusion());
        else
          aBuilder.UpdateEdge(edge, c2d21, c2d2, surf2, loc2, Precision::Confusion());
      }
      else
      {
        aBuilder.UpdateEdge(edge, c2d2, surf2, loc2, Precision::Confusion());
      }
    }
  }
  double tolReached = Precision::Infinite();
  bool   isSamePar  = false;
  try
  {
    if (isResEdge)
      SameParameter(edge);

    if (BRep_Tool::SameParameter(edge))
    {
      isSamePar  = true;
      tolReached = BRep_Tool::Tolerance(edge);
    }
  }

  catch (Standard_Failure const&)
  {
    isSamePar = false;
  }

  if (firstCall && (!isResEdge || !isSamePar || tolReached > myTolerance))
  {
    int whichSecn = whichSec;
    // Try to merge on the second section
    bool        second_ok = false;
    TopoDS_Edge s_edge    = SameParameterEdge(edgeFirst,
                                           edgeLast,
                                           listFacesFirst,
                                           listFacesLast,
                                           secForward,
                                           whichSecn,
                                           false);
    if (!s_edge.IsNull())
    {
      double tolReached_2 = BRep_Tool::Tolerance(s_edge);
      second_ok           = (BRep_Tool::SameParameter(s_edge) && tolReached_2 < tolReached);
      if (second_ok)
      {
        edge       = s_edge;
        whichSec   = whichSecn;
        tolReached = tolReached_2;
      }
    }

    if (!second_ok && !edge.IsNull())
    {

      GeomAdaptor_Curve c3dAdapt(c3d);

      // Discretize edge curve
      int                        i, j, nbp = 23;
      double                     deltaT = (last3d - first3d) / (nbp - 1);
      NCollection_Array1<gp_Pnt> c3dpnt(1, nbp);
      for (i = 1; i <= nbp; i++)
        c3dpnt(i) = c3dAdapt.Value(first3d + (i - 1) * deltaT);

      double dist = 0., maxTol = -1.0;
      bool   more = true;

      for (j = 1; more; j++)
      {
        occ::handle<Geom2d_Curve> c2d2;
        BRep_Tool::CurveOnSurface(edge, c2d2, surf2, loc2, first, last, j);

        more = !c2d2.IsNull();
        if (more)
        {
          occ::handle<Geom_Surface> aS = surf2;
          if (!loc2.IsIdentity())
            aS = occ::down_cast<Geom_Surface>(surf2->Transformed(loc2));

          double dist2 = 0.;
          deltaT       = (last - first) / (nbp - 1);
          for (i = 1; i <= nbp; i++)
          {
            gp_Pnt2d aP2d = c2d2->Value(first + (i - 1) * deltaT);
            gp_Pnt   aP2(0., 0., 0.);
            aS->D0(aP2d.X(), aP2d.Y(), aP2);
            gp_Pnt aP1 = c3dpnt(i);
            dist       = aP2.SquareDistance(aP1);
            if (dist > dist2)
              dist2 = dist;
          }
          maxTol = std::max(sqrt(dist2) * (1. + 1e-7), Precision::Confusion());
        }
      }
      if (maxTol >= 0. && maxTol < tolReached)
      {
        if (tolReached > MaxTolerance())
        {
          // Set tolerance directly to overwrite too large tolerance
          static_cast<BRep_TEdge*>(edge.TShape().get())->Tolerance(maxTol);
        }
        else
        {
          // just update tolerance with computed distance
          aBuilder.UpdateEdge(edge, maxTol);
        }
      }
      aBuilder.SameParameter(edge, true);
    }
  }

  double tolEdge1 = BRep_Tool::Tolerance(edge);
  if (tolEdge1 > MaxTolerance())
    edge.Nullify();
  return edge;
}

//=================================================================================================

void BRepBuilderAPI_Sewing::EvaluateAngulars(NCollection_Sequence<TopoDS_Shape>& sequenceSec,
                                             NCollection_Array1<bool>&           secForward,
                                             NCollection_Array1<double>&         tabAng,
                                             const int                           indRef) const
{
  tabAng.Init(-1.0);

  int i, j, npt = 4, lengSec = sequenceSec.Length();

  TopoDS_Edge                edge;
  TopoDS_Face                face;
  TopLoc_Location            loc;
  double                     first, last;
  occ::handle<Geom_Curve>    c3d;
  occ::handle<Geom2d_Curve>  c2d;
  occ::handle<Geom_Surface>  surf;
  NCollection_Array1<gp_Vec> normRef(1, npt);

  for (i = indRef; i <= lengSec; i++)
  {

    edge = TopoDS::Edge(sequenceSec(i));

    TopoDS_Shape bnd = edge;
    if (mySectionBound.IsBound(bnd))
      bnd = mySectionBound(bnd);
    if (myBoundFaces.Contains(bnd))
    {
      face = TopoDS::Face(myBoundFaces.FindFromKey(bnd).First());
      surf = BRep_Tool::Surface(face, loc);
      if (!loc.IsIdentity())
      {
        surf = occ::down_cast<Geom_Surface>(surf->Copy());
        surf->Transform(loc.Transformation());
      }
      c2d = BRep_Tool::CurveOnSurface(edge, face, first, last);
    }
    else if (i == indRef)
      return;
    else
      continue;

    c3d = BRep_Tool::Curve(edge, loc, first, last);
    if (!loc.IsIdentity())
    {
      c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
      c3d->Transform(loc.Transformation());
    }

    GeomAdaptor_Curve      adapt(c3d);
    GCPnts_UniformAbscissa uniAbs(adapt, npt, first, last);

    double cumulateAngular = 0.0;
    int    nbComputedAngle = 0;

    for (j = 1; j <= npt; j++)
    {
      gp_Pnt2d P;
      c2d->D0(uniAbs.Parameter((secForward(i) || i == indRef) ? j : npt - j + 1), P);
      gp_Vec w1, w2;
      gp_Pnt unused;
      surf->D1(P.X(), P.Y(), unused, w1, w2);
      gp_Vec n = w1 ^ w2; // Compute the normal vector
      if (i == indRef)
        normRef(j) = n;
      else if ((n.Magnitude() > gp::Resolution()) && (normRef(j).Magnitude() > gp::Resolution()))
      {
        nbComputedAngle++;
        double angular = n.Angle(normRef(j));
        if (angular > M_PI / 2.)
          angular = M_PI - angular;
        cumulateAngular += angular;
      }
    }

    if (nbComputedAngle)
      tabAng(i) = cumulateAngular / ((double)nbComputedAngle);
  }
}

//=======================================================================
// function : EvaluateDistances
// purpose  : internal use
// Evaluate distance between edges with indice indRef and the following edges in the list
// Remarks (lengSec - indRef) must be >= 1
//=======================================================================
void BRepBuilderAPI_Sewing::EvaluateDistances(NCollection_Sequence<TopoDS_Shape>& sequenceSec,
                                              NCollection_Array1<bool>&           secForward,
                                              NCollection_Array1<double>&         tabDst,
                                              NCollection_Array1<double>&         arrLen,
                                              NCollection_Array1<double>&         tabMinDist,
                                              const int                           indRef) const
{
  secForward.Init(true);
  tabDst.Init(-1.0);
  arrLen.Init(0.);
  tabMinDist.Init(Precision::Infinite());
  const int                  npt = 8; // Number of points for curve discretization
  NCollection_Array1<gp_Pnt> ptsRef(1, npt), ptsSec(1, npt);

  int                          i, j, lengSec = sequenceSec.Length();
  NCollection_Sequence<gp_Pnt> seqSec;

  occ::handle<Geom_Curve> c3dRef;
  double                  firstRef = 0., lastRef = 0.;

  for (i = indRef; i <= lengSec; i++)
  {

    // reading of the edge (attention for the first one: reference)
    const TopoDS_Edge& sec = TopoDS::Edge(sequenceSec(i));

    TopLoc_Location         loc;
    double                  first, last;
    occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(sec, loc, first, last);
    if (c3d.IsNull())
      continue;
    if (!loc.IsIdentity())
    {
      c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
      c3d->Transform(loc.Transformation());
    }

    if (i == indRef)
    {
      c3dRef   = c3d;
      firstRef = first;
      lastRef  = last;
    }

    double dist = Precision::Infinite(), distFor = -1.0, distRev = -1.0;
    double aMinDist = Precision::Infinite();

    double T, deltaT = (last - first) / (npt - 1);
    double aLenSec2 = 0.;

    int nbFound = 0;
    for (j = 1; j <= npt; j++)
    {

      // Uniform parameter on curve
      if (j == 1)
        T = first;
      else if (j == npt)
        T = last;
      else
        T = first + (j - 1) * deltaT;

      // Take point on curve
      gp_Pnt pt = c3d->Value(T);

      if (i == indRef)
      {
        ptsRef(j) = pt;
        if (j > 1)
          aLenSec2 += pt.SquareDistance(ptsRef(j - 1));
      }
      else
      {
        ptsSec(j) = pt;
        // protection to avoid merging with small sections
        if (j > 1)
          aLenSec2 += pt.SquareDistance(ptsSec(j - 1));
        // To evaluate mutual orientation and distance
        dist = pt.Distance(ptsRef(j));
        if (aMinDist > dist)
          aMinDist = dist;
        if (distFor < dist)
          distFor = dist;
        dist = pt.Distance(ptsRef(npt - j + 1));

        if (aMinDist > dist)
          aMinDist = dist;
        if (distRev < dist)
          distRev = dist;

        // Check that point lays between vertices of reference curve
        const gp_Pnt& p11 = ptsRef(1);
        const gp_Pnt& p12 = ptsRef(npt);
        const gp_Vec  aVec1(pt, p11);
        const gp_Vec  aVec2(pt, p12);
        const gp_Vec  aVecRef(p11, p12);
        if ((aVecRef * aVec1) * (aVecRef * aVec2) < 0.)
          nbFound++;
      }
    }

    double aLenSec = sqrt(aLenSec2);

    // if(aLenSec < myMinTolerance )
    //  continue;
    arrLen.SetValue(i, aLenSec);
    // Record mutual orientation
    bool isForward = (distFor < distRev); // szv debug: <=
    secForward(i)  = isForward;

    dist = (isForward ? distFor : distRev);
    if (i == indRef || (dist < myTolerance && nbFound >= npt * 0.5))
    {
      tabDst(i)     = dist;
      tabMinDist(i) = aMinDist;
    }
    else
    {
      nbFound = 0, aMinDist = Precision::Infinite(), dist = -1;
      NCollection_Array1<gp_Pnt> arrProj(1, npt);
      NCollection_Array1<double> arrDist(1, npt), arrPara(1, npt);
      if (arrLen(indRef) >= arrLen(i))
        ProjectPointsOnCurve(ptsSec, c3dRef, firstRef, lastRef, arrDist, arrPara, arrProj, false);
      else
        ProjectPointsOnCurve(ptsRef, c3d, first, last, arrDist, arrPara, arrProj, false);
      for (j = 1; j <= npt; j++)
      {
        if (arrDist(j) < 0.)
          continue;
        if (dist < arrDist(j))
          dist = arrDist(j);
        if (aMinDist > arrDist(j))
          aMinDist = arrDist(j);
        nbFound++;
      }
      if (nbFound > 1)
      {
        tabDst(i)     = dist;
        tabMinDist(i) = aMinDist;
      }
    }
  }

  /*
  // Project distant points
  int nbFailed = seqSec.Length();
  if (!nbFailed) return;

  NCollection_Array1<gp_Pnt> arrPnt(1, nbFailed), arrProj(1, nbFailed);
  for (i = 1; i <= nbFailed; i++) arrPnt(i) = seqSec(i); seqSec.Clear();
  NCollection_Array1<double> arrDist(1, nbFailed), arrPara(1, nbFailed);

  ProjectPointsOnCurve(arrPnt,c3dRef,firstRef,lastRef,arrDist,arrPara,arrProj,false);

  // Process distant sections
  int idx1 = 1;
  for (i = indRef + 1; i <= lengSec; i++) {

    // Skip section if already evaluated
    if (tabDst(i) >= 0.0) continue;

    double dist, distMax = -1.0, aMinDist = Precision::Infinite();

    int idx2 = (idx1 - 1)*npt;

    for (j = 1; j <= npt; j++) {

      dist = arrDist(idx2 + j);
      // If point is not projected - stop evaluation
      if (dist < 0.0) { distMax = -1.0; break; }
      if (distMax < dist) distMax = dist;
      if(aMinDist > dist) aMinDist = dist;
    }

    // If section is close - record distance
    if (distMax >= 0.0) {
      if (secForward(i)) {
        dist = arrPnt(idx2+1).Distance(ptsRef(1));
        if (distMax < dist) distMax = dist;
        if(aMinDist > dist) aMinDist = dist;
        dist = arrPnt(idx2+npt).Distance(ptsRef(npt));
        if (distMax < dist) distMax = dist;
        if(aMinDist > dist) aMinDist = dist;
      }
      else {
        dist = arrPnt(idx2+1).Distance(ptsRef(npt));
        if (distMax < dist) distMax = dist;
        if(aMinDist > dist) aMinDist = dist;
        dist = arrPnt(idx2+npt).Distance(ptsRef(1));
        if (distMax < dist) distMax = dist;
        if(aMinDist > dist) aMinDist = dist;
      }

      if (distMax < myTolerance)
      {
        tabDst(i) = distMax;
        tabMinDist(i) = aMinDist;
      }
    }

    idx1++; // To the next distant curve
  }*/
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsMergedClosed(const TopoDS_Edge& Edge1,
                                           const TopoDS_Edge& Edge2,
                                           const TopoDS_Face& face) const
{
  // Check for closed surface
  TopLoc_Location           loc;
  occ::handle<Geom_Surface> surf      = BRep_Tool::Surface(face, loc);
  bool                      isUClosed = IsUClosedSurface(surf, Edge1, loc);
  bool                      isVClosed = IsVClosedSurface(surf, Edge1, loc);
  if (!isUClosed && !isVClosed)
    return false;
  // Check condition on closed surface
  /*
  double first1,last1,first2,last2;
  occ::handle<Geom_Curve> C3d1 = BRep_Tool::Curve(Edge1,first1,last1);
  occ::handle<Geom_Curve> C3d2 = BRep_Tool::Curve(Edge2,first2,last2);
  if (C3d1.IsNull() || C3d2.IsNull()) return false;
  */
  double                    first2d1, last2d1, first2d2, last2d2;
  occ::handle<Geom2d_Curve> C2d1 = BRep_Tool::CurveOnSurface(Edge1, face, first2d1, last2d1);
  occ::handle<Geom2d_Curve> C2d2 = BRep_Tool::CurveOnSurface(Edge2, face, first2d2, last2d2);
  if (C2d1.IsNull() || C2d2.IsNull())
    return false;
  /*
  gp_Pnt p1 = C3d1->Value(0.5*(first1 + last1));
  gp_Pnt p2 = C3d1->Value(0.5*(first2 + last2));
  double dist = p1.Distance(p2);
  gp_Pnt2d p12d = C2d1->Value(0.5*(first2d1 + last2d1));
  gp_Pnt2d p22d = C2d1->Value(0.5*(first2d2 + last2d2));
  double dist2d = p12d.Distance(p22d);
  GeomAdaptor_Surface Ads(BRep_Tool::Surface(face));
  double distSurf = std::max(Ads.UResolution(dist), Ads.VResolution(dist));
  return (dist2d*0.2 >= distSurf);
  */
  int    isULongC1, isULongC2, isVLongC1, isVLongC2;
  double SUmin, SUmax, SVmin, SVmax;
  double C1Umin, C1Vmin, C1Umax, C1Vmax;
  double C2Umin, C2Vmin, C2Umax, C2Vmax;
  { // szv: Use brackets to destroy local variables
    Bnd_Box2d           B1, B2;
    Geom2dAdaptor_Curve aC2d1(C2d1), aC2d2(C2d2);
    BndLib_Add2dCurve::Add(aC2d1, first2d1, last2d1, Precision::PConfusion(), B1);
    BndLib_Add2dCurve::Add(aC2d2, first2d2, last2d2, Precision::PConfusion(), B2);
    B1.Get(C1Umin, C1Vmin, C1Umax, C1Vmax);
    B2.Get(C2Umin, C2Vmin, C2Umax, C2Vmax);
    double du, dv;
    du        = (C1Umax - C1Umin);
    dv        = (C1Vmax - C1Vmin);
    isULongC1 = (dv <= du);
    isVLongC1 = (du <= dv);
    du        = (C2Umax - C2Umin);
    dv        = (C2Vmax - C2Vmin);
    isULongC2 = (dv <= du);
    isVLongC2 = (du <= dv);
    surf->Bounds(SUmin, SUmax, SVmin, SVmax);
  }
  if (isUClosed && isVLongC1 && isVLongC2)
  {
    // Do not merge if not overlapped by V
    double dist = std::max((C2Vmin - C1Vmax), (C1Vmin - C2Vmax));
    if (dist < 0.0)
    {
      double distInner = std::max((C2Umin - C1Umax), (C1Umin - C2Umax));
      double distOuter = (SUmax - SUmin) - std::max((C2Umax - C1Umin), (C1Umax - C2Umin));
      if (distOuter <= distInner)
        return true;
    }
  }
  if (isVClosed && isULongC1 && isULongC2)
  {
    // Do not merge if not overlapped by U
    double dist = std::max((C2Umin - C1Umax), (C1Umin - C2Umax));
    if (dist < 0.0)
    {
      double distInner = std::max((C2Vmin - C1Vmax), (C1Vmin - C2Vmax));
      double distOuter = (SVmax - SVmin) - std::max((C2Vmax - C1Vmin), (C1Vmax - C2Vmin));
      if (distOuter <= distInner)
        return true;
    }
  }
  return false;
}

//=================================================================================================

void BRepBuilderAPI_Sewing::AnalysisNearestEdges(
  const NCollection_Sequence<TopoDS_Shape>& sequenceSec,
  NCollection_Sequence<int>&                seqIndCandidate,
  NCollection_Sequence<bool>&               seqOrientations,
  const bool                                evalDist)
{

  int                            workIndex = seqIndCandidate.First();
  TopoDS_Shape                   workedge  = sequenceSec.Value(workIndex);
  TopoDS_Shape                   bnd       = workedge;
  NCollection_List<TopoDS_Shape> workfaces;
  if (mySectionBound.IsBound(bnd))
    bnd = mySectionBound(bnd);
  if (myBoundFaces.Contains(bnd))
    workfaces = myBoundFaces.FindFromKey(bnd);
  if (workfaces.IsEmpty())
    return;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapFaces;
  NCollection_List<TopoDS_Shape>::Iterator               lIt;
  for (lIt.Initialize(workfaces); lIt.More(); lIt.Next())
    mapFaces.Add(lIt.Value());
  NCollection_Sequence<int> seqNotCandidate;
  NCollection_Sequence<int> seqNewForward;
  // Separates edges belonging the same face as work edge
  // for exception of edges belonging closed faces

  seqNotCandidate.Append(workIndex);
  for (int i = 1; i <= seqIndCandidate.Length();)
  {
    int  index    = seqIndCandidate.Value(i);
    bool isRemove = false;
    if (index == workIndex)
    {
      seqIndCandidate.Remove(i);
      seqOrientations.Remove(i);
      isRemove = true;
    }
    if (!isRemove)
    {
      TopoDS_Shape bnd2 = sequenceSec.Value(index);
      if (mySectionBound.IsBound(bnd2))
        bnd2 = mySectionBound(bnd2);

      if (myBoundFaces.Contains(bnd2))
      {
        const NCollection_List<TopoDS_Shape>& listfaces = myBoundFaces.FindFromKey(bnd2);
        bool                                  isMerged  = true;
        for (lIt.Initialize(listfaces); lIt.More() && isMerged; lIt.Next())
        {
          if (mapFaces.Contains(lIt.Value()))
          {
            TopLoc_Location           loc;
            occ::handle<Geom_Surface> surf = BRep_Tool::Surface(TopoDS::Face(lIt.Value()), loc);
            isMerged = ((IsUClosedSurface(surf, bnd2, loc) || IsVClosedSurface(surf, bnd2, loc))
                        && IsMergedClosed(TopoDS::Edge(sequenceSec.Value(index)),
                                          TopoDS::Edge(workedge),
                                          TopoDS::Face(lIt.Value())));
          }
        }
        if (!isMerged)
        {
          seqNotCandidate.Append(index);
          seqIndCandidate.Remove(i);
          seqOrientations.Remove(i);
          isRemove = true;
        }
      }
      else
      {
        seqIndCandidate.Remove(i);
        seqOrientations.Remove(i);
        isRemove = true;
      }
    }
    if (!isRemove)
      i++;
  }
  if (seqIndCandidate.Length() == 0 || seqNotCandidate.Length() == 1)
    return;
  if (!evalDist)
    return;
  NCollection_Array2<double> TotTabDist(1, seqNotCandidate.Length(), 1, seqIndCandidate.Length());
  NCollection_Map<int>       MapIndex;
  NCollection_Sequence<int>  seqForward;

  // Definition and removing edges which are not candidate for work edge
  // (they have other nearest edges belonging to the work face)
  for (int k = 1; k <= seqNotCandidate.Length(); k++)
  {
    int                                index1 = seqNotCandidate.Value(k);
    const TopoDS_Shape&                edge   = sequenceSec.Value(index1);
    NCollection_Sequence<TopoDS_Shape> tmpSeq;
    tmpSeq.Append(edge);
    for (int kk = 1; kk <= seqIndCandidate.Length(); kk++)
      tmpSeq.Append(sequenceSec.Value(seqIndCandidate.Value(kk)));

    int                        lengSec = tmpSeq.Length();
    NCollection_Array1<bool>   tabForward(1, lengSec);
    NCollection_Array1<double> tabDist(1, lengSec);
    NCollection_Array1<double> arrLen(1, lengSec);
    NCollection_Array1<double> tabMinDist(1, lengSec);
    for (int i1 = 1; i1 <= lengSec; i1++)
      tabDist(i1) = -1;

    EvaluateDistances(tmpSeq, tabForward, tabDist, arrLen, tabMinDist, 1);
    if (k == 1)
    {
      for (int n = 1; n < lengSec; n++)
      {
        if (tabDist(n + 1) == -1 || tabDist(n + 1) > myTolerance)
        {
          MapIndex.Add(n);
          continue;
        }
        TotTabDist(k, n) = tabDist(n + 1);
        seqForward.Append(tabForward(n + 1) ? 1 : 0);
      }
    }
    else
    {
      for (int n = 1; n < lengSec; n++)
      {
        if (tabDist(n) == -1 || tabDist(n) > myTolerance)
          continue;
        if (tabDist(n + 1) < TotTabDist(1, n))
        {
          MapIndex.Add(n);
        }
      }
    }
  }

  int i2 = seqIndCandidate.Length();
  for (; i2 >= 1; i2--)
  {
    if (MapIndex.Contains(i2))
    {
      seqIndCandidate.Remove(i2);
      seqOrientations.Remove(i2);
    }
  }
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::FindCandidates(NCollection_Sequence<TopoDS_Shape>& seqSections,
                                           NCollection_IndexedMap<int>&        mapReference,
                                           NCollection_Sequence<int>&          seqCandidates,
                                           NCollection_Sequence<bool>&         seqOrientations)
{
  int i, nbSections = seqSections.Length();
  if (nbSections <= 1)
    return false;
  // Retrieve last reference index
  int indReference = mapReference(mapReference.Extent());
  int nbCandidates = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Faces1;
  // if (nbSections > 1) {

  TopoDS_Edge Edge1 = TopoDS::Edge(seqSections(indReference));

  // Retrieve faces for reference section

  { // szv: Use brackets to destroy local variables
    TopoDS_Shape bnd = Edge1;
    if (mySectionBound.IsBound(bnd))
      bnd = mySectionBound(bnd);
    if (myBoundFaces.Contains(bnd))
    {
      NCollection_List<TopoDS_Shape>::Iterator itf1(myBoundFaces.FindFromKey(bnd));
      for (; itf1.More(); itf1.Next())
        Faces1.Add(itf1.Value());
    }
  }

  // Check merging conditions for candidates and remove unsatisfactory
  NCollection_Sequence<TopoDS_Shape> seqSectionsNew;
  NCollection_Sequence<int>          seqCandidatesNew;
  for (i = 1; i <= nbSections; i++)
  {
    if (i == indReference)
    {
      seqSectionsNew.Prepend(Edge1);
      seqCandidatesNew.Prepend(i);
    }
    else
    {
      const TopoDS_Edge& Edge2 = TopoDS::Edge(seqSections(i));
      seqSectionsNew.Append(Edge2);
      seqCandidatesNew.Append(i);
    }
  }

  int nbSectionsNew = seqSectionsNew.Length();
  if (nbSectionsNew > 1)
  {

    // Evaluate distances between reference and other sections
    NCollection_Array1<bool>   arrForward(1, nbSectionsNew);
    NCollection_Array1<double> arrDistance(1, nbSectionsNew);
    NCollection_Array1<double> arrLen(1, nbSectionsNew);
    NCollection_Array1<double> arrMinDist(1, nbSectionsNew);
    EvaluateDistances(seqSectionsNew, arrForward, arrDistance, arrLen, arrMinDist, 1);

    // Fill sequence of candidate indices sorted by distance
    for (i = 2; i <= nbSectionsNew; i++)
    {
      double aMaxDist = arrDistance(i);
      if (aMaxDist >= 0.0 && aMaxDist <= myTolerance && arrLen(i) > myMinTolerance)
      {

        // Reference section is connected to section #i
        bool isInserted = false;
        bool ori        = arrForward(i);
        for (int j = 1; (j <= seqCandidates.Length()) && !isInserted; j++)
        {
          double aDelta = arrDistance(i) - arrDistance(seqCandidates.Value(j));

          if (aDelta < Precision::Confusion())
          {

            if (fabs(aDelta) > RealSmall() || arrMinDist(i) < arrMinDist(seqCandidates.Value(j)))
            {
              seqCandidates.InsertBefore(j, i);
              seqOrientations.InsertBefore(j, ori);
              isInserted = true;
            }
          }
        }
        if (!isInserted)
        {
          seqCandidates.Append(i);
          seqOrientations.Append(ori);
        }
      }
    }

    nbCandidates = seqCandidates.Length();
    if (!nbCandidates)
      return false; // Section has no candidates to merge

    // Replace candidate indices

    for (i = 1; i <= nbCandidates; i++)
      seqCandidates(i) = seqCandidatesNew(seqCandidates(i));
  }

  if (!nbCandidates)
    return false; // Section has no candidates to merge

  if (myNonmanifold && nbCandidates > 1)
  {
    NCollection_Sequence<int>  seqNewCandidates;
    NCollection_Sequence<bool> seqOrientationsNew;
    seqCandidates.Prepend(1);
    seqOrientations.Prepend(true);
    for (int k = 1; k <= seqSections.Length() && seqCandidates.Length() > 1; k++)
    {
      AnalysisNearestEdges(seqSections, seqCandidates, seqOrientations, (k == 1));
      if (k == 1 && !seqCandidates.Length())
        return false;
      if (seqCandidates.Length())
      {
        seqNewCandidates.Append(seqCandidates.First());
        seqOrientationsNew.Append(seqOrientations.First());
      }
    }
    seqCandidates.Prepend(seqNewCandidates);
    seqOrientations.Prepend(seqOrientationsNew);
    return true;
  }
  else
  {

    // For manifold case leave only one candidate from equidistant candidates
    /*int minIndex = seqCandidateIndex.First();
    double minDistance = arrDistance(minIndex);

    // Find equidistant candidates
    NCollection_Sequence<int> seqEqDistantIndex; seqEqDistantIndex.Append(1);
    for (i = 2; i <= nbCandidates; i++) {
    int index = seqCandidateIndex(i);
    if (std::abs(minDistance - arrDistance(index)) <= Precision::Confusion())
    seqEqDistantIndex.Append(index);
    }

    int eqLen = seqEqDistantIndex.Length();
    if (eqLen > 2) {

    // Fill map of faces which equidistant sections belong to
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapFace;
    for (i = 1; i <= eqLen; i++) {
    int index = seqEqDistantIndex.Value(i);
    if (isCandidate(index)) {
    mapFace.Add(arrFace(index));
    }
    }

    // Non Manifold case
    // Edges are merged by pair among a face continuity C1 criterion
    if (mapFace.Extent() == eqLen) {

    tabDist.Init(-1);
    tabMinInd.Init(-1);
    min=10000000.;
    //indMin = -1;
    int indMin = -1;// To check if the edge can be merged.
    // Computation of distances between the edges.
    NCollection_Sequence<TopoDS_Shape> seqSh;
    int nbInd = EqDistSeq.Length();
    NCollection_Array1<bool> tmptabForward(1,nbInd);
    seqSh.Append(sequenceSec.Value(1));
    for (j = 2; j <= EqDistSeq.Length(); j++) {
    int index = EqDistSeq.Value(j);
    tmptabForward(j) = tabForward(index);
    seqSh.Append(sequenceSec.Value(index));
    }

    EvaluateAngulars(seqSh, tmptabForward, tabDist,1);

    for(j=2; j <= seqSh.Length(); j++) {
    if (tabDist(j) > -1.) {  // if edge(j) is connected to edge(i)
    if (min > tabDist(j)) {
    min = tabDist(j);
    indMin = j;
    }
    }
    }

    //  Construct minDist, tabMinInd , tabMinForward(i) = tabForward(j);
    if (indMin > 0) {
    seqSh.Remove(indMin);
    for(j =2; j <= tmpSeq.Length(); ) {
    TopoDS_Shape sh = tmpSeq.Value(j);
    bool isRem = false;
    for(int k = 1; k<= seqSh.Length();k++) {
    if(seqSh.Value(k) == sh) {
    isRem = true;
    break;
    }
    }
    if(isRem) {
    tmpSeq.Remove(j);
    tabMinForward.Remove(j); // = -1;
    }
    else j++;
    }
    }
    }
    }*/

    // Find the best approved candidate
    while (nbCandidates)
    {
      // Retrieve first candidate
      int indCandidate = seqCandidates.First();
      // Candidate is approved if it is in the map
      if (mapReference.Contains(indCandidate))
        break;
      // Find candidates for candidate #indCandidate
      mapReference.Add(indCandidate); // Push candidate in the map
      NCollection_Sequence<int>  seqCandidates1;
      NCollection_Sequence<bool> seqOrientations1;
      bool isFound = FindCandidates(seqSections, mapReference, seqCandidates1, seqOrientations1);
      mapReference.RemoveLast(); // Pop candidate from the map
      if (isFound)
        isFound = (seqCandidates1.Length() > 0);
      if (isFound)
      {
        int indCandidate1 = seqCandidates1.First();
        // If indReference is the best candidate for indCandidate
        // then indCandidate is the best candidate for indReference
        if (indCandidate1 == indReference)
          break;
        // If some other reference in the map is the best candidate for indCandidate
        // then assume that reference is the best candidate for indReference
        if (mapReference.Contains(indCandidate1))
        {
          seqCandidates.Prepend(indCandidate1);
          nbCandidates++;
          break;
        }
        isFound = false;
      }
      if (!isFound)
      {
        // Remove candidate #1
        seqCandidates.Remove(1);
        seqOrientations.Remove(1);
        nbCandidates--;
      }
    }
  }
  // gka
  if (nbCandidates > 0)
  {
    int          anInd = seqCandidates.Value(1);
    TopoDS_Edge  Edge2 = TopoDS::Edge(seqSections(anInd));
    TopoDS_Shape bnd   = Edge2;
    if (mySectionBound.IsBound(bnd))
      bnd = mySectionBound(bnd);
    // gka
    if (myBoundFaces.Contains(bnd))
    {
      bool                                     isOK = true;
      NCollection_List<TopoDS_Shape>::Iterator itf2(myBoundFaces.FindFromKey(bnd));
      for (; itf2.More() && isOK; itf2.Next())
      {
        const TopoDS_Face& Face2 = TopoDS::Face(itf2.Value());
        // Check whether condition is satisfied
        isOK = !Faces1.Contains(Face2);
        if (!isOK)
          isOK = IsMergedClosed(Edge1, Edge2, Face2);
      }
      if (!isOK)
        return false;
    }
  }
  return (nbCandidates > 0);
}

//=================================================================================================

BRepBuilderAPI_Sewing::BRepBuilderAPI_Sewing(const double tolerance,
                                             const bool   optionSewing,
                                             const bool   optionAnalysis,
                                             const bool   optionCutting,
                                             const bool   optionNonmanifold)
{
  myReShape = new BRepTools_ReShape;
  Init(tolerance, optionSewing, optionAnalysis, optionCutting, optionNonmanifold);
}

//=======================================================================
// function : Init
// purpose  : Initialise Talerance, and options sewing, faceAnalysis and cutting
//=======================================================================

void BRepBuilderAPI_Sewing::Init(const double tolerance,
                                 const bool   optionSewing,
                                 const bool   optionAnalysis,
                                 const bool   optionCutting,
                                 const bool   optionNonmanifold)
{
  // Set tolerance and Perform options
  myTolerance   = std::max(tolerance, Precision::Confusion());
  mySewing      = optionSewing;
  myAnalysis    = optionAnalysis;
  myCutting     = optionCutting;
  myNonmanifold = optionNonmanifold;
  // Set min and max tolerances
  myMinTolerance = myTolerance * 1e-4; // szv: proposal
  if (myMinTolerance < Precision::Confusion())
    myMinTolerance = Precision::Confusion();
  myMaxTolerance = Precision::Infinite();
  // Set other modes
  myFaceMode          = true;
  myFloatingEdgesMode = false;
  // myCuttingFloatingEdgesMode = false; //gka
  mySameParameterMode  = true;
  myLocalToleranceMode = false;
  mySewedShape.Nullify();
  // Load empty shape
  Load(TopoDS_Shape());
}

//=======================================================================
// function : Load
// purpose  : Loads the context shape
//=======================================================================

void BRepBuilderAPI_Sewing::Load(const TopoDS_Shape& theShape)
{
  myReShape->Clear();
  if (theShape.IsNull())
    myShape.Nullify();
  else
    myShape = myReShape->Apply(theShape);
  mySewedShape.Nullify();
  // Nullify flags and counters
  myNbShapes = myNbEdges = myNbVertices = 0;
  // Clear all maps
  myOldShapes.Clear();
  // myOldFaces.Clear();
  myDegenerated.Clear();
  myFreeEdges.Clear();
  myMultipleEdges.Clear();
  myContigousEdges.Clear();
  myContigSecBound.Clear();
  myBoundFaces.Clear();
  myBoundSections.Clear();
  myVertexNode.Clear();
  myVertexNodeFree.Clear();
  myNodeSections.Clear();
  myCuttingNode.Clear();
  mySectionBound.Clear();
  myLittleFace.Clear();
}

//=================================================================================================

void BRepBuilderAPI_Sewing::Add(const TopoDS_Shape& aShape)
{
  if (aShape.IsNull())
    return;
  TopoDS_Shape oShape = myReShape->Apply(aShape);
  myOldShapes.Add(aShape, oShape);
  myNbShapes = myOldShapes.Extent();
}

//=================================================================================================

#ifdef OCCT_DEBUG
  #include <OSD_Timer.hxx>
#endif

void BRepBuilderAPI_Sewing::Perform(const Message_ProgressRange& theProgress)
{
  const int             aNumberOfStages = myAnalysis + myCutting + mySewing + 2;
  Message_ProgressScope aPS(theProgress, "Sewing", aNumberOfStages);
#ifdef OCCT_DEBUG
  double          t_total = 0., t_analysis = 0., t_assembling = 0., t_cutting = 0., t_merging = 0.;
  OSD_Chronometer chr_total, chr_local;
  chr_total.Reset();
  chr_total.Start();
#endif

  // face analysis
  if (myAnalysis)
  {
#ifdef OCCT_DEBUG
    std::cout << "Begin face analysis..." << std::endl;
    chr_local.Reset();
    chr_local.Start();
#endif
    FaceAnalysis(aPS.Next());
    if (!aPS.More())
      return;
#ifdef OCCT_DEBUG
    chr_local.Stop();
    chr_local.Show(t_analysis);
    std::cout << "Face analysis finished after " << t_analysis << " s" << std::endl;
#endif
  }

  if (myNbShapes || !myShape.IsNull())
  {

    FindFreeBoundaries();

    if (myBoundFaces.Extent())
    {

#ifdef OCCT_DEBUG
      std::cout << "Begin vertices assembling..." << std::endl;
      chr_local.Reset();
      chr_local.Start();
#endif
      VerticesAssembling(aPS.Next());
      if (!aPS.More())
        return;
#ifdef OCCT_DEBUG
      chr_local.Stop();
      chr_local.Show(t_assembling);
      std::cout << "Vertices assembling finished after " << t_assembling << " s" << std::endl;
#endif
      if (myCutting)
      {
#ifdef OCCT_DEBUG
        std::cout << "Begin cutting..." << std::endl;
        chr_local.Reset();
        chr_local.Start();
#endif
        Cutting(aPS.Next());
        if (!aPS.More())
          return;
#ifdef OCCT_DEBUG
        chr_local.Stop();
        chr_local.Show(t_cutting);
        std::cout << "Cutting finished after " << t_cutting << " s" << std::endl;
#endif
      }
#ifdef OCCT_DEBUG
      std::cout << "Begin merging..." << std::endl;
      chr_local.Reset();
      chr_local.Start();
#endif
      Merging(true, aPS.Next());
      if (!aPS.More())
        return;
#ifdef OCCT_DEBUG
      chr_local.Stop();
      chr_local.Show(t_merging);
      std::cout << "Merging finished after " << t_merging << " s" << std::endl;
#endif
    }
    else
    {
      aPS.Next();
      if (myCutting)
        aPS.Next();
      aPS.Next();
      if (!aPS.More())
        return;
    }

    if (mySewing)
    {

#ifdef OCCT_DEBUG
      std::cout << "Creating sewed shape..." << std::endl;
#endif
      // examine the multiple edges if any and process sameparameter for edges if necessary
      EdgeProcessing(aPS.Next());
      if (!aPS.More())
        return;
      CreateSewedShape();
      if (!aPS.More())
      {
        mySewedShape.Nullify();
        return;
      }

      EdgeRegularity(aPS.Next());

      if (mySameParameterMode && myFaceMode)
        SameParameterShape();
      if (!aPS.More())
      {
        mySewedShape.Nullify();
        return;
      }
#ifdef OCCT_DEBUG
      std::cout << "Sewed shape created" << std::endl;
#endif
    }

    // create edge information for output
    CreateOutputInformations();
    if (!aPS.More())
    {
      mySewedShape.Nullify();
      return;
    }
  }
#ifdef OCCT_DEBUG
  chr_total.Stop();
  chr_total.Show(t_total);
  std::cout << "Sewing finished!" << std::endl;
  std::cout << " analysis time   : " << t_analysis << " s" << std::endl;
  std::cout << " assembling time : " << t_assembling << " s" << std::endl;
  std::cout << " cutting time    : " << t_cutting << " s" << std::endl;
  std::cout << " merging time    : " << t_merging << " s" << std::endl;
  std::cout << "Total time       : " << t_total << " s" << std::endl;
#endif
}

//=======================================================================
// function : SewedShape
// purpose  : give the sewed shape
//           if a null shape, reasons:
//             -- no useable input shapes : all input shapes are degenerated
//             -- has multiple edges
//=======================================================================

const TopoDS_Shape& BRepBuilderAPI_Sewing::SewedShape() const
{
  return mySewedShape;
}

//=================================================================================================

int BRepBuilderAPI_Sewing::NbFreeEdges() const
{
  return myFreeEdges.Extent();
}

//=================================================================================================

const TopoDS_Edge& BRepBuilderAPI_Sewing::FreeEdge(const int index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbFreeEdges(),
                               "BRepBuilderAPI_Sewing::FreeEdge");
  return TopoDS::Edge(myFreeEdges(index));
}

//=================================================================================================

int BRepBuilderAPI_Sewing::NbMultipleEdges() const
{
  return myMultipleEdges.Extent();
}

//=================================================================================================

const TopoDS_Edge& BRepBuilderAPI_Sewing::MultipleEdge(const int index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbMultipleEdges(),
                               "BRepBuilderAPI_Sewing::MultipleEdge");
  return TopoDS::Edge(myMultipleEdges(index));
}

//=================================================================================================

int BRepBuilderAPI_Sewing::NbContigousEdges() const
{
  return myContigousEdges.Extent();
}

//=================================================================================================

const TopoDS_Edge& BRepBuilderAPI_Sewing::ContigousEdge(const int index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbContigousEdges(),
                               "BRepBuilderAPI_Sewing::ContigousEdge");
  return TopoDS::Edge(myContigousEdges.FindKey(index));
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepBuilderAPI_Sewing::ContigousEdgeCouple(
  const int index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbContigousEdges(),
                               "BRepBuilderAPI_Sewing::ContigousEdgeCouple");
  return myContigousEdges(index);
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsSectionBound(const TopoDS_Edge& section) const
{
  return myContigSecBound.IsBound(section);
}

//=================================================================================================

const TopoDS_Edge& BRepBuilderAPI_Sewing::SectionToBoundary(const TopoDS_Edge& section) const
{
  Standard_NoSuchObject_Raise_if(!IsSectionBound(section),
                                 "BRepBuilderAPI_Sewing::SectionToBoundary");
  return TopoDS::Edge(myContigSecBound(section));
}

//=================================================================================================

int BRepBuilderAPI_Sewing::NbDeletedFaces() const
{
  return myLittleFace.Extent();
}

//=================================================================================================

const TopoDS_Face& BRepBuilderAPI_Sewing::DeletedFace(const int index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbDeletedFaces(),
                               "BRepBuilderAPI_Sewing::DeletedFace");
  return TopoDS::Face(myLittleFace(index));
}

//=================================================================================================

int BRepBuilderAPI_Sewing::NbDegeneratedShapes() const
{
  return myDegenerated.Extent();
}

//=================================================================================================

const TopoDS_Shape& BRepBuilderAPI_Sewing::DegeneratedShape(const int index) const
{
  Standard_OutOfRange_Raise_if(index < 0 || index > NbDegeneratedShapes(),
                               "BRepBuilderAPI_Sewing::DegereratedShape");
  return myDegenerated(index);
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsDegenerated(const TopoDS_Shape& aShape) const
{
  TopoDS_Shape NewShape = myReShape->Apply(aShape);
  // Degenerated face
  if (aShape.ShapeType() == TopAbs_FACE)
    return NewShape.IsNull();
  if (NewShape.IsNull())
    return false;
  // Degenerated edge
  if (NewShape.ShapeType() == TopAbs_EDGE)
    return BRep_Tool::Degenerated(TopoDS::Edge(NewShape));
  // Degenerated wire
  if (NewShape.ShapeType() == TopAbs_WIRE)
  {
    bool isDegenerated = true;
    for (TopoDS_Iterator aIt(NewShape); aIt.More() && isDegenerated; aIt.Next())
      isDegenerated = BRep_Tool::Degenerated(TopoDS::Edge(aIt.Value()));
    return isDegenerated;
  }
  return false;
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsModified(const TopoDS_Shape& aShape) const
{
  TopoDS_Shape NewShape = aShape;
  if (myOldShapes.Contains(aShape))
    NewShape = myOldShapes.FindFromKey(aShape);
  if (!NewShape.IsSame(aShape))
    return true;
  return false;
}

//=================================================================================================

const TopoDS_Shape& BRepBuilderAPI_Sewing::Modified(const TopoDS_Shape& aShape) const
{
  if (myOldShapes.Contains(aShape))
    return myOldShapes.FindFromKey(aShape);
  // if (myOldFaces.Contains(aShape)) return myOldFaces.FindFromKey(aShape);
  return aShape;
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::IsModifiedSubShape(const TopoDS_Shape& aShape) const
{
  TopoDS_Shape NewShape = myReShape->Apply(aShape);
  return !NewShape.IsSame(aShape);
}

//=================================================================================================

TopoDS_Shape BRepBuilderAPI_Sewing::ModifiedSubShape(const TopoDS_Shape& aShape) const
{
  return myReShape->Apply(aShape);
}

//=================================================================================================

void BRepBuilderAPI_Sewing::Dump() const
{
  int i, NbBounds = myBoundFaces.Extent(), NbSections = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapVertices, mapEdges;
  for (i = 1; i <= NbBounds; i++)
  {
    TopoDS_Shape bound = myBoundFaces.FindKey(i);
    if (myBoundSections.IsBound(bound))
      NbSections += myBoundSections(bound).Extent();
    else
      NbSections++;
    TopExp_Explorer aExp(myReShape->Apply(bound), TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      TopoDS_Edge E = TopoDS::Edge(aExp.Current());
      mapEdges.Add(E);
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(E, V1, V2);
      mapVertices.Add(V1);
      mapVertices.Add(V2);
    }
  }
  std::cout << " " << std::endl;
  std::cout << "                        Information                         " << std::endl;
  std::cout << " ===========================================================" << std::endl;
  std::cout << " " << std::endl;
  std::cout << " Number of input shapes      : " << myOldShapes.Extent() << std::endl;
  std::cout << " Number of actual shapes     : " << myNbShapes << std::endl;
  std::cout << " Number of Bounds            : " << NbBounds << std::endl;
  std::cout << " Number of Sections          : " << NbSections << std::endl;
  std::cout << " Number of Edges             : " << mapEdges.Extent() << std::endl;
  std::cout << " Number of Vertices          : " << myNbVertices << std::endl;
  std::cout << " Number of Nodes             : " << mapVertices.Extent() << std::endl;
  std::cout << " Number of Free Edges        : " << myFreeEdges.Extent() << std::endl;
  std::cout << " Number of Contiguous Edges  : " << myContigousEdges.Extent() << std::endl;
  std::cout << " Number of Multiple Edges    : " << myMultipleEdges.Extent() << std::endl;
  std::cout << " Number of Degenerated Edges : " << myDegenerated.Extent() << std::endl;
  std::cout << " ===========================================================" << std::endl;
  std::cout << " " << std::endl;
}

//=======================================================================
// function : FaceAnalysis
// purpose  : Remove
//	     Modifies:
//                      myNbShapes
//                      myOldShapes
//
//           Constructs:
//                      myDegenerated
//=======================================================================

void BRepBuilderAPI_Sewing::FaceAnalysis(const Message_ProgressRange& theProgress)
{
  if (!myShape.IsNull() && myOldShapes.IsEmpty())
  {
    Add(myShape);
    myShape.Nullify();
  }

  BRep_Builder                                           B;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> SmallEdges;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                        GluedVertices;
  int                   i = 1;
  Message_ProgressScope aPS(theProgress, "Shape analysis", myOldShapes.Extent());
  for (i = 1; i <= myOldShapes.Extent() && aPS.More(); i++, aPS.Next())
  {
    for (TopExp_Explorer fexp(myOldShapes(i), TopAbs_FACE); fexp.More(); fexp.Next())
    {

      // Retrieve current face
      TopoDS_Shape aTmpShape = fexp.Current(); // for porting
      TopoDS_Face  face      = TopoDS::Face(aTmpShape);
      int          nbEdges = 0, nbSmall = 0;

      // Build replacing face
      aTmpShape                 = face.EmptyCopied().Oriented(TopAbs_FORWARD); // for porting
      TopoDS_Face nface         = TopoDS::Face(aTmpShape);
      bool        isFaceChanged = false;

      TopoDS_Iterator witer(face.Oriented(TopAbs_FORWARD));
      for (; witer.More(); witer.Next())
      {

        // Retrieve current wire
        aTmpShape = witer.Value(); // for porting
        if (aTmpShape.ShapeType() != TopAbs_WIRE)
          continue;
        TopoDS_Wire wire = TopoDS::Wire(aTmpShape);

        // Build replacing wire
        aTmpShape                 = wire.EmptyCopied().Oriented(TopAbs_FORWARD);
        TopoDS_Wire nwire         = TopoDS::Wire(aTmpShape);
        bool        isWireChanged = false;

        TopoDS_Iterator eiter(wire.Oriented(TopAbs_FORWARD));
        for (; eiter.More(); eiter.Next())
        {

          // Retrieve current edge
          aTmpShape        = eiter.Value(); // for porting
          TopoDS_Edge edge = TopoDS::Edge(aTmpShape);
          nbEdges++;

          // Process degenerated edge
          if (BRep_Tool::Degenerated(edge))
          {
            B.Add(nwire, edge); // Old edge kept
            myDegenerated.Add(edge);
            nbSmall++;
            continue;
          }

          bool isSmall = SmallEdges.Contains(edge);
          if (!isSmall)
          {

            // Check for small edge
            double                  first, last;
            occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(edge, first, last);
            if (c3d.IsNull())
            {
#ifdef OCCT_DEBUG
              std::cout << "Warning: Possibly small edge can be sewed: No 3D curve" << std::endl;
#endif
            }
            else
            {
              // Evaluate curve compactness
              const int npt = 5;
              gp_Pnt    cp((c3d->Value(first).XYZ() + c3d->Value(last).XYZ()) * 0.5);
              double    dist, maxdist = 0.0;
              double    delta = (last - first) / (npt - 1);
              for (int idx = 0; idx < npt; idx++)
              {
                dist = cp.Distance(c3d->Value(first + idx * delta));
                if (maxdist < dist)
                  maxdist = dist;
              }
              isSmall = (2. * maxdist <= MinTolerance());
              /*try {
            GeomAdaptor_Curve cAdapt(c3d);
            double length = GCPnts_AbscissaPoint::Length(cAdapt,first,last);
            isSmall = (length <= MinTolerance());
              }
              catch (Standard_Failure) {
    #ifdef OCCT_DEBUG
            std::cout << "Warning: Possibly small edge can be sewed: ";
            Standard_Failure::Caught()->Print(std::cout); std::cout << std::endl;
    #endif
              }*/
            }

            if (isSmall)
            {

              // Store small edge in the map
              SmallEdges.Add(edge);

              TopoDS_Vertex v1, v2;
              TopExp::Vertices(edge, v1, v2);
              TopoDS_Shape nv1 = myReShape->Apply(v1), nv2 = myReShape->Apply(v2);

              // Store glued vertices
              if (!nv1.IsSame(v1))
              {
                NCollection_List<TopoDS_Shape>& vlist1 = GluedVertices.ChangeFromKey(nv1);
                // First vertex was already glued
                if (!nv2.IsSame(v2))
                {
                  // Merge lists of glued vertices
                  if (!nv1.IsSame(nv2))
                  {
                    NCollection_List<TopoDS_Shape>::Iterator liter(GluedVertices.FindFromKey(nv2));
                    for (; liter.More(); liter.Next())
                    {
                      TopoDS_Shape v = liter.Value();
                      myReShape->Replace(v, nv1.Oriented(v.Orientation()));
                      vlist1.Append(v);
                    }
                    GluedVertices.RemoveKey(nv2);
                  }
                }
                else
                {
                  // Add second vertex to the existing list
                  vlist1.Append(v2);
                  myReShape->Replace(v2, nv1.Oriented(v2.Orientation()));
                }
              }
              else if (!nv2.IsSame(v2))
              {
                // Add first vertex to the existing list
                GluedVertices.ChangeFromKey(nv2).Append(v1);
                myReShape->Replace(v1, nv2.Oriented(v1.Orientation()));
              }
              else if (!v1.IsSame(v2))
              {
                // Record new glued vertices
                TopoDS_Vertex nv;
                B.MakeVertex(nv);
                NCollection_List<TopoDS_Shape> vlist;
                vlist.Append(v1);
                vlist.Append(v2);
                GluedVertices.Add(nv, vlist);
                myReShape->Replace(v1, nv.Oriented(v1.Orientation()));
                myReShape->Replace(v2, nv.Oriented(v2.Orientation()));
              }
            }
          }

          // Replace small edge
          if (isSmall)
          {
#ifdef OCCT_DEBUG
            std::cout << "Warning: Small edge made degenerated by FaceAnalysis" << std::endl;
#endif
            nbSmall++;
            // Create new degenerated edge
            aTmpShape                       = edge.Oriented(TopAbs_FORWARD);
            TopoDS_Edge               fedge = TopoDS::Edge(aTmpShape);
            double                    pfirst, plast;
            occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(fedge, face, pfirst, plast);
            if (!c2d.IsNull())
            {
              TopoDS_Edge nedge;
              B.MakeEdge(nedge);
              B.UpdateEdge(nedge, c2d, face, Precision::Confusion());
              B.Range(nedge, pfirst, plast);
              B.Degenerated(nedge, true);
              TopoDS_Vertex v1, v2;
              TopExp::Vertices(fedge, v1, v2);
              B.Add(nedge, myReShape->Apply(v1).Oriented(v1.Orientation()));
              B.Add(nedge, myReShape->Apply(v2).Oriented(v2.Orientation()));
              B.Add(nwire, nedge.Oriented(edge.Orientation()));
              myDegenerated.Add(nedge);
            }
            isWireChanged = true;
          }
          else
            B.Add(nwire, edge); // Old edge kept
        }

        // Record wire in the new face
        if (isWireChanged)
        {
          B.Add(nface, nwire.Oriented(wire.Orientation()));
          isFaceChanged = true;
        }
        else
          B.Add(nface, wire);
      }

      // Remove small face
      if (nbSmall == nbEdges)
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: Small face removed by FaceAnalysis" << std::endl;
#endif
        myLittleFace.Add(face);
        myReShape->Remove(face);
      }
      else if (isFaceChanged)
      {

        myReShape->Replace(face, nface.Oriented(face.Orientation()));
      }
    }
  }

  // Update glued vertices
  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator aMIter(GluedVertices);
  for (; aMIter.More(); aMIter.Next())
  {
    const TopoDS_Vertex&                     vnew = TopoDS::Vertex(aMIter.Key());
    gp_XYZ                                   coord(0., 0., 0.);
    int                                      nbPoints = 0;
    const NCollection_List<TopoDS_Shape>&    vlist    = aMIter.Value();
    NCollection_List<TopoDS_Shape>::Iterator liter1(vlist);
    for (; liter1.More(); liter1.Next())
    {
      coord += BRep_Tool::Pnt(TopoDS::Vertex(liter1.Value())).XYZ();
      nbPoints++;
    }
    if (nbPoints)
    {
      gp_Pnt                                   vp(coord / nbPoints);
      double                                   tol = 0.0, mtol = 0.0;
      NCollection_List<TopoDS_Shape>::Iterator liter2(vlist);
      for (; liter2.More(); liter2.Next())
      {
        double vtol = BRep_Tool::Tolerance(TopoDS::Vertex(liter2.Value()));
        if (mtol < vtol)
          mtol = vtol;
        vtol = vp.Distance(BRep_Tool::Pnt(TopoDS::Vertex(liter2.Value())));
        if (tol < vtol)
          tol = vtol;
      }
      B.UpdateVertex(vnew, vp, tol + mtol);
    }
  }

  // Update input shapes
  for (i = 1; i <= myOldShapes.Extent(); i++)
    myOldShapes(i) = myReShape->Apply(myOldShapes(i));
}

//=======================================================================
// function : FindFreeBoundaries
// purpose  : Constructs :
//                      myBoundFaces     (bound = list of faces) - REFERENCE
//                      myVertexNode     (vertex = node)
//                      myVertexNodeFree (floating vertex = node)
//
//=======================================================================

void BRepBuilderAPI_Sewing::FindFreeBoundaries()
{
  // Take into account the context shape if needed
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> NewShapes;
  if (!myShape.IsNull())
  {
    if (myOldShapes.IsEmpty())
    {
      Add(myShape);
      myShape.Nullify();
    }
    else
    {
      TopoDS_Shape newShape = myReShape->Apply(myShape);
      if (!newShape.IsNull())
        NewShapes.Add(newShape);
    }
  }
  // Create map Edge -> Faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      EdgeFaces;
  int i, nbShapes = myOldShapes.Extent();
  for (i = 1; i <= nbShapes; i++)
  {
    // Retrieve new shape
    const TopoDS_Shape& shape = myOldShapes(i);
    if (shape.IsNull())
      continue;
    NewShapes.Add(shape);
    // Explore shape to find all boundaries
    for (TopExp_Explorer eExp(shape, TopAbs_EDGE); eExp.More(); eExp.Next())
    {
      const TopoDS_Shape& edge = eExp.Current();
      EdgeFaces.Add(edge, NCollection_List<TopoDS_Shape>());
    }
  }
  // Fill map Edge -> Faces
  nbShapes = NewShapes.Extent();
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapFaces;
  for (i = 1; i <= nbShapes; i++)
  {
    // Explore shape to find all faces
    TopExp_Explorer fExp(NewShapes.FindKey(i), TopAbs_FACE);
    for (; fExp.More(); fExp.Next())
    {
      const TopoDS_Shape& face = fExp.Current();
      if (!mapFaces.Add(face))
        continue;
      // Explore face to find all boundaries
      for (TopoDS_Iterator aIw(face); aIw.More(); aIw.Next())
      {
        if (aIw.Value().ShapeType() != TopAbs_WIRE)
          continue;
        for (TopoDS_Iterator aIIe(aIw.Value()); aIIe.More(); aIIe.Next())
        {

          const TopoDS_Shape& edge = aIIe.Value();

          if (EdgeFaces.Contains(edge))
          {
            EdgeFaces.ChangeFromKey(edge).Append(face);
            // NCollection_List<TopoDS_Shape>& listFaces = EdgeFaces.ChangeFromKey(edge);
            // bool isContained = false;
            // NCollection_List<TopoDS_Shape>::Iterator itf(listFaces);
            // for (; itf.More() && !isContained; itf.Next())
            //   isContained = face.IsSame(itf.Value());
            // if (!isContained) listFaces.Append(face);
          }
        }
      }
    }
  }
  // Find free boundaries
  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator anIterEF(EdgeFaces);
  for (; anIterEF.More(); anIterEF.Next())
  {
    NCollection_List<TopoDS_Shape>& listFaces = anIterEF.ChangeValue();
    int                             nbFaces   = listFaces.Extent();
    TopoDS_Shape                    edge      = anIterEF.Key();
    if (edge.Orientation() == TopAbs_INTERNAL)
      continue;
    bool isSeam = false;
    if (nbFaces == 1)
    {
      TopoDS_Face face = TopoDS::Face(listFaces.First());
      isSeam           = BRep_Tool::IsClosed(TopoDS::Edge(edge), face);
      if (isSeam)
      {
        /// occ::handle<Geom_Surface> surf = BRep_Tool::Surface(face);
        // isSeam = (IsUClosedSurface(surf) || IsVClosedSurface(surf));
        // if(!isSeam) {
        BRep_Builder    aB;
        TopoDS_Shape    anewEdge = edge.EmptyCopied();
        TopoDS_Iterator aItV(edge);
        for (; aItV.More(); aItV.Next())
          aB.Add(anewEdge, aItV.Value());

        double                    first2d, last2d;
        occ::handle<Geom2d_Curve> c2dold =
          BRep_Tool::CurveOnSurface(TopoDS::Edge(edge),
                                    TopoDS::Face(listFaces.First()),
                                    first2d,
                                    last2d);

        occ::handle<Geom2d_Curve> c2d;
        BRep_Builder              B;
        B.UpdateEdge(TopoDS::Edge(anewEdge), c2d, c2d, TopoDS::Face(listFaces.First()), 0);
        B.UpdateEdge(TopoDS::Edge(anewEdge), c2dold, TopoDS::Face(listFaces.First()), 0);

        double aFirst, aLast;
        BRep_Tool::Range(TopoDS::Edge(edge), aFirst, aLast);
        aB.Range(TopoDS::Edge(anewEdge), aFirst, aLast);
        aB.Range(TopoDS::Edge(anewEdge), TopoDS::Face(listFaces.First()), first2d, last2d);
        myReShape->Replace(edge, anewEdge);
        edge = anewEdge;

        //}
        isSeam = false;
      }
    }
    bool isBoundFloat = (myFloatingEdgesMode && !nbFaces);
    bool isBound      = (myFaceMode && ((myNonmanifold && nbFaces) || (nbFaces == 1 && !isSeam)));
    if (isBound || isBoundFloat)
    {
      // Ignore degenerated edge
      if (BRep_Tool::Degenerated(TopoDS::Edge(edge)))
        continue;
      // Ignore edge with internal vertices
      // int nbVtx = 0;
      // for (TopExp_Explorer vExp(edge,TopAbs_VERTEX); vExp.More(); vExp.Next()) nbVtx++;
      // if (nbVtx != 2) continue;
      // Add to BoundFaces
      NCollection_List<TopoDS_Shape> listFacesCopy;
      listFacesCopy.Append(listFaces);
      myBoundFaces.Add(edge, listFacesCopy);
      // Process edge vertices
      TopoDS_Vertex vFirst, vLast;
      TopExp::Vertices(TopoDS::Edge(edge), vFirst, vLast);
      if (vFirst.IsNull() || vLast.IsNull())
        continue;
      if (vFirst.Orientation() == TopAbs_INTERNAL || vLast.Orientation() == TopAbs_INTERNAL)
        continue;
      if (isBound)
      {
        // Add to VertexNode
        myVertexNode.Add(vFirst, vFirst);
        myVertexNode.Add(vLast, vLast);
      }
      else
      {
        // Add to VertexNodeFree
        myVertexNodeFree.Add(vFirst, vFirst);
        myVertexNodeFree.Add(vLast, vLast);
      }
    }
  }
}

//=======================================================================
// function : VerticesAssembling
// purpose  : Modifies :
//                      myVertexNode     (nodes glued)
//                      myVertexNodeFree (nodes glued)
//                      myNodeSections   (lists of sections merged for glued nodes)
//
//=======================================================================

static bool CreateNewNodes(
  const NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&
                                                                                   NodeNearestNode,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                       NodeVertices,
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& aVertexNode,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    aNodeEdges)
{
  // Create new nodes
  BRep_Builder                                                             B;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> OldNodeNewNode;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    NewNodeOldNodes;
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator anIter(
    NodeNearestNode);
  for (; anIter.More(); anIter.Next())
  {
    // Retrieve a pair of nodes to merge
    const TopoDS_Shape& oldnode1 = anIter.Key();
    const TopoDS_Shape& oldnode2 = anIter.Value();
    // Second node should also be in the map
    if (!NodeNearestNode.Contains(oldnode2))
      continue;
    // Get new node for old node #1
    if (OldNodeNewNode.IsBound(oldnode1))
    {
      const TopoDS_Shape& newnode1 = OldNodeNewNode(oldnode1);
      if (OldNodeNewNode.IsBound(oldnode2))
      {
        TopoDS_Shape newnode2 = OldNodeNewNode(oldnode2);
        if (!newnode1.IsSame(newnode2))
        {
          // Change data for new node #2
          NCollection_List<TopoDS_Shape>&          lnode1 = NewNodeOldNodes.ChangeFromKey(newnode1);
          NCollection_List<TopoDS_Shape>::Iterator itn(NewNodeOldNodes.FindFromKey(newnode2));
          for (; itn.More(); itn.Next())
          {
            const TopoDS_Shape& node2 = itn.Value();
            lnode1.Append(node2);
            OldNodeNewNode(node2) = newnode1;
          }
          NewNodeOldNodes.RemoveKey(newnode2);
        }
      }
      else
      {
        // Old node #2 is not bound - add to old node #1
        OldNodeNewNode.Bind(oldnode2, newnode1);
        NewNodeOldNodes.ChangeFromKey(newnode1).Append(oldnode2);
      }
    }
    else
    {
      if (OldNodeNewNode.IsBound(oldnode2))
      {
        // Old node #1 is not bound - add to old node #2
        const TopoDS_Shape& newnode2 = OldNodeNewNode(oldnode2);
        OldNodeNewNode.Bind(oldnode1, newnode2);
        NewNodeOldNodes.ChangeFromKey(newnode2).Append(oldnode1);
      }
      else
      {
        // Nodes are not bound - create new node
        TopoDS_Vertex newnode;
        B.MakeVertex(newnode);
        OldNodeNewNode.Bind(oldnode1, newnode);
        OldNodeNewNode.Bind(oldnode2, newnode);
        NCollection_List<TopoDS_Shape> lnodes;
        lnodes.Append(oldnode1);
        lnodes.Append(oldnode2);
        NewNodeOldNodes.Add(newnode, lnodes);
      }
    }
  }

  // Stop if no new nodes created
  if (!NewNodeOldNodes.Extent())
    return false;

  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator anIter1(NewNodeOldNodes);
  for (; anIter1.More(); anIter1.Next())
  {
    const TopoDS_Vertex& newnode = TopoDS::Vertex(anIter1.Key());
    // Calculate new node center point
    gp_XYZ                                                 theCoordinates(0., 0., 0.);
    NCollection_List<TopoDS_Shape>                         lvert; // Accumulate node vertices
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> medge;
    NCollection_List<TopoDS_Shape>                         ledge; // Accumulate node edges
    // Iterate on old nodes
    NCollection_List<TopoDS_Shape>::Iterator itn(anIter1.Value());
    for (; itn.More(); itn.Next())
    {
      const TopoDS_Shape& oldnode = itn.Value();
      // Iterate on node vertices
      NCollection_List<TopoDS_Shape>::Iterator itv(NodeVertices.FindFromKey(oldnode));
      for (; itv.More(); itv.Next())
      {
        const TopoDS_Vertex& vertex = TopoDS::Vertex(itv.Value());
        // Change node for vertex
        aVertexNode.ChangeFromKey(vertex) = newnode;
        // Accumulate coordinates
        theCoordinates += BRep_Tool::Pnt(vertex).XYZ();
        lvert.Append(vertex);
      }
      // Iterate on node edges
      const NCollection_List<TopoDS_Shape>&    edges = aNodeEdges(oldnode);
      NCollection_List<TopoDS_Shape>::Iterator ite(edges);
      for (; ite.More(); ite.Next())
      {
        const TopoDS_Shape& edge = ite.Value();
        if (medge.Add(edge))
        {
          ledge.Append(edge);
        }
      }
      // Unbind old node edges
      aNodeEdges.UnBind(oldnode);
    }
    // Bind new node edges
    aNodeEdges.Bind(newnode, ledge);
    gp_Pnt center(theCoordinates / lvert.Extent());
    // Calculate new node tolerance
    double                                   toler = 0.0;
    NCollection_List<TopoDS_Shape>::Iterator itv(lvert);
    for (; itv.More(); itv.Next())
    {
      const TopoDS_Vertex& vertex = TopoDS::Vertex(itv.Value());
      double t = center.Distance(BRep_Tool::Pnt(vertex)) + BRep_Tool::Tolerance(vertex);
      if (toler < t)
        toler = t;
    }
    // Update new node parameters
    B.UpdateVertex(newnode, center, toler);
  }

  return true;
}

static int IsMergedVertices(const TopoDS_Shape& face1,
                            const TopoDS_Shape& e1,
                            const TopoDS_Shape& e2,
                            const TopoDS_Shape& vtx1,
                            const TopoDS_Shape& vtx2)
{
  // Case of floating edges
  if (face1.IsNull())
    return (!IsClosedShape(e1, vtx1, vtx2));

  // Find wires containing given edges
  TopoDS_Shape    wire1, wire2;
  TopExp_Explorer itw(face1, TopAbs_WIRE);
  for (; itw.More() && (wire1.IsNull() || wire2.IsNull()); itw.Next())
  {
    TopoDS_Iterator ite(itw.Current(), false);
    for (; ite.More() && (wire1.IsNull() || wire2.IsNull()); ite.Next())
    {
      if (wire1.IsNull() && e1.IsSame(ite.Value()))
        wire1 = itw.Current();
      if (wire2.IsNull() && e2.IsSame(ite.Value()))
        wire2 = itw.Current();
    }
  }
  int Status = 0;
  if (!wire1.IsNull() && !wire2.IsNull())
  {
    if (wire1.IsSame(wire2))
    {
      for (TopoDS_Iterator aIte(wire1, false); aIte.More(); aIte.Next())
      {
        TopoDS_Vertex ve1, ve2;
        TopExp::Vertices(TopoDS::Edge(aIte.Value()), ve1, ve2);
        if ((ve1.IsSame(vtx1) && ve2.IsSame(vtx2)) || (ve2.IsSame(vtx1) && ve1.IsSame(vtx2)))
          return (IsClosedShape(aIte.Value(), vtx1, vtx2) ? 0 : 1);
      }
      if (IsClosedShape(wire1, vtx1, vtx2))
      {
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(TopoDS::Wire(wire1), V1, V2);
        bool isEndVertex =
          ((V1.IsSame(vtx1) && V2.IsSame(vtx2)) || (V2.IsSame(vtx1) && V1.IsSame(vtx2)));
        if (!isEndVertex)
          Status = 1;
      }
      else
        Status = 1;
    }
    else
      Status = -1;
  }
  return Status;
}

static bool GlueVertices(
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& aVertexNode,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                             aNodeEdges,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& aBoundFaces,
  const double                                               Tolerance,
  const Message_ProgressRange&                               theProgress)
{
  // Create map of node -> vertices
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                         NodeVertices;
  NCollection_CellFilter<BRepBuilderAPI_VertexInspector> aFilter(Tolerance);
  BRepBuilderAPI_VertexInspector                         anInspector(Tolerance);
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator anIter1(
    aVertexNode);
  for (; anIter1.More(); anIter1.Next())
  {
    const TopoDS_Shape&  vertex = anIter1.Key();
    const TopoDS_Vertex& node   = TopoDS::Vertex(anIter1.Value());
    if (NodeVertices.Contains(node))
    {
      NodeVertices.ChangeFromKey(node).Append(vertex);
    }
    else
    {
      NCollection_List<TopoDS_Shape> vlist;
      vlist.Append(vertex);
      NodeVertices.Add(node, vlist);
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(node));
      aFilter.Add(NodeVertices.FindIndex(node), aPnt.XYZ());
      anInspector.Add(aPnt.XYZ());
    }
  }
  int nbNodes = NodeVertices.Extent();
#ifdef OCCT_DEBUG
  std::cout << "Glueing " << nbNodes << " nodes..." << std::endl;
#endif
  // Merge nearest nodes
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> NodeNearestNode;
  Message_ProgressScope aPS(theProgress, "Glueing nodes", nbNodes, true);
  for (int i = 1; i <= nbNodes && aPS.More(); i++, aPS.Next())
  {
    const TopoDS_Vertex& node1 = TopoDS::Vertex(NodeVertices.FindKey(i));
    // Find near nodes
    gp_Pnt pt1 = BRep_Tool::Pnt(node1);
    anInspector.SetCurrent(pt1.XYZ());
    gp_XYZ aPntMin = anInspector.Shift(pt1.XYZ(), -Tolerance);
    gp_XYZ aPntMax = anInspector.Shift(pt1.XYZ(), Tolerance);
    aFilter.Inspect(aPntMin, aPntMax, anInspector);
    if (anInspector.ResInd().IsEmpty())
      continue;
    // Retrieve list of edges for the first node
    const NCollection_List<TopoDS_Shape>& ledges1 = aNodeEdges(node1);
    // Explore list of near nodes and fill the sequence of glued nodes
    NCollection_Sequence<TopoDS_Shape> SeqNodes;
    NCollection_List<TopoDS_Shape>     listNodesSameEdge;
    // gp_Pnt pt1 = BRep_Tool::Pnt(node1);
    NCollection_List<int>::Iterator iter1(anInspector.ResInd());
    for (; iter1.More(); iter1.Next())
    {
      const TopoDS_Vertex& node2 = TopoDS::Vertex(NodeVertices.FindKey(iter1.Value()));
      if (node1 == node2)
        continue;
      // Retrieve list of edges for the second node
      const NCollection_List<TopoDS_Shape>& ledges2 = aNodeEdges(node2);
      // Check merging condition for the pair of nodes
      int Status = 0, isSameEdge = false;
      // Explore edges of the first node
      NCollection_List<TopoDS_Shape>::Iterator Ie1(ledges1);
      for (; Ie1.More() && !Status && !isSameEdge; Ie1.Next())
      {
        const TopoDS_Shape& e1 = Ie1.Value();
        // Obtain real vertex from edge
        TopoDS_Shape v1 = node1;
        { // szv: Use brackets to destroy local variables
          TopoDS_Vertex ov1, ov2;
          TopExp::Vertices(TopoDS::Edge(e1), ov1, ov2);
          if (const TopoDS_Shape* pOv1 = aVertexNode.Seek(ov1))
          {
            if (node1.IsSame(*pOv1))
              v1 = ov1;
          }
          if (const TopoDS_Shape* pOv2 = aVertexNode.Seek(ov2))
          {
            if (node1.IsSame(*pOv2))
              v1 = ov2;
          }
        }
        // Create map of faces for e1
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Faces1;
        const NCollection_List<TopoDS_Shape>&                  lfac1 = aBoundFaces.FindFromKey(e1);
        if (lfac1.Extent())
        {
          NCollection_List<TopoDS_Shape>::Iterator itf(lfac1);
          for (; itf.More(); itf.Next())
            if (!itf.Value().IsNull())
              Faces1.Add(itf.Value());
        }
        // Explore edges of the second node
        NCollection_List<TopoDS_Shape>::Iterator Ie2(ledges2);
        for (; Ie2.More() && !Status && !isSameEdge; Ie2.Next())
        {
          const TopoDS_Shape& e2 = Ie2.Value();
          // Obtain real vertex from edge
          TopoDS_Shape v2 = node2;
          { // szv: Use brackets to destroy local variables
            TopoDS_Vertex ov1, ov2;
            TopExp::Vertices(TopoDS::Edge(e2), ov1, ov2);
            if (const TopoDS_Shape* pOv1 = aVertexNode.Seek(ov1))
            {
              if (node2.IsSame(*pOv1))
                v2 = ov1;
            }
            if (const TopoDS_Shape* pOv2 = aVertexNode.Seek(ov2))
            {
              if (node2.IsSame(*pOv2))
                v2 = ov2;
            }
          }
          // Explore faces for e2
          const NCollection_List<TopoDS_Shape>& lfac2 = aBoundFaces.FindFromKey(e2);
          if (lfac2.Extent())
          {
            NCollection_List<TopoDS_Shape>::Iterator itf(lfac2);
            for (; itf.More() && !Status && !isSameEdge; itf.Next())
            {
              // Check merging conditions for the same face
              if (Faces1.Contains(itf.Value()))
              {
                int stat = IsMergedVertices(itf.Value(), e1, e2, v1, v2);
                if (stat == 1)
                  isSameEdge = true;
                else
                  Status = stat;
              }
            }
          }
          else if (Faces1.IsEmpty() && e1 == e2)
          {
            int stat = IsMergedVertices(TopoDS_Face(), e1, e1, v1, v2);
            if (stat == 1)
              isSameEdge = true;
            else
              Status = stat;
            break;
          }
        }
      }
      if (Status)
        continue;
      if (isSameEdge)
        listNodesSameEdge.Append(node2);
      // Append near node to the sequence
      gp_Pnt pt2  = BRep_Tool::Pnt(node2);
      double dist = pt1.Distance(pt2);
      if (dist < Tolerance)
      {
        bool isIns = false;
        for (int kk = 1; kk <= SeqNodes.Length() && !isIns; kk++)
        {
          gp_Pnt pt = BRep_Tool::Pnt(TopoDS::Vertex(SeqNodes.Value(kk)));
          if (dist < pt1.Distance(pt))
          {
            SeqNodes.InsertBefore(kk, node2);
            isIns = true;
          }
        }
        if (!isIns)
          SeqNodes.Append(node2);
      }
    }
    if (SeqNodes.Length())
    {
      // Remove nodes near to some other from the same edge
      if (listNodesSameEdge.Extent())
      {
        NCollection_List<TopoDS_Shape>::Iterator lInt(listNodesSameEdge);
        for (; lInt.More(); lInt.Next())
        {
          const TopoDS_Vertex& n2 = TopoDS::Vertex(lInt.Value());
          gp_Pnt               p2 = BRep_Tool::Pnt(n2);
          for (int k = 1; k <= SeqNodes.Length();)
          {
            const TopoDS_Vertex& n1 = TopoDS::Vertex(SeqNodes.Value(k));
            if (n1 != n2)
            {
              gp_Pnt p1 = BRep_Tool::Pnt(n1);
              if (p2.Distance(p1) >= pt1.Distance(p1))
              {
                k++;
                continue;
              }
            }
            SeqNodes.Remove(k);
          }
        }
      }
      // Bind nearest node if at least one exists
      if (SeqNodes.Length())
        NodeNearestNode.Add(node1, SeqNodes.First());
    }
    anInspector.ClearResList();
  }

  // Create new nodes for chained nearest nodes
  if (NodeNearestNode.IsEmpty())
    return false;

  return CreateNewNodes(NodeNearestNode, NodeVertices, aVertexNode, aNodeEdges);
}

void BRepBuilderAPI_Sewing::VerticesAssembling(const Message_ProgressRange& theProgress)
{
  int                   nbVert     = myVertexNode.Extent();
  int                   nbVertFree = myVertexNodeFree.Extent();
  Message_ProgressScope aPS(theProgress, "Vertices assembling", 2);
  if (nbVert || nbVertFree)
  {
    // Fill map node -> sections
    int i;
    for (i = 1; i <= myBoundFaces.Extent(); i++)
    {
      TopoDS_Shape bound = myBoundFaces.FindKey(i);
      for (TopoDS_Iterator itv(bound, false); itv.More(); itv.Next())
      {
        const TopoDS_Shape& node = itv.Value();
        myNodeSections.TryBound(node, NCollection_List<TopoDS_Shape>())->Append(bound);
      }
    }
    // Glue vertices
    if (nbVert)
    {
#ifdef OCCT_DEBUG
      std::cout << "Assemble " << nbVert << " vertices on faces..." << std::endl;
#endif
      while (GlueVertices(myVertexNode, myNodeSections, myBoundFaces, myTolerance, aPS.Next()))
        ;
    }
    if (!aPS.More())
      return;
    if (nbVertFree)
    {
#ifdef OCCT_DEBUG
      std::cout << "Assemble " << nbVertFree << " vertices on floating edges..." << std::endl;
#endif
      while (GlueVertices(myVertexNodeFree, myNodeSections, myBoundFaces, myTolerance, aPS.Next()))
        ;
    }
  }
}

//=======================================================================
// function : replaceNMVertices
// purpose  : internal use (static)
//=======================================================================
static void replaceNMVertices(const TopoDS_Edge&                    theEdge,
                              const TopoDS_Vertex&                  theV1,
                              const TopoDS_Vertex&                  theV2,
                              const occ::handle<BRepTools_ReShape>& theReShape)
{
  // To keep NM vertices on edge
  NCollection_Sequence<TopoDS_Shape> aSeqNMVert;
  NCollection_Sequence<double>       aSeqNMPars;
  bool                               hasNMVert = findNMVertices(theEdge, aSeqNMVert, aSeqNMPars);
  if (!hasNMVert)
    return;
  double first, last;
  BRep_Tool::Range(theEdge, first, last);
  TopLoc_Location         aLoc;
  occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(theEdge, aLoc, first, last);
  if (c3d.IsNull())
    return;
  NCollection_Sequence<TopoDS_Shape> aEdVert;
  NCollection_Sequence<double>       aEdParams;
  int                                i = 1, nb = aSeqNMPars.Length();

  for (; i <= nb; i++)
  {
    double apar = aSeqNMPars.Value(i);
    if (fabs(apar - first) <= Precision::PConfusion())
    {
      theReShape->Replace(aSeqNMVert.Value(i), theV1);
      continue;
    }
    if (fabs(apar - last) <= Precision::PConfusion())
    {
      theReShape->Replace(aSeqNMVert.Value(i), theV2);
      continue;
    }
    const TopoDS_Shape& aV = aSeqNMVert.Value(i);
    int                 j  = 1;
    for (; j <= aEdParams.Length(); j++)
    {
      double apar2 = aEdParams.Value(j);
      if (fabs(apar - apar2) <= Precision::PConfusion())
      {
        theReShape->Replace(aV, aEdVert.Value(j));
        break;
      }
      else if (apar < apar2)
      {
        TopoDS_Shape anewV = aV.EmptyCopied();
        aEdVert.InsertBefore(j, anewV);
        aEdParams.InsertBefore(j, apar);
        NCollection_List<occ::handle<BRep_PointRepresentation>>& alistrep =
          (*((occ::handle<BRep_TVertex>*)&anewV.TShape()))->ChangePoints();
        occ::handle<BRep_PointOnCurve> aPRep = new BRep_PointOnCurve(apar, c3d, aLoc);
        alistrep.Append(aPRep);
        theReShape->Replace(aV, anewV);
        break;
      }
    }
    if (j > aEdParams.Length())
    {
      TopoDS_Shape anewV = aV.EmptyCopied();
      aEdVert.Append(anewV);
      aEdParams.Append(apar);
      NCollection_List<occ::handle<BRep_PointRepresentation>>& alistrep =
        (*((occ::handle<BRep_TVertex>*)&anewV.TShape()))->ChangePoints();
      occ::handle<BRep_PointOnCurve> aPRep = new BRep_PointOnCurve(apar, c3d, aLoc);
      alistrep.Append(aPRep);
      theReShape->Replace(aV, anewV);
    }
  }

  int newnb = aEdParams.Length();
  if (newnb < nb)
  {

    TopoDS_Shape       anewEdge = theEdge.EmptyCopied();
    TopAbs_Orientation anOri    = theEdge.Orientation();
    anewEdge.Orientation(TopAbs_FORWARD);
    BRep_Builder aB;
    aB.Add(anewEdge, theV1);
    aB.Add(anewEdge, theV2);

    for (i = 1; i <= aEdVert.Length(); i++)
      aB.Add(anewEdge, aEdVert.Value(i));
    anewEdge.Orientation(anOri);
    theReShape->Replace(theEdge, anewEdge);
  }
}

//=======================================================================
// function : ReplaceEdge
// purpose  : internal use (static)
//=======================================================================

static void ReplaceEdge(const TopoDS_Shape&                   oldEdge,
                        const TopoDS_Shape&                   theNewShape,
                        const occ::handle<BRepTools_ReShape>& aReShape)
{
  TopoDS_Shape oldShape = aReShape->Apply(oldEdge);
  TopoDS_Shape newShape = aReShape->Apply(theNewShape);
  if (oldShape.IsSame(newShape) || aReShape->IsRecorded(newShape))
    return;

  aReShape->Replace(oldShape, newShape);
  TopoDS_Vertex V1old, V2old, V1new, V2new;
  TopExp::Vertices(TopoDS::Edge(oldShape), V1old, V2old);
  TopAbs_Orientation Orold = oldShape.Orientation();
  TopAbs_Orientation Ornew = Orold;
  if (newShape.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Edge aEn = TopoDS::Edge(newShape);
    TopExp::Vertices(aEn, V1new, V2new);
    Ornew = aEn.Orientation();
    replaceNMVertices(aEn, V1new, V2new, aReShape);
  }
  else if (newShape.ShapeType() == TopAbs_WIRE)
  {
    for (TopExp_Explorer aex(newShape, TopAbs_EDGE); aex.More(); aex.Next())
    {
      TopoDS_Edge ed = TopoDS::Edge(aex.Current());
      Ornew          = ed.Orientation();
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(ed, aV1, aV2);
      replaceNMVertices(ed, aV1, aV2, aReShape);
      if (V1new.IsNull())
        V1new = aV1;
      V2new = aV2;
    }
  }

  V1new.Orientation(V1old.Orientation());
  V2new.Orientation(V2old.Orientation());
  if (V1old.IsSame(V2old) && !V1old.IsSame(V1new) && !aReShape->IsRecorded(V1new))
  {
    aReShape->Replace(V1old, V1new);
    return;
  }
  if (Orold == Ornew)
  {
    V1new.Orientation(V1old.Orientation());
    V2new.Orientation(V2old.Orientation());
    if (!V1old.IsSame(V1new) && !V1old.IsSame(V2new) && !aReShape->IsRecorded(V1new))
      aReShape->Replace(V1old, V1new);
    if (!V2old.IsSame(V2new) && !V2old.IsSame(V1new) && !aReShape->IsRecorded(V2new))
      aReShape->Replace(V2old, V2new);
  }
  else
  {
    V1new.Orientation(V2old.Orientation());
    V2new.Orientation(V1old.Orientation());
    if (!V1old.IsSame(V2new) && !V1old.IsSame(V1new) && !aReShape->IsRecorded(V2new))
      aReShape->Replace(V1old, V2new);
    if (!V2old.IsSame(V2new) && !V2old.IsSame(V1new) && !aReShape->IsRecorded(V1new))
      aReShape->Replace(V2old, V1new);
  }
}

//=======================================================================
// function : Merging
// purpose  : Modifies :
//                   myHasFreeBound
//
//=======================================================================

void BRepBuilderAPI_Sewing::Merging(const bool /* firstTime */,
                                    const Message_ProgressRange& theProgress)
{
  BRep_Builder B;
  //  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MergedEdges;
  Message_ProgressScope aPS(theProgress, "Merging bounds", myBoundFaces.Extent());
  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator anIterB(myBoundFaces);
  for (; anIterB.More() && aPS.More(); anIterB.Next(), aPS.Next())
  {

    const TopoDS_Shape& bound = anIterB.Key();

    // If bound was already merged - continue
    if (myMergedEdges.Contains(bound))
      continue;

    if (!anIterB.Value().Extent())
    {
      // Merge free edge - only vertices
      TopoDS_Vertex no1, no2;
      TopExp::Vertices(TopoDS::Edge(bound), no1, no2);
      TopoDS_Shape nno1 = no1, nno2 = no2;
      if (myVertexNodeFree.Contains(no1))
        nno1 = myVertexNodeFree.FindFromKey(no1);
      if (myVertexNodeFree.Contains(no2))
        nno2 = myVertexNodeFree.FindFromKey(no2);
      if (!no1.IsSame(nno1))
      {
        nno1.Orientation(no1.Orientation());
        myReShape->Replace(no1, nno1);
      }
      if (!no2.IsSame(nno2))
      {
        nno2.Orientation(no2.Orientation());
        myReShape->Replace(no2, nno2);
      }
      myMergedEdges.Add(bound);
      continue;
    }

    // Check for previous splitting, build replacing wire
    TopoDS_Wire BoundWire;
    bool        isPrevSplit        = false;
    bool        hasCuttingSections = myBoundSections.IsBound(bound);
    if (hasCuttingSections)
    {
      B.MakeWire(BoundWire);
      BoundWire.Orientation(bound.Orientation());
      // Iterate on cutting sections
      NCollection_List<TopoDS_Shape>::Iterator its(myBoundSections(bound));
      for (; its.More(); its.Next())
      {
        TopoDS_Shape section = its.Value();
        B.Add(BoundWire, section);
        if (myMergedEdges.Contains(section))
          isPrevSplit = true;
      }
    }

    // Merge with bound
    NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> MergedWithBound;
    if (!isPrevSplit)
    {
      // Obtain sequence of edges merged with bound
      NCollection_Sequence<TopoDS_Shape> seqMergedWithBound;
      NCollection_Sequence<bool>         seqMergedWithBoundOri;
      if (MergedNearestEdges(bound, seqMergedWithBound, seqMergedWithBoundOri))
      {
        // Store bound in the map
        MergedWithBound.Add(bound, bound);
        // Iterate on edges merged with bound
        int ii = 1;
        while (ii <= seqMergedWithBound.Length())
        {
          TopoDS_Shape iedge = seqMergedWithBound.Value(ii);
          // Remove edge if recorded as merged
          bool isRejected = (myMergedEdges.Contains(iedge) || MergedWithBound.Contains(iedge));
          if (!isRejected)
          {
            if (myBoundSections.IsBound(iedge))
            {
              // Edge is split - check sections
              NCollection_List<TopoDS_Shape>::Iterator lit(myBoundSections(iedge));
              for (; lit.More() && !isRejected; lit.Next())
              {
                const TopoDS_Shape& sec = lit.Value();
                // Remove edge (bound) if at least one of its sections already merged
                isRejected = (myMergedEdges.Contains(sec) || MergedWithBound.Contains(sec));
              }
            }
            if (!isRejected)
            {
              if (mySectionBound.IsBound(iedge))
              {
                // Edge is a section - check bound
                const TopoDS_Shape& bnd = mySectionBound(iedge);
                // Remove edge (section) if its bound already merged
                isRejected = (myMergedEdges.Contains(bnd) || MergedWithBound.Contains(bnd));
              }
            }
          }
          // To the next merged edge
          if (isRejected)
          {
            // Remove rejected edge
            seqMergedWithBound.Remove(ii);
            seqMergedWithBoundOri.Remove(ii);
          }
          else
          {
            // Process accepted edge
            MergedWithBound.Add(iedge, iedge);
            ii++;
          }
        }
        int nbMerged = seqMergedWithBound.Length();
        if (nbMerged)
        {
          // Create same parameter edge
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> ActuallyMerged;
          TopoDS_Edge MergedEdge = SameParameterEdge(bound,
                                                     seqMergedWithBound,
                                                     seqMergedWithBoundOri,
                                                     ActuallyMerged,
                                                     myReShape);
          bool        isForward  = false;
          if (!MergedEdge.IsNull())
            isForward = (MergedEdge.Orientation() == TopAbs_FORWARD);
          // Process actually merged edges
          int nbActuallyMerged = 0;
          for (ii = 1; ii <= nbMerged; ii++)
          {
            const TopoDS_Shape& iedge = seqMergedWithBound(ii);
            if (ActuallyMerged.Contains(iedge))
            {
              nbActuallyMerged++;
              // Record merged edge in the map
              TopAbs_Orientation orient = iedge.Orientation();
              if (!isForward)
                orient = TopAbs::Reverse(orient);
              if (!seqMergedWithBoundOri(ii))
                orient = TopAbs::Reverse(orient);
              MergedWithBound.ChangeFromKey(iedge) = MergedEdge.Oriented(orient);
            }
            else
              MergedWithBound.RemoveKey(iedge);
          }
          if (nbActuallyMerged)
          {
            // Record merged bound in the map
            TopAbs_Orientation orient = bound.Orientation();
            if (!isForward)
              orient = TopAbs::Reverse(orient);
            MergedWithBound.ChangeFromKey(bound) = MergedEdge.Oriented(orient);
          }
          nbMerged = nbActuallyMerged;
        }
        // Remove bound from the map if not finally merged
        if (!nbMerged)
          MergedWithBound.RemoveKey(bound);
      }
    }
    const bool isMerged = !MergedWithBound.IsEmpty();

    // Merge with cutting sections
    occ::handle<BRepTools_ReShape> SectionsReShape = new BRepTools_ReShape;
    NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
      MergedWithSections;
    if (hasCuttingSections)
    {
      // Iterate on cutting sections
      NCollection_List<TopoDS_Shape>::Iterator its(myBoundSections(bound));
      for (; its.More(); its.Next())
      {
        // Retrieve cutting section
        TopoDS_Shape section = its.Value();
        // Skip section if already merged
        if (myMergedEdges.Contains(section))
          continue;
        // Merge cutting section
        NCollection_Sequence<TopoDS_Shape> seqMergedWithSection;
        NCollection_Sequence<bool>         seqMergedWithSectionOri;
        if (MergedNearestEdges(section, seqMergedWithSection, seqMergedWithSectionOri))
        {
          // Store section in the map
          MergedWithSections.Add(section, section);
          // Iterate on edges merged with section
          int ii = 1;
          while (ii <= seqMergedWithSection.Length())
          {
            TopoDS_Shape iedge = seqMergedWithSection.Value(ii);
            // Remove edge if recorded as merged
            bool isRejected = (myMergedEdges.Contains(iedge) || MergedWithSections.Contains(iedge));
            if (!isRejected)
            {
              if (myBoundSections.IsBound(iedge))
              {
                // Edge is split - check sections
                NCollection_List<TopoDS_Shape>::Iterator lit(myBoundSections(iedge));
                for (; lit.More() && !isRejected; lit.Next())
                {
                  const TopoDS_Shape& sec = lit.Value();
                  // Remove edge (bound) if at least one of its sections already merged
                  isRejected = (myMergedEdges.Contains(sec) || MergedWithSections.Contains(sec));
                }
              }
              if (!isRejected)
              {
                if (mySectionBound.IsBound(iedge))
                {
                  // Edge is a section - check bound
                  const TopoDS_Shape& bnd = mySectionBound(iedge);
                  // Remove edge (section) if its bound already merged
                  isRejected = (myMergedEdges.Contains(bnd) || MergedWithSections.Contains(bnd));
                }
              }
            }
            // To the next merged edge
            if (isRejected)
            {
              // Remove rejected edge
              seqMergedWithSection.Remove(ii);
              seqMergedWithSectionOri.Remove(ii);
            }
            else
            {
              // Process accepted edge
              MergedWithSections.Add(iedge, iedge);
              ii++;
            }
          }
          int nbMerged = seqMergedWithSection.Length();
          if (nbMerged)
          {
            // Create same parameter edge
            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> ActuallyMerged;
            TopoDS_Edge MergedEdge = SameParameterEdge(section,
                                                       seqMergedWithSection,
                                                       seqMergedWithSectionOri,
                                                       ActuallyMerged,
                                                       SectionsReShape);
            bool        isForward  = false;
            if (!MergedEdge.IsNull())
              isForward = (MergedEdge.Orientation() == TopAbs_FORWARD);
            // Process actually merged edges
            int nbActuallyMerged = 0;
            for (ii = 1; ii <= nbMerged; ii++)
            {
              const TopoDS_Shape& iedge = seqMergedWithSection(ii);
              if (ActuallyMerged.Contains(iedge))
              {
                nbActuallyMerged++;
                // Record merged edge in the map
                TopAbs_Orientation orient = iedge.Orientation();
                if (!isForward)
                  orient = TopAbs::Reverse(orient);
                if (!seqMergedWithSectionOri(ii))
                  orient = TopAbs::Reverse(orient);
                const TopoDS_Shape& oedge               = MergedEdge.Oriented(orient);
                MergedWithSections.ChangeFromKey(iedge) = oedge;
                ReplaceEdge(myReShape->Apply(iedge), oedge, SectionsReShape);
              }
              else
                MergedWithSections.RemoveKey(iedge);
            }
            if (nbActuallyMerged)
            {
              // Record merged section in the map
              TopAbs_Orientation orient = section.Orientation();
              if (!isForward)
                orient = TopAbs::Reverse(orient);
              const TopoDS_Shape& oedge                 = MergedEdge.Oriented(orient);
              MergedWithSections.ChangeFromKey(section) = oedge;
              ReplaceEdge(myReShape->Apply(section), oedge, SectionsReShape);
            }
            nbMerged = nbActuallyMerged;
          }
          // Remove section from the map if not finally merged
          if (!nbMerged)
            MergedWithSections.RemoveKey(section);
        }
        else if (isMerged)
        {
          // Reject merging of sections
          MergedWithSections.Clear();
          break;
        }
      }
    }
    const bool isMergedSplit = !MergedWithSections.IsEmpty();

    if (!isMerged && !isMergedSplit)
    {
      // Nothing was merged in this iteration
      if (isPrevSplit)
      {
        // Replace previously split bound
        myReShape->Replace(myReShape->Apply(bound), myReShape->Apply(BoundWire));
      }
      //      else if (hasCuttingSections) {
      //	myBoundSections.UnBind(bound); //szv: are you sure ???
      //      }
      continue;
    }

    // Set splitting flag
    bool isSplitted = ((!isMerged && isMergedSplit) || isPrevSplit);

    // Choose between bound and sections merging
    if (isMerged && isMergedSplit && !isPrevSplit)
    {
      // Fill map of merged cutting sections
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapSplitEdges;
      NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator
        anItm(MergedWithSections);
      for (; anItm.More(); anItm.Next())
      {
        const TopoDS_Shape& edge = anItm.Key();
        MapSplitEdges.Add(edge);
      }
      // Iterate on edges merged with bound
      NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator
        anItm1(MergedWithBound);
      for (; anItm1.More(); anItm1.Next())
      {
        // Retrieve edge merged with bound
        const TopoDS_Shape& edge = anItm1.Key();
        // Remove edge from the map
        if (MapSplitEdges.Contains(edge))
          MapSplitEdges.RemoveKey(edge);
        if (myBoundSections.IsBound(edge))
        {
          // Edge has cutting sections
          NCollection_List<TopoDS_Shape>::Iterator its(myBoundSections(edge));
          for (; its.More(); its.Next())
          {
            const TopoDS_Shape& sec = its.Value();
            // Remove section from the map
            if (MapSplitEdges.Contains(sec))
              MapSplitEdges.RemoveKey(sec);
          }
        }
      }
      // Calculate section merging tolerance
      double MinSplitTol = RealLast();
      for (int ii = 1; ii <= MapSplitEdges.Extent(); ii++)
      {
        const TopoDS_Edge& edge =
          TopoDS::Edge(MergedWithSections.FindFromKey(MapSplitEdges.FindKey(ii)));
        MinSplitTol = std::min(MinSplitTol, BRep_Tool::Tolerance(edge));
      }
      // Calculate bound merging tolerance
      const TopoDS_Edge& BoundEdge    = TopoDS::Edge(MergedWithBound.FindFromKey(bound));
      double             BoundEdgeTol = BRep_Tool::Tolerance(BoundEdge);
      isSplitted = ((MinSplitTol < BoundEdgeTol + MinTolerance()) || myNonmanifold);
      isSplitted = (!MapSplitEdges.IsEmpty() && isSplitted);
    }

    if (isSplitted)
    {
      // Merging of cutting sections
      // myMergedEdges.Add(bound);
      myReShape->Replace(myReShape->Apply(bound), myReShape->Apply(BoundWire));
      NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator
        anItm(MergedWithSections);
      for (; anItm.More(); anItm.Next())
      {
        const TopoDS_Shape& oldedge = anItm.Key();
        TopoDS_Shape        newedge = SectionsReShape->Apply(anItm.Value());
        ReplaceEdge(myReShape->Apply(oldedge), newedge, myReShape);
        myMergedEdges.Add(oldedge);
        if (myBoundSections.IsBound(oldedge))
          myBoundSections.UnBind(oldedge);
      }
    }
    else
    {
      // Merging of initial bound
      // myMergedEdges.Add(bound);
      NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator
        anItm(MergedWithBound);
      for (; anItm.More(); anItm.Next())
      {
        const TopoDS_Shape& oldedge = anItm.Key();
        const TopoDS_Shape& newedge = anItm.Value();
        ReplaceEdge(myReShape->Apply(oldedge), newedge, myReShape);
        myMergedEdges.Add(oldedge);
        if (myBoundSections.IsBound(oldedge))
          myBoundSections.UnBind(oldedge);
      }
      if (myBoundSections.IsBound(bound))
        myBoundSections.UnBind(bound);
      myMergedEdges.Add(bound);
    }
  }

  myNbVertices = myVertexNode.Extent() + myVertexNodeFree.Extent();
  myNodeSections.Clear();
  myVertexNode.Clear();
  myVertexNodeFree.Clear();
  myCuttingNode.Clear();
}

//=================================================================================================

bool BRepBuilderAPI_Sewing::MergedNearestEdges(const TopoDS_Shape&                 edge,
                                               NCollection_Sequence<TopoDS_Shape>& SeqMergedEdge,
                                               NCollection_Sequence<bool>&         SeqMergedOri)
{
  // Retrieve edge nodes
  TopoDS_Vertex no1, no2;
  TopExp::Vertices(TopoDS::Edge(edge), no1, no2);
  TopoDS_Shape nno1 = no1, nno2 = no2;
  bool         isNode1 = myVertexNode.Contains(no1);
  bool         isNode2 = myVertexNode.Contains(no2);
  if (isNode1)
    nno1 = myVertexNode.FindFromKey(no1);
  if (isNode2)
    nno2 = myVertexNode.FindFromKey(no2);

  // Fill map of nodes connected to the node #1
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapVert1;
  mapVert1.Add(nno1);
  if (myCuttingNode.IsBound(nno1))
  {
    NCollection_List<TopoDS_Shape>::Iterator ilv(myCuttingNode(nno1));
    for (; ilv.More(); ilv.Next())
    {
      TopoDS_Shape v1 = ilv.Value();
      mapVert1.Add(v1);
      if (!isNode1 && myCuttingNode.IsBound(v1))
      {
        NCollection_List<TopoDS_Shape>::Iterator ilvn(myCuttingNode(v1));
        for (; ilvn.More(); ilvn.Next())
        {
          TopoDS_Shape vn = ilvn.Value();
          mapVert1.Add(vn);
        }
      }
    }
  }

  // Fill map of nodes connected to the node #2
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapVert2;
  mapVert2.Add(nno2);
  if (myCuttingNode.IsBound(nno2))
  {
    NCollection_List<TopoDS_Shape>::Iterator ilv(myCuttingNode(nno2));
    for (; ilv.More(); ilv.Next())
    {
      TopoDS_Shape v1 = ilv.Value();
      mapVert2.Add(v1);
      if (!isNode2 && myCuttingNode.IsBound(v1))
      {
        NCollection_List<TopoDS_Shape>::Iterator ilvn(myCuttingNode(v1));
        for (; ilvn.More(); ilvn.Next())
        {
          TopoDS_Shape vn = ilvn.Value();
          mapVert2.Add(vn);
        }
      }
    }
  }

  // Find all possible contiguous edges
  NCollection_Sequence<TopoDS_Shape> seqEdges;
  seqEdges.Append(edge);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapEdges;
  mapEdges.Add(edge);
  for (int i = 1; i <= mapVert1.Extent(); i++)
  {
    const TopoDS_Shape& node1 = mapVert1.FindKey(i);
    if (!myNodeSections.IsBound(node1))
      continue;
    NCollection_List<TopoDS_Shape>::Iterator ilsec(myNodeSections(node1));
    for (; ilsec.More(); ilsec.Next())
    {
      TopoDS_Shape sec = ilsec.Value();
      if (sec.IsSame(edge))
        continue;
      // Retrieve section nodes
      TopoDS_Vertex vs1, vs2;
      TopExp::Vertices(TopoDS::Edge(sec), vs1, vs2);
      TopoDS_Shape vs1n = vs1, vs2n = vs2;
      if (const TopoDS_Shape* pVs1n = myVertexNode.Seek(vs1))
        vs1n = *pVs1n;
      if (const TopoDS_Shape* pVs2n = myVertexNode.Seek(vs2))
        vs2n = *pVs2n;
      if ((mapVert1.Contains(vs1n) && mapVert2.Contains(vs2n))
          || (mapVert1.Contains(vs2n) && mapVert2.Contains(vs1n)))
        if (mapEdges.Add(sec))
        {
          // Check for rejected cutting
          bool isRejected = myMergedEdges.Contains(sec);
          if (!isRejected && myBoundSections.IsBound(sec))
          {
            NCollection_List<TopoDS_Shape>::Iterator its(myBoundSections(sec));
            for (; its.More() && !isRejected; its.Next())
            {
              TopoDS_Shape section = its.Value();

              if (myMergedEdges.Contains(section))
                isRejected = true;
            }
          }
          if (!isRejected && mySectionBound.IsBound(sec))
          {
            const TopoDS_Shape& bnd = mySectionBound(sec);
            isRejected = (!myBoundSections.IsBound(bnd) || myMergedEdges.Contains(bnd));
          }

          if (!isRejected)
            seqEdges.Append(sec);
        }
    }
  }

  mapEdges.Clear();

  bool success = false;

  int nbSection = seqEdges.Length();
  if (nbSection > 1)
  {
    // Find the longest edge CCI60011
    int i, indRef = 1;
    if (myNonmanifold)
    {
      double lenRef = 0.;
      for (i = 1; i <= nbSection; i++)
      {
        double                  f, l;
        occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(TopoDS::Edge(seqEdges(i)), f, l);
        GeomAdaptor_Curve       cAdapt(c3d);
        double                  len = GCPnts_AbscissaPoint::Length(cAdapt, f, l);
        if (len > lenRef)
        {
          indRef = i;
          lenRef = len;
        }
      }
      if (indRef != 1)
      {
        TopoDS_Shape longEdge = seqEdges(indRef);
        seqEdges(indRef)      = seqEdges(1);
        seqEdges(1)           = longEdge;
      }
    }

    // Find merging candidates
    NCollection_Sequence<bool>  seqForward;
    NCollection_Sequence<int>   seqCandidates;
    NCollection_IndexedMap<int> mapReference;
    mapReference.Add(indRef); // Add index of reference section
    if (FindCandidates(seqEdges, mapReference, seqCandidates, seqForward))
    {
      int nbCandidates = seqCandidates.Length();
      // Record candidate sections
      for (i = 1; i <= nbCandidates; i++)
      {
        // Retrieve merged edge
        TopoDS_Shape iedge = seqEdges(seqCandidates(i));
        bool         ori   = seqForward(i) != 0;
        SeqMergedEdge.Append(iedge);
        SeqMergedOri.Append(ori);
        if (!myNonmanifold)
          break;
      }
      success = (nbCandidates != 0);
    }
  }

  return success;
}

//=======================================================================
// function : Cutting
// purpose  : Modifies :
//                     myBoundSections
//                     myNodeSections
//                     myCuttingNode
//=======================================================================

void BRepBuilderAPI_Sewing::Cutting(const Message_ProgressRange& theProgress)
{
  int i, nbVertices = myVertexNode.Extent();
  if (!nbVertices)
    return;
  // Create a box tree with vertices
  double                                 eps = myTolerance * 0.5;
  NCollection_UBTree<int, Bnd_Box>       aTree;
  NCollection_UBTreeFiller<int, Bnd_Box> aTreeFiller(aTree);
  BRepBuilderAPI_BndBoxTreeSelector      aSelector;
  for (i = 1; i <= nbVertices; i++)
  {
    gp_Pnt  pt = BRep_Tool::Pnt(TopoDS::Vertex(myVertexNode.FindKey(i)));
    Bnd_Box aBox;
    aBox.Set(pt);
    aBox.Enlarge(eps);
    aTreeFiller.Add(i, aBox);
  }
  aTreeFiller.Fill();

  occ::handle<Geom_Curve> c3d;
  TopLoc_Location         loc;
  double                  first, last;
  // Iterate on all boundaries
  int                   nbBounds = myBoundFaces.Extent();
  Message_ProgressScope aPS(theProgress, "Cutting bounds", nbBounds);
  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator anIterB(myBoundFaces);
  for (; anIterB.More() && aPS.More(); anIterB.Next(), aPS.Next())
  {
    const TopoDS_Edge& bound = TopoDS::Edge(anIterB.Key());
    // Do not cut floating edges
    if (!anIterB.Value().Extent())
      continue;
    // Obtain bound curve
    c3d = BRep_Tool::Curve(bound, loc, first, last);
    if (c3d.IsNull())
      continue;
    if (!loc.IsIdentity())
    {
      c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
      c3d->Transform(loc.Transformation());
    }
    // Create cutting sections
    NCollection_List<TopoDS_Shape> listSections;
    { // szv: Use brackets to destroy local variables
      // Obtain candidate vertices
      TopoDS_Vertex                                                 V1, V2;
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> CandidateVertices;
      { // szv: Use brackets to destroy local variables
        // Create bounding box around curve
        Bnd_Box           aGlobalBox;
        GeomAdaptor_Curve adptC(c3d, first, last);
        BndLib_Add3dCurve::Add(adptC, myTolerance, aGlobalBox);
        // Sort vertices to find candidates
        aSelector.SetCurrent(aGlobalBox);
        aTree.Select(aSelector);
        // Skip bound if no node is in the boundind box
        if (!aSelector.ResInd().Extent())
          continue;
        // Retrieve bound nodes
        TopExp::Vertices(bound, V1, V2);
        const TopoDS_Shape& Node1 = myVertexNode.FindFromKey(V1);
        const TopoDS_Shape& Node2 = myVertexNode.FindFromKey(V2);
        // Fill map of candidate vertices
        NCollection_List<int>::Iterator itl(aSelector.ResInd());
        for (; itl.More(); itl.Next())
        {
          const int           index = itl.Value();
          const TopoDS_Shape& Node  = myVertexNode.FindFromIndex(index);
          if (!Node.IsSame(Node1) && !Node.IsSame(Node2))
          {
            TopoDS_Shape vertex = myVertexNode.FindKey(index);
            CandidateVertices.Add(vertex);
          }
        }
        aSelector.ClearResList();
      }
      int nbCandidates = CandidateVertices.Extent();
      if (!nbCandidates)
        continue;
      // Project vertices on curve
      NCollection_Array1<double> arrPara(1, nbCandidates), arrDist(1, nbCandidates);
      NCollection_Array1<gp_Pnt> arrPnt(1, nbCandidates), arrProj(1, nbCandidates);
      for (int j = 1; j <= nbCandidates; j++)
        arrPnt(j) = BRep_Tool::Pnt(TopoDS::Vertex(CandidateVertices(j)));
      ProjectPointsOnCurve(arrPnt, c3d, first, last, arrDist, arrPara, arrProj, true);
      // Create cutting nodes
      NCollection_Sequence<TopoDS_Shape> seqNode;
      NCollection_Sequence<double>       seqPara;
      CreateCuttingNodes(CandidateVertices,
                         bound,
                         V1,
                         V2,
                         arrDist,
                         arrPara,
                         arrProj,
                         seqNode,
                         seqPara);
      if (!seqPara.Length())
        continue;
      // Create cutting sections
      CreateSections(bound, seqNode, seqPara, listSections);
    }
    if (listSections.Extent() > 1)
    {
      // modification of maps:
      //                     myBoundSections
      NCollection_List<TopoDS_Shape>::Iterator its(listSections);
      for (; its.More(); its.Next())
      {
        TopoDS_Shape section = its.Value();
        // Iterate on section vertices
        for (TopoDS_Iterator itv(section); itv.More(); itv.Next())
        {
          TopoDS_Shape vertex = itv.Value();
          // Convert vertex to node
          if (const TopoDS_Shape* pVertex = myVertexNode.Seek(vertex))
            vertex = TopoDS::Vertex(*pVertex);
          // Update node sections
          myNodeSections.TryBound(vertex, NCollection_List<TopoDS_Shape>())->Append(section);
        }
        // Store bound for section
        mySectionBound.Bind(section, bound);
      }
      // Store split bound
      myBoundSections.Bind(bound, listSections);
    }
  }
#ifdef OCCT_DEBUG
  std::cout << "From " << nbBounds << " bounds " << myBoundSections.Extent() << " were cut into "
            << mySectionBound.Extent() << " sections" << std::endl;
#endif
}

//=================================================================================================

static void GetSeqEdges(
  const TopoDS_Shape&                 edge,
  NCollection_Sequence<TopoDS_Shape>& seqEdges,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    VertEdge)
{
  int numV = 0;
  for (TopoDS_Iterator Iv(edge, false); Iv.More(); Iv.Next())
  {
    const TopoDS_Vertex& V1 = TopoDS::Vertex(Iv.Value());
    numV++;
    if (VertEdge.IsBound(V1))
    {
      const NCollection_List<TopoDS_Shape>& listEdges = VertEdge.Find(V1);
      for (NCollection_List<TopoDS_Shape>::Iterator lIt(listEdges); lIt.More(); lIt.Next())
      {
        const TopoDS_Shape& edge1 = lIt.Value();
        if (edge1.IsSame(edge))
          continue;
        bool isContained = false;
        int  i, index = 1;
        for (i = 1; i <= seqEdges.Length() && !isContained; i++)
        {
          isContained = seqEdges.Value(i).IsSame(edge1);
          if (!isContained && seqEdges.Value(i).IsSame(edge))
            index = i;
        }
        if (!isContained)
        {
          if (numV == 1)
            seqEdges.InsertBefore(index, edge1);
          else
            seqEdges.InsertAfter(index, edge1);
          GetSeqEdges(edge1, seqEdges, VertEdge);
        }
      }
    }
  }
}

//=================================================================================================

void BRepBuilderAPI_Sewing::GetFreeWires(
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MapFreeEdges,
  NCollection_Sequence<TopoDS_Shape>&                            seqWires)
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                     VertEdge;
  NCollection_Sequence<TopoDS_Shape> seqFreeEdges;
  for (int i = 1; i <= MapFreeEdges.Extent(); i++)
  {
    const TopoDS_Shape& edge = MapFreeEdges.FindKey(i);
    seqFreeEdges.Append(edge);
    for (TopoDS_Iterator Iv(edge, false); Iv.More(); Iv.Next())
    {
      const TopoDS_Vertex& V1 = TopoDS::Vertex(Iv.Value());
      VertEdge.TryBound(V1, NCollection_List<TopoDS_Shape>())->Append(edge);
    }
  }
  BRep_Builder B;
  int          i, j;
  for (i = 1; i <= seqFreeEdges.Length(); i++)
  {
    NCollection_Sequence<TopoDS_Shape> seqEdges;
    const TopoDS_Shape&                edge = seqFreeEdges.Value(i);
    if (!MapFreeEdges.Contains(edge))
      continue;
    seqEdges.Append(edge);
    GetSeqEdges(edge, seqEdges, VertEdge);
    TopoDS_Wire wire;
    B.MakeWire(wire);
    for (j = 1; j <= seqEdges.Length(); j++)
    {
      B.Add(wire, seqEdges.Value(j));
      MapFreeEdges.RemoveKey(seqEdges.Value(j));
    }
    seqWires.Append(wire);
    if (MapFreeEdges.IsEmpty())
      break;
  }
}

//=================================================================================================

static bool IsDegeneratedWire(const TopoDS_Shape& wire)
{
  if (wire.ShapeType() != TopAbs_WIRE)
    return false;
  // Get maximal vertices tolerance
  TopoDS_Vertex V1, V2;
  // TopExp::Vertices(TopoDS::Wire(wire),V1,V2);
  // double tol = std::max(BRep_Tool::Tolerance(V1),BRep_Tool::Tolerance(V2));
  double          wireLength = 0.0;
  TopLoc_Location loc;
  double          first, last;
  int             nume    = 0;
  int             isSmall = 0;
  for (TopoDS_Iterator aIt(wire, false); aIt.More(); aIt.Next())
  {
    nume++;
    TopoDS_Shape  edge = aIt.Value();
    TopoDS_Vertex Ve1, Ve2;
    TopExp::Vertices(TopoDS::Edge(edge), Ve1, Ve2);
    if (nume == 1)
    {
      V1 = Ve1;
      V2 = Ve2;
    }
    else
    {
      if (Ve1.IsSame(V1))
        V1 = Ve2;
      else if (Ve1.IsSame(V2))
        V2 = Ve2;
      if (Ve2.IsSame(V1))
        V1 = Ve1;
      else if (Ve2.IsSame(V2))
        V2 = Ve1;
    }
    occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(TopoDS::Edge(aIt.Value()), loc, first, last);
    if (!c3d.IsNull())
    {
      c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
      if (!loc.IsIdentity())
      {
        // c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
        c3d->Transform(loc.Transformation());
      }
      gp_Pnt pfirst = c3d->Value(first);
      gp_Pnt plast  = c3d->Value(last);
      gp_Pnt pmid   = c3d->Value((first + last) * 0.5);
      double length = 0;
      if (pfirst.Distance(plast) > pfirst.Distance(pmid))
      {
        length = pfirst.Distance(plast);
      }
      else
      {
        GeomAdaptor_Curve cAdapt(c3d);
        length = GCPnts_AbscissaPoint::Length(cAdapt, first, last);
      }
      double tole = BRep_Tool::Tolerance(Ve1) + BRep_Tool::Tolerance(Ve2);
      if (length <= tole)
        isSmall++;
      wireLength += length;
    }
  }
  if (isSmall == nume)
    return true;
  // clang-format off
  double tol = BRep_Tool::Tolerance(V1)+BRep_Tool::Tolerance(V2);//Max(BRep_Tool::Tolerance(V1),BRep_Tool::Tolerance(V2));
  // clang-format on
  return wireLength <= tol;
}

//=======================================================================
// function :  DegeneratedSection
// purpose  :  internal use
//            create a new degenerated edge if the section is degenerated
//=======================================================================

static TopoDS_Edge DegeneratedSection(const TopoDS_Shape& section, const TopoDS_Shape& face)
{
  // Return if section is already degenerated
  if (BRep_Tool::Degenerated(TopoDS::Edge(section)))
    return TopoDS::Edge(section);

  // Retrieve edge curve
  TopLoc_Location         loc;
  double                  first, last;
  occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(TopoDS::Edge(section), loc, first, last);
  if (c3d.IsNull())
  { // gka
    BRep_Builder aB;
    TopoDS_Edge  edge1 = TopoDS::Edge(section);
    aB.Degenerated(edge1, true);
    return edge1;
  }
  if (!loc.IsIdentity())
  {
    c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
    c3d->Transform(loc.Transformation());
  }

  // Test if the new edge is degenerated
  TopoDS_Vertex v1, v2;
  TopExp::Vertices(TopoDS::Edge(section), v1, v2);
  // double tol = std::max(BRep_Tool::Tolerance(v1),BRep_Tool::Tolerance(v2));
  // tol = std::max(tolerance,tol);

  gp_Pnt p1, p2, p3;
  p1 = BRep_Tool::Pnt(v1);
  p3 = BRep_Tool::Pnt(v2);
  c3d->D0(0.5 * (first + last), p2);

  // bool isDegenerated = false;
  // if (p1.Distance(p3) < tol) {
  // GeomAdaptor_Curve cAdapt(c3d);
  // double length = GCPnts_AbscissaPoint::Length(cAdapt, first, last);
  // isDegenerated =  true; //(length < tol);
  // }

  TopoDS_Edge edge;
  // if (!isDegenerated) return edge;

  // processing
  BRep_Builder aBuilder;
  edge = TopoDS::Edge(section);
  edge.EmptyCopy();
  if (v1.IsSame(v2))
  {
    TopoDS_Shape anEdge = edge.Oriented(TopAbs_FORWARD);
    aBuilder.Add(anEdge, v1.Oriented(TopAbs_FORWARD));
    aBuilder.Add(anEdge, v2.Oriented(TopAbs_REVERSED));
  }
  else
  {
    TopoDS_Vertex newVertex;
    if (p1.Distance(p3) < BRep_Tool::Tolerance(v1))
      newVertex = v1;
    else if (p1.Distance(p3) < BRep_Tool::Tolerance(v2))
      newVertex = v2;
    else
    {
      double d1           = BRep_Tool::Tolerance(v1) + p2.Distance(p1);
      double d2           = BRep_Tool::Tolerance(v2) + p2.Distance(p3);
      double newTolerance = std::max(d1, d2);
      aBuilder.MakeVertex(newVertex, p2, newTolerance);
    }
    TopoDS_Shape anEdge = edge.Oriented(TopAbs_FORWARD);
    aBuilder.Add(anEdge, newVertex.Oriented(TopAbs_FORWARD));
    aBuilder.Add(anEdge, newVertex.Oriented(TopAbs_REVERSED));
  }

  BRep_Tool::Range(TopoDS::Edge(section), first, last);
  TopoDS_Shape anEdge = edge.Oriented(TopAbs_FORWARD);
  aBuilder.Range(TopoDS::Edge(anEdge), first, last);
  aBuilder.Degenerated(edge, true);
  occ::handle<Geom_Curve> aC3dNew;
  if (!face.IsNull())
  {
    double                    af, al;
    occ::handle<Geom2d_Curve> aC2dt =
      BRep_Tool::CurveOnSurface(TopoDS::Edge(section), TopoDS::Face(face), af, al);
    aBuilder.UpdateEdge(edge, aC3dNew, 0);
    occ::handle<Geom2d_Curve> aC2dn = BRep_Tool::CurveOnSurface(edge, TopoDS::Face(face), af, al);
    if (aC2dn.IsNull())
      aBuilder.UpdateEdge(edge, aC2dt, TopoDS::Face(face), 0);
  }

  return edge;
}

//=======================================================================
// function : EdgeProcessing
// purpose  : modifies :
//                       myNbEdges
//                       myHasMultipleEdge
//                       myHasFreeBound
//           . if multiple edge
//              - length < 100.*myTolerance -> several free edge
//           . if no multiple edge
//              - make the contiguous edges sameparameter
//=======================================================================

void BRepBuilderAPI_Sewing::EdgeProcessing(const Message_ProgressRange& theProgress)
{
  // constructs sectionEdge
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            MapFreeEdges;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> EdgeFace;
  Message_ProgressScope aPS(theProgress, "Edge processing", myBoundFaces.Extent());
  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator anIterB(myBoundFaces);
  for (; anIterB.More() && aPS.More(); anIterB.Next(), aPS.Next())
  {
    const TopoDS_Shape&                   bound     = anIterB.Key();
    const NCollection_List<TopoDS_Shape>& listFaces = anIterB.Value();
    if (listFaces.Extent() == 1)
    {
      if (myBoundSections.IsBound(bound))
      {
        NCollection_List<TopoDS_Shape>::Iterator liter(myBoundSections(bound));
        for (; liter.More(); liter.Next())
        {
          if (!myMergedEdges.Contains(liter.Value()))
          { // myReShape->IsRecorded(liter.Value())) {
            const TopoDS_Shape& edge = myReShape->Apply(liter.Value());
            if (!MapFreeEdges.Contains(edge))
            {
              const TopoDS_Shape& face = listFaces.First();
              EdgeFace.Bind(edge, face);
              MapFreeEdges.Add(edge);
            }
          }
        }
      }
      else
      {
        if (!myMergedEdges.Contains(bound))
        {
          const TopoDS_Shape& edge = myReShape->Apply(bound);
          if (!MapFreeEdges.Contains(edge))
          {
            const TopoDS_Shape& face = listFaces.First();
            EdgeFace.Bind(edge, face);
            MapFreeEdges.Add(edge);
          }
        }
      }
    }
  }

  if (!MapFreeEdges.IsEmpty())
  {
    NCollection_Sequence<TopoDS_Shape> seqWires;
    GetFreeWires(MapFreeEdges, seqWires);
    for (int j = 1; j <= seqWires.Length(); j++)
    {
      const TopoDS_Wire& wire = TopoDS::Wire(seqWires.Value(j));
      if (!IsDegeneratedWire(wire))
        continue;
      for (TopoDS_Iterator Ie(wire, false); Ie.More(); Ie.Next())
      {
        TopoDS_Shape       aTmpShape = myReShape->Apply(Ie.Value()); // for porting
        const TopoDS_Edge& edge      = TopoDS::Edge(aTmpShape);
        TopoDS_Shape       face;
        if (EdgeFace.IsBound(edge))
          face = EdgeFace.Find(edge);
        TopoDS_Shape degedge = DegeneratedSection(edge, face);
        if (degedge.IsNull())
          continue;
        if (!degedge.IsSame(edge))
          ReplaceEdge(edge, degedge, myReShape);
        if (BRep_Tool::Degenerated(TopoDS::Edge(degedge)))
          myDegenerated.Add(degedge);
      }
    }
  }
}

//=======================================================================
// function : EdgeRegularity
// purpose  : update Continuity flag on newly created edges
//=======================================================================

void BRepBuilderAPI_Sewing::EdgeRegularity(const Message_ProgressRange& theProgress)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMapEF;
  TopExp::MapShapesAndAncestors(mySewedShape, TopAbs_EDGE, TopAbs_FACE, aMapEF);

  Message_ProgressScope aPS(theProgress, "Encode edge regularity", myMergedEdges.Extent());
  for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aMEIt(myMergedEdges);
       aMEIt.More() && aPS.More();
       aMEIt.Next(), aPS.Next())
  {
    TopoDS_Edge                           anEdge = TopoDS::Edge(myReShape->Apply(aMEIt.Value()));
    const NCollection_List<TopoDS_Shape>* aFaces = aMapEF.Seek(anEdge);
    // encode regularity if and only if edges is shared by two faces
    if (aFaces && aFaces->Extent() == 2)
      BRepLib::EncodeRegularity(anEdge,
                                TopoDS::Face(aFaces->First()),
                                TopoDS::Face(aFaces->Last()));
  }

  myMergedEdges.Clear();
}

//=================================================================================================

void BRepBuilderAPI_Sewing::CreateSewedShape()
{
  // ---------------------
  // create the new shapes
  // ---------------------
  BRepTools_Quilt aQuilt;
  bool            isLocal = !myShape.IsNull();
  if (isLocal)
  {
    // Local sewing
    TopoDS_Shape ns = myReShape->Apply(myShape);
    aQuilt.Add(ns);
  }
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator anIter(
    myOldShapes);
  for (; anIter.More(); anIter.Next())
  {
    TopoDS_Shape sh = anIter.Value();
    if (!sh.IsNull())
    {
      sh                   = myReShape->Apply(sh);
      anIter.ChangeValue() = sh;
      if (!isLocal)
        aQuilt.Add(sh);
    }
  }
  TopoDS_Shape aNewShape = aQuilt.Shells();
  int          numsh     = 0;

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> OldShells;

  BRep_Builder    aB;
  TopoDS_Compound aComp;
  aB.MakeCompound(aComp);
  for (TopoDS_Iterator aExpSh(aNewShape, false); aExpSh.More(); aExpSh.Next())
  {
    TopoDS_Shape sh       = aExpSh.Value();
    bool         hasEdges = false;
    if (sh.ShapeType() == TopAbs_SHELL)
    {
      if (myNonmanifold)
        hasEdges = !OldShells.Contains(sh);
      else
      {
        TopoDS_Shape face;
        int          numf = 0;
        for (TopExp_Explorer aExpF(sh, TopAbs_FACE); aExpF.More() && (numf < 2); aExpF.Next())
        {
          face = aExpF.Current();
          numf++;
        }
        if (numf == 1)
          aB.Add(aComp, face);
        else if (numf > 1)
          aB.Add(aComp, sh);
        if (numf)
          numsh++;
      }
    }
    else if (sh.ShapeType() == TopAbs_FACE)
    {
      if (myNonmanifold)
      {
        TopoDS_Shell ss;
        aB.MakeShell(ss);
        aB.Add(ss, sh);
        sh       = ss;
        hasEdges = true;
      }
      else
      {
        aB.Add(aComp, sh);
        numsh++;
      }
    }
    else
    {
      aB.Add(aComp, sh);
      numsh++;
    }
    if (hasEdges)
      OldShells.Add(sh);
  }
  // Process collected shells
  if (myNonmanifold)
  {
    int nbOldShells = OldShells.Extent();
    if (nbOldShells == 1)
    {
      // Single shell - check for single face
      const TopoDS_Shape& sh = OldShells.FindKey(1);
      TopoDS_Shape        face;
      int                 numf = 0;
      for (TopExp_Explorer aExpF(sh, TopAbs_FACE); aExpF.More() && (numf < 2); aExpF.Next())
      {
        face = aExpF.Current();
        numf++;
      }
      if (numf == 1)
        aB.Add(aComp, face);
      else if (numf > 1)
        aB.Add(aComp, sh);
      if (numf)
        numsh++;
    }
    else if (nbOldShells)
    {
      // Several shells should be merged
      NCollection_Map<int> IndexMerged;
      while (IndexMerged.Extent() < nbOldShells)
      {
        TopoDS_Shell                                           NewShell;
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> NewEdges;
        for (int i = 1; i <= nbOldShells; i++)
        {
          if (IndexMerged.Contains(i))
            continue;
          const TopoDS_Shell& shell = TopoDS::Shell(OldShells.FindKey(i));
          if (NewShell.IsNull())
          {
            aB.MakeShell(NewShell);
            TopoDS_Iterator aItSS(shell);
            for (; aItSS.More(); aItSS.Next())
              aB.Add(NewShell, aItSS.Value());
            // Fill map of edges
            for (TopExp_Explorer eexp(shell, TopAbs_EDGE); eexp.More(); eexp.Next())
            {
              const TopoDS_Shape& edge = eexp.Current();
              NewEdges.Add(edge);
            }
            IndexMerged.Add(i);
          }
          else
          {
            bool            hasSharedEdge = false;
            TopExp_Explorer eexp(shell, TopAbs_EDGE);
            for (; eexp.More() && !hasSharedEdge; eexp.Next())
              hasSharedEdge = NewEdges.Contains(eexp.Current());
            if (hasSharedEdge)
            {
              // Add edges to the map
              for (TopExp_Explorer eexp1(shell, TopAbs_EDGE); eexp1.More(); eexp1.Next())
              {
                const TopoDS_Shape& edge = eexp1.Current();
                NewEdges.Add(edge);
              }
              // Add faces to the shell
              for (TopExp_Explorer fexp(shell, TopAbs_FACE); fexp.More(); fexp.Next())
              {
                const TopoDS_Shape& face = fexp.Current();
                aB.Add(NewShell, face);
              }
              IndexMerged.Add(i);
            }
          }
        }
        // Process new shell
        TopoDS_Shape    face;
        int             numf = 0;
        TopExp_Explorer aExpF(NewShell, TopAbs_FACE);
        for (; aExpF.More() && (numf < 2); aExpF.Next())
        {
          face = aExpF.Current();
          numf++;
        }
        if (numf == 1)
          aB.Add(aComp, face);
        else if (numf > 1)
          aB.Add(aComp, NewShell);
        if (numf)
          numsh++;
      }
    }
  }
  if (numsh == 1)
  {
    // Extract single component
    TopoDS_Iterator aIt(aComp, false);
    mySewedShape = aIt.Value();
  }
  else
    mySewedShape = aComp;
}

//=======================================================================
// function : CreateOutputInformations
// purpose  : constructs :
//                       myEdgeSections
//                       mySectionBound
//                       myNbFreeEdges
//                       myNbContigousEdges
//                       myNbMultipleEdges
//                       myNbDegenerated
//=======================================================================

void BRepBuilderAPI_Sewing::CreateOutputInformations()
{
  // Construct edgeSections
  int i;
  // NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
  // edgeSections;
  // clang-format off
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> edgeSections; //use index map for regulating free edges
  // clang-format on
  for (i = 1; i <= myBoundFaces.Extent(); i++)
  {
    const TopoDS_Shape&            bound = myBoundFaces.FindKey(i);
    NCollection_List<TopoDS_Shape> lsect;
    if (myBoundSections.IsBound(bound))
      lsect = myBoundSections(bound);
    TopExp_Explorer aExp(myReShape->Apply(bound), TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      TopoDS_Shape                             sec  = bound;
      const TopoDS_Shape&                      edge = aExp.Current();
      NCollection_List<TopoDS_Shape>::Iterator aI(lsect);
      for (; aI.More(); aI.Next())
      {
        const TopoDS_Shape& section = aI.Value();
        if (edge.IsSame(myReShape->Apply(section)))
        {
          sec = section;
          break;
        }
      }
      if (edgeSections.Contains(edge))
        edgeSections.ChangeFromKey(edge).Append(sec);
      else
      {
        NCollection_List<TopoDS_Shape> listSec;
        listSec.Append(sec);
        edgeSections.Add(edge, listSec);
      }
    }
  }

  // Fill maps of Free, Contiguous and Multiple edges
  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator anIter(edgeSections);
  for (; anIter.More(); anIter.Next())
  {
    const TopoDS_Shape&                   edge        = anIter.Key();
    const NCollection_List<TopoDS_Shape>& listSection = anIter.Value();
    if (listSection.Extent() == 1)
    {
      if (BRep_Tool::Degenerated(TopoDS::Edge(edge)))
        myDegenerated.Add(edge);
      else
        myFreeEdges.Add(edge);
    }
    else if (listSection.Extent() == 2)
    {
      myContigousEdges.Add(edge, listSection);
    }
    else
    {
      myMultipleEdges.Add(edge);
    }
  }

  // constructs myContigSectBound
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aEdgeMap; // gka
  for (i = 1; i <= myBoundFaces.Extent(); i++)
  {
    const TopoDS_Shape& bound = myBoundFaces.FindKey(i);
    if (myBoundSections.IsBound(bound))
    {
      NCollection_List<TopoDS_Shape>::Iterator iter(myBoundSections(bound));
      for (; iter.More(); iter.Next())
      {
        const TopoDS_Shape& section = iter.Value();
        if (!myMergedEdges.Contains(section))
          continue;
        // if (!myReShape->IsRecorded(section)) continue; // section is free
        TopoDS_Shape nedge = myReShape->Apply(section);
        if (nedge.IsNull())
          continue; // szv debug
        if (!bound.IsSame(section))
          if (myContigousEdges.Contains(nedge))
            myContigSecBound.Bind(section, bound);
      }
    }
  }
}

//=================================================================================================

void BRepBuilderAPI_Sewing::ProjectPointsOnCurve(const NCollection_Array1<gp_Pnt>& arrPnt,
                                                 const occ::handle<Geom_Curve>&    c3d,
                                                 const double                      first,
                                                 const double                      last,
                                                 NCollection_Array1<double>&       arrDist,
                                                 NCollection_Array1<double>&       arrPara,
                                                 NCollection_Array1<gp_Pnt>&       arrProj,
                                                 const bool isConsiderEnds) const
{
  arrDist.Init(-1.0);

  GeomAdaptor_Curve GAC(c3d);
  Extrema_ExtPC     locProj;
  locProj.Initialize(GAC, first, last);
  gp_Pnt pfirst = GAC.Value(first), plast = GAC.Value(last);
  int    find = 1;         //(isConsiderEnds ? 1 : 2);
                           // clang-format off
  int lind = arrPnt.Length();//(isConsiderEnds ? arrPnt.Length() : arrPnt.Length() -1);
                           // clang-format on

  for (int i1 = find; i1 <= lind; i1++)
  {
    gp_Pnt pt          = arrPnt(i1);
    double worktol     = myTolerance;
    double distF2      = pfirst.SquareDistance(pt);
    double distL2      = plast.SquareDistance(pt);
    bool   isProjected = false;
    try
    {

      // Project current point on curve
      locProj.Perform(pt);
      if (locProj.IsDone() && locProj.NbExt() > 0)
      {
        double dist2Min = (isConsiderEnds || i1 == find || i1 == lind ? std::min(distF2, distL2)
                                                                      : Precision::Infinite());
        int    ind, indMin = 0;
        for (ind = 1; ind <= locProj.NbExt(); ind++)
        {
          double dProj2 = locProj.SquareDistance(ind);
          if (dProj2 < dist2Min)
          {
            indMin   = ind;
            dist2Min = dProj2;
          }
        }
        if (indMin)
        {
          isProjected               = true;
          Extrema_POnCurv pOnC      = locProj.Point(indMin);
          double          paramProj = pOnC.Parameter();
          gp_Pnt          ptProj    = GAC.Value(paramProj);
          double          distProj2 = ptProj.SquareDistance(pt);
          if (!locProj.IsMin(indMin))
          {
            if (std::min(distF2, distL2) < dist2Min)
            {
              if (distF2 < distL2)
              {
                paramProj = first;
                distProj2 = distF2;
                ptProj    = pfirst;
              }
              else
              {
                paramProj = last;
                distProj2 = distL2;
                ptProj    = plast;
              }
            }
          }
          if (distProj2 < worktol * worktol || !isConsiderEnds)
          {
            arrDist(i1) = sqrt(distProj2);
            arrPara(i1) = paramProj;
            arrProj(i1) = ptProj;
          }
        }
      }
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "Exception in BRepBuilderAPI_Sewing::ProjectPointsOnCurve: ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
      worktol = MinTolerance();
    }
    if (!isProjected && isConsiderEnds)
    {
      if (std::min(distF2, distL2) < worktol * worktol)
      {
        if (distF2 < distL2)
        {
          arrDist(i1) = sqrt(distF2);
          arrPara(i1) = first;
          arrProj(i1) = pfirst;
        }
        else
        {
          arrDist(i1) = sqrt(distL2);
          arrPara(i1) = last;
          arrProj(i1) = plast;
        }
      }
    }
  }
}

//=================================================================================================

void BRepBuilderAPI_Sewing::CreateCuttingNodes(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MapVert,
  const TopoDS_Shape&                                                  bound,
  const TopoDS_Shape&                                                  vfirst,
  const TopoDS_Shape&                                                  vlast,
  const NCollection_Array1<double>&                                    arrDist,
  const NCollection_Array1<double>&                                    arrPara,
  const NCollection_Array1<gp_Pnt>&                                    arrPnt,
  NCollection_Sequence<TopoDS_Shape>&                                  seqVert,
  NCollection_Sequence<double>&                                        seqPara)
{
  int i, j, nbProj = MapVert.Extent();

  // Reorder projections by distance
  NCollection_Sequence<int> seqOrderedIndex;
  { // szv: Use brackets to destroy local variables
    NCollection_Sequence<double> seqOrderedDistance;
    for (i = 1; i <= nbProj; i++)
    {
      double distProj = arrDist(i);
      if (distProj < 0.0)
        continue; // Skip vertex if not projected
      bool isInserted = false;
      for (j = 1; j <= seqOrderedIndex.Length() && !isInserted; j++)
      {
        isInserted = (distProj < seqOrderedDistance(j));
        if (isInserted)
        {
          seqOrderedIndex.InsertBefore(j, i);
          seqOrderedDistance.InsertBefore(j, distProj);
        }
      }
      if (!isInserted)
      {
        seqOrderedIndex.Append(i);
        seqOrderedDistance.Append(distProj);
      }
    }
  }
  nbProj = seqOrderedIndex.Length();
  if (!nbProj)
    return;

  BRep_Builder aBuilder;

  // Insert two initial vertices (to be removed later)
  NCollection_Sequence<double> seqDist;
  NCollection_Sequence<gp_Pnt> seqPnt;
  { // szv: Use brackets to destroy local variables
    // Retrieve bound curve
    TopLoc_Location         loc;
    double                  first, last;
    occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(TopoDS::Edge(bound), loc, first, last);
    if (!loc.IsIdentity())
    {
      c3d = occ::down_cast<Geom_Curve>(c3d->Copy());
      c3d->Transform(loc.Transformation());
    }
    GeomAdaptor_Curve GAC(c3d);
    seqVert.Prepend(vfirst);
    seqVert.Append(vlast);
    seqPara.Prepend(first);
    seqPara.Append(last);
    seqDist.Prepend(-1.0);
    seqDist.Append(-1.0);
    seqPnt.Prepend(GAC.Value(first));
    seqPnt.Append(GAC.Value(last));
  }

  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> NodeCuttingVertex;
  for (i = 1; i <= nbProj; i++)
  {

    const int index   = seqOrderedIndex(i);
    double    disProj = arrDist(index);
    gp_Pnt    pntProj = arrPnt(index);

    // Skip node if already bound to cutting vertex
    TopoDS_Shape node = myVertexNode.FindFromKey(MapVert(index));
    if (NodeCuttingVertex.Contains(node))
      continue;

    // Find the closest vertex
    int    indexMin = 1;
    double dist, distMin = pntProj.Distance(seqPnt(1));
    for (j = 2; j <= seqPnt.Length(); j++)
    {
      dist = pntProj.Distance(seqPnt(j));
      if (dist < distMin)
      {
        distMin  = dist;
        indexMin = j;
      }
    }

    // Check if current point is close to one of the existent
    if (distMin <= std::max(disProj * 0.1, MinTolerance()))
    {
      // Check distance if close
      double jdist = seqDist.Value(indexMin);
      if (jdist < 0.0)
      {
        // Bind new cutting node (end vertex only)
        seqDist.SetValue(indexMin, disProj);
        const TopoDS_Shape& cvertex = seqVert.Value(indexMin);
        NodeCuttingVertex.Add(node, cvertex);
      }
      else
      {
        // Bind secondary cutting nodes
        NodeCuttingVertex.Add(node, TopoDS_Vertex());
      }
    }
    else
    {
      // Build new cutting vertex
      TopoDS_Vertex cvertex;
      aBuilder.MakeVertex(cvertex, pntProj, Precision::Confusion());
      // Bind new cutting vertex
      NodeCuttingVertex.Add(node, cvertex);
      // Insert cutting vertex in the sequences
      double parProj = arrPara(index);
      for (j = 2; j <= seqPara.Length(); j++)
      {
        if (parProj <= seqPara.Value(j))
        {
          seqVert.InsertBefore(j, cvertex);
          seqPara.InsertBefore(j, parProj);
          seqDist.InsertBefore(j, disProj);
          seqPnt.InsertBefore(j, pntProj);
          break;
        }
      }
    }
  }

  // filling map for cutting nodes
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aMIt(
    NodeCuttingVertex);
  for (; aMIt.More(); aMIt.Next())
  {
    TopoDS_Shape cnode = aMIt.Value();
    // Skip secondary nodes
    if (cnode.IsNull())
      continue;
    // Obtain vertex node
    const TopoDS_Shape& node = aMIt.Key();
    if (myVertexNode.Contains(cnode))
    {
      // This is an end vertex
      cnode = myVertexNode.FindFromKey(cnode);
    }
    else
    {
      // Create link: cutting vertex -> node
      NCollection_List<TopoDS_Shape> ls;
      ls.Append(node);
      myCuttingNode.Bind(cnode, ls);
    }
    // Create link: node -> cutting vertex
    if (myCuttingNode.IsBound(node))
    {
      myCuttingNode.ChangeFind(node).Append(cnode);
    }
    else
    {
      NCollection_List<TopoDS_Shape> ls;
      ls.Append(cnode);
      myCuttingNode.Bind(node, ls);
    }
  }

  // Remove two initial vertices
  seqVert.Remove(1);
  seqVert.Remove(seqVert.Length());
  seqPara.Remove(1);
  seqPara.Remove(seqPara.Length());
}

//=================================================================================================

void BRepBuilderAPI_Sewing::CreateSections(const TopoDS_Shape&                       section,
                                           const NCollection_Sequence<TopoDS_Shape>& seqNode,
                                           const NCollection_Sequence<double>&       seqPara,
                                           NCollection_List<TopoDS_Shape>&           listEdge)
{
  const TopoDS_Edge& sec = TopoDS::Edge(section);
  //  TopAbs_Orientation aInitOr = sec.Orientation();

  // To keep NM vertices on edge
  NCollection_Sequence<TopoDS_Shape> aSeqNMVert;
  NCollection_Sequence<double>       aSeqNMPars;
  findNMVertices(sec, aSeqNMVert, aSeqNMPars);

  BRep_Builder aBuilder;

  double first, last;
  BRep_Tool::Range(sec, first, last);

  // Create cutting sections
  double       par1, par2;
  TopoDS_Shape V1, V2;
  int          i, len = seqPara.Length() + 1;
  for (i = 1; i <= len; i++)
  {

    TopoDS_Edge edge = sec;
    edge.EmptyCopy();

    if (i == 1)
    {
      par1 = first;
      par2 = seqPara(i);
      V1   = TopExp::FirstVertex(sec);
      V2   = seqNode(i);
    }
    else if (i == len)
    {
      par1 = seqPara(i - 1);
      par2 = last;
      V1   = seqNode(i - 1);
      V2   = TopExp::LastVertex(sec);
    }
    else
    {
      par1 = seqPara(i - 1);
      par2 = seqPara(i);
      V1   = seqNode(i - 1);
      V2   = seqNode(i);
    }

    TopoDS_Shape aTmpShape = edge.Oriented(TopAbs_FORWARD);
    TopoDS_Edge  aTmpEdge  = TopoDS::Edge(aTmpShape); // for porting
    aTmpShape              = V1.Oriented(TopAbs_FORWARD);
    aBuilder.Add(aTmpEdge, aTmpShape);
    aTmpShape = V2.Oriented(TopAbs_REVERSED);
    aBuilder.Add(aTmpEdge, aTmpShape);
    aBuilder.Range(aTmpEdge, par1, par2);
    //    if(aInitOr == TopAbs_REVERSED)
    //      listEdge.Prepend(edge);
    //    else

    int k = 1;
    for (; k <= aSeqNMPars.Length(); k++)
    {
      double apar = aSeqNMPars.Value(k);
      if (apar >= par1 && apar <= par2)
      {
        aBuilder.Add(aTmpEdge, aSeqNMVert.Value(k));
        aSeqNMVert.Remove(k);
        aSeqNMPars.Remove(k);
        k--;
      }
    }
    listEdge.Append(edge);
  }

  const NCollection_List<TopoDS_Shape>& listFaces = myBoundFaces.FindFromKey(sec);
  if (!listFaces.Extent())
    return;

  double tolEdge = BRep_Tool::Tolerance(sec);

  // Add cutting pcurves
  NCollection_List<TopoDS_Shape>::Iterator itf(listFaces);
  for (; itf.More(); itf.Next())
  {

    const TopoDS_Face& fac = TopoDS::Face(itf.Value());

    // Retrieve curve on surface
    double                    first2d = 0., last2d = 0., first2d1 = 0, last2d1 = 0.;
    occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(sec, fac, first2d, last2d);
    if (c2d.IsNull())
      continue;
    occ::handle<Geom2d_Curve> c2d1;
    bool                      isSeam = BRep_Tool::IsClosed(sec, fac);

    // gka  - Convert to BSpline was commented because
    // it is not necessary to create BSpline instead of Lines or cIrcles.
    // Besides after conversion circles to BSpline
    // it is necessary to recompute parameters of cutting because parameterization of created
    // BSpline curve differs from parametrization of circle.

    // Convert pcurve to BSpline
    /*occ::handle<Geom2d_BSplineCurve> c2dBSP,c2dBSP1;
    if (c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    c2dBSP = occ::down_cast<Geom2d_BSplineCurve>(c2d);
    }
    else {
    if (first > (c2d->FirstParameter() + Precision::PConfusion()) ||
    last < (c2d->LastParameter() - Precision::PConfusion())) {
    occ::handle<Geom2d_TrimmedCurve> TC = new Geom2d_TrimmedCurve(c2d, first, last);
    c2dBSP = Geom2dConvert::CurveToBSplineCurve(TC);
    }
    else c2dBSP = Geom2dConvert::CurveToBSplineCurve(c2d);
    }
    if (c2dBSP.IsNull()) continue;*/
    // gka fix for bug OCC12203 21.04.06 addition second curve for seam edges

    if (isSeam)
    {
      TopoDS_Edge secRev = TopoDS::Edge(sec.Reversed());

      c2d1 = BRep_Tool::CurveOnSurface(secRev, fac, first2d1, last2d1);
      if (c2d1.IsNull())
        continue;

      /*if (c2d1->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
      c2dBSP1 = occ::down_cast<Geom2d_BSplineCurve>(c2d1);
      }
      else {
      if (first > (c2d1->FirstParameter() + Precision::PConfusion()) ||
      last < (c2d1->LastParameter() - Precision::PConfusion())) {
      occ::handle<Geom2d_TrimmedCurve> TC = new Geom2d_TrimmedCurve(c2d1, first, last);
      //c2dBSP1 = Geom2dConvert::CurveToBSplineCurve(TC);
      }
      //else c2dBSP1 = Geom2dConvert::CurveToBSplineCurve(c2d);

      }*/
    }

    /*first2d = c2dBSP->FirstParameter();
    last2d = c2dBSP->LastParameter();

    if(!c2dBSP1.IsNull()) {
    first2d1 = c2dBSP1->FirstParameter();
    last2d1 = c2dBSP1->LastParameter();
    }*/

    // Update cutting sections
    occ::handle<Geom2d_Curve>                c2dNew, c2d1New;
    NCollection_List<TopoDS_Shape>::Iterator ite(listEdge);
    for (; ite.More(); ite.Next())
    {

      // Retrieve cutting section
      const TopoDS_Edge& edge = TopoDS::Edge(ite.Value());
      BRep_Tool::Range(edge, par1, par2);

      // Cut BSpline pcurve
      // try {
      c2dNew = occ::down_cast<Geom2d_Curve>(c2d->Copy());
      // c2dNew = occ::down_cast<Geom2d_Curve>(c2dBSP->Copy());
      // occ::down_cast<Geom2d_BSplineCurve>(c2dNew)->Segment(std::max(first2d,par1),Min(par2,last2d));
      if (!c2d1.IsNull())
      { // if(!c2dBSP1.IsNull()) {
        c2d1New = occ::down_cast<Geom2d_Curve>(c2d1->Copy());
        // c2d1New = occ::down_cast<Geom2d_Curve>(c2dBSP1->Copy());
        // occ::down_cast<Geom2d_BSplineCurve>(c2d1New)->Segment(std::max(first2d1,par1),Min(par2,last2d1));
      }
      //}
      /*catch (Standard_Failure) {
      #ifdef OCCT_DEBUG
      std::cout << "Exception in CreateSections: segment [" << par1 << "," << par2 << "]: ";
      Standard_Failure::Caught()->Print(std::cout); std::cout << std::endl;
      #endif
      occ::handle<Geom2d_TrimmedCurve> c2dT = new
      Geom2d_TrimmedCurve(c2dNew,Max(first2d,par1),Min(par2,last2d)); c2dNew = c2dT;
      }*/

      if (!isSeam && c2d1New.IsNull())
        aBuilder.UpdateEdge(edge, c2dNew, fac, tolEdge);
      else
      {
        TopAbs_Orientation Ori = edge.Orientation();
        if (fac.Orientation() == TopAbs_REVERSED)
          Ori = TopAbs::Reverse(Ori);

        if (Ori == TopAbs_FORWARD)
          aBuilder.UpdateEdge(edge, c2dNew, c2d1New, fac, tolEdge);
        else
          aBuilder.UpdateEdge(edge, c2d1New, c2dNew, fac, tolEdge);
      }
    }
  }
}

//=================================================================================================

void BRepBuilderAPI_Sewing::SameParameterShape()
{
  if (!mySameParameterMode)
    return;
  TopExp_Explorer exp(mySewedShape, TopAbs_EDGE);
  // Le flag sameparameter est a false pour chaque edge cousue
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Edge& sec = TopoDS::Edge(exp.Current());
    try
    {

      BRepLib::SameParameter(sec, BRep_Tool::Tolerance(sec));
    }
    catch (Standard_Failure const&)
    {
#ifdef OCCT_DEBUG
      std::cout
        << "Fail: BRepBuilderAPI_Sewing::SameParameterShape exception in BRepLib::SameParameter"
        << std::endl;
#endif
      continue;
    }
  }
}

//=======================================================================
// function : Inspect
// purpose  : Used for selection and storage of coinciding points
//=======================================================================

NCollection_CellFilter_Action BRepBuilderAPI_VertexInspector::Inspect(const int theTarget)
{
  /*gp_Pnt aPnt = gp_Pnt (myPoints.Value (theTarget - 1));
  if (aPnt.SquareDistance (gp_Pnt (myCurrent)) <= myTol)
    myResInd.Append (theTarget);*/

  const gp_XYZ& aPnt = myPoints.Value(theTarget - 1);
  double        aDx, aDy, aDz;
  aDx = myCurrent.X() - aPnt.X();
  aDy = myCurrent.Y() - aPnt.Y();
  aDz = myCurrent.Z() - aPnt.Z();

  if ((aDx * aDx <= myTol) && (aDy * aDy <= myTol) && (aDz * aDz <= myTol))
    myResInd.Append(theTarget);
  return CellFilter_Keep;
}

//=================================================================================================

const occ::handle<BRepTools_ReShape>& BRepBuilderAPI_Sewing::GetContext() const
{
  return myReShape;
}

//=================================================================================================

void BRepBuilderAPI_Sewing::SetContext(const occ::handle<BRepTools_ReShape>& theContext)
{
  myReShape = theContext;
}
