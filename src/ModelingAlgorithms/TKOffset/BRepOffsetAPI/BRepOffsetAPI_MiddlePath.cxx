// Created on: 2012-08-06
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepOffsetAPI_MiddlePath.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GC_MakeCircle.hxx>
#include <GCE2d_MakeLine.hxx>
#include <gce_MakeLin.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

static bool IsLinear(const TopoDS_Edge& anEdge, gp_Lin& aLine)
{
  double                  fpar, lpar;
  occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
  if (aCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    aCurve = occ::down_cast<Geom_TrimmedCurve>(aCurve)->BasisCurve();

  gp_Pnt Pnt1, Pnt2;
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
  {
    aLine = occ::down_cast<Geom_Line>(aCurve)->Lin();
    return true;
  }
  else if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
  {
    occ::handle<Geom_BezierCurve> theBezier = occ::down_cast<Geom_BezierCurve>(aCurve);
    if (theBezier->NbPoles() == 2)
    {
      Pnt1  = theBezier->Pole(1);
      Pnt2  = theBezier->Pole(2);
      aLine = gce_MakeLin(Pnt1, Pnt2);
      return true;
    }
  }
  else if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> theBSpline = occ::down_cast<Geom_BSplineCurve>(aCurve);
    if (theBSpline->NbPoles() == 2)
    {
      Pnt1  = theBSpline->Pole(1);
      Pnt2  = theBSpline->Pole(2);
      aLine = gce_MakeLin(Pnt1, Pnt2);
      return true;
    }
  }

  return false;
}

static GeomAbs_CurveType TypeOfEdge(const TopoDS_Edge& anEdge)
{
  gp_Lin aLin;
  if (IsLinear(anEdge, aLin))
    return GeomAbs_Line;

  BRepAdaptor_Curve BAcurve(anEdge);
  return BAcurve.GetType();
}

static gp_Vec TangentOfEdge(const TopoDS_Shape& aShape, const bool OnFirst)
{
  TopoDS_Edge        anEdge = TopoDS::Edge(aShape);
  TopAbs_Orientation anOr   = anEdge.Orientation();

  double                  fpar, lpar;
  occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
  double                  thePar;
  if (OnFirst)
    thePar = (anOr == TopAbs_FORWARD) ? fpar : lpar;
  else
    thePar = (anOr == TopAbs_FORWARD) ? lpar : fpar;

  gp_Pnt thePoint;
  gp_Vec theTangent;
  aCurve->D1(thePar, thePoint, theTangent);
  if (anOr == TopAbs_REVERSED)
    theTangent.Reverse();

  return theTangent;
}

static bool IsValidEdge(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace)
{
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(theEdge, V1, V2);

  constexpr double Tol = Precision::Confusion();
  int              i;

  TopExp_Explorer Explo(theFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Shape&        anEdge = Explo.Current();
    BRepExtrema_DistShapeShape DistMini(theEdge, anEdge);
    if (DistMini.Value() <= Tol)
    {
      for (i = 1; i <= DistMini.NbSolution(); i++)
      {
        BRepExtrema_SupportType theType = DistMini.SupportTypeShape2(i);
        if (theType == BRepExtrema_IsOnEdge)
          return false;
        // theType is "IsVertex"
        TopoDS_Shape aVertex = DistMini.SupportOnShape2(i);
        if (!(aVertex.IsSame(V1) || aVertex.IsSame(V2)))
          return false;
      }
    }
  }

  return true;
}

/*
//=================================================================================================

BRepOffsetAPI_MiddlePath::BRepOffsetAPI_MiddlePath(const TopoDS_Shape& aShape,
                                                   const TopoDS_Wire&  StartWire)
{
  myInitialShape = aShape;
  myStartWire    = StartWire;
  myClosedSection = myStartWire.Closed();
}

//=================================================================================================

BRepOffsetAPI_MiddlePath::BRepOffsetAPI_MiddlePath(const TopoDS_Shape& aShape,
                                                   const TopoDS_Edge&  StartEdge)
{
  myInitialShape = aShape;

  BRepLib_MakeWire MW(StartEdge);

  //BB.Add(myStartWire, StartEdge);

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
EFmap; NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
TopTools_ShapeMapHasher> VEmap; TopExp::MapShapesAndAncestors(myInitialShape, TopAbs_EDGE,
TopAbs_FACE, EFmap); TopExp::MapShapesAndAncestors(myInitialShape, TopAbs_VERTEX, TopAbs_EDGE,
VEmap);

  //bool Start = true;
  //if (Start)
  //{
  //TopExp::Vertices(CurEdge, V1, V2);
  //  StartVertex = V1;
  //  CurVertex   = V2;
  //  if (VEmap(CurVertex).Extent() == 2) //end: two free edges
  //  {
  //    StartVertex = V2;
  //    CurVertex   = V1;
  //    if (VEmap(CurVertex).Extent() == 2) //end: two free edges
  //      break;
  //  }
  //  Start = false;
  //  continue;
  //}

  TopoDS_Vertex StartVertex, CurVertex, V1, V2;
  TopExp::Vertices(StartEdge, StartVertex, CurVertex);
  TopoDS_Edge CurEdge = StartEdge;
  int i;
  for (i = 1; i <= 2; i++)
  {
    for (;;)
    {
      const NCollection_List<TopoDS_Shape>& LE = VEmap.FindFromKey(CurVertex);
      if (LE.Extent() == 2) //end: two free edges or one closed free edge
        break;
      NCollection_List<TopoDS_Shape>::Iterator itl(LE);
      TopoDS_Edge anEdge;
      for (; itl.More(); itl.Next())
      {
        anEdge = TopoDS::Edge(itl.Value());
        if (anEdge.IsSame(CurEdge))
          continue;
        if (EFmap.FindFromKey(anEdge).Extent() == 1) //another free edge found
          break;
      }
      //BB.Add(myStartWire, anEdge);
      MW.Add(anEdge);
      TopExp::Vertices(anEdge, V1, V2);
      CurVertex = (V1.IsSame(CurVertex))? V2 : V1;
      CurEdge = anEdge;
      if (CurVertex.IsSame(StartVertex))
        break;
    }
    if (CurVertex.IsSame(StartVertex))
      break;
    CurVertex = StartVertex;
    CurEdge = StartEdge;
  }

  myStartWire = MW.Wire();
  myClosedSection = myStartWire.Closed();
}
*/

//=================================================================================================

static TopoDS_Wire GetUnifiedWire(const TopoDS_Wire&            theWire,
                                  ShapeUpgrade_UnifySameDomain& theUnifier)
{
  BRepLib_MakeWire                                       aWMaker;
  BRepTools_WireExplorer                                 wexp(theWire);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aGeneratedEdges;
  for (; wexp.More(); wexp.Next())
  {
    TopoDS_Shape                          anEdge = wexp.Current();
    const NCollection_List<TopoDS_Shape>& aLS    = theUnifier.History()->Modified(anEdge);
    if (!aLS.IsEmpty())
    {
      NCollection_List<TopoDS_Shape>::Iterator anIt(aLS);
      for (; anIt.More(); anIt.Next())
      {
        const TopoDS_Shape& aShape = anIt.Value();
        // wire shouldn't contain duplicated generated edges
        if (aGeneratedEdges.Add(aShape))
          aWMaker.Add(TopoDS::Edge(aShape));
      }
    }
    else
    {
      // no change, put original edge
      aWMaker.Add(TopoDS::Edge(anEdge));
    }
  }
  return aWMaker.Wire();
}

//=================================================================================================

BRepOffsetAPI_MiddlePath::BRepOffsetAPI_MiddlePath(const TopoDS_Shape& aShape,
                                                   const TopoDS_Shape& StartShape,
                                                   const TopoDS_Shape& EndShape)
{
  ShapeUpgrade_UnifySameDomain Unifier(aShape);
  Unifier.Build();
  myInitialShape = Unifier.Shape();

  TopoDS_Wire aStartWire, anEndWire;
  if (StartShape.ShapeType() == TopAbs_FACE)
  {
    const TopoDS_Face& StartFace = TopoDS::Face(StartShape);
    aStartWire                   = BRepTools::OuterWire(StartFace);
  }
  else
    aStartWire = TopoDS::Wire(StartShape);

  if (EndShape.ShapeType() == TopAbs_FACE)
  {
    const TopoDS_Face& EndFace = TopoDS::Face(EndShape);
    anEndWire                  = BRepTools::OuterWire(EndFace);
  }
  else
    anEndWire = TopoDS::Wire(EndShape);

  myStartWire = GetUnifiedWire(aStartWire, Unifier);
  myEndWire   = GetUnifiedWire(anEndWire, Unifier);

  myClosedSection = myStartWire.Closed();
  myClosedRing    = myStartWire.IsSame(myEndWire);
}

//=================================================================================================

void BRepOffsetAPI_MiddlePath::Build(const Message_ProgressRange& /*theRange*/)
{
  NCollection_List<TopoDS_Shape>::Iterator itl;

  NCollection_Sequence<TopoDS_Shape>                       StartVertices;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>   EndVertices;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>   EndEdges;
  NCollection_Sequence<NCollection_Sequence<TopoDS_Shape>> SectionsEdges;

  BRepTools_WireExplorer             wexp(myStartWire);
  NCollection_Sequence<TopoDS_Shape> EdgeSeq;
  for (; wexp.More(); wexp.Next())
  {
    StartVertices.Append(wexp.CurrentVertex());
    EdgeSeq.Append(wexp.Current());
  }
  if (!myClosedSection)
    StartVertices.Append(wexp.CurrentVertex());
  SectionsEdges.Append(EdgeSeq);

  for (wexp.Init(myEndWire); wexp.More(); wexp.Next())
  {
    EndVertices.Add(wexp.CurrentVertex());
    EndEdges.Add(wexp.Current());
  }
  if (!myClosedSection)
    EndVertices.Add(wexp.CurrentVertex());

  TopoDS_Iterator itw(myStartWire);
  for (; itw.More(); itw.Next())
    myStartWireEdges.Add(itw.Value());
  for (itw.Initialize(myEndWire); itw.More(); itw.Next())
    myEndWireEdges.Add(itw.Value());

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    VEmap;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    EFmap;
  TopExp::MapShapesAndAncestors(myInitialShape, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
  TopExp::MapShapesAndAncestors(myInitialShape, TopAbs_EDGE, TopAbs_FACE, EFmap);

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> CurVertices;

  int           i, j, k;
  TopoDS_Edge   anEdge;
  TopoDS_Vertex V1, V2, NextVertex;
  // Initialization of <myPaths>
  for (i = 1; i <= StartVertices.Length(); i++)
  {
    NCollection_Sequence<TopoDS_Shape>    Edges;
    const NCollection_List<TopoDS_Shape>& LE = VEmap.FindFromKey(StartVertices(i));
    for (itl.Initialize(LE); itl.More(); itl.Next())
    {
      anEdge = TopoDS::Edge(itl.Value());
      if (!myStartWireEdges.Contains(anEdge))
      {
        TopExp::Vertices(anEdge, V1, V2, true);
        if (V1.IsSame(StartVertices(i)))
          CurVertices.Add(V2);
        else
        {
          anEdge.Reverse();
          CurVertices.Add(V1);
        }
        Edges.Append(anEdge);
        break;
      }
    }
    if (!Edges.IsEmpty())
      myPaths.Append(Edges);
    else
      return;
  }

  // Filling of "myPaths"
  NCollection_List<TopoDS_Shape> NextVertices;
  for (;;)
  {
    for (i = 1; i <= myPaths.Length(); i++)
    {
      const TopoDS_Shape& theShape = myPaths(i).Last();
      TopoDS_Edge         theEdge;
      TopoDS_Vertex       theVertex;
      if (theShape.ShapeType() == TopAbs_EDGE)
      {
        theEdge   = TopoDS::Edge(theShape);
        theVertex = TopExp::LastVertex(theEdge, true);
      }
      else // last segment of path was punctual
      {
        theEdge   = TopoDS::Edge(myPaths(i)(myPaths(i).Length() - 1));
        theVertex = TopoDS::Vertex(theShape);
      }

      if (EndVertices.Contains(theVertex))
        continue;
      const NCollection_List<TopoDS_Shape>&                  LE = VEmap.FindFromKey(theVertex);
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> NextEdgeCandidates;
      for (itl.Initialize(LE); itl.More(); itl.Next())
      {
        anEdge = TopoDS::Edge(itl.Value());
        if (anEdge.IsSame(theEdge))
          continue;
        TopExp::Vertices(anEdge, V1, V2, true);
        if (V1.IsSame(theVertex))
          NextVertex = V2;
        else
        {
          anEdge.Reverse();
          NextVertex = V1;
        }
        if (!CurVertices.Contains(NextVertex))
          NextEdgeCandidates.Add(anEdge);
      }
      if (!NextEdgeCandidates.IsEmpty())
      {
        if (NextEdgeCandidates.Extent() > 1)
          myPaths(i).Append(theVertex); // punctual segment of path
        else
        {
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator mapit(
            NextEdgeCandidates);
          anEdge = TopoDS::Edge(mapit.Key());
          myPaths(i).Append(anEdge);
          NextVertex = TopExp::LastVertex(anEdge, true);
          NextVertices.Append(NextVertex);
        }
      }
    }
    if (NextVertices.IsEmpty())
      break;
    for (itl.Initialize(NextVertices); itl.More(); itl.Next())
      CurVertices.Add(itl.Value());
    NextVertices.Clear();
  }

  // Temporary
  /*
  TopoDS_Compound aCompound, aCmp1;
  BRep_Builder BB;
  BB.MakeCompound(aCompound);
  BB.MakeCompound(aCmp1);
  for (i = 1; i <= myPaths.Length(); i++)
  {
    TopoDS_Compound aCmp;
    BB.MakeCompound(aCmp);
    for (j = 1; j <= myPaths(i).Length(); j++)
      BB.Add(aCmp, myPaths(i)(j));
    BB.Add(aCmp1, aCmp);
  }
  BB.Add(aCompound, aCmp1);

  myShape = aCompound;

  Done();
  return;
  */
  ////////////

  // Building of set of sections
  int NbE     = EdgeSeq.Length();
  int NbPaths = myPaths.Length();
  int NbVer   = myPaths.Length();
  if (myClosedSection)
    NbVer++;
  i = 1;
  for (;;)
  {
    for (j = 1; j <= EdgeSeq.Length(); j++)
      EdgeSeq(j).Nullify();

    bool ToInsertVertex = false;

    for (j = 2; j <= NbVer; j++)
    {
      if (!EdgeSeq(j - 1).IsNull())
        continue;

      // for the end of initial shape
      if (myPaths(j - 1).Length() < i)
      {
        TopoDS_Edge  aE1     = TopoDS::Edge(myPaths(j - 1)(i - 1));
        TopoDS_Shape LastVer = TopExp::LastVertex(aE1, true);
        myPaths(j - 1).Append(LastVer);
      }
      if (myPaths((j <= NbPaths) ? j : 1).Length() < i)
      {
        TopoDS_Edge  aE2     = TopoDS::Edge(myPaths((j <= NbPaths) ? j : 1)(i - 1));
        TopoDS_Shape LastVer = TopExp::LastVertex(aE2, true);
        myPaths((j <= NbPaths) ? j : 1).Append(LastVer);
      }
      //////////////////////////////

      if (ToInsertVertex)
      {
        if (myPaths(j - 1)(i).ShapeType() == TopAbs_EDGE)
        {
          TopoDS_Edge  aE1  = TopoDS::Edge(myPaths(j - 1)(i));
          TopoDS_Shape fver = TopExp::FirstVertex(aE1, true);
          myPaths(j - 1).InsertBefore(i, fver);
        }
        if (myPaths((j <= NbPaths) ? j : 1)(i).ShapeType() == TopAbs_EDGE)
        {
          TopoDS_Edge  aE2  = TopoDS::Edge(myPaths((j <= NbPaths) ? j : 1)(i));
          TopoDS_Shape fver = TopExp::FirstVertex(aE2, true);
          myPaths((j <= NbPaths) ? j : 1).InsertBefore(i, fver);
        }
        ToInsertVertex = false;
      }

      TopoDS_Edge E1, E2;
      if (myPaths(j - 1)(i).ShapeType() == TopAbs_EDGE)
        E1 = TopoDS::Edge(myPaths(j - 1)(i));
      if (myPaths((j <= NbPaths) ? j : 1)(i).ShapeType() == TopAbs_EDGE)
        E2 = TopoDS::Edge(myPaths((j <= NbPaths) ? j : 1)(i));
      TopoDS_Edge E12 = TopoDS::Edge(SectionsEdges(i)(j - 1));
      // Find the face on which (E1 or E2) and E12 lie
      TopoDS_Shape E1orE2 = (E1.IsNull()) ? E2 : E1;
      if (E1orE2.IsNull()) // both E1 and E2 are vertices =>
      {
        EdgeSeq(j - 1) = E12; // => proper edge is the edge of previous section between them
        continue;
      }
      const NCollection_List<TopoDS_Shape>& LF = EFmap.FindFromKey(E1orE2);
      TopoDS_Face                           theFace;
      for (itl.Initialize(LF); itl.More(); itl.Next())
      {
        const TopoDS_Shape&                      aFace = itl.Value();
        const NCollection_List<TopoDS_Shape>&    LF2   = EFmap.FindFromKey(E12);
        NCollection_List<TopoDS_Shape>::Iterator itl2(LF2);
        for (; itl2.More(); itl2.Next())
        {
          const TopoDS_Shape& aFace2 = itl2.Value();
          if (aFace.IsSame(aFace2))
          {
            theFace = TopoDS::Face(aFace);
            break;
          }
        }
        if (!theFace.IsNull())
          break;
      }

      TopoDS_Vertex PrevVertex =
        (E1.IsNull()) ? TopoDS::Vertex(myPaths(j - 1)(i)) : TopExp::LastVertex(E1, true);
      TopoDS_Vertex CurVertex = (E2.IsNull()) ? TopoDS::Vertex(myPaths((j <= NbPaths) ? j : 1)(i))
                                              : TopExp::LastVertex(E2, true);

      TopoDS_Edge                           ProperEdge;
      const NCollection_List<TopoDS_Shape>& LE = VEmap.FindFromKey(PrevVertex);
      // Temporary
      // int LenList = LE.Extent();
      ///////////
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> EdgesOfTheFace;
      TopExp::MapShapes(theFace, TopAbs_EDGE, EdgesOfTheFace);
      for (itl.Initialize(LE); itl.More(); itl.Next())
      {
        anEdge = TopoDS::Edge(itl.Value());
        TopExp::Vertices(anEdge, V1, V2);
        if (((V1.IsSame(PrevVertex) && V2.IsSame(CurVertex))
             || (V1.IsSame(CurVertex) && V2.IsSame(PrevVertex)))
            && EdgesOfTheFace.Contains(anEdge) && // this condition is for a section of two edges
            !anEdge.IsSame(E1))                   // the last condition is for torus-like shape
        {
          ProperEdge = anEdge;
          break;
        }
      }

      if ((myPaths(j - 1)(i)).ShapeType() == TopAbs_VERTEX
          && (myPaths((j <= NbPaths) ? j : 1)(i)).ShapeType() == TopAbs_VERTEX)
      {
        EdgeSeq(j - 1) = ProperEdge;
        continue;
      }

      TopoDS_Vertex PrevPrevVer = (E1.IsNull()) ? PrevVertex : TopExp::FirstVertex(E1, true);
      TopoDS_Vertex PrevCurVer  = (E2.IsNull()) ? CurVertex : TopExp::FirstVertex(E2, true);
      if (ProperEdge.IsNull()) // no connection between these two vertices
      {
        // Find the face on which E1, E2 and E12 lie
        // ToInsertVertex = false;
        NCollection_List<TopoDS_Shape> ListOneFace;
        ListOneFace.Append(theFace);

        if (E1.IsNull() || E2.IsNull())
        {
          if (E1.IsNull())
            E1 = TopoDS::Edge(myPaths(j - 1)(i - 1));
          if (E2.IsNull())
            E2 = TopoDS::Edge(myPaths((j <= NbPaths) ? j : 1)(i - 1));
          double                    fpar1, lpar1, fpar2, lpar2;
          double                    LastPar1, LastPar2;
          occ::handle<Geom2d_Curve> PCurve1 = BRep_Tool::CurveOnSurface(E1, theFace, fpar1, lpar1);
          occ::handle<Geom2d_Curve> PCurve2 = BRep_Tool::CurveOnSurface(E2, theFace, fpar2, lpar2);
          if (E1.Orientation() == TopAbs_FORWARD)
          {
            LastPar1 = lpar1;
          }
          else
          {
            LastPar1 = fpar1;
          }
          if (E2.Orientation() == TopAbs_FORWARD)
          {
            LastPar2 = lpar2;
          }
          else
          {
            LastPar2 = fpar2;
          }
          gp_Pnt2d                  FirstPnt2d = PCurve1->Value(LastPar1);
          gp_Pnt2d                  LastPnt2d  = PCurve2->Value(LastPar2);
          occ::handle<Geom_Surface> theSurf    = BRep_Tool::Surface(theFace);
          occ::handle<Geom2d_Line>  theLine    = GCE2d_MakeLine(FirstPnt2d, LastPnt2d);
          double                    len_ne     = FirstPnt2d.Distance(LastPnt2d);
          TopoDS_Edge               NewEdge =
            BRepLib_MakeEdge(theLine, theSurf, PrevVertex, CurVertex, 0., len_ne);
          BRepLib::BuildCurve3d(NewEdge);
          EdgeSeq(j - 1) = NewEdge;
          EFmap.Add(NewEdge, ListOneFace);
        }
        else // E1 is edge
        {
          // Extract points 2d
          double                    fpar1, lpar1, fpar2, lpar2;
          double                    FirstPar1, LastPar1, FirstPar2, LastPar2;
          occ::handle<Geom2d_Curve> PCurve1 = BRep_Tool::CurveOnSurface(E1, theFace, fpar1, lpar1);
          occ::handle<Geom2d_Curve> PCurve2 = BRep_Tool::CurveOnSurface(E2, theFace, fpar2, lpar2);
          if (E1.Orientation() == TopAbs_FORWARD)
          {
            FirstPar1 = fpar1;
            LastPar1  = lpar1;
          }
          else
          {
            FirstPar1 = lpar1;
            LastPar1  = fpar1;
          }
          if (E2.Orientation() == TopAbs_FORWARD)
          {
            FirstPar2 = fpar2;
            LastPar2  = lpar2;
          }
          else
          {
            FirstPar2 = lpar2;
            LastPar2  = fpar2;
          }
          gp_Pnt2d                  FirstPnt2d = PCurve1->Value(LastPar1);
          gp_Pnt2d                  LastPnt2d  = PCurve2->Value(LastPar2);
          occ::handle<Geom_Surface> theSurf    = BRep_Tool::Surface(theFace);
          occ::handle<Geom2d_Line>  theLine    = GCE2d_MakeLine(FirstPnt2d, LastPnt2d);
          double                    len_ne     = FirstPnt2d.Distance(LastPnt2d);
          TopoDS_Edge               NewEdge =
            BRepLib_MakeEdge(theLine, theSurf, PrevVertex, CurVertex, 0., len_ne);
          BRepLib::BuildCurve3d(NewEdge);
          gp_Pnt2d                 PrevFirstPnt2d = PCurve1->Value(FirstPar1);
          gp_Pnt2d                 PrevLastPnt2d  = PCurve2->Value(FirstPar2);
          occ::handle<Geom2d_Line> Line1          = GCE2d_MakeLine(PrevFirstPnt2d, LastPnt2d);
          occ::handle<Geom2d_Line> Line2          = GCE2d_MakeLine(FirstPnt2d, PrevLastPnt2d);
          double                   len_ne1        = PrevFirstPnt2d.Distance(LastPnt2d);
          TopoDS_Edge              NewEdge1 =
            BRepLib_MakeEdge(Line1, theSurf, PrevPrevVer, CurVertex, 0., len_ne1);
          BRepLib::BuildCurve3d(NewEdge1);
          double      len_ne2 = FirstPnt2d.Distance(PrevLastPnt2d);
          TopoDS_Edge NewEdge2 =
            BRepLib_MakeEdge(Line2, theSurf, PrevVertex, PrevCurVer, 0., len_ne2);
          BRepLib::BuildCurve3d(NewEdge2);
          bool good_ne  = IsValidEdge(NewEdge, theFace);
          bool good_ne1 = IsValidEdge(NewEdge1, theFace);

          GeomAbs_CurveType type_E1 = TypeOfEdge(E1);
          GeomAbs_CurveType type_E2 = TypeOfEdge(E2);

          int ChooseEdge = 0;

          if (!good_ne || type_E1 != type_E2)
          {
            if (type_E1 == type_E2) //! good_ne
            {
              if (good_ne1)
                ChooseEdge = 1;
              else
                ChooseEdge = 2;
            }
            else // types are different
            {
              if (type_E1 == GeomAbs_Line)
                ChooseEdge = 1;
              else if (type_E2 == GeomAbs_Line)
                ChooseEdge = 2;
              else // to be developed later...
              {
              }
            }
          }

          if (ChooseEdge == 0)
          {
            EdgeSeq(j - 1) = NewEdge;
            EFmap.Add(NewEdge, ListOneFace);
          }
          else if (ChooseEdge == 1)
          {
            EdgeSeq(j - 1) = NewEdge1;
            EFmap.Add(NewEdge1, ListOneFace);
            for (k = 1; k < j - 1; k++)
              EdgeSeq(k).Nullify();
            for (k = 1; k <= j - 1; k++)
            {
              TopoDS_Edge  aLastEdge    = TopoDS::Edge(myPaths(k)(i));
              TopoDS_Shape VertexAsEdge = TopExp::FirstVertex(aLastEdge, true);
              myPaths(k).InsertBefore(i, VertexAsEdge);
            }
            j = 1; // start from beginning
          }
          else if (ChooseEdge == 2)
          {
            EdgeSeq(j - 1) = NewEdge2;
            EFmap.Add(NewEdge2, ListOneFace);
            ToInsertVertex = true;
          }
        } // else //E1 is edge
      } // if (ProperEdge.IsNull())
      else // connecting edge exists
      {
        /*
        if (ToInsertVertex)
        {
          myPaths(j-1).InsertBefore(i, PrevPrevVer);
          myPaths((j<=NbPaths)? j : 1).InsertBefore(i, PrevCurVer);
          EdgeSeq(j-1) = E12;
        }
        else
        */
        EdgeSeq(j - 1) = ProperEdge;
      }
    } // for (j = 2; j <= NbVer; j++)
    SectionsEdges.Append(EdgeSeq);

    // check for exit from for(;;)
    int NbEndEdges = 0;
    for (j = 1; j <= EdgeSeq.Length(); j++)
      if (EndEdges.Contains(EdgeSeq(j)))
        NbEndEdges++;
    if (NbEndEdges == NbE)
      break;

    i++;
  } // for (;;)

  // final phase: building of middle path
  int                              NbSecFaces = SectionsEdges.Length();
  NCollection_Array1<TopoDS_Shape> SecFaces(1, NbSecFaces);
  for (i = 1; i <= NbSecFaces; i++)
  {
    BRepLib_MakeWire MW;
    for (j = 1; j <= NbE; j++)
    {
      anEdge = TopoDS::Edge(SectionsEdges(i)(j));
      MW.Add(anEdge);
    }
    if (!myClosedSection)
    {
      TopExp::Vertices(MW.Wire(), V1, V2);
      anEdge = BRepLib_MakeEdge(V2, V1);
      MW.Add(anEdge);
    }
    TopoDS_Wire      aWire = MW.Wire();
    BRepLib_MakeFace MF(aWire, true); // Only plane
    if (MF.IsDone())
      SecFaces(i) = MF.Face();
    else
      SecFaces(i) = aWire;
  }

  NCollection_Array1<gp_Pnt> Centers(1, NbSecFaces);
  for (i = 1; i <= NbSecFaces; i++)
  {
    GProp_GProps Properties;
    if (SecFaces(i).ShapeType() == TopAbs_FACE)
      BRepGProp::SurfaceProperties(SecFaces(i), Properties);
    else // wire
      BRepGProp::LinearProperties(SecFaces(i), Properties);

    Centers(i) = Properties.CentreOfMass();
  }

  NCollection_Array1<TopoDS_Shape> MidEdges(1, NbSecFaces - 1);
  double                           LinTol = 1.e-5;
  double                           AngTol = 1.e-7;
  gp_Pnt                           Pnt1, Pnt2;
  for (i = 1; i < NbSecFaces; i++)
  {
    GeomAbs_CurveType TypeOfMidEdge = GeomAbs_OtherCurve;
    for (j = 1; j <= myPaths.Length(); j++)
    {
      const TopoDS_Shape& aShape = myPaths(j)(i);
      if (aShape.ShapeType() == TopAbs_VERTEX)
      {
        TypeOfMidEdge = GeomAbs_OtherCurve;
        break;
      }
      anEdge                  = TopoDS::Edge(aShape);
      GeomAbs_CurveType aType = TypeOfEdge(anEdge);
      if (j == 1)
        TypeOfMidEdge = aType;
      else
      {
        if (aType != TypeOfMidEdge)
        {
          TypeOfMidEdge = GeomAbs_OtherCurve;
          break;
        }
      }
    }
    if (TypeOfMidEdge == GeomAbs_Line)
      MidEdges(i) = BRepLib_MakeEdge(Centers(i), Centers(i + 1));
    else if (TypeOfMidEdge == GeomAbs_Circle)
    {
      gp_Ax1 theAxis;
      gp_Dir theDir1, theDir2;
      double theAngle = 0.;
      gp_Vec theTangent;
      bool   SimilarArcs = true;
      for (j = 1; j <= myPaths.Length(); j++)
      {
        anEdge = TopoDS::Edge(myPaths(j)(i));
        double                  fpar, lpar;
        occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
        if (aCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
          aCurve = occ::down_cast<Geom_TrimmedCurve>(aCurve)->BasisCurve();
        Pnt1                             = aCurve->Value(fpar);
        Pnt2                             = aCurve->Value(lpar);
        occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(aCurve);
        gp_Circ                  aCirc   = aCircle->Circ();
        if (j == 1)
        {
          theAxis         = aCirc.Axis();
          theDir1         = gp_Vec(aCirc.Location(), Pnt1);
          theDir2         = gp_Vec(aCirc.Location(), Pnt2);
          theAngle        = lpar - fpar;
          double theParam = (anEdge.Orientation() == TopAbs_FORWARD) ? fpar : lpar;
          aCurve->D1(theParam, Pnt1, theTangent);
          if (anEdge.Orientation() == TopAbs_REVERSED)
            theTangent.Reverse();
        }
        else
        {
          gp_Ax1 anAxis = aCirc.Axis();
          gp_Lin aLin(anAxis);
          if (!aLin.Contains(theAxis.Location(), LinTol) || !anAxis.IsParallel(theAxis, AngTol))
          {
            SimilarArcs = false;
            break;
          }
          gp_Dir aDir1 = gp_Vec(aCirc.Location(), Pnt1);
          gp_Dir aDir2 = gp_Vec(aCirc.Location(), Pnt2);
          if (!((aDir1.IsEqual(theDir1, AngTol) && aDir2.IsEqual(theDir2, AngTol))
                || (aDir1.IsEqual(theDir2, AngTol) && aDir2.IsEqual(theDir1, AngTol))))
          {
            SimilarArcs = false;
            break;
          }
        }
      }
      if (SimilarArcs)
      {
        gp_XYZ AxisLoc   = theAxis.Location().XYZ();
        gp_XYZ AxisDir   = theAxis.Direction().XYZ();
        double Parameter = (Centers(i).XYZ() - AxisLoc) * AxisDir;
        gp_Pnt theCenterOfCirc(AxisLoc + Parameter * AxisDir);

        gp_Vec Vec1(theCenterOfCirc, Centers(i));
        gp_Vec Vec2(theCenterOfCirc, Centers(i + 1));
        /*
        gp_Dir VecProd = Vec1 ^ Vec2;
        if (theAxis.Direction() * VecProd < 0.)
          theAxis.Reverse();
        */

        double anAngle = Vec1.AngleWithRef(Vec2, theAxis.Direction());
        if (anAngle < 0.)
          anAngle += 2. * M_PI;
        if (std::abs(anAngle - theAngle) > AngTol)
          theAxis.Reverse();
        gp_Ax2                   theAx2(theCenterOfCirc, theAxis.Direction(), Vec1);
        occ::handle<Geom_Circle> theCircle = GC_MakeCircle(theAx2, Vec1.Magnitude());
        gp_Vec                   aTangent;
        theCircle->D1(0., Pnt1, aTangent);
        if (aTangent * theTangent < 0.)
        {
          theAxis.Reverse();
          theAx2    = gp_Ax2(theCenterOfCirc, theAxis.Direction(), Vec1);
          theCircle = GC_MakeCircle(theAx2, Vec1.Magnitude());
        }
        BRepLib_MakeEdge aME(theCircle, 0., theAngle);
        aME.Build();

        MidEdges(i) = aME.IsDone() ? aME.Shape() : TopoDS_Edge();
      }
    }
  }

  // Build missed edges
  for (i = 1; i < NbSecFaces; i++)
  {
    if (MidEdges(i).IsNull())
    {
      for (j = i + 1; j < NbSecFaces; j++)
      {
        if (!MidEdges(j).IsNull())
          break;
      }
      // from i to j-1 all edges are null
      occ::handle<NCollection_HArray1<gp_Pnt>> thePoints =
        new NCollection_HArray1<gp_Pnt>(1, j - i + 1);
      NCollection_Array1<gp_Vec>             theTangents(1, j - i + 1);
      occ::handle<NCollection_HArray1<bool>> theFlags = new NCollection_HArray1<bool>(1, j - i + 1);
      for (k = i; k <= j; k++)
        thePoints->SetValue(k - i + 1, Centers(k));
      for (k = i; k <= j; k++)
      {
        NCollection_Sequence<gp_Pnt> PntSeq;
        for (int indp = 1; indp <= myPaths.Length(); indp++)
        {
          gp_Vec aTangent;
          if (k == i)
          {
            if (myPaths(indp)(k).ShapeType() == TopAbs_VERTEX)
              continue;
            aTangent = TangentOfEdge(myPaths(indp)(k), true); // at begin
          }
          else if (k == j)
          {
            if (myPaths(indp)(k - 1).ShapeType() == TopAbs_VERTEX)
              continue;
            aTangent = TangentOfEdge(myPaths(indp)(k - 1), false); // at end
          }
          else
          {
            if (myPaths(indp)(k - 1).ShapeType() == TopAbs_VERTEX
                || myPaths(indp)(k).ShapeType() == TopAbs_VERTEX)
              continue;
            gp_Vec Tangent1 = TangentOfEdge(myPaths(indp)(k - 1), false); // at end
            gp_Vec Tangent2 = TangentOfEdge(myPaths(indp)(k), true);      // at begin
            aTangent        = Tangent1 + Tangent2;
          }
          aTangent.Normalize();
          gp_Pnt aPnt(aTangent.XYZ());
          PntSeq.Append(aPnt);
        }
        NCollection_Array1<gp_Pnt> PntArray(1, PntSeq.Length());
        for (int ip = 1; ip <= PntSeq.Length(); ip++)
          PntArray(ip) = PntSeq(ip);
        gp_Pnt theBary;
        gp_Dir xdir, ydir;
        double xgap, ygap, zgap;
        GeomLib::Inertia(PntArray, theBary, xdir, ydir, xgap, ygap, zgap);
        gp_Vec theTangent(theBary.XYZ());
        theTangents(k - i + 1) = theTangent;
      }
      theFlags->Init(true);

      GeomAPI_Interpolate Interpol(thePoints, false, LinTol);
      Interpol.Load(theTangents, theFlags);
      Interpol.Perform();
      if (!Interpol.IsDone())
      {
        std::cout << std::endl << "Interpolation failed" << std::endl;
      }
      occ::handle<Geom_Curve> InterCurve(Interpol.Curve());
      MidEdges(i) = BRepLib_MakeEdge(InterCurve);
      i           = j;
    }
  }

  BRepLib_MakeWire MakeFinalWire;
  for (i = 1; i < NbSecFaces; i++)
    if (!MidEdges(i).IsNull())
      MakeFinalWire.Add(TopoDS::Edge(MidEdges(i)));

  TopoDS_Wire FinalWire = MakeFinalWire.Wire();
  myShape               = MakeFinalWire.Wire();

  // Temporary
  /*
  TopoDS_Compound aCompound, aCmp1, aCmp2;
  BRep_Builder BB;
  BB.MakeCompound(aCompound);
  BB.MakeCompound(aCmp1);
  BB.MakeCompound(aCmp2);
  for (i = 1; i <= myPaths.Length(); i++)
  {
    TopoDS_Compound aCmp;
    BB.MakeCompound(aCmp);
    for (j = 1; j <= myPaths(i).Length(); j++)
      BB.Add(aCmp, myPaths(i)(j));
    BB.Add(aCmp1, aCmp);
  }
  for (i = 1; i <= SectionsEdges.Length(); i++)
  {
    TopoDS_Wire aWire;
    BB.MakeWire(aWire);
    for (j = 1; j <= SectionsEdges(i).Length(); j++)
      BB.Add(aWire, SectionsEdges(i)(j));
    BB.Add(aCmp2, aWire);
  }
  BB.Add(aCompound, aCmp1);
  BB.Add(aCompound, aCmp2);
  BB.Add(aCompound, FinalWire);

  myShape = aCompound;
  */
  ////////////

  Done();
}
