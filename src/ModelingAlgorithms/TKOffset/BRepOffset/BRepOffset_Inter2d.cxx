// Created on: 1996-09-03
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

//  Modified by skv - Wed Dec 24 18:08:39 2003 OCC4455

#include <BRepOffset_Inter2d.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#include <cstdio>

//=================================================================================================

static TopoDS_Vertex CommonVertex(TopoDS_Edge& E1, TopoDS_Edge& E2)
{
  TopoDS_Vertex V1[2], V2[2], V;
  //
  TopExp::Vertices(E1, V1[0], V1[1], true);
  TopExp::Vertices(E2, V2[0], V2[1], true);
  // The first edge is the current one, the second edge is the next one.
  // We check last vertex of the first edge first.
  if (V1[1].IsSame(V2[0]) || V1[1].IsSame(V2[1]))
    return V1[1];
  if (V1[0].IsSame(V2[0]) || V1[0].IsSame(V2[1]))
    return V1[0];
  //
  return V;
}

static int DefineClosedness(const TopoDS_Face& theFace)
{
  TopExp_Explorer anExplo(theFace, TopAbs_EDGE);
  for (; anExplo.More(); anExplo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExplo.Current());
    if (BRepTools::IsReallyClosed(anEdge, theFace))
    {
      double                    fpar, lpar;
      occ::handle<Geom2d_Curve> aPCurve  = BRep_Tool::CurveOnSurface(anEdge, theFace, fpar, lpar);
      gp_Vec2d                  aTangent = aPCurve->DN(fpar, 1);
      double                    aCrossProd1 = aTangent ^ gp::DX2d();
      double                    aCrossProd2 = aTangent ^ gp::DY2d();
      if (std::abs(aCrossProd2) < std::abs(aCrossProd1)) // pcurve is parallel to OY
        return 1;
      else
        return 2;
    }
  }

  return 0;
}

static void GetEdgesOrientedInFace(const TopoDS_Shape&                 theShape,
                                   const TopoDS_Face&                  theFace,
                                   const occ::handle<BRepAlgo_AsDes>&  theAsDes,
                                   NCollection_Sequence<TopoDS_Shape>& theSeqEdges)
{
  const NCollection_List<TopoDS_Shape>& aEdges = theAsDes->Descendant(theFace);

  TopExp_Explorer anExplo(theShape, TopAbs_EDGE);
  for (; anExplo.More(); anExplo.Next())
  {
    const TopoDS_Shape&                      anEdge = anExplo.Current();
    NCollection_List<TopoDS_Shape>::Iterator itl(aEdges);
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Shape& anEdgeInFace = itl.Value();
      if (anEdgeInFace.IsSame(anEdge))
      {
        theSeqEdges.Append(anEdgeInFace);
        break;
      }
    }
  }

  if (theSeqEdges.Length() == 1)
    return;

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aVEmap;
  for (int ii = 1; ii <= theSeqEdges.Length(); ii++)
    TopExp::MapShapesAndAncestors(theSeqEdges(ii), TopAbs_VERTEX, TopAbs_EDGE, aVEmap);

  TopoDS_Vertex aFirstVertex;
  TopoDS_Edge   aFirstEdge;
  for (int ii = 1; ii <= aVEmap.Extent(); ii++)
  {
    const TopoDS_Vertex&                  aVertex = TopoDS::Vertex(aVEmap.FindKey(ii));
    const NCollection_List<TopoDS_Shape>& aElist  = aVEmap(ii);
    if (aElist.Extent() == 1)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(aElist.First());
      TopoDS_Vertex      aV1, aV2;
      TopExp::Vertices(anEdge, aV1, aV2, true); // with orientation
      if (aV1.IsSame(aVertex))
      {
        aFirstVertex = aVertex;
        aFirstEdge   = anEdge;
        break;
      }
    }
  }

  if (aFirstEdge.IsNull()) // closed set of edges
  {
    // double aPeriod = 0.;
    int IndCoord = DefineClosedness(theFace);
    /*
    BRepAdaptor_Surface aBAsurf (theFace, false);
    if (IndCoord == 1)
      aPeriod = aBAsurf.LastUParameter() - aBAsurf.FirstUParameter();
    else if (IndCoord == 2)
      aPeriod = aBAsurf.LastVParameter() - aBAsurf.FirstVParameter();
    */

    if (IndCoord != 0)
    {
      double aMaxDelta = 0.;
      for (int ii = 1; ii <= aVEmap.Extent(); ii++)
      {
        const TopoDS_Vertex&                  aVertex = TopoDS::Vertex(aVEmap.FindKey(ii));
        const NCollection_List<TopoDS_Shape>& aElist  = aVEmap(ii);
        const TopoDS_Edge&                    anEdge1 = TopoDS::Edge(aElist.First());
        const TopoDS_Edge&                    anEdge2 = TopoDS::Edge(aElist.Last());
        double                                aParam1 = BRep_Tool::Parameter(aVertex, anEdge1);
        double                                aParam2 = BRep_Tool::Parameter(aVertex, anEdge2);
        BRepAdaptor_Curve2d                   aBAcurve1(anEdge1, theFace);
        BRepAdaptor_Curve2d                   aBAcurve2(anEdge2, theFace);
        gp_Pnt2d                              aPnt1 = aBAcurve1.Value(aParam1);
        gp_Pnt2d                              aPnt2 = aBAcurve2.Value(aParam2);
        double aDelta = std::abs(aPnt1.Coord(IndCoord) - aPnt2.Coord(IndCoord));
        if (aDelta > aMaxDelta)
        {
          aMaxDelta    = aDelta;
          aFirstVertex = aVertex;
        }
      }
      const NCollection_List<TopoDS_Shape>&    aElist = aVEmap.FindFromKey(aFirstVertex);
      NCollection_List<TopoDS_Shape>::Iterator itl(aElist);
      for (; itl.More(); itl.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
        TopoDS_Vertex      aV1, aV2;
        TopExp::Vertices(anEdge, aV1, aV2, true); // with orientation
        if (aV1.IsSame(aFirstVertex))
        {
          aFirstEdge = anEdge;
          break;
        }
      }
    }
  }

  int aNbEdges = theSeqEdges.Length();
  theSeqEdges.Clear();
  theSeqEdges.Append(aFirstEdge);
  TopoDS_Edge anEdge = aFirstEdge;
  for (;;)
  {
    TopoDS_Vertex aLastVertex = TopExp::LastVertex(anEdge, true); // with orientation
    if (aLastVertex.IsSame(aFirstVertex))
      break;

    const NCollection_List<TopoDS_Shape>& aElist = aVEmap.FindFromKey(aLastVertex);
    if (aElist.Extent() == 1)
      break;

    if (aElist.First().IsSame(anEdge))
      anEdge = TopoDS::Edge(aElist.Last());
    else
      anEdge = TopoDS::Edge(aElist.First());

    theSeqEdges.Append(anEdge);
    if (theSeqEdges.Length() == aNbEdges)
      break;
  }
}

//=======================================================================
// function : Store
// purpose  : Store the vertices <theLV> into AsDes for the edge <theEdge>.
//           The vertices are added despite of the coincidence with
//           already added vertices. When all vertices for all edges
//           are added the coinciding chains of vertices should be fused
//           using FuseVertices() method.
//=======================================================================
static void Store(
  const TopoDS_Edge&                    theEdge,
  const NCollection_List<TopoDS_Shape>& theLV,
  const double                          theTol,
  const bool                            IsToUpdate,
  const occ::handle<BRepAlgo_AsDes>&    theAsDes2d,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMVV)
{
  // Update vertices
  NCollection_List<TopoDS_Shape>::Iterator aIt(theLV);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aIt.Value());
    BRep_Builder().UpdateVertex(aV, theTol);
  }

  // Get vertices already added to the edge and check the distances to the new ones
  const NCollection_List<TopoDS_Shape>& aLVEx = theAsDes2d->Descendant(theEdge);
  if (!IsToUpdate && aLVEx.IsEmpty())
  {
    if (theLV.Extent())
      theAsDes2d->Add(theEdge, theLV);
    return;
  }
  //
  GeomAPI_ProjectPointOnCurve aProjPC;
  double                      aTolE = 0.0;
  if (IsToUpdate)
  {
    double                         aT1, aT2;
    const occ::handle<Geom_Curve>& aC = BRep_Tool::Curve(theEdge, aT1, aT2);
    aProjPC.Init(aC, aT1, aT2);
    aTolE = BRep_Tool::Tolerance(theEdge);
  }
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMV;
  for (aIt.Init(theLV); aIt.More(); aIt.Next())
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aIt.Value());
    if (!aMV.Add(aV))
    {
      continue;
    }
    //
    const gp_Pnt& aP   = BRep_Tool::Pnt(aV);
    const double  aTol = BRep_Tool::Tolerance(aV);
    //
    NCollection_List<TopoDS_Shape>           aLVC;
    NCollection_List<TopoDS_Shape>::Iterator aItEx(aLVEx);
    for (; aItEx.More(); aItEx.Next())
    {
      const TopoDS_Vertex& aVEx = TopoDS::Vertex(aItEx.Value());
      if (aV.IsSame(aVEx))
      {
        break;
      }
      const gp_Pnt& aPEx    = BRep_Tool::Pnt(aVEx);
      const double  aTolVEx = BRep_Tool::Tolerance(aVEx);
      if (aP.IsEqual(aPEx, aTol + aTolVEx))
      {
        aLVC.Append(aVEx);
      }
    }
    //
    if (aItEx.More())
    {
      continue;
    }
    //
    if (IsToUpdate)
    {
      // get parameter of the vertex on the edge
      aProjPC.Perform(aP);
      if (!aProjPC.NbPoints())
      {
        continue;
      }
      //
      if (aProjPC.LowerDistance() > aTol + aTolE)
      {
        continue;
      }
      //
      double       aT          = aProjPC.LowerDistanceParameter();
      TopoDS_Shape aLocalShape = aV.Oriented(TopAbs_INTERNAL);
      BRep_Builder().UpdateVertex(TopoDS::Vertex(aLocalShape), aT, theEdge, aTol);
    }
    //
    if (aLVC.Extent())
    {
      NCollection_List<TopoDS_Shape>::Iterator aItLV(aLVC);
      for (; aItLV.More(); aItLV.Next())
      {
        const TopoDS_Shape&             aVC = aItLV.Value();
        NCollection_List<TopoDS_Shape>* pLV = theDMVV.ChangeSeek(aVC);
        if (!pLV)
        {
          pLV = &theDMVV(theDMVV.Add(aVC, NCollection_List<TopoDS_Shape>()));
        }
        pLV->Append(aV);
      }
      //
      NCollection_List<TopoDS_Shape>* pLV = theDMVV.ChangeSeek(aV);
      if (!pLV)
      {
        pLV = &theDMVV(theDMVV.Add(aV, NCollection_List<TopoDS_Shape>()));
      }
      pLV->Append(aLVC);
    }
    theAsDes2d->Add(theEdge, aV);
  }
}

//=======================================================================
// function : Store
// purpose  : Store the intersection vertices between two edges into AsDes
//=======================================================================
static void Store(
  const TopoDS_Edge&                    theE1,
  const TopoDS_Edge&                    theE2,
  const NCollection_List<TopoDS_Shape>& theLV1,
  const NCollection_List<TopoDS_Shape>& theLV2,
  const double                          theTol,
  const occ::handle<BRepAlgo_AsDes>&    theAsDes2d,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMVV)
{
  for (int i = 0; i < 2; ++i)
  {
    const TopoDS_Edge&                    aE  = !i ? theE1 : theE2;
    const NCollection_List<TopoDS_Shape>& aLV = !i ? theLV1 : theLV2;
    Store(aE, aLV, theTol, false, theAsDes2d, theDMVV);
  }
}

//=================================================================================================

static void EdgeInter(
  const TopoDS_Face&                 F,
  const BRepAdaptor_Surface&         BAsurf,
  const TopoDS_Edge&                 E1,
  const TopoDS_Edge&                 E2,
  const occ::handle<BRepAlgo_AsDes>& AsDes,
  double                             Tol,
  bool                               WithOri,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    aDMVV)
{

  if (E1.IsSame(E2))
    return;

  double f[3], l[3];
  double TolDub = 1.e-7;
  int    i;

  BRep_Tool::Range(E1, f[1], l[1]);
  BRep_Tool::Range(E2, f[2], l[2]);

  BRepAdaptor_Curve CE1(E1, F);
  BRepAdaptor_Curve CE2(E2, F);

  TopoDS_Edge EI[3];
  EI[1] = E1;
  EI[2] = E2;
  NCollection_List<TopoDS_Shape> LV1;
  NCollection_List<TopoDS_Shape> LV2;
  BRep_Builder                   B;

  TopoDS_Vertex CV;
  if (!TopExp::CommonVertex(E1, E2, CV))
  {
    BRepLib::BuildCurve3d(E1);
    BRepLib::BuildCurve3d(E2);

    double TolSum = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
    TolSum        = std::max(TolSum, 1.e-5);

    NCollection_Sequence<gp_Pnt> ResPoints;
    NCollection_Sequence<double> ResParamsOnE1, ResParamsOnE2;
    gp_Pnt                       DegPoint;
    bool WithDegen = BRep_Tool::Degenerated(E1) || BRep_Tool::Degenerated(E2);

    if (WithDegen)
    {
      int             ideg = (BRep_Tool::Degenerated(E1)) ? 1 : 2;
      TopoDS_Iterator iter(EI[ideg]);
      if (iter.More())
      {
        const TopoDS_Vertex& vdeg = TopoDS::Vertex(iter.Value());
        DegPoint                  = BRep_Tool::Pnt(vdeg);
      }
      else
      {
        BRepAdaptor_Curve CEdeg(EI[ideg], F);
        DegPoint = CEdeg.Value(CEdeg.FirstParameter());
      }
    }
    //
    occ::handle<Geom2d_Curve> pcurve1 = BRep_Tool::CurveOnSurface(E1, F, f[1], l[1]);
    occ::handle<Geom2d_Curve> pcurve2 = BRep_Tool::CurveOnSurface(E2, F, f[2], l[2]);
    Geom2dAdaptor_Curve       GAC1(pcurve1, f[1], l[1]);
    Geom2dAdaptor_Curve       GAC2(pcurve2, f[2], l[2]);
    Geom2dInt_GInter          Inter2d(GAC1, GAC2, TolDub, TolDub);
    for (i = 1; i <= Inter2d.NbPoints(); i++)
    {
      gp_Pnt P3d;
      if (WithDegen)
        P3d = DegPoint;
      else
      {
        gp_Pnt2d P2d = Inter2d.Point(i).Value();
        P3d          = BAsurf.Value(P2d.X(), P2d.Y());
      }
      ResPoints.Append(P3d);
      ResParamsOnE1.Append(Inter2d.Point(i).ParamOnFirst());
      ResParamsOnE2.Append(Inter2d.Point(i).ParamOnSecond());
    }

    for (i = 1; i <= ResPoints.Length(); i++)
    {
      double aT1 = ResParamsOnE1(i); // ponc1.Parameter();
      double aT2 = ResParamsOnE2(i); // ponc2.Parameter();
      if (Precision::IsInfinite(aT1) || Precision::IsInfinite(aT2))
      {
#ifdef OCCT_DEBUG
        std::cout << "Inter2d : Solution rejected due to infinite parameter" << std::endl;
#endif
        continue;
      }

      gp_Pnt        P          = ResPoints(i); // ponc1.Value();
      TopoDS_Vertex aNewVertex = BRepLib_MakeVertex(P);
      aNewVertex.Orientation(TopAbs_INTERNAL);
      B.UpdateVertex(aNewVertex, aT1, E1, Tol);
      B.UpdateVertex(aNewVertex, aT2, E2, Tol);
      gp_Pnt P1 = CE1.Value(aT1);
      gp_Pnt P2 = CE2.Value(aT2);
      double dist1, dist2, dist3;
      dist1 = P1.Distance(P);
      dist2 = P2.Distance(P);
      dist3 = P1.Distance(P2);
      dist1 = std::max(dist1, dist2);
      dist1 = std::max(dist1, dist3);
      B.UpdateVertex(aNewVertex, dist1);

#ifdef OCCT_DEBUG
      if (aT1 < f[1] - Tol || aT1 > l[1] + Tol)
      {
        std::cout << "out of limit" << std::endl;
        std::cout << "aT1 = " << aT1 << ", f[1] = " << f[1] << ", l[1] = " << l[1] << std::endl;
      }
      if (aT2 < f[2] - Tol || aT2 > l[2] + Tol)
      {
        std::cout << "out of limit" << std::endl;
        std::cout << "aT2 = " << aT2 << ", f[2] = " << f[2] << ", l[2] = " << l[2] << std::endl;
      }
      double MilTol2 = 1000 * Tol * Tol;
      if (P1.SquareDistance(P) > MilTol2 || P2.SquareDistance(P) > MilTol2
          || P1.Distance(P2) > 2. * Tol)
      {
        std::cout << "Inter2d : Solution rejected " << std::endl;
        std::cout << "P  = " << P.X() << " " << P.Y() << " " << P.Z() << std::endl;
        std::cout << "P1 = " << P1.X() << " " << P1.Y() << " " << P1.Z() << std::endl;
        std::cout << "P2 = " << P2.X() << " " << P2.Y() << " " << P2.Z() << std::endl;
        std::cout << "MaxDist = " << dist1 << std::endl;
      }
#endif
      // define the orientation of a new vertex
      TopAbs_Orientation OO1 = TopAbs_REVERSED;
      TopAbs_Orientation OO2 = TopAbs_REVERSED;
      if (WithOri)
      {
        BRepAdaptor_Curve2d PCE1(E1, F);
        BRepAdaptor_Curve2d PCE2(E2, F);
        gp_Pnt2d            P2d1, P2d2;
        gp_Vec2d            V1, V2, V1or, V2or;
        PCE1.D1(aT1, P2d1, V1);
        PCE2.D1(aT2, P2d2, V2);
        V1or = V1;
        V2or = V2;
        if (E1.Orientation() == TopAbs_REVERSED)
          V1or.Reverse();
        if (E2.Orientation() == TopAbs_REVERSED)
          V2or.Reverse();
        double CrossProd = V2or ^ V1;
#ifdef OCCT_DEBUG
        if (std::abs(CrossProd) <= gp::Resolution())
          std::cout << std::endl << "CrossProd = " << CrossProd << std::endl;
#endif
        if (CrossProd > 0.)
          OO1 = TopAbs_FORWARD;
        CrossProd = V1or ^ V2;
        if (CrossProd > 0.)
          OO2 = TopAbs_FORWARD;
      }
      LV1.Append(aNewVertex.Oriented(OO1));
      LV2.Append(aNewVertex.Oriented(OO2));
    }
  }

  //----------------------------------
  // Test at end.
  //---------------------------------
  double        U1, U2;
  double        TolConf = Tol;
  TopoDS_Vertex V1[2], V2[2];
  TopExp::Vertices(E1, V1[0], V1[1]);
  TopExp::Vertices(E2, V2[0], V2[1]);

  int j;
  for (j = 0; j < 2; j++)
  {
    if (V1[j].IsNull())
      continue;
    for (int k = 0; k < 2; k++)
    {
      if (V2[k].IsNull())
        continue;
      if (V1[j].IsSame(V2[k]))
      {
        if (AsDes->HasAscendant(V1[j]))
        {
          continue;
        }
      }
      //
      gp_Pnt P1   = BRep_Tool::Pnt(V1[j]);
      gp_Pnt P2   = BRep_Tool::Pnt(V2[k]);
      double Dist = P1.Distance(P2);
      if (Dist < TolConf)
      {
        double        aTol = std::max(BRep_Tool::Tolerance(V1[j]), BRep_Tool::Tolerance(V2[k]));
        TopoDS_Vertex V    = BRepLib_MakeVertex(P1);
        U1                 = (j == 0) ? f[1] : l[1];
        U2                 = (k == 0) ? f[2] : l[2];
        //
        TopoDS_Shape aLocalShape = V.Oriented(TopAbs_INTERNAL);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape), U1, E1, aTol);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape), U2, E2, aTol);
        //
        LV1.Prepend(V.Oriented(V1[j].Orientation()));
        LV2.Prepend(V.Oriented(V2[k].Orientation()));
      }
    }
  }

  bool AffichPurge = false;

  if (!LV1.IsEmpty())
  {
    //----------------------------------
    // Remove all vertices.
    // There can be doubles
    //----------------------------------
    NCollection_List<TopoDS_Shape>::Iterator it1LV1, it1LV2, it2LV1;
    gp_Pnt                                   P1, P2;
    bool                                     Purge = true;

    while (Purge)
    {
      i     = 1;
      Purge = false;
      for (it1LV1.Initialize(LV1), it1LV2.Initialize(LV2); it1LV1.More();
           it1LV1.Next(), it1LV2.Next())
      {
        j = 1;
        it2LV1.Initialize(LV1);
        while (j < i)
        {
          P1 = BRep_Tool::Pnt(TopoDS::Vertex(it1LV1.Value()));
          P2 = BRep_Tool::Pnt(TopoDS::Vertex(it2LV1.Value()));
          //  Modified by skv - Thu Jan 22 18:19:04 2004 OCC4455 Begin
          //           if (P1.IsEqual(P2,10*Tol)) {
          double aTol;

          aTol = std::max(BRep_Tool::Tolerance(TopoDS::Vertex(it1LV1.Value())),
                          BRep_Tool::Tolerance(TopoDS::Vertex(it2LV1.Value())));
          if (P1.IsEqual(P2, aTol))
          {
            //  Modified by skv - Thu Jan 22 18:19:05 2004 OCC4455 End
            LV1.Remove(it1LV1);
            LV2.Remove(it1LV2);
            if (AffichPurge)
              std::cout << "Doubles removed in EdgeInter." << std::endl;
            Purge = true;
            break;
          }
          j++;
          it2LV1.Next();
        }
        if (Purge)
          break;
        i++;
      }
    }
    //---------------------------------
    // Vertex storage in DS.
    //---------------------------------
    double TolStore = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
    TolStore        = std::max(TolStore, Tol);
    Store(E1, E2, LV1, LV2, TolStore, AsDes, aDMVV);
  }
}

//=================================================================================================

static void RefEdgeInter(
  const TopoDS_Face&                 F,
  const BRepAdaptor_Surface&         BAsurf,
  const TopoDS_Edge&                 E1,
  const TopoDS_Edge&                 E2,
  const TopAbs_Orientation           theOr1,
  const TopAbs_Orientation           theOr2,
  const occ::handle<BRepAlgo_AsDes>& AsDes,
  double                             Tol,
  bool                               WithOri,
  const TopoDS_Vertex&               theVref,
  BRepAlgo_Image&                    theImageVV,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
        aDMVV,
  bool& theCoincide)
{
  //
  theCoincide = false;
  //
  if (E1.IsSame(E2))
    return;
  if (E1.IsNull() || E2.IsNull())
    return;

  double f[3], l[3];
  double TolDub = 1.e-7, TolLL = 0.0;
  int    i;

  occ::handle<Geom2d_Curve> pcurve1 = BRep_Tool::CurveOnSurface(E1, F, f[1], l[1]);
  occ::handle<Geom2d_Curve> pcurve2 = BRep_Tool::CurveOnSurface(E2, F, f[2], l[2]);
  if (pcurve1.IsNull() || pcurve2.IsNull())
    return;

  BRepAdaptor_Curve CE1(E1, F);
  BRepAdaptor_Curve CE2(E2, F);

  TopoDS_Edge EI[3];
  EI[1] = E1;
  EI[2] = E2;
  NCollection_List<TopoDS_Shape> LV1;
  NCollection_List<TopoDS_Shape> LV2;
  BRep_Builder                   B;

  BRepLib::BuildCurve3d(E1);
  BRepLib::BuildCurve3d(E2);

  NCollection_Sequence<gp_Pnt> ResPoints;
  NCollection_Sequence<double> ResParamsOnE1, ResParamsOnE2;
  gp_Pnt                       DegPoint;
  bool                         WithDegen = BRep_Tool::Degenerated(E1) || BRep_Tool::Degenerated(E2);

  if (WithDegen)
  {
    int             ideg = (BRep_Tool::Degenerated(E1)) ? 1 : 2;
    TopoDS_Iterator iter(EI[ideg]);
    if (iter.More())
    {
      const TopoDS_Vertex& vdeg = TopoDS::Vertex(iter.Value());
      DegPoint                  = BRep_Tool::Pnt(vdeg);
    }
    else
    {
      BRepAdaptor_Curve CEdeg(EI[ideg], F);
      DegPoint = CEdeg.Value(CEdeg.FirstParameter());
    }
  }
  //
  Geom2dAdaptor_Curve GAC1(pcurve1, f[1], l[1]);
  Geom2dAdaptor_Curve GAC2(pcurve2, f[2], l[2]);
  if ((GAC1.GetType() == GeomAbs_Line) && (GAC2.GetType() == GeomAbs_Line))
  {
    // Just quickly check if lines coincide
    double anAngle = std::abs(GAC1.Line().Direction().Angle(GAC2.Line().Direction()));
    if (anAngle <= 1.e-8 || M_PI - anAngle <= 1.e-8)
    {
      theCoincide = true;
      return;
    }
    else
    {
      // Take into account the intersection range of line-line intersection
      // (the smaller angle between curves, the bigger range)
      TolLL = IntTools_Tools::ComputeIntRange(TolDub, TolDub, anAngle);
      TolLL = std::min(TolLL, 1.e-5);
    }
  }

  Geom2dInt_GInter Inter2d(GAC1, GAC2, TolDub, TolDub);
  //
  if (!Inter2d.IsDone() || !Inter2d.NbPoints())
  {
    theCoincide = (Inter2d.NbSegments() && (GAC1.GetType() == GeomAbs_Line)
                   && (GAC2.GetType() == GeomAbs_Line));
    return;
  }
  //
  for (i = 1; i <= Inter2d.NbPoints(); i++)
  {
    gp_Pnt P3d;
    if (WithDegen)
      P3d = DegPoint;
    else
    {
      gp_Pnt2d P2d = Inter2d.Point(i).Value();
      P3d          = BAsurf.Value(P2d.X(), P2d.Y());
    }
    ResPoints.Append(P3d);
    ResParamsOnE1.Append(Inter2d.Point(i).ParamOnFirst());
    ResParamsOnE2.Append(Inter2d.Point(i).ParamOnSecond());
  }

  for (i = 1; i <= ResPoints.Length(); i++)
  {
    double aT1 = ResParamsOnE1(i); // ponc1.Parameter();
    double aT2 = ResParamsOnE2(i); // ponc2.Parameter();
    if (Precision::IsInfinite(aT1) || Precision::IsInfinite(aT2))
    {
#ifdef OCCT_DEBUG
      std::cout << "Inter2d : Solution rejected due to infinite parameter" << std::endl;
#endif
      continue;
    }

    gp_Pnt        P          = ResPoints(i); // ponc1.Value();
    TopoDS_Vertex aNewVertex = BRepLib_MakeVertex(P);
    aNewVertex.Orientation(TopAbs_INTERNAL);
    B.UpdateVertex(aNewVertex, aT1, E1, Tol);
    B.UpdateVertex(aNewVertex, aT2, E2, Tol);
    gp_Pnt P1 = CE1.Value(aT1);
    gp_Pnt P2 = CE2.Value(aT2);
    double dist1, dist2, dist3;
    dist1 = P1.Distance(P);
    dist2 = P2.Distance(P);
    dist3 = P1.Distance(P2);
    dist1 = std::max(dist1, dist2);
    dist1 = std::max(dist1, dist3);
    B.UpdateVertex(aNewVertex, dist1);

#ifdef OCCT_DEBUG
    if (aT1 < f[1] - Tol || aT1 > l[1] + Tol)
    {
      std::cout << "out of limit" << std::endl;
      std::cout << "aT1 = " << aT1 << ", f[1] = " << f[1] << ", l[1] = " << l[1] << std::endl;
    }
    if (aT2 < f[2] - Tol || aT2 > l[2] + Tol)
    {
      std::cout << "out of limit" << std::endl;
      std::cout << "aT2 = " << aT2 << ", f[2] = " << f[2] << ", l[2] = " << l[2] << std::endl;
    }
    double MilTol2 = 1000 * Tol * Tol;
    if (P1.SquareDistance(P) > MilTol2 || P2.SquareDistance(P) > MilTol2
        || P1.Distance(P2) > 2. * Tol)
    {
      std::cout << "Inter2d : Solution rejected" << std::endl;
      std::cout << "P  = " << P.X() << " " << P.Y() << " " << P.Z() << std::endl;
      std::cout << "P1 = " << P1.X() << " " << P1.Y() << " " << P1.Z() << std::endl;
      std::cout << "P2 = " << P2.X() << " " << P2.Y() << " " << P2.Z() << std::endl;
      std::cout << "MaxDist = " << dist1 << std::endl;
    }
#endif
    // define the orientation of a new vertex
    TopAbs_Orientation OO1 = TopAbs_REVERSED;
    TopAbs_Orientation OO2 = TopAbs_REVERSED;
    if (WithOri)
    {
      BRepAdaptor_Curve2d PCE1(E1, F);
      BRepAdaptor_Curve2d PCE2(E2, F);
      gp_Pnt2d            P2d1, P2d2;
      gp_Vec2d            V1, V2, V1or, V2or;
      PCE1.D1(aT1, P2d1, V1);
      PCE2.D1(aT2, P2d2, V2);
      V1or = V1;
      V2or = V2;
      if (E1.Orientation() == TopAbs_REVERSED)
        V1or.Reverse();
      if (E2.Orientation() == TopAbs_REVERSED)
        V2or.Reverse();
      double CrossProd = V2or ^ V1;
#ifdef OCCT_DEBUG
      if (std::abs(CrossProd) <= gp::Resolution())
        std::cout << std::endl << "CrossProd = " << CrossProd << std::endl;
#endif
      if (CrossProd > 0.)
        OO1 = TopAbs_FORWARD;
      CrossProd = V1or ^ V2;
      if (CrossProd > 0.)
        OO2 = TopAbs_FORWARD;
    }

    if (theOr1 != TopAbs_EXTERNAL)
      OO1 = theOr1;
    if (theOr2 != TopAbs_EXTERNAL)
      OO2 = theOr2;

    LV1.Append(aNewVertex.Oriented(OO1));
    LV2.Append(aNewVertex.Oriented(OO2));
  }

  //----------------------------------
  // Test at end.
  //---------------------------------
  double        U1, U2;
  double        TolConf = Tol;
  TopoDS_Vertex V1[2], V2[2];
  TopExp::Vertices(E1, V1[0], V1[1]);
  TopExp::Vertices(E2, V2[0], V2[1]);

  int j;
  for (j = 0; j < 2; j++)
  {
    if (V1[j].IsNull())
      continue;
    for (int k = 0; k < 2; k++)
    {
      if (V2[k].IsNull())
        continue;
      if (V1[j].IsSame(V2[k]))
      {
        if (AsDes->HasAscendant(V1[j]))
        {
          continue;
        }
      }
      //
      gp_Pnt P1   = BRep_Tool::Pnt(V1[j]);
      gp_Pnt P2   = BRep_Tool::Pnt(V2[k]);
      double Dist = P1.Distance(P2);
      if (Dist < TolConf)
      {
        TopoDS_Vertex V          = BRepLib_MakeVertex(P1);
        U1                       = (j == 0) ? f[1] : l[1];
        U2                       = (k == 0) ? f[2] : l[2];
        TopoDS_Shape aLocalShape = V.Oriented(TopAbs_INTERNAL);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape), U1, E1, Tol);
        B.UpdateVertex(TopoDS::Vertex(aLocalShape), U2, E2, Tol);
        LV1.Prepend(V.Oriented(V1[j].Orientation()));
        LV2.Prepend(V.Oriented(V2[k].Orientation()));
      }
    }
  }

  bool AffichPurge = false;

  if (!LV1.IsEmpty())
  {
    //----------------------------------
    // Remove all vertices.
    // there can be doubles
    //----------------------------------
    NCollection_List<TopoDS_Shape>::Iterator it1LV1, it1LV2, it2LV1;
    gp_Pnt                                   P1, P2;
    bool                                     Purge = true;

    while (Purge)
    {
      i     = 1;
      Purge = false;
      for (it1LV1.Initialize(LV1), it1LV2.Initialize(LV2); it1LV1.More();
           it1LV1.Next(), it1LV2.Next())
      {
        j = 1;
        it2LV1.Initialize(LV1);
        while (j < i)
        {
          P1 = BRep_Tool::Pnt(TopoDS::Vertex(it1LV1.Value()));
          P2 = BRep_Tool::Pnt(TopoDS::Vertex(it2LV1.Value()));
          if (P1.IsEqual(P2, Tol))
          {
            LV1.Remove(it1LV1);
            LV2.Remove(it1LV2);
            if (AffichPurge)
              std::cout << "Doubles removed in EdgeInter." << std::endl;
            Purge = true;
            break;
          }
          j++;
          it2LV1.Next();
        }
        if (Purge)
          break;
        i++;
      }
    }
    //---------------------------------
    // Vertex storage in SD.
    //---------------------------------
    ////-----------------------------------------------------
    if (LV1.Extent() > 1)
    {
      // std::cout << "IFV - RefEdgeInter: remove vertex" << std::endl;
      gp_Pnt        Pref = BRep_Tool::Pnt(theVref);
      double        dmin = RealLast();
      TopoDS_Vertex Vmin;
      for (it1LV1.Initialize(LV1); it1LV1.More(); it1LV1.Next())
      {
        gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(it1LV1.Value()));
        double d = P.SquareDistance(Pref);
        if (d < dmin)
        {
          dmin = d;
          Vmin = TopoDS::Vertex(it1LV1.Value());
        }
      }
      for (it1LV1.Initialize(LV1), it1LV2.Initialize(LV2); it1LV1.More();
           it1LV1.Next(), it1LV2.Next())
      {
        if (!Vmin.IsSame(it1LV1.Value()))
        {
          LV1.Remove(it1LV1);
          LV2.Remove(it1LV2);
          if (!it1LV1.More())
            break;
        }
      }
    }

    NCollection_List<TopoDS_Shape>::Iterator itl(LV1);
    for (; itl.More(); itl.Next())
    {
      TopoDS_Shape aNewVertex = itl.Value();
      aNewVertex.Orientation(TopAbs_FORWARD);
      if (theImageVV.HasImage(theVref))
        theImageVV.Add(theVref.Oriented(TopAbs_FORWARD), aNewVertex);
      else
        theImageVV.Bind(theVref.Oriented(TopAbs_FORWARD), aNewVertex);
    }

    ////-----------------------------------------------------
    double TolStore = BRep_Tool::Tolerance(E1) + BRep_Tool::Tolerance(E2);
    TolStore        = std::max(TolStore, Tol);
    // Compare to Line-Line tolerance
    TolStore = std::max(TolStore, TolLL);
    Store(E1, E2, LV1, LV2, TolStore, AsDes, aDMVV);
  }
}

//======================================================================
// function : EvaluateMaxSegment
// purpose  : return MaxSegment to pass in approximation
//======================================================================

static int evaluateMaxSegment(const Adaptor3d_CurveOnSurface& aCurveOnSurface)
{
  const occ::handle<Adaptor3d_Surface>& aSurf   = aCurveOnSurface.GetSurface();
  const occ::handle<Adaptor2d_Curve2d>& aCurv2d = aCurveOnSurface.GetCurve();

  double aNbSKnots = 0, aNbC2dKnots = 0;

  if (aSurf->GetType() == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_BSplineSurface> aBSpline = aSurf->BSpline();
    aNbSKnots = std::max(aBSpline->NbUKnots(), aBSpline->NbVKnots());
  }
  if (aCurv2d->GetType() == GeomAbs_BSplineCurve)
  {
    aNbC2dKnots = aCurv2d->NbKnots();
  }
  int aReturn = (int)(30 + std::max(aNbSKnots, aNbC2dKnots));
  return aReturn;
}

//=================================================================================================

static bool ExtendPCurve(const occ::handle<Geom2d_Curve>& aPCurve,
                         const double                     anEf,
                         const double                     anEl,
                         const double                     a2Offset,
                         occ::handle<Geom2d_Curve>&       NewPCurve)
{
  NewPCurve = aPCurve;
  if (NewPCurve->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    NewPCurve = occ::down_cast<Geom2d_TrimmedCurve>(NewPCurve)->BasisCurve();

  double FirstPar = NewPCurve->FirstParameter();
  double LastPar  = NewPCurve->LastParameter();

  if (NewPCurve->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve))
      && (FirstPar > anEf - a2Offset || LastPar < anEl + a2Offset))
  {
    if (NewPCurve->IsInstance(STANDARD_TYPE(Geom2d_BezierCurve)))
    {
      occ::handle<Geom2d_BezierCurve> aBezier = occ::down_cast<Geom2d_BezierCurve>(NewPCurve);
      if (aBezier->NbPoles() == 2)
      {
        const NCollection_Array1<gp_Pnt2d>& thePoles = aBezier->Poles();
        gp_Vec2d aVec(thePoles(1), thePoles(2));
        NewPCurve = new Geom2d_Line(thePoles(1), aVec);
        return true;
      }
    }
    else if (NewPCurve->IsInstance(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      occ::handle<Geom2d_BSplineCurve> aBSpline = occ::down_cast<Geom2d_BSplineCurve>(NewPCurve);
      if (aBSpline->NbKnots() == 2 && aBSpline->NbPoles() == 2)
      {
        const NCollection_Array1<gp_Pnt2d>& thePoles = aBSpline->Poles();
        gp_Vec2d aVec(thePoles(1), thePoles(2));
        NewPCurve = new Geom2d_Line(thePoles(1), aVec);
        return true;
      }
    }
  }

  FirstPar                                  = aPCurve->FirstParameter();
  LastPar                                   = aPCurve->LastParameter();
  occ::handle<Geom2d_TrimmedCurve> aTrCurve = new Geom2d_TrimmedCurve(aPCurve, FirstPar, LastPar);

  // The curve is not prolonged on begin or end.
  // Trying to prolong it adding a segment to its bound.
  gp_Pnt2d                              aPBnd;
  gp_Vec2d                              aVBnd;
  gp_Pnt2d                              aPBeg;
  gp_Dir2d                              aDBnd;
  occ::handle<Geom2d_Line>              aLin;
  occ::handle<Geom2d_TrimmedCurve>      aSegment;
  Geom2dConvert_CompCurveToBSplineCurve aCompCurve(aTrCurve, Convert_RationalC1);
  constexpr double                      aTol   = Precision::Confusion();
  double                                aDelta = std::max(a2Offset, 1.);

  if (FirstPar > anEf - a2Offset)
  {
    aPCurve->D1(FirstPar, aPBnd, aVBnd);
    aDBnd.SetXY(aVBnd.XY());
    aPBeg    = aPBnd.Translated(gp_Vec2d(-aDelta * aDBnd.XY()));
    aLin     = new Geom2d_Line(aPBeg, aDBnd);
    aSegment = new Geom2d_TrimmedCurve(aLin, 0, aDelta);

    if (!aCompCurve.Add(aSegment, aTol))
      return false;
  }

  if (LastPar < anEl + a2Offset)
  {
    aPCurve->D1(LastPar, aPBeg, aVBnd);
    aDBnd.SetXY(aVBnd.XY());
    aLin     = new Geom2d_Line(aPBeg, aDBnd);
    aSegment = new Geom2d_TrimmedCurve(aLin, 0, aDelta);

    if (!aCompCurve.Add(aSegment, aTol))
      return false;
  }

  NewPCurve = aCompCurve.BSplineCurve();
  return true;
}

//=================================================================================================

//  Modified by skv - Fri Dec 26 17:00:55 2003 OCC4455 Begin
// static void ExtentEdge(const TopoDS_Edge& E,TopoDS_Edge& NE)
bool BRepOffset_Inter2d::ExtentEdge(const TopoDS_Edge& E, TopoDS_Edge& NE, const double theOffset)
{
  // BRepLib::BuildCurve3d(E);

  TopoDS_Shape aLocalShape = E.EmptyCopied();
  double       anEf;
  double       anEl;
  double       a2Offset = 2. * std::abs(theOffset);
  BRep_Builder BB;
  int          i, j;

  BRep_Tool::Range(E, anEf, anEl);
  NE = TopoDS::Edge(aLocalShape);
  //  NE = TopoDS::Edge(E.EmptyCopied());
  // Enough for analytic edges, for general case reconstruct the
  // geometry of the edge recalculating the intersection of surfaces.

  // BRepLib::BuildCurve3d(E);

  int                       NbPCurves    = 0;
  double                    FirstParOnPC = RealFirst(), LastParOnPC = RealLast();
  occ::handle<Geom2d_Curve> MinPC;
  occ::handle<Geom_Surface> MinSurf;
  TopLoc_Location           MinLoc;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itr(
    (occ::down_cast<BRep_TEdge>(NE.TShape()))->ChangeCurves());
  for (; itr.More(); itr.Next())
  {
    occ::handle<BRep_CurveRepresentation> CurveRep = itr.Value();
    double                                FirstPar, LastPar;
    if (CurveRep->IsCurveOnSurface())
    {
      NbPCurves++;
      occ::handle<Geom2d_Curve> theCurve = CurveRep->PCurve();
      FirstPar                           = theCurve->FirstParameter();
      LastPar                            = theCurve->LastParameter();

      if (theCurve->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve))
          && (FirstPar > anEf - a2Offset || LastPar < anEl + a2Offset))
      {
        occ::handle<Geom2d_Curve> NewPCurve;
        if (ExtendPCurve(theCurve, anEf, anEl, a2Offset, NewPCurve))
        {
          CurveRep->PCurve(NewPCurve);
          FirstPar = NewPCurve->FirstParameter();
          LastPar  = NewPCurve->LastParameter();
          if (CurveRep->IsCurveOnClosedSurface())
          {
            occ::handle<Geom2d_Curve> PCurve2 = CurveRep->PCurve2();
            if (ExtendPCurve(PCurve2, anEf, anEl, a2Offset, NewPCurve))
              CurveRep->PCurve2(NewPCurve);
          }
        }
      }
      else if (theCurve->IsPeriodic())
      {
        double delta = (theCurve->Period() - (anEl - anEf)) * 0.5;
        delta *= 0.95;
        FirstPar = anEf - delta;
        LastPar  = anEl + delta;
      }
      else if (theCurve->IsClosed())
        LastPar -= 0.05 * (LastPar - FirstPar);

      // check FirstPar and LastPar: the pcurve should be in its surface
      theCurve                          = CurveRep->PCurve();
      occ::handle<Geom_Surface> theSurf = CurveRep->Surface();
      double                    Umin, Umax, Vmin, Vmax;
      theSurf->Bounds(Umin, Umax, Vmin, Vmax);
      NCollection_Sequence<occ::handle<Geom2d_Curve>> BoundLines;
      if (!Precision::IsInfinite(Vmin))
      {
        occ::handle<Geom2d_Line> aLine =
          new Geom2d_Line(gp_Pnt2d(0., Vmin), gp_Dir2d(gp_Dir2d::D::X));
        BoundLines.Append(aLine);
      }
      if (!Precision::IsInfinite(Umin))
      {
        occ::handle<Geom2d_Line> aLine =
          new Geom2d_Line(gp_Pnt2d(Umin, 0.), gp_Dir2d(gp_Dir2d::D::Y));
        BoundLines.Append(aLine);
      }
      if (!Precision::IsInfinite(Vmax))
      {
        occ::handle<Geom2d_Line> aLine =
          new Geom2d_Line(gp_Pnt2d(0., Vmax), gp_Dir2d(gp_Dir2d::D::X));
        BoundLines.Append(aLine);
      }
      if (!Precision::IsInfinite(Umax))
      {
        occ::handle<Geom2d_Line> aLine =
          new Geom2d_Line(gp_Pnt2d(Umax, 0.), gp_Dir2d(gp_Dir2d::D::Y));
        BoundLines.Append(aLine);
      }

      NCollection_Sequence<double> params;
      Geom2dInt_GInter             IntCC;
      Geom2dAdaptor_Curve          GAcurve(theCurve);
      for (i = 1; i <= BoundLines.Length(); i++)
      {
        Geom2dAdaptor_Curve GAline(BoundLines(i));
        IntCC.Perform(GAcurve, GAline, Precision::PConfusion(), Precision::PConfusion());
        if (IntCC.IsDone())
        {
          for (j = 1; j <= IntCC.NbPoints(); j++)
          {
            const IntRes2d_IntersectionPoint& ip     = IntCC.Point(j);
            gp_Pnt2d                          aPoint = ip.Value();
            if (aPoint.X() >= Umin && aPoint.X() <= Umax && aPoint.Y() >= Vmin
                && aPoint.Y() <= Vmax)
              params.Append(ip.ParamOnFirst());
          }
          for (j = 1; j <= IntCC.NbSegments(); j++)
          {
            const IntRes2d_IntersectionSegment& is = IntCC.Segment(j);
            if (is.HasFirstPoint())
            {
              const IntRes2d_IntersectionPoint& ip     = is.FirstPoint();
              gp_Pnt2d                          aPoint = ip.Value();
              if (aPoint.X() >= Umin && aPoint.X() <= Umax && aPoint.Y() >= Vmin
                  && aPoint.Y() <= Vmax)
                params.Append(ip.ParamOnFirst());
            }
            if (is.HasLastPoint())
            {
              const IntRes2d_IntersectionPoint& ip     = is.LastPoint();
              gp_Pnt2d                          aPoint = ip.Value();
              if (aPoint.X() >= Umin && aPoint.X() <= Umax && aPoint.Y() >= Vmin
                  && aPoint.Y() <= Vmax)
                params.Append(ip.ParamOnFirst());
            }
          }
        }
      }
      if (!params.IsEmpty())
      {
        if (params.Length() == 1)
        {
          gp_Pnt2d PntFirst = theCurve->Value(FirstPar);
          if (PntFirst.X() >= Umin && PntFirst.X() <= Umax && PntFirst.Y() >= Vmin
              && PntFirst.Y() <= Vmax)
          {
            if (LastPar > params(1))
              LastPar = params(1);
          }
          else if (FirstPar < params(1))
            FirstPar = params(1);
        }
        else
        {
          double fpar = RealLast(), lpar = RealFirst();
          for (i = 1; i <= params.Length(); i++)
          {
            if (params(i) < fpar)
              fpar = params(i);
            if (params(i) > lpar)
              lpar = params(i);
          }
          if (FirstPar < fpar)
            FirstPar = fpar;
          if (LastPar > lpar)
            LastPar = lpar;
        }
      }
      //// end of check ////
      (occ::down_cast<BRep_GCurve>(CurveRep))->SetRange(FirstPar, LastPar);
      // gp_Pnt2d Pfirst = theCurve->Value(FirstPar);
      // gp_Pnt2d Plast  = theCurve->Value(LastPar);
      //(occ::down_cast<BRep_CurveOnSurface>(CurveRep))->SetUVPoints( Pfirst, Plast );

      // update FirstParOnPC and LastParOnPC
      if (FirstPar > FirstParOnPC)
      {
        FirstParOnPC = FirstPar;
        MinPC        = theCurve;
        MinSurf      = theSurf;
        MinLoc       = CurveRep->Location();
      }
      if (LastPar < LastParOnPC)
      {
        LastParOnPC = LastPar;
        MinPC       = theCurve;
        MinSurf     = theSurf;
        MinLoc      = CurveRep->Location();
      }
    }
  }

  double                  f, l;
  occ::handle<Geom_Curve> C3d = BRep_Tool::Curve(NE, f, l);
  if (NbPCurves)
  {
    MinLoc = E.Location() * MinLoc;
    if (!C3d.IsNull())
    {
      if (MinPC->IsClosed())
      {
        f = FirstParOnPC;
        l = LastParOnPC;
      }
      else if (C3d->IsPeriodic())
      {
        double delta = (C3d->Period() - (l - f)) * 0.5;
        delta *= 0.95;
        f -= delta;
        l += delta;
      }
      else if (C3d->IsClosed())
        l -= 0.05 * (l - f);
      else
      {
        f = FirstParOnPC;
        l = LastParOnPC;
        GeomAPI_ProjectPointOnCurve Projector;
        if (!Precision::IsInfinite(FirstParOnPC))
        {
          gp_Pnt2d P2d1 = MinPC->Value(FirstParOnPC);
          gp_Pnt   P1   = MinSurf->Value(P2d1.X(), P2d1.Y());
          P1.Transform(MinLoc.Transformation());
          Projector.Init(P1, C3d);
          if (Projector.NbPoints() > 0)
            f = Projector.LowerDistanceParameter();
#ifdef OCCT_DEBUG
          else
            std::cout << "ProjectPointOnCurve not done" << std::endl;
#endif
        }
        if (!Precision::IsInfinite(LastParOnPC))
        {
          gp_Pnt2d P2d2 = MinPC->Value(LastParOnPC);
          gp_Pnt   P2   = MinSurf->Value(P2d2.X(), P2d2.Y());
          P2.Transform(MinLoc.Transformation());
          Projector.Init(P2, C3d);
          if (Projector.NbPoints() > 0)
            l = Projector.LowerDistanceParameter();
#ifdef OCCT_DEBUG
          else
            std::cout << "ProjectPointOnCurve not done" << std::endl;
#endif
        }
      }
      BB.Range(NE, f, l);
      if (!Precision::IsInfinite(f) && !Precision::IsInfinite(l))
        BRepLib::SameParameter(NE, Precision::Confusion(), true);
    }
    else if (!BRep_Tool::Degenerated(E)) // no 3d curve
    {
      MinSurf = occ::down_cast<Geom_Surface>(MinSurf->Transformed(MinLoc.Transformation()));
      double max_deviation = 0.;
      if (Precision::IsInfinite(FirstParOnPC) || Precision::IsInfinite(LastParOnPC))
      {
        if (MinPC->IsInstance(STANDARD_TYPE(Geom2d_Line)))
        {
          bool IsLine = false;
          if (MinSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
            IsLine = true;
          else if (MinSurf->IsInstance(STANDARD_TYPE(Geom_CylindricalSurface))
                   || MinSurf->IsInstance(STANDARD_TYPE(Geom_ConicalSurface)))
          {
            occ::handle<Geom2d_Line> theLine = occ::down_cast<Geom2d_Line>(MinPC);
            gp_Dir2d                 LineDir = theLine->Direction();
            if (LineDir.IsParallel(gp::DY2d(), Precision::Angular()))
              IsLine = true;
          }
          if (IsLine)
          {
            gp_Pnt2d P2d1 = MinPC->Value(0.), P2d2 = MinPC->Value(1.);
            gp_Pnt   P1 = MinSurf->Value(P2d1.X(), P2d1.Y());
            gp_Pnt   P2 = MinSurf->Value(P2d2.X(), P2d2.Y());
            gp_Vec   aVec(P1, P2);
            C3d = new Geom_Line(P1, aVec);
          }
        }
      }
      else
      {
        Geom2dAdaptor_Curve              AC2d(MinPC, FirstParOnPC, LastParOnPC);
        GeomAdaptor_Surface              GAsurf(MinSurf);
        occ::handle<Geom2dAdaptor_Curve> HC2d  = new Geom2dAdaptor_Curve(AC2d);
        occ::handle<GeomAdaptor_Surface> HSurf = new GeomAdaptor_Surface(GAsurf);
        Adaptor3d_CurveOnSurface         ConS(HC2d, HSurf);
        double /*max_deviation,*/        average_deviation;
        GeomAbs_Shape                    Continuity = GeomAbs_C1;
        int                              MaxDegree  = 14;
        int                              MaxSegment = evaluateMaxSegment(ConS);
        GeomLib::BuildCurve3d(Precision::Confusion(),
                              ConS,
                              FirstParOnPC,
                              LastParOnPC,
                              C3d,
                              max_deviation,
                              average_deviation,
                              Continuity,
                              MaxDegree,
                              MaxSegment);
      }
      BB.UpdateEdge(NE, C3d, max_deviation);
      // BB.Range( NE, FirstParOnPC, LastParOnPC );
      bool ProjectionSuccess = true;
      if (NbPCurves > 1)
        // BRepLib::SameParameter( NE, Precision::Confusion(), true );
        for (itr.Initialize((occ::down_cast<BRep_TEdge>(NE.TShape()))->ChangeCurves()); itr.More();
             itr.Next())
        {
          occ::handle<BRep_CurveRepresentation> CurveRep = itr.Value();
          double                                FirstPar, LastPar;
          if (CurveRep->IsCurveOnSurface())
          {
            occ::handle<Geom2d_Curve> theCurve = CurveRep->PCurve();
            occ::handle<Geom_Surface> theSurf  = CurveRep->Surface();
            TopLoc_Location           theLoc   = CurveRep->Location();
            if (theCurve == MinPC && theSurf == MinSurf && theLoc == MinLoc)
              continue;
            FirstPar = (occ::down_cast<BRep_GCurve>(CurveRep))->First();
            LastPar  = (occ::down_cast<BRep_GCurve>(CurveRep))->Last();
            if (std::abs(FirstPar - FirstParOnPC) > Precision::PConfusion()
                || std::abs(LastPar - LastParOnPC) > Precision::PConfusion())
            {
              theLoc  = E.Location() * theLoc;
              theSurf = occ::down_cast<Geom_Surface>(theSurf->Transformed(theLoc.Transformation()));

              if (theCurve->IsInstance(STANDARD_TYPE(Geom2d_Line))
                  && theSurf->IsKind(STANDARD_TYPE(Geom_BoundedSurface)))
              {
                gp_Dir2d theDir = occ::down_cast<Geom2d_Line>(theCurve)->Direction();
                if (theDir.IsParallel(gp::DX2d(), Precision::Angular())
                    || theDir.IsParallel(gp::DY2d(), Precision::Angular()))
                {
                  double U1, U2, V1, V2;
                  theSurf->Bounds(U1, U2, V1, V2);
                  gp_Pnt2d Origin = occ::down_cast<Geom2d_Line>(theCurve)->Location();
                  if (std::abs(Origin.X() - U1) <= Precision::Confusion()
                      || std::abs(Origin.X() - U2) <= Precision::Confusion()
                      || std::abs(Origin.Y() - V1) <= Precision::Confusion()
                      || std::abs(Origin.Y() - V2) <= Precision::Confusion())
                  {
                    BRepLib::SameParameter(NE, Precision::Confusion(), true);
                    break;
                  }
                }
              }
              if (!C3d.IsNull() && FirstParOnPC < LastParOnPC)
              {
                occ::handle<Geom2d_Curve> ProjPCurve =
                  GeomProjLib::Curve2d(C3d, FirstParOnPC, LastParOnPC, theSurf);
                if (ProjPCurve.IsNull())
                  ProjectionSuccess = false;
                else
                  CurveRep->PCurve(ProjPCurve);
              }
              else
              {
                return false;
              }
            }
          }
        }
      if (ProjectionSuccess)
        BB.Range(NE, FirstParOnPC, LastParOnPC);
      else
      {
        BB.Range(NE, FirstParOnPC, LastParOnPC, true);
        BRepLib::SameParameter(NE, Precision::Confusion(), true);
      }
    }
  }
  else // no pcurves
  {
    double FirstPar = C3d->FirstParameter();
    double LastPar  = C3d->LastParameter();

    if (C3d->IsKind(STANDARD_TYPE(Geom_BoundedCurve))
        && (FirstPar > anEf - a2Offset || LastPar < anEl + a2Offset))
    {
      occ::handle<Geom_TrimmedCurve> aTrCurve = new Geom_TrimmedCurve(C3d, FirstPar, LastPar);

      // The curve is not prolonged on begin or end.
      // Trying to prolong it adding a segment to its bound.
      gp_Pnt                              aPBnd;
      gp_Vec                              aVBnd;
      gp_Pnt                              aPBeg;
      gp_Dir                              aDBnd;
      occ::handle<Geom_Line>              aLin;
      occ::handle<Geom_TrimmedCurve>      aSegment;
      GeomConvert_CompCurveToBSplineCurve aCompCurve(aTrCurve, Convert_RationalC1);
      constexpr double                    aTol   = Precision::Confusion();
      double                              aDelta = std::max(a2Offset, 1.);

      if (FirstPar > anEf - a2Offset)
      {
        C3d->D1(FirstPar, aPBnd, aVBnd);
        aDBnd.SetXYZ(aVBnd.XYZ());
        aPBeg    = aPBnd.Translated(gp_Vec(-aDelta * aDBnd.XYZ()));
        aLin     = new Geom_Line(aPBeg, aDBnd);
        aSegment = new Geom_TrimmedCurve(aLin, 0, aDelta);

        if (!aCompCurve.Add(aSegment, aTol))
          return true;
      }

      if (LastPar < anEl + a2Offset)
      {
        C3d->D1(LastPar, aPBeg, aVBnd);
        aDBnd.SetXYZ(aVBnd.XYZ());
        aLin     = new Geom_Line(aPBeg, aDBnd);
        aSegment = new Geom_TrimmedCurve(aLin, 0, aDelta);

        if (!aCompCurve.Add(aSegment, aTol))
          return true;
      }

      C3d      = aCompCurve.BSplineCurve();
      FirstPar = C3d->FirstParameter();
      LastPar  = C3d->LastParameter();
      BB.UpdateEdge(NE, C3d, Precision::Confusion());
    }
    else if (C3d->IsPeriodic())
    {
      double delta = (C3d->Period() - (anEl - anEf)) * 0.5;
      delta *= 0.95;
      FirstPar = anEf - delta;
      LastPar  = anEl + delta;
    }
    else if (C3d->IsClosed())
      LastPar -= 0.05 * (LastPar - FirstPar);

    BB.Range(NE, FirstPar, LastPar);
  }
  return true;
}

//  Modified by skv - Fri Dec 26 17:00:57 2003 OCC4455 End

//=================================================================================================

static bool UpdateVertex(const TopoDS_Vertex& V, TopoDS_Edge& OE, TopoDS_Edge& NE, double TolConf)
{
  BRepAdaptor_Curve OC(OE);
  BRepAdaptor_Curve NC(NE);
  double            Of     = OC.FirstParameter();
  double            Ol     = OC.LastParameter();
  double            Nf     = NC.FirstParameter();
  double            Nl     = NC.LastParameter();
  double            U      = 0.;
  constexpr double  ParTol = Precision::PConfusion();
  gp_Pnt            P      = BRep_Tool::Pnt(V);
  bool              OK     = false;

  if (P.Distance(OC.Value(Of)) < TolConf)
  {
    if (Of >= Nf + ParTol && Of <= Nl + ParTol && P.Distance(NC.Value(Of)) < TolConf)
    {
      OK = true;
      U  = Of;
    }
  }
  if (P.Distance(OC.Value(Ol)) < TolConf)
  {
    if (Ol >= Nf + ParTol && Ol <= Nl + ParTol && P.Distance(NC.Value(Ol)) < TolConf)
    {
      OK = true;
      U  = Ol;
    }
  }
  if (OK)
  {
    BRep_Builder B;
    TopoDS_Shape aLocalShape = NE.Oriented(TopAbs_FORWARD);
    TopoDS_Edge  EE          = TopoDS::Edge(aLocalShape);
    //    TopoDS_Edge EE = TopoDS::Edge(NE.Oriented(TopAbs_FORWARD));
    aLocalShape = V.Oriented(TopAbs_INTERNAL);
    B.UpdateVertex(TopoDS::Vertex(aLocalShape), U, NE, BRep_Tool::Tolerance(NE));
    //    B.UpdateVertex(TopoDS::Vertex(V.Oriented(TopAbs_INTERNAL)),
    //                   U,NE,BRep_Tool::Tolerance(NE));
  }
  return OK;
}

//=================================================================================================

void BRepOffset_Inter2d::Compute(
  const occ::handle<BRepAlgo_AsDes>&                                   AsDes,
  const TopoDS_Face&                                                   F,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges,
  const double                                                         Tol,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEdgeIntEdges,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                               theDMVV,
  const Message_ProgressRange& theRange)
{

  // Do not intersect the edges of face
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> EdgesOfFace;
  TopExp_Explorer                                        Explo(F, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
    EdgesOfFace.Add(Explo.Current());

  //-----------------------------------------------------------
  // calculate intersections2d on faces touched by
  // intersection3d
  //---------------------------------------------------------
  NCollection_List<TopoDS_Shape>::Iterator it1LE;
  NCollection_List<TopoDS_Shape>::Iterator it2LE;

  //-----------------------------------------------
  // Intersection of edges 2*2.
  //-----------------------------------------------
  const NCollection_List<TopoDS_Shape>& LE = AsDes->Descendant(F);
  TopoDS_Vertex                         V1, V2;
  int                                   j, i = 1;
  BRepAdaptor_Surface                   BAsurf(F);
  //
  Message_ProgressScope aPS(theRange, "Intersecting edges on faces", LE.Size());
  for (it1LE.Initialize(LE); it1LE.More(); it1LE.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Edge& E1 = TopoDS::Edge(it1LE.Value());
    j                     = 1;
    it2LE.Initialize(LE);

    while (j < i && it2LE.More())
    {
      const TopoDS_Edge& E2 = TopoDS::Edge(it2LE.Value());

      bool ToIntersect = true;
      if (theEdgeIntEdges.IsBound(E1))
      {
        const NCollection_List<TopoDS_Shape>&    aElist = theEdgeIntEdges(E1);
        NCollection_List<TopoDS_Shape>::Iterator itedges(aElist);
        for (; itedges.More(); itedges.Next())
          if (E2.IsSame(itedges.Value()))
            ToIntersect = false;

        if (ToIntersect)
        {
          for (itedges.Initialize(aElist); itedges.More(); itedges.Next())
          {
            const TopoDS_Shape& anEdge = itedges.Value();
            if (theEdgeIntEdges.IsBound(anEdge))
            {
              const NCollection_List<TopoDS_Shape>&    aElist2 = theEdgeIntEdges(anEdge);
              NCollection_List<TopoDS_Shape>::Iterator itedges2(aElist2);
              for (; itedges2.More(); itedges2.Next())
                if (E2.IsSame(itedges2.Value()))
                  ToIntersect = false;
            }
          }
        }
      }

      //--------------------------------------------------------------
      // Intersections of New edges obtained by intersection
      // between them and with edges of restrictions
      //------------------------------------------------------
      if (ToIntersect && (!EdgesOfFace.Contains(E1) || !EdgesOfFace.Contains(E2))
          && (NewEdges.Contains(E1) || NewEdges.Contains(E2)))
      {

        TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);
        EdgeInter(TopoDS::Face(aLocalShape), BAsurf, E1, E2, AsDes, Tol, true, theDMVV);
        //          EdgeInter(TopoDS::Face(F.Oriented(TopAbs_FORWARD)),E1,E2,AsDes,Tol,true);
      }
      it2LE.Next();
      j++;
    }
    i++;
  }
}

//=================================================================================================

bool BRepOffset_Inter2d::ConnexIntByInt(
  const TopoDS_Face&                                                              FI,
  BRepOffset_Offset&                                                              OFI,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&       MES,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& Build,
  const occ::handle<BRepAlgo_AsDes>&                                              theAsDes,
  const occ::handle<BRepAlgo_AsDes>&                                              AsDes2d,
  const double                                                                    Offset,
  const double                                                                    Tol,
  const BRepOffset_Analyse&                                                       Analyse,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&                  FacesWithVerts,
  BRepAlgo_Image&                                                                 theImageVV,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEdgeIntEdges,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                               theDMVV,
  const Message_ProgressRange& theRange)
{

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> MVE;
  BRepOffset_Tool::MapVertexEdges(FI, MVE);
  Message_ProgressScope aPS(theRange,
                            "Intersecting edges obtained as intersection of faces",
                            1,
                            true);
  //---------------------
  // Extension of edges.
  //---------------------
  TopoDS_Edge NE;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator it(MVE);
  for (; it.More(); it.Next())
  {
    if (!aPS.More())
    {
      return false;
    }
    const NCollection_List<TopoDS_Shape>&    L       = it.Value();
    bool                                     YaBuild = false;
    NCollection_List<TopoDS_Shape>::Iterator itL(L);
    for (; itL.More(); itL.Next())
    {
      YaBuild = Build.IsBound(itL.Value());
      if (YaBuild)
        break;
    }
    if (YaBuild)
    {
      for (itL.Initialize(L); itL.More(); itL.Next())
      {
        const TopoDS_Edge& EI = TopoDS::Edge(itL.Value());
        if (EI.Orientation() != TopAbs_FORWARD && EI.Orientation() != TopAbs_REVERSED)
          continue;
        TopoDS_Shape       aLocalShape = OFI.Generated(EI);
        const TopoDS_Edge& OE          = TopoDS::Edge(aLocalShape);
        if (!MES.IsBound(OE) && !Build.IsBound(EI))
        {
          if (!ExtentEdge(OE, NE, Offset))
          {
            return false;
          }
          MES.Bind(OE, NE);
        }
      }
    }
  }

  TopoDS_Face FIO = TopoDS::Face(OFI.Face());
  if (MES.IsBound(FIO))
    FIO = TopoDS::Face(MES(FIO));
  //
  BRepAdaptor_Surface BAsurf(FIO);

  TopExp_Explorer exp(FI.Oriented(TopAbs_FORWARD), TopAbs_WIRE);
  for (; exp.More(); exp.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return false;
    }
    const TopoDS_Wire&     W = TopoDS::Wire(exp.Current());
    BRepTools_WireExplorer wexp;
    bool                   end = false;
    TopoDS_Edge            FirstE, CurE, NextE;

    TopoDS_Shape aLocalWire = W.Oriented(TopAbs_FORWARD);
    TopoDS_Shape aLocalFace = FI.Oriented(TopAbs_FORWARD);
    wexp.Init(TopoDS::Wire(aLocalWire), TopoDS::Face(aLocalFace));
    if (!wexp.More())
      continue; // Protection from case when explorer does not contain edges.
    CurE = FirstE = wexp.Current();
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Edges;

    while (!end)
    {
      wexp.Next();
      if (wexp.More())
      {
        NextE = wexp.Current();
      }
      else
      {
        NextE = FirstE;
        end   = true;
      }
      if (CurE.IsSame(NextE))
        continue;

      TopoDS_Vertex Vref = CommonVertex(CurE, NextE);

      CurE  = Analyse.EdgeReplacement(FI, CurE);
      NextE = Analyse.EdgeReplacement(FI, NextE);

      TopoDS_Shape aLocalShape = OFI.Generated(CurE);
      TopoDS_Edge  CEO         = TopoDS::Edge(aLocalShape);
      aLocalShape              = OFI.Generated(NextE);
      TopoDS_Edge NEO          = TopoDS::Edge(aLocalShape);
      //------------------------------------------
      // Inter processing of images of CurE NextE.
      //------------------------------------------
      NCollection_List<TopoDS_Shape>     LV1, LV2;
      bool                               DoInter = true;
      TopoDS_Shape                       NE1, NE2;
      NCollection_Sequence<TopoDS_Shape> NE1seq, NE2seq;
      TopAbs_Orientation                 anOr1 = TopAbs_EXTERNAL, anOr2 = TopAbs_EXTERNAL;

      int aChoice = 0;
      if (Build.IsBound(CurE) && Build.IsBound(NextE))
      {
        aChoice = 1;
        NE1     = Build(CurE);
        NE2     = Build(NextE);
        GetEdgesOrientedInFace(NE1, FIO, theAsDes, NE1seq);
        GetEdgesOrientedInFace(NE2, FIO, theAsDes, NE2seq);
        anOr1 = TopAbs_REVERSED;
        anOr2 = TopAbs_FORWARD;
      }
      else if (Build.IsBound(CurE) && MES.IsBound(NEO))
      {
        aChoice = 2;
        NE1     = Build(CurE);
        NE2     = MES(NEO);
        NE2.Orientation(NextE.Orientation());
        GetEdgesOrientedInFace(NE1, FIO, theAsDes, NE1seq);
        NE2seq.Append(NE2);
        anOr1 = TopAbs_REVERSED;
        anOr2 = TopAbs_FORWARD;
      }
      else if (Build.IsBound(NextE) && MES.IsBound(CEO))
      {
        aChoice = 3;
        NE1     = Build(NextE);
        NE2     = MES(CEO);
        NE2.Orientation(CurE.Orientation());
        GetEdgesOrientedInFace(NE1, FIO, theAsDes, NE1seq);
        NE2seq.Append(NE2);
        anOr1 = TopAbs_FORWARD;
        anOr2 = TopAbs_REVERSED;
      }
      else
      {
        DoInter = false;
      }
      if (DoInter)
      {
        //------------------------------------
        // NE1,NE2 can be a compound of Edges.
        //------------------------------------
        bool        bCoincide;
        TopoDS_Edge aE1, aE2;
        if (aChoice == 1 || aChoice == 2)
        {
          aE1 = TopoDS::Edge(NE1seq.Last());
          aE2 = TopoDS::Edge(NE2seq.First());
        }
        else // aChoice == 3
        {
          aE1 = TopoDS::Edge(NE1seq.First());
          aE2 = TopoDS::Edge(NE2seq.Last());
        }

        if (aE1.Orientation() == TopAbs_REVERSED)
          anOr1 = TopAbs::Reverse(anOr1);
        if (aE2.Orientation() == TopAbs_REVERSED)
          anOr2 = TopAbs::Reverse(anOr2);

        RefEdgeInter(FIO,
                     BAsurf,
                     aE1,
                     aE2,
                     anOr1,
                     anOr2,
                     AsDes2d,
                     Tol,
                     true,
                     Vref,
                     theImageVV,
                     theDMVV,
                     bCoincide);

        if (theEdgeIntEdges.IsBound(aE1))
          theEdgeIntEdges(aE1).Append(aE2);
        else
        {
          NCollection_List<TopoDS_Shape> aElist;
          aElist.Append(aE2);
          theEdgeIntEdges.Bind(aE1, aElist);
        }
        if (theEdgeIntEdges.IsBound(aE2))
          theEdgeIntEdges(aE2).Append(aE1);
        else
        {
          NCollection_List<TopoDS_Shape> aElist;
          aElist.Append(aE1);
          theEdgeIntEdges.Bind(aE2, aElist);
        }

        //
        // check if some of the offset edges have been
        // generated out of the common vertex
        if (Build.IsBound(Vref))
        {
          FacesWithVerts.Add(FI);
        }
      }
      else
      {
        TopoDS_Vertex V = CommonVertex(CEO, NEO);
        if (!V.IsNull())
        {
          if (MES.IsBound(CEO))
          {
            UpdateVertex(V, CEO, TopoDS::Edge(MES(CEO)), Tol);
            AsDes2d->Add(MES(CEO), V);
          }
          if (MES.IsBound(NEO))
          {
            UpdateVertex(V, NEO, TopoDS::Edge(MES(NEO)), Tol);
            AsDes2d->Add(MES(NEO), V);
          }
        }
      }
      CurE = wexp.Current();
    }
  }
  return true;
}

//=======================================================================
// function : ConnexIntByIntInVert
// purpose  : Intersection of the edges generated out of vertices
//=======================================================================
void BRepOffset_Inter2d::ConnexIntByIntInVert(
  const TopoDS_Face&                                                              FI,
  BRepOffset_Offset&                                                              OFI,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&       MES,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& Build,
  const occ::handle<BRepAlgo_AsDes>&                                              AsDes,
  const occ::handle<BRepAlgo_AsDes>&                                              AsDes2d,
  const double                                                                    Tol,
  const BRepOffset_Analyse&                                                       Analyse,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                               theDMVV,
  const Message_ProgressRange& theRange)
{
  TopoDS_Face FIO = TopoDS::Face(OFI.Face());
  if (MES.IsBound(FIO))
    FIO = TopoDS::Face(MES(FIO));
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aME;
  const NCollection_List<TopoDS_Shape>&                  aLE = AsDes->Descendant(FIO);
  NCollection_List<TopoDS_Shape>::Iterator               aItLE(aLE);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aE = aItLE.Value();
    aME.Add(aE);
  }
  //
  BRepAdaptor_Surface BAsurf(FIO);
  //
  Message_ProgressScope aPS(theRange, "Intersecting edges created from vertices", 1, true);
  TopExp_Explorer       exp(FI.Oriented(TopAbs_FORWARD), TopAbs_WIRE);
  for (; exp.More(); exp.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Wire& W = TopoDS::Wire(exp.Current());
    //
    BRepTools_WireExplorer wexp;
    bool                   end = false;
    TopoDS_Edge            FirstE, CurE, NextE;
    //
    TopoDS_Shape aLocalWire = W.Oriented(TopAbs_FORWARD);
    TopoDS_Shape aLocalFace = FI.Oriented(TopAbs_FORWARD);
    wexp.Init(TopoDS::Wire(aLocalWire), TopoDS::Face(aLocalFace));
    if (!wexp.More())
      continue; // Protection from case when explorer does not contain edges.
    //
    CurE = FirstE = wexp.Current();
    while (!end)
    {
      wexp.Next();
      if (wexp.More())
      {
        NextE = wexp.Current();
      }
      else
      {
        NextE = FirstE;
        end   = true;
      }
      if (CurE.IsSame(NextE))
        continue;
      //
      TopoDS_Vertex Vref = CommonVertex(CurE, NextE);
      if (!Build.IsBound(Vref))
      {
        CurE = NextE;
        continue;
      }

      CurE  = Analyse.EdgeReplacement(FI, CurE);
      NextE = Analyse.EdgeReplacement(FI, NextE);

      TopoDS_Shape aLocalShape = OFI.Generated(CurE);
      TopoDS_Edge  CEO         = TopoDS::Edge(aLocalShape);
      aLocalShape              = OFI.Generated(NextE);
      TopoDS_Edge NEO          = TopoDS::Edge(aLocalShape);
      //
      TopoDS_Shape       NE1, NE2;
      TopAbs_Orientation anOr1 = TopAbs_EXTERNAL, anOr2 = TopAbs_EXTERNAL;

      if (Build.IsBound(CurE) && Build.IsBound(NextE))
      {
        NE1 = Build(CurE);
        NE2 = Build(NextE);
      }
      else if (Build.IsBound(CurE) && MES.IsBound(NEO))
      {
        NE1 = Build(CurE);
        NE2 = MES(NEO);
      }
      else if (Build.IsBound(NextE) && MES.IsBound(CEO))
      {
        NE1 = Build(NextE);
        NE2 = MES(CEO);
      }
      else
      {
        CurE = wexp.Current();
        continue;
      }
      //
      TopExp_Explorer Exp1, Exp2;
      bool            bCoincide;
      // intersect edges generated from vertex with the edges of the face
      const TopoDS_Shape& NE3 = Build(Vref);
      //
      for (Exp2.Init(NE3, TopAbs_EDGE); Exp2.More(); Exp2.Next())
      {
        const TopoDS_Edge& aE3 = *(TopoDS_Edge*)&Exp2.Current();
        if (!aME.Contains(aE3))
        {
          continue;
        }
        //
        // intersection with first edge
        for (Exp1.Init(NE1, TopAbs_EDGE); Exp1.More(); Exp1.Next())
        {
          const TopoDS_Edge& aE1 = TopoDS::Edge(Exp1.Current());
          BRepAlgo_Image     anEmptyImage;
          RefEdgeInter(FIO,
                       BAsurf,
                       aE1,
                       aE3,
                       anOr1,
                       anOr2,
                       AsDes2d,
                       Tol,
                       true,
                       Vref,
                       anEmptyImage,
                       theDMVV,
                       bCoincide);
          if (bCoincide)
          {
            // in case of coincidence trim the edge E3 the same way as E1
            Store(aE3, AsDes2d->Descendant(aE1), Tol, true, AsDes2d, theDMVV);
          }
        }
        //
        // intersection with second edge
        for (Exp1.Init(NE2, TopAbs_EDGE); Exp1.More(); Exp1.Next())
        {
          const TopoDS_Edge& aE2 = TopoDS::Edge(Exp1.Current());
          BRepAlgo_Image     anEmptyImage;
          RefEdgeInter(FIO,
                       BAsurf,
                       aE2,
                       aE3,
                       anOr1,
                       anOr2,
                       AsDes2d,
                       Tol,
                       true,
                       Vref,
                       anEmptyImage,
                       theDMVV,
                       bCoincide);
          if (bCoincide)
          {
            // in case of coincidence trim the edge E3 the same way as E2
            Store(aE3, AsDes2d->Descendant(aE2), Tol, true, AsDes2d, theDMVV);
          }
        }
        //
        // intersection of the edges generated from vertex
        // among themselves
        for (Exp1.Init(NE3, TopAbs_EDGE); Exp1.More(); Exp1.Next())
        {
          if (aE3.IsSame(Exp1.Current()))
          {
            break;
          }
        }
        //
        for (Exp1.Next(); Exp1.More(); Exp1.Next())
        {
          const TopoDS_Edge& aE3Next = TopoDS::Edge(Exp1.Current());
          if (aME.Contains(aE3Next))
          {
            BRepAlgo_Image anEmptyImage;
            RefEdgeInter(FIO,
                         BAsurf,
                         aE3Next,
                         aE3,
                         anOr1,
                         anOr2,
                         AsDes2d,
                         Tol,
                         true,
                         Vref,
                         anEmptyImage,
                         theDMVV,
                         bCoincide);
          }
        }
      }
      CurE = wexp.Current();
    }
  }
}

//=================================================================================================

static void MakeChain(const TopoDS_Shape&                                        theV,
                      const NCollection_IndexedDataMap<TopoDS_Shape,
                                                       NCollection_List<TopoDS_Shape>,
                                                       TopTools_ShapeMapHasher>& theDMVV,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&    theMDone,
                      NCollection_List<TopoDS_Shape>&                            theChain)
{
  if (theMDone.Add(theV))
  {
    theChain.Append(theV);
    const NCollection_List<TopoDS_Shape>* pLV = theDMVV.Seek(theV);
    if (pLV)
    {
      NCollection_List<TopoDS_Shape>::Iterator aIt(*pLV);
      for (; aIt.More(); aIt.Next())
      {
        MakeChain(aIt.Value(), theDMVV, theMDone, theChain);
      }
    }
  }
}

//=================================================================================================

bool BRepOffset_Inter2d::FuseVertices(
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& theDMVV,
  const occ::handle<BRepAlgo_AsDes>&                         theAsDes,
  BRepAlgo_Image&                                            theImageVV)
{
  BRep_Builder                                           aBB;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVDone;
  int                                                    i, aNb = theDMVV.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(theDMVV.FindKey(i));
    //
    // find chain of vertices
    NCollection_List<TopoDS_Shape> aLVChain;
    MakeChain(aV, theDMVV, aMVDone, aLVChain);
    //
    if (aLVChain.Extent() < 2)
    {
      continue;
    }
    //
    // make new vertex
    TopoDS_Vertex aVNew;
    BOPTools_AlgoTools::MakeVertex(aLVChain, aVNew);
    //
    TopoDS_Vertex aVNewInt = TopoDS::Vertex(aVNew.Oriented(TopAbs_INTERNAL));
    //
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLVChain);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aVOld = aIt.Value();
      // update the parameters on edges
      TopoDS_Vertex aVOldInt                    = TopoDS::Vertex(aVOld.Oriented(TopAbs_INTERNAL));
      const NCollection_List<TopoDS_Shape>& aLE = theAsDes->Ascendant(aVOld);
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLE(aLE);
      for (; aItLE.More(); aItLE.Next())
      {
        const TopoDS_Edge& aE    = TopoDS::Edge(aItLE.Value());
        double             aTolE = BRep_Tool::Tolerance(aE);
        double             aT;
        if (!BRep_Tool::Parameter(aVOldInt, aE, aT))
        {
          return false;
        }
        aBB.UpdateVertex(aVNewInt, aT, aE, aTolE);
      }
      // and replace the vertex
      theAsDes->Replace(aVOld, aVNew);
      if (theImageVV.IsImage(aVOld))
      {
        const TopoDS_Vertex& aProVertex = TopoDS::Vertex(theImageVV.ImageFrom(aVOld));
        theImageVV.Add(aProVertex, aVNew.Oriented(TopAbs_FORWARD));
      }
    }
  }
  return true;
}
