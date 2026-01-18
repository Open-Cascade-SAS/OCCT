// Created on: 1995-04-20
// Created by: Bruno DUMORTIER
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

//  Modified by skv - Fri Jul  8 11:21:38 2005 OCC9145

#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_OffsetCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <MAT_Node.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <BRepFill_OffsetWire.hxx>
#include <BRepFill_TrimEdgeTool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepMAT2d_BisectingLocus.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepMAT2d_LinkTopoBilo.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <MAT2d_CutCurve.hxx>
#include <MAT_Arc.hxx>
#include <MAT_Graph.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>

#include <cstdio>
#ifdef OCCT_DEBUG
#endif

//  Modified by Sergey KHROMOV - Thu Nov 16 17:24:39 2000 Begin

static void QuasiFleche(const Adaptor3d_Curve&        C,
                        const double                  Deflection2,
                        const double                  Udeb,
                        const gp_Pnt&                 Pdeb,
                        const gp_Vec&                 Vdeb,
                        const double                  Ufin,
                        const gp_Pnt&                 Pfin,
                        const gp_Vec&                 Vfin,
                        const int                     Nbmin,
                        const double                  Eps,
                        NCollection_Sequence<double>& Parameters,
                        NCollection_Sequence<gp_Pnt>& Points);

static bool PerformCurve(NCollection_Sequence<double>& Parameters,
                         NCollection_Sequence<gp_Pnt>& Points,
                         const Adaptor3d_Curve&        C,
                         const double                  Deflection,
                         const double                  U1,
                         const double                  U2,
                         const double                  EPSILON,
                         const int                     Nbmin);

static void CheckBadEdges(const TopoDS_Face&              Spine,
                          const double                    Offset,
                          const BRepMAT2d_BisectingLocus& Locus,
                          const BRepMAT2d_LinkTopoBilo&   Link,
                          NCollection_List<TopoDS_Shape>& BadEdges);

static int CutEdge(const TopoDS_Edge&              E,
                   const TopoDS_Face&              F,
                   int                             ForceCut,
                   NCollection_List<TopoDS_Shape>& Cuts);

static void CutCurve(const occ::handle<Geom2d_TrimmedCurve>&          C,
                     const int                                        nbParts,
                     NCollection_Sequence<occ::handle<Geom2d_Curve>>& theCurves);

//  Modified by Sergey KHROMOV - Thu Nov 16 17:24:47 2000 End

static void EdgeVertices(const TopoDS_Edge& E, TopoDS_Vertex& V1, TopoDS_Vertex& V2)
{
  if (E.Orientation() == TopAbs_REVERSED)
  {
    TopExp::Vertices(E, V2, V1);
  }
  else
  {
    TopExp::Vertices(E, V1, V2);
  }
}

static bool VertexFromNode(const occ::handle<MAT_Node>&                              aNode,
                           const double                                              Offset,
                           gp_Pnt2d&                                                 PN,
                           NCollection_DataMap<occ::handle<MAT_Node>, TopoDS_Shape>& MapNodeVertex,
                           TopoDS_Vertex&                                            VN);

static void StoreInMap(
  const TopoDS_Shape&                                                              V1,
  const TopoDS_Shape&                                                              V2,
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& MapVV);

static void TrimEdge(
  const TopoDS_Edge&                                                               CurrentEdge,
  const TopoDS_Shape&                                                              CurrentSpine,
  const TopoDS_Face&                                                               AllSpine,
  const NCollection_List<TopoDS_Shape>&                                            D,
  NCollection_Sequence<TopoDS_Shape>&                                              Sv,
  NCollection_Sequence<double>&                                                    MapverPar,
  NCollection_Sequence<TopoDS_Shape>&                                              S,
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& MapVV,
  const int                                                                        IndOfE);

static bool IsInnerEdge(const TopoDS_Shape& ProE,
                        const TopoDS_Face&  AllSpine,
                        double&             TrPar1,
                        double&             TrPar2);

static bool DoubleOrNotInside(const NCollection_List<TopoDS_Shape>& EC, const TopoDS_Vertex& V);

static bool IsSmallClosedEdge(const TopoDS_Edge& anEdge, const TopoDS_Vertex& aVertex);

static void MakeCircle(
  const TopoDS_Edge&                                                        E,
  const TopoDS_Vertex&                                                      V,
  const TopoDS_Face&                                                        F,
  const double                                                              Offset,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& Map,
  const occ::handle<Geom_Plane>&                                            RefPlane);

static void MakeOffset(
  const TopoDS_Edge&                                                        E,
  const TopoDS_Face&                                                        F,
  const double                                                              Offset,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& Map,
  const occ::handle<Geom_Plane>&                                            RefPlane,
  const bool                                                                IsOpenResult,
  const GeomAbs_JoinType                                                    theJoinType,
  const TopoDS_Vertex*                                                      Ends);

bool CheckSmallParamOnEdge(const TopoDS_Edge& anEdge);

//=================================================================================================

static bool KPartCircle(
  const TopoDS_Face&                                                        mySpine,
  const double                                                              myOffset,
  const bool                                                                IsOpenResult,
  const double                                                              Alt,
  TopoDS_Shape&                                                             myShape,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& myMap,
  bool&                                                                     myIsDone)
{
  TopoDS_Edge E;
  for (TopExp_Explorer anEdgeIter(mySpine, TopAbs_EDGE); anEdgeIter.More(); anEdgeIter.Next())
  {
    if (!E.IsNull())
    {
      return false;
    }
    E = TopoDS::Edge(anEdgeIter.Current());
  }
  if (E.IsNull())
  {
    return false;
  }

  double                  f, l;
  TopLoc_Location         L;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, L, f, l);
  if (C.IsNull())
  {
    return false;
  }

  if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> Ct = occ::down_cast<Geom_TrimmedCurve>(C);
    C                                 = Ct->BasisCurve();
  }

  if ((C->IsKind(STANDARD_TYPE(Geom_Circle)) && BRep_Tool::IsClosed(E)) || // closed circle
      IsOpenResult)
  {
    double anOffset = myOffset;

    occ::handle<Geom2d_Curve>        aPCurve = BRep_Tool::CurveOnSurface(E, mySpine, f, l);
    occ::handle<Geom2dAdaptor_Curve> AHC     = new Geom2dAdaptor_Curve(aPCurve, f, l);
    occ::handle<Geom2d_Curve>        OC;
    if (AHC->GetType() == GeomAbs_Line)
    {
      if (E.Orientation() == TopAbs_FORWARD)
        anOffset *= -1;
      Adaptor2d_OffsetCurve Off(AHC, anOffset);
      OC = new Geom2d_Line(Off.Line());
    }
    else if (AHC->GetType() == GeomAbs_Circle)
    {
      if (E.Orientation() == TopAbs_FORWARD)
        anOffset *= -1;
      if (!BRep_Tool::IsClosed(E))
      {
        anOffset *= -1;
      }
      gp_Circ2d theCirc = AHC->Circle();
      if (anOffset > 0. || std::abs(anOffset) < theCirc.Radius())
        OC = new Geom2d_Circle(theCirc.Position(), theCirc.Radius() + anOffset);
      else
      {
        myIsDone = false;
        return false;
      }
    }
    else
    {
      if (E.Orientation() == TopAbs_FORWARD)
        anOffset *= -1;
      occ::handle<Geom2d_TrimmedCurve> G2dT = new Geom2d_TrimmedCurve(aPCurve, f, l);
      OC                                    = new Geom2d_OffsetCurve(G2dT, anOffset);
    }
    occ::handle<Geom_Surface> aSurf  = BRep_Tool::Surface(mySpine);
    occ::handle<Geom_Plane>   aPlane = occ::down_cast<Geom_Plane>(aSurf);
    myShape                          = BRepLib_MakeEdge(OC, aPlane, f, l);
    BRepLib::BuildCurve3d(TopoDS::Edge(myShape));

    myShape.Orientation(E.Orientation());
    myShape.Location(L);
    if (fabs(Alt) > gp::Resolution())
    {
      BRepAdaptor_Surface S(mySpine, false);
      gp_Ax1              Nor = S.Plane().Axis();
      gp_Trsf             T;
      gp_Vec              Trans(Nor.Direction());
      Trans = Alt * Trans;
      T.SetTranslation(Trans);
      myShape.Move(TopLoc_Location(T));
    }

    NCollection_List<TopoDS_Shape> LL;
    LL.Append(myShape);
    myMap.Add(E, LL);

    TopoDS_Edge myEdge = TopoDS::Edge(myShape);
    myShape            = BRepLib_MakeWire(myEdge);

    myIsDone = true;
    return true;
  }

  return false;
}

//=================================================================================================

BRepFill_OffsetWire::BRepFill_OffsetWire()
    : myIsOpenResult(false),
      myIsDone(false)
{
}

//=================================================================================================

BRepFill_OffsetWire::BRepFill_OffsetWire(const TopoDS_Face&     Spine,
                                         const GeomAbs_JoinType Join,
                                         const bool             IsOpenResult)
{
  Init(Spine, Join, IsOpenResult);
}

//=================================================================================================

void BRepFill_OffsetWire::Init(const TopoDS_Face&     Spine,
                               const GeomAbs_JoinType Join,
                               const bool             IsOpenResult)
{
  myIsDone                 = false;
  TopoDS_Shape aLocalShape = Spine.Oriented(TopAbs_FORWARD);
  mySpine                  = TopoDS::Face(aLocalShape);
  //  mySpine    = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  myJoinType     = Join;
  myIsOpenResult = IsOpenResult;

  myMap.Clear();
  myMapSpine.Clear();

  //------------------------------------------------------------------
  // cut the spine for bissectors.
  //------------------------------------------------------------------
  BRepMAT2d_Explorer Exp;
  Exp.Perform(mySpine);
  mySpine = TopoDS::Face(Exp.ModifiedShape(mySpine));
  PrepareSpine();

  TopoDS_Shape                                                             aShape;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>> aMap;
  bool                                                                     Done;
  if (KPartCircle(myWorkSpine, 1., myIsOpenResult, 0., aShape, aMap, Done))
    return;

  //-----------------------------------------------------
  // Calculate the map of bissectors to the left.
  // and Links Topology -> base elements of the map.
  //-----------------------------------------------------

  Exp.Perform(myWorkSpine);
  myBilo.Compute(Exp, 1, MAT_Left, myJoinType, myIsOpenResult);
  myLink.Perform(Exp, myBilo);
}

//=================================================================================================

bool BRepFill_OffsetWire::IsDone() const
{
  return myIsDone;
}

//=================================================================================================

const TopoDS_Face& BRepFill_OffsetWire::Spine() const
{
  return mySpine;
}

//=================================================================================================

const TopoDS_Shape& BRepFill_OffsetWire::Shape() const
{
  return myShape;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFill_OffsetWire::GeneratedShapes(
  const TopoDS_Shape& SpineShape)
{
  if (!myCallGen)
  {
    if (!myMapSpine.IsEmpty())
    {
      // myMapSpine can be empty if passed by PerformWithBilo.
      NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(
        myMapSpine);
      for (; it.More(); it.Next())
      {
        if (myMap.Contains(it.Key()))
        {
          myMap.Bound(it.Value(), NCollection_List<TopoDS_Shape>());
          if (!it.Value().IsSame(it.Key()))
          {
            myMap.ChangeFromKey(it.Value()).Append(myMap.ChangeFromKey(it.Key()));
            myMap.RemoveKey(it.Key());
          }
        }
        if (myMap.Contains(it.Key().Reversed()))
        {
          myMap.Bound(it.Value().Reversed(), NCollection_List<TopoDS_Shape>());
          if (!it.Value().IsSame(it.Key()))
          {
            myMap.ChangeFromKey(it.Value().Reversed())
              .Append(myMap.ChangeFromKey(it.Key().Reversed()));
            myMap.RemoveKey(it.Key().Reversed());
          }
        }
      }
    }
    myCallGen = true;
  }

  if (myMap.Contains(SpineShape))
  {
    return myMap.FindFromKey(SpineShape);
  }
  else
  {
    static NCollection_List<TopoDS_Shape> Empty;
    return Empty;
  }
}

//=================================================================================================

GeomAbs_JoinType BRepFill_OffsetWire::JoinType() const
{
  return myJoinType;
}

//=================================================================================================

void BRepFill_OffsetWire::Perform(const double Offset, const double Alt)
{
  //  Modified by skv - Fri Jul  8 11:21:38 2005 OCC9145 Begin
  try
  {
    OCC_CATCH_SIGNALS
    myCallGen = false;
    if (KPartCircle(myWorkSpine, Offset, myIsOpenResult, Alt, myShape, myMap, myIsDone))
      return;

    TopoDS_Face oldWorkSpain = myWorkSpine;

    NCollection_List<TopoDS_Shape> BadEdges;
    CheckBadEdges(myWorkSpine, Offset, myBilo, myLink, BadEdges);

    if (!BadEdges.IsEmpty())
    {
      // Modification of myWorkSpine;
      // std::cout << "Modification of myWorkSpine : " << BadEdges.Extent() << std::endl;
      BRepTools_Substitution                   aSubst;
      NCollection_List<TopoDS_Shape>::Iterator it(BadEdges);
      NCollection_List<TopoDS_Shape>           aL;
      double                                   aDefl = .01 * std::abs(Offset);
      NCollection_Sequence<double>             Parameters;
      NCollection_Sequence<gp_Pnt>             Points;

      for (; it.More(); it.Next())
      {
        aL.Clear();
        Parameters.Clear();
        Points.Clear();
        const TopoDS_Shape& anE = it.Value();

        TopoDS_Vertex Vf, Vl;
        TopExp::Vertices(TopoDS::Edge(anE), Vf, Vl);

        double                  f, l;
        occ::handle<Geom_Curve> G3d = BRep_Tool::Curve(TopoDS::Edge(anE), f, l);
        GeomAdaptor_Curve       AC(G3d, f, l);

        PerformCurve(Parameters, Points, AC, aDefl, f, l, Precision::Confusion(), 2);

        int NPnts = Points.Length();
        if (NPnts > 2)
        {
          // std::cout << NPnts << " points " << std::endl;
          TopoDS_Vertex FV = Vf;
          TopoDS_Vertex LV;
          TopoDS_Edge   newE;
          int           np;
          for (np = 2; np < NPnts; np++)
          {
            gp_Pnt LP = Points(np);
            LV        = BRepLib_MakeVertex(LP);
            newE      = BRepLib_MakeEdge(FV, LV);
            aL.Append(newE);
            FV = LV;
          }
          LV   = Vl;
          newE = BRepLib_MakeEdge(FV, LV);
          aL.Append(newE);
        }
        else
        {
          // std::cout << " 2 points " << std::endl;
          TopoDS_Edge newE = BRepLib_MakeEdge(Vf, Vl);
          aL.Append(newE);
        }
        // Update myMapSpine
        if (myMapSpine.IsBound(anE))
        {
          NCollection_List<TopoDS_Shape>::Iterator newit(aL);
          for (; newit.More(); newit.Next())
          {
            TopoDS_Edge NewEdge = TopoDS::Edge(newit.Value());
            myMapSpine.Bind(NewEdge, myMapSpine(anE));
            TopoDS_Vertex NewV1, NewV2;
            EdgeVertices(NewEdge, NewV1, NewV2);
            myMapSpine.TryBound(NewV1, myMapSpine(anE));
            myMapSpine.TryBound(NewV2, myMapSpine(anE));
          }
          myMapSpine.UnBind(anE);
        }
        ///////////////////
        aSubst.Substitute(anE, aL);
      }

      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                      wwmap;
      TopoDS_Iterator itws(myWorkSpine);
      for (; itws.More(); itws.Next())
      {
        const TopoDS_Shape& aWire = itws.Value();
        aSubst.Build(aWire);
        if (aSubst.IsCopied(aWire))
        {
          TopoDS_Wire NewWire = TopoDS::Wire(aSubst.Copy(aWire).First());
          NewWire.Closed(aWire.Closed());
          NCollection_List<TopoDS_Shape> Lw;
          Lw.Append(NewWire);
          wwmap.Bind(aWire, Lw);
        }
      }
      aSubst.Clear();
      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
        Iterator itmap(wwmap);
      for (; itmap.More(); itmap.Next())
        aSubst.Substitute(itmap.Key(), itmap.Value());

      aSubst.Build(myWorkSpine);

      if (aSubst.IsCopied(myWorkSpine))
      {
        myWorkSpine = TopoDS::Face(aSubst.Copy(myWorkSpine).First());

        BRepMAT2d_Explorer newExp;
        newExp.Perform(myWorkSpine);
        BRepMAT2d_BisectingLocus newBilo;
        BRepMAT2d_LinkTopoBilo   newLink;
        newBilo.Compute(newExp, 1, MAT_Left, myJoinType, myIsOpenResult);

        if (!newBilo.IsDone())
        {
          myShape.Nullify();
          myIsDone = false;
          return;
        }

        newLink.Perform(newExp, newBilo);
        PerformWithBiLo(myWorkSpine, Offset, newBilo, newLink, myJoinType, Alt);
        myWorkSpine = oldWorkSpain;
      }
      else
      {
        PerformWithBiLo(myWorkSpine, Offset, myBilo, myLink, myJoinType, Alt);
      }
    }
    else
    {
      PerformWithBiLo(myWorkSpine, Offset, myBilo, myLink, myJoinType, Alt);
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "An exception was caught in BRepFill_OffsetWire::Perform : ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    myShape.Nullify();
    myIsDone = false;

    return;
  }

  //  Modified by skv - Fri Jul  8 11:21:38 2005 OCC9145 End
  //  Modified by Sergey KHROMOV - Thu Mar 14 10:48:15 2002 Begin
  if (!myIsOpenResult)
  {
    TopExp_Explorer anExp(myShape, TopAbs_WIRE);

    for (; anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aWire = anExp.Current();

      if (!aWire.Closed())
      {
        myShape.Nullify();
        myIsDone = false;
        throw Standard_ConstructionError("Offset wire is not closed.");
      }
    }
  }
  //  Modified by Sergey KHROMOV - Thu Mar 14 10:48:16 2002 End
}

//=================================================================================================

void Compute(const TopoDS_Face&                                                        Spine,
             TopoDS_Shape&                                                             aShape,
             NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& Map,
             const double                                                              Alt)
{
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(aShape));
  double ALT = Alt;
  if (Spine.Orientation() == TopAbs_REVERSED)
    ALT = -Alt;
  gp_Trsf T;
  T.SetTranslation(gp_Vec(0., 0., ALT));
  TopLoc_Location L(T);

  for (TopExp_Explorer exp(Spine, TopAbs_WIRE); exp.More(); exp.Next())
  {
    const TopoDS_Wire& CurW        = TopoDS::Wire(exp.Current());
    TopoDS_Shape       aLocalShape = CurW.Moved(L);
    TopoDS_Wire        NewW        = TopoDS::Wire(aLocalShape);
    //    TopoDS_Wire        NewW = TopoDS::Wire(CurW.Moved(L));
    B.Add(aShape, NewW);
    // update Map.
    TopoDS_Iterator it1(CurW);
    TopoDS_Iterator it2(NewW);
    for (; it1.More(); it1.Next(), it2.Next())
    {
      NCollection_List<TopoDS_Shape> List;
      List.Append(it2.Value());
      Map.Add(it1.Value(), List);
    }
  }
}

//=================================================================================================

void BRepFill_OffsetWire::PerformWithBiLo(const TopoDS_Face&              Spine,
                                          const double                    Offset,
                                          const BRepMAT2d_BisectingLocus& Locus,
                                          BRepMAT2d_LinkTopoBilo&         Link,
                                          const GeomAbs_JoinType          Join,
                                          const double                    Alt)
{
  myIsDone                     = false;
  TopoDS_Shape aLocalShapeWork = Spine.Oriented(TopAbs_FORWARD);
  myWorkSpine                  = TopoDS::Face(aLocalShapeWork);
  //  myWorkSpine  = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  myJoinType = Join;
  myOffset   = Offset;
  myShape.Nullify();

  if (mySpine.IsNull())
  {
    TopoDS_Shape aLocalShape = Spine.Oriented(TopAbs_FORWARD);
    mySpine                  = TopoDS::Face(aLocalShape);
    //    mySpine = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  }
  myMap.Clear();

  if (std::abs(myOffset) < Precision::Confusion())
  {
    Compute(mySpine, myShape, myMap, Alt);
    myIsDone = true;
    return;
  }

  //********************************
  // Calculate for a non null offset
  //********************************
  if (KPartCircle(myWorkSpine, Offset, myIsOpenResult, Alt, myShape, myMap, myIsDone))
    return;

  BRep_Builder myBuilder;

  //---------------------------------------------------------------------
  // MapNodeVertex : associate to each node of the map (key1) and to
  //                 each element of the profile (key2) a vertex (item).
  // MapBis        : all edges or vertices (item) generated by
  //                 a bisectrice on a face or an edge (key) of revolution tubes.
  // MapVerPar     : Map of parameters of vertices on parallel edges
  //                 the list contained in MapVerPar (E) corresponds to
  //                 parameters on E of vertices contained in MapBis(E);
  //---------------------------------------------------------------------

  NCollection_DataMap<occ::handle<MAT_Node>, TopoDS_Shape> MapNodeVertex;
  NCollection_DataMap<TopoDS_Shape, NCollection_Sequence<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MapBis;
  NCollection_DataMap<TopoDS_Shape, NCollection_Sequence<double>, TopTools_ShapeMapHasher>
    MapVerPar;

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> EmptyMap;
  NCollection_Sequence<TopoDS_Shape>                                       EmptySeq;
  NCollection_List<TopoDS_Shape>                                           EmptyList;
  NCollection_Sequence<double>                                             EmptySeqOfReal;

  occ::handle<Geom_Plane> RefPlane = occ::down_cast<Geom_Plane>(BRep_Tool::Surface(myWorkSpine));
  if (fabs(Alt) > gp::Resolution())
  {
    double anAlt = Alt;
    if (myWorkSpine.Orientation() == TopAbs_REVERSED)
      anAlt = -Alt;
    RefPlane = occ::down_cast<Geom_Plane>(
      RefPlane->Translated(anAlt * gp_Vec(RefPlane->Axis().Direction())));
  }

  //---------------------------------------------------------------
  // Construction of Circles and OffsetCurves
  //---------------------------------------------------------------

  TopoDS_Vertex Ends[2];
  if (myIsOpenResult)
  {
    TopoDS_Wire     theWire;
    TopoDS_Iterator iter(mySpine);
    theWire = TopoDS::Wire(iter.Value());
    TopExp::Vertices(theWire, Ends[0], Ends[1]);
  }

  if (Locus.NumberOfContours() == 0)
  {
    return;
  }

  for (int ic = 1; ic <= Locus.NumberOfContours(); ic++)
  {
    TopoDS_Shape  PEE = Link.GeneratingShape(Locus.BasicElt(ic, Locus.NumberOfElts(ic)));
    TopoDS_Shape& PE  = PEE;
    for (int ie = 1; ie <= Locus.NumberOfElts(ic); ie++)
    {
      const TopoDS_Shape& SE = Link.GeneratingShape(Locus.BasicElt(ic, ie));
      if (SE.ShapeType() == TopAbs_VERTEX)
      {
        if (!SE.IsSame(Ends[0]) && !SE.IsSame(Ends[1]))
          MakeCircle(TopoDS::Edge(PE), TopoDS::Vertex(SE), myWorkSpine, myOffset, myMap, RefPlane);
      }
      else
      {
        MakeOffset(TopoDS::Edge(SE),
                   myWorkSpine,
                   myOffset,
                   myMap,
                   RefPlane,
                   myIsOpenResult,
                   myJoinType,
                   Ends);
        PE = SE;
      }
    }
  }

  // Remove possible hanging arcs on vertices
  if (myIsOpenResult && myJoinType == GeomAbs_Arc)
  {
    if (!myMap.IsEmpty() && myMap.FindKey(1).ShapeType() == TopAbs_VERTEX)
    {
      myMap.RemoveFromIndex(1);
    }
    if (!myMap.IsEmpty() && myMap.FindKey(myMap.Extent()).ShapeType() == TopAbs_VERTEX)
      myMap.RemoveLast();
  }

#ifdef OCCT_DEBUG
#endif

  //---------------------------------------------------
  // Construction of offset vertices.
  //---------------------------------------------------
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>> Detromp;
  occ::handle<MAT_Arc>                                              CurrentArc;
  occ::handle<Geom2d_Curve>                                         Bis, PCurve1, PCurve2;
  occ::handle<Geom_Curve>                                           CBis;
  bool                                                              Reverse;
  TopoDS_Edge                                                       CurrentEdge;
  TopoDS_Shape                                                      S[2];
  TopoDS_Edge                                                       E[2];
  TopLoc_Location                                                   L;
  int                                                               j, k;

  for (int i = 1; i <= Locus.Graph()->NumberOfArcs(); i++)
  {

    CurrentArc           = Locus.Graph()->Arc(i);
    Bisector_Bisec Bisec = Locus.GeomBis(CurrentArc, Reverse);
#ifdef OCCT_DEBUG
#endif

    //-------------------------------------------------------------------
    // Return elements of the spine corresponding to separate basicElts.
    //-------------------------------------------------------------------
    S[0] = Link.GeneratingShape(CurrentArc->FirstElement());
    S[1] = Link.GeneratingShape(CurrentArc->SecondElement());

    NCollection_Sequence<TopoDS_Shape> Vertices;
    NCollection_Sequence<gp_Pnt>       Params;

    NCollection_DataMap<TopoDS_Shape, NCollection_Sequence<TopoDS_Shape>, TopTools_ShapeMapHasher>
      MapSeqVer;
    NCollection_DataMap<TopoDS_Shape, NCollection_Sequence<gp_Pnt>, TopTools_ShapeMapHasher>
      MapSeqPar;

    //-----------------------------------------------------------
    // Return parallel edges on each face.
    // If no offset generated => move to the next bissectrice.
    //--------------------------------------------------------------
    if (myMap.Contains(S[0]) && myMap.Contains(S[1]))
    {
      E[0] = TopoDS::Edge(myMap.FindFromKey(S[0]).First());
      E[1] = TopoDS::Edge(myMap.FindFromKey(S[1]).First());
    }
    else
      continue;

    //-----------------------------------------------------------
    // Construction of vertices corresponding to the node of the map.
    // if they are on the offset.
    //-----------------------------------------------------------
    TopoDS_Vertex         VS, VE;
    occ::handle<MAT_Node> Node1, Node2;

    if (Reverse)
    {
      Node1 = CurrentArc->SecondNode();
      Node2 = CurrentArc->FirstNode();
    }
    else
    {
      Node1 = CurrentArc->FirstNode();
      Node2 = CurrentArc->SecondNode();
    }

    bool StartOnEdge = false, EndOnEdge = false;

    if (!Node1->Infinite())
    {
      gp_Pnt2d aLocalPnt2d = Locus.GeomElt(Node1);
      StartOnEdge          = VertexFromNode(Node1, myOffset, aLocalPnt2d, MapNodeVertex, VS);
      //      StartOnEdge = VertexFromNode(Node1, myOffset, Locus.GeomElt(Node1),
      //				   MapNodeVertex,VS);
    }
    if (!Node2->Infinite())
    {
      gp_Pnt2d aLocalPnt2d = Locus.GeomElt(Node2);
      EndOnEdge            = VertexFromNode(Node2, myOffset, aLocalPnt2d, MapNodeVertex, VE);
      //      EndOnEdge   = VertexFromNode(Node2, myOffset, Locus.GeomElt(Node2),
      //				   MapNodeVertex,VE);
    }

    if (myJoinType == GeomAbs_Intersection)
      StartOnEdge = EndOnEdge = false;

    //---------------------------------------------
    // Construction of geometries.
    //---------------------------------------------
    BRepFill_TrimEdgeTool Trim(Bisec,
                               Locus.GeomElt(CurrentArc->FirstElement()),
                               Locus.GeomElt(CurrentArc->SecondElement()),
                               myOffset);

    //-----------------------------------------------------------
    // Construction of vertices on edges parallel to the spine.
    //-----------------------------------------------------------

    Trim
      .IntersectWith(E[0], E[1], S[0], S[1], Ends[0], Ends[1], myJoinType, myIsOpenResult, Params);

    for (int s = 1; s <= Params.Length(); s++)
    {
      TopoDS_Vertex VC;
      myBuilder.MakeVertex(VC);
      gp_Pnt2d P2 = Bisec.Value()->Value(Params.Value(s).X());
      gp_Pnt   PVC(P2.X(), P2.Y(), 0.);

      myBuilder.UpdateVertex(VC, PVC, Precision::Confusion());
      Vertices.Append(VC);
    }
    if (StartOnEdge)
    {
      bool Start = true;
      Trim.AddOrConfuse(Start, E[0], E[1], Params);
      if (Params.Length() == Vertices.Length() && Params.Length() != 0)
        Vertices.SetValue(1, VS);

      else
        // the point was not found by IntersectWith
        Vertices.Prepend(VS);
    }
    if (EndOnEdge)
    {
      bool Start = false;
      Trim.AddOrConfuse(Start, E[0], E[1], Params);
      if (Params.Length() == Vertices.Length() && Params.Length() != 0)
        Vertices.SetValue(Params.Length(), VE);

      else
        // the point was not found by IntersectWith
        Vertices.Append(VE);
    }

    //------------------------------------------------------------
    // Update Detromp.
    // Detromp allows to remove vertices on the offset
    // corresponding to tangency zones
    // Detromp ranks the vertices that limit
    // the parts of the bissectrices located between the spine and the
    // offset.
    //------------------------------------------------------------
    if (!Detromp.IsBound(S[0]))
      Detromp.Bind(S[0], EmptyList);
    if (!Detromp.IsBound(S[1]))
      Detromp.Bind(S[1], EmptyList);

    UpdateDetromp(Detromp, S[0], S[1], Vertices, Params, Bisec, StartOnEdge, EndOnEdge, Trim);
    //----------------------------------------------
    // Storage of vertices on parallel edges.
    // fill MapBis and MapVerPar.
    //----------------------------------------------
    if (!Vertices.IsEmpty() && Params.Length() == Vertices.Length())
    {
      for (k = 0; k <= 1; k++)
      {
        MapBis.TryBound(E[k], EmptySeq);
        MapVerPar.TryBound(E[k], EmptySeqOfReal);
        for (int ii = 1; ii <= Vertices.Length(); ii++)
        {
          MapBis(E[k]).Append(Vertices.Value(ii));
          if (k == 0)
            MapVerPar(E[k]).Append(Params.Value(ii).Y());
          else
            MapVerPar(E[k]).Append(Params.Value(ii).Z());
        }
      }
    }
    else
    {
      //------------------------------------------------------------
      // FOR COMPLETE CIRCLES. the parallel line can be contained
      // in the zone without intersection with the border
      // no intersection
      // if myoffset is < distance of nodes the parallel can be valid.
      //-------------------------------------------------------------
      for (k = 0; k <= 1; k++)
      {
        if (!MapBis.IsBound(E[k]))
        {
          if (Node1->Distance() > myOffset && Node2->Distance() > myOffset)
          {
            MapBis.Bind(E[k], EmptySeq);
            MapVerPar.Bind(E[k], EmptySeqOfReal);
          }
        }
      }
    }
  }

#ifdef OCCT_DEBUG
#endif

  //----------------------------------
  // Construction of parallel edges.
  //----------------------------------
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> MapVV;

  TopoDS_Shape CurrentSpine;

  // NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>::Iterator ite1;

  for (j = 1; j <= myMap.Extent(); j++)
  {
    CurrentSpine = myMap.FindKey(j);
    CurrentEdge  = TopoDS::Edge(myMap(j).First());

    myMap(j).Clear();
    if (MapBis.IsBound(CurrentEdge))
    {
      NCollection_Sequence<TopoDS_Shape> aSeqOfShape;
      if (!MapBis(CurrentEdge).IsEmpty())
      {
        int IndOfE = 0;
        if (myIsOpenResult)
        {
          if (j == 1)
            IndOfE = 1;
          else if (j == myMap.Extent())
            IndOfE = -1;
        }
        TrimEdge(CurrentEdge,
                 CurrentSpine,
                 mySpine,
                 Detromp(CurrentSpine),
                 MapBis(CurrentEdge),
                 MapVerPar(CurrentEdge),
                 aSeqOfShape,
                 MapVV,
                 IndOfE);
        for (k = 1; k <= aSeqOfShape.Length(); k++)
        {
          myMap(j).Append(aSeqOfShape.Value(k));
        }
      }
      else
      {
        //-----------------
        // Complete circles
        //-----------------
        myMap(j).Append(CurrentEdge);
      }
    }
  }

  int ind;
  for (ind = 1; ind <= MapVV.Extent(); ind++)
  {
    TopoDS_Vertex OldV = TopoDS::Vertex(MapVV.FindKey(ind));
    TopoDS_Vertex NewV = TopoDS::Vertex(MapVV(ind));
    gp_Pnt        P1   = BRep_Tool::Pnt(OldV);
    gp_Pnt        P2   = BRep_Tool::Pnt(NewV);
    myBuilder.UpdateVertex(NewV, P1.Distance(P2));
    NCollection_List<TopoDS_Shape> LV;
    LV.Append(NewV.Oriented(TopAbs_FORWARD));
    BRepTools_Substitution aSubst;
    aSubst.Substitute(OldV, LV);
    for (j = 1; j <= myMap.Extent(); j++)
    {
      NCollection_List<TopoDS_Shape>::Iterator itl(myMap(j));
      for (; itl.More(); itl.Next())
      {
        aSubst.Build(itl.Value());
        if (aSubst.IsCopied(itl.Value()))
        {
          const NCollection_List<TopoDS_Shape>& listSh = aSubst.Copy(itl.Value());
          TopAbs_Orientation                    SaveOr = itl.Value().Orientation();
          itl.ChangeValue()                            = listSh.First();
          itl.ChangeValue().Orientation(SaveOr);
        }
      }
    }
  }

  //----------------------------------
  // Construction of offset wires.
  //----------------------------------
  MakeWires();

  // Update vertices ( Constructed in the plane Z = 0) !!!
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapVertex;
  for (TopExp_Explorer exp(myShape, TopAbs_VERTEX); exp.More(); exp.Next())
  {
    TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
    if (MapVertex.Add(V))
    {
      gp_Pnt P = BRep_Tool::Pnt(V);
      P        = RefPlane->Value(P.X(), P.Y());
      myBuilder.UpdateVertex(V, P, Precision::Confusion());
    }
  }

  // Construction of curves 3d.
  BRepLib::BuildCurves3d(myShape);
  MapVertex.Clear();
  TopExp_Explorer Explo(myShape, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Edge   anEdge = TopoDS::Edge(Explo.Current());
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anEdge, V1, V2);
    occ::handle<BRep_TVertex>& TV1 = *((occ::handle<BRep_TVertex>*)&(V1).TShape());
    occ::handle<BRep_TVertex>& TV2 = *((occ::handle<BRep_TVertex>*)&(V2).TShape());

    TopLoc_Location         loc;
    double                  f, l;
    occ::handle<Geom_Curve> theCurve = BRep_Tool::Curve(anEdge, loc, f, l);
    theCurve                         = occ::down_cast<Geom_Curve>(theCurve->Copy());
    theCurve->Transform(loc.Transformation());
    gp_Pnt f3d = theCurve->Value(f);
    gp_Pnt l3d = theCurve->Value(l);

    double dist1, dist2;
    dist1 = f3d.Distance(TV1->Pnt());
    dist2 = l3d.Distance(TV2->Pnt());
    if (!MapVertex.Contains(V1))
    {

      TV1->Pnt(f3d);
      MapVertex.Add(V1);
    }
    else
      TV1->UpdateTolerance(1.5 * dist1);
    if (!MapVertex.Contains(V2))
    {
      TV2->Pnt(l3d);
      MapVertex.Add(V2);
    }
    else
      TV2->UpdateTolerance(1.5 * dist2);
  }

  if (!myIsOpenResult)
    FixHoles();

  myIsDone = true;
}

//=================================================================================================

NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& BRepFill_OffsetWire::
  Generated()
{
  return myMap;
}

//=================================================================================================

void BRepFill_OffsetWire::PrepareSpine()
{
  BRep_Builder                             B;
  NCollection_List<TopoDS_Shape>           Cuts;
  NCollection_List<TopoDS_Shape>::Iterator IteCuts;
  TopoDS_Vertex                            V1, V2;

  myWorkSpine.Nullify();
  myMapSpine.Clear();

  TopLoc_Location                  L;
  const occ::handle<Geom_Surface>& S    = BRep_Tool::Surface(mySpine, L);
  double                           TolF = BRep_Tool::Tolerance(mySpine);
  B.MakeFace(myWorkSpine, S, L, TolF);

  for (TopoDS_Iterator IteF(mySpine); IteF.More(); IteF.Next())
  {

    TopoDS_Wire NW;
    B.MakeWire(NW);

    //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:55 2000 Begin
    int                                                           ForcedCut  = 0;
    int                                                           nbResEdges = -1;
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> EdgeMap;

    TopExp::MapShapes(IteF.Value(), TopAbs_EDGE, EdgeMap);
    int nbEdges = EdgeMap.Extent();

    if (nbEdges == 1 && !myIsOpenResult) // in case of open wire there's no need to do it
      ForcedCut = 2;
    //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:48 2000 End

    for (TopoDS_Iterator IteW(IteF.Value()); IteW.More(); IteW.Next())
    {

      const TopoDS_Edge& E = TopoDS::Edge(IteW.Value());
      EdgeVertices(E, V1, V2);
      myMapSpine.Bind(V1, V1);
      myMapSpine.Bind(V2, V2);
      Cuts.Clear();

      // Cut
      TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
      //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:29 2000 Begin
      occ::handle<BRep_TEdge> TEdge            = occ::down_cast<BRep_TEdge>(E.TShape());
      const int               aNumCurvesInEdge = TEdge->Curves().Size();
      if (nbEdges == 2 && nbResEdges == 0 && aNumCurvesInEdge > 1)
        ForcedCut = 1;
      //  Modified by Sergey KHROMOV - Thu Nov 16 17:29:33 2000 End
      nbResEdges = CutEdge(TopoDS::Edge(aLocalShape), mySpine, ForcedCut, Cuts);

      if (Cuts.IsEmpty())
      {
        B.Add(NW, E);
        myMapSpine.Bind(E, E);
      }
      else
      {
        for (IteCuts.Initialize(Cuts); IteCuts.More(); IteCuts.Next())
        {
          TopoDS_Edge NE = TopoDS::Edge(IteCuts.Value());
          NE.Orientation(E.Orientation());
          B.Add(NW, NE);
          myMapSpine.Bind(NE, E);
          EdgeVertices(NE, V1, V2);
          myMapSpine.TryBound(V1, E);
          myMapSpine.TryBound(V2, E);
        }
      }
    }
    //  Modified by Sergey KHROMOV - Thu Mar  7 09:17:41 2002 Begin
    TopoDS_Vertex aV1;
    TopoDS_Vertex aV2;

    TopExp::Vertices(NW, aV1, aV2);

    NW.Closed(aV1.IsSame(aV2));

    //  Modified by Sergey KHROMOV - Thu Mar  7 09:17:43 2002 End
    B.Add(myWorkSpine, NW);
  }

#ifdef OCCT_DEBUG
#endif
}

//=======================================================================
// function : UpdateDetromp
// purpose  : For each interval on bissectrice defined by parameters
//           test if the medium point is at a distance > offset
//           in this case vertices corresponding to the extremities of the interval
//           are ranked in the proofing.
//           => If the same vertex appears in the proofing, the
//           border of the zone of proximity is tangent to the offset .
//=======================================================================

void BRepFill_OffsetWire::UpdateDetromp(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& Detromp,
  const TopoDS_Shape&                                                Shape1,
  const TopoDS_Shape&                                                Shape2,
  const NCollection_Sequence<TopoDS_Shape>&                          Vertices,
  const NCollection_Sequence<gp_Pnt>&                                Params,
  const Bisector_Bisec&                                              Bisec,
  const bool                                                         SOnE,
  const bool                                                         EOnE,
  const BRepFill_TrimEdgeTool&                                       Trim) const
{
  int ii = 1;

  if (myJoinType == GeomAbs_Intersection)
  {
    for (; ii <= Vertices.Length(); ii++)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(Vertices.Value(ii));
      Detromp(Shape1).Append(aVertex);
      Detromp(Shape2).Append(aVertex);
    }
  }
  else // myJoinType == GeomAbs_Arc
  {
    double        U1, U2;
    TopoDS_Vertex V1, V2;

    const occ::handle<Geom2d_Curve>& Bis      = Bisec.Value();
    bool                             ForceAdd = false;
    occ::handle<Geom2d_TrimmedCurve> aTC      = occ::down_cast<Geom2d_TrimmedCurve>(Bis);
    if (!aTC.IsNull() && aTC->BasisCurve()->IsPeriodic())
    {
      gp_Pnt2d Pf = Bis->Value(Bis->FirstParameter());
      gp_Pnt2d Pl = Bis->Value(Bis->LastParameter());
      ForceAdd    = Pf.Distance(Pl) <= Precision::Confusion();
    }

    U1 = Bis->FirstParameter();

    if (SOnE)
    {
      // the first point of the bissectrice is on the offset
      V1 = TopoDS::Vertex(Vertices.Value(ii));
      ii++;
    }

    while (ii <= Vertices.Length() && ii <= Params.Length())
    {
      U2 = Params.Value(ii).X();
      V2 = TopoDS::Vertex(Vertices.Value(ii));

      gp_Pnt2d P = Bis->Value((U2 + U1) * 0.5);
      if (!Trim.IsInside(P) || ForceAdd)
      {
        if (!V1.IsNull())
        {
          Detromp(Shape1).Append(V1);
          Detromp(Shape2).Append(V1);
        }
        Detromp(Shape1).Append(V2);
        Detromp(Shape2).Append(V2);
      }
      U1 = U2;
      V1 = V2;
      ii++;
    }

    // test medium point between the last parameter and the end of the bissectrice.
    U2 = Bis->LastParameter();
    if (!EOnE)
    {
      if (!Precision::IsInfinite(U2))
      {
        gp_Pnt2d P = Bis->Value((U2 + U1) * 0.5);
        if (!Trim.IsInside(P) || ForceAdd)
        {
          if (!V1.IsNull())
          {
            Detromp(Shape1).Append(V1);
            Detromp(Shape2).Append(V1);
          }
        }
      }
      else
      {
        if (!V1.IsNull())
        {
          Detromp(Shape1).Append(V1);
          Detromp(Shape2).Append(V1);
        }
      }
    }
    // else if(myJoinType != GeomAbs_Arc)
    //{
    //   if (!V1.IsNull()) {
    //     Detromp(Shape1).Append(V1);
    //     Detromp(Shape2).Append(V1);
    //   }
    // }
  } // end of else (myJoinType==GeomAbs_Arc)
}

//=================================================================================================

void BRepFill_OffsetWire::MakeWires()
{
  //--------------------------------------------------------
  // creation of a single list of created parallel edges.
  //--------------------------------------------------------
  NCollection_Sequence<TopoDS_Shape>       TheEdges;
  NCollection_List<TopoDS_Shape>           TheWires;
  NCollection_List<TopoDS_Shape>::Iterator itl;
  // NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>::Iterator ite;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MVE;
  // NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
  // TopTools_ShapeMapHasher>::Iterator         MVEit;
  TopoDS_Vertex V1, V2, VF, CV;
  int           i;

  for (i = 1; i <= myMap.Extent(); i++)
  {
    for (itl.Initialize(myMap(i)); itl.More(); itl.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(itl.Value());
      TopExp::Vertices(E, V1, V2);
      if (V1.IsSame(V2) && IsSmallClosedEdge(E, V1))
        continue; // remove small closed edges
      if (!CheckSmallParamOnEdge(E))
        continue;
      MVE.Bound(V1, NCollection_List<TopoDS_Shape>())->Append(E);
      MVE.Bound(V2, NCollection_List<TopoDS_Shape>())->Append(E);
    }
  }

  //--------------------------------------
  // Creation of parallel wires.
  //--------------------------------------
  BRep_Builder B;

  //  int NbEdges;
  //  bool NewWire  = true;
  //  bool AddEdge  = false;

  TopoDS_Wire NW;
  bool        End;
  TopoDS_Edge CE;

  while (!MVE.IsEmpty())
  {
    B.MakeWire(NW);

    // MVEit.Initialize(MVE);
    for (i = 1; i <= MVE.Extent(); i++)
      if (MVE(i).Extent() == 1)
        break;

    // if(!MVEit.More()) MVEit.Initialize(MVE);
    if (i > MVE.Extent())
      i = 1;

    CV = VF = TopoDS::Vertex(MVE.FindKey(i));
    CE      = TopoDS::Edge(MVE(i).First());
    End     = false;
    MVE.ChangeFromKey(CV).RemoveFirst();

    if (myIsOpenResult && MVE.FindFromKey(CV).IsEmpty())
    {
      MVE.RemoveKey(CV);
    }

    //  Modified by Sergey KHROMOV - Thu Mar 14 11:29:59 2002 Begin
    bool isClosed = false;

    //  Modified by Sergey KHROMOV - Thu Mar 14 11:30:00 2002 End

    while (!End)
    {
      //-------------------------------
      // Construction of a wire.
      //-------------------------------
      TopExp::Vertices(CE, V1, V2);
      if (!CV.IsSame(V1))
        CV = V1;
      else
        CV = V2;

      B.Add(NW, CE);

      if (VF.IsSame(CV) || !MVE.Contains(CV))
      {
        //  Modified by Sergey KHROMOV - Thu Mar 14 11:30:14 2002 Begin
        isClosed = VF.IsSame(CV);
        //  Modified by Sergey KHROMOV - Thu Mar 14 11:30:15 2002 End
        End = true;
        MVE.RemoveKey(VF);
      }

      if (!End)
      {
        if (MVE.FindFromKey(CV).Extent() > 2)
        {
          // std::cout <<"vertex on more that 2 edges in a face."<<std::endl;
        }
        for (itl.Initialize(MVE.FindFromKey(CV)); itl.More(); itl.Next())
        {
          if (itl.Value().IsSame(CE))
          {
            MVE.ChangeFromKey(CV).Remove(itl);
            break;
          }
        }
        if (!MVE.FindFromKey(CV).IsEmpty())
        {
          CE = TopoDS::Edge(MVE.FindFromKey(CV).First());
          MVE.ChangeFromKey(CV).RemoveFirst();
        }
        else if (myIsOpenResult) // CV was a vertex with one edge
          End = true;

        if (MVE.FindFromKey(CV).IsEmpty())
        {
          MVE.RemoveKey(CV);
        }
      }
    }
    //  Modified by Sergey KHROMOV - Thu Mar 14 11:29:31 2002 Begin
    //     NW.Closed(true);
    NW.Closed(isClosed);
    //  Modified by Sergey KHROMOV - Thu Mar 14 11:29:37 2002 End
    TheWires.Append(NW);
  }

  // update myShape :
  //      -- if only one wire : myShape is a Wire
  //      -- if several wires : myShape is a Compound.
  if (TheWires.Extent() == 1)
  {
    myShape = TheWires.First();
  }
  else
  {
    TopoDS_Compound R;
    B.MakeCompound(R);
    NCollection_List<TopoDS_Shape>::Iterator it(TheWires);
    for (; it.More(); it.Next())
    {
      B.Add(R, it.Value());
    }
    myShape = R;
  }
}

//=================================================================================================

void BRepFill_OffsetWire::FixHoles()
{
  NCollection_Sequence<TopoDS_Shape> ClosedWires, UnclosedWires, IsolatedWires;

  double       MaxTol = 0.;
  BRep_Builder BB;

  TopExp_Explorer Explo(mySpine, TopAbs_VERTEX);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(Explo.Current());
    double               Tol     = BRep_Tool::Tolerance(aVertex);
    if (Tol > MaxTol)
      MaxTol = Tol;
  }
  MaxTol *= 100.;

  Explo.Init(myShape, TopAbs_WIRE);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Shape aWire = Explo.Current();
    // Remove duplicated edges
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                    EEmap;
    TopoDS_Iterator it(aWire);
    for (; it.More(); it.Next())
    {
      const TopoDS_Shape& anEdge = it.Value();
      if (!EEmap.IsBound(anEdge))
      {
        NCollection_List<TopoDS_Shape> LE;
        EEmap.Bind(anEdge, LE);
      }
      else
        EEmap(anEdge).Append(anEdge);
    }
    aWire.Free(true);
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
      Iterator mapit(EEmap);
    for (; mapit.More(); mapit.Next())
    {
      const NCollection_List<TopoDS_Shape>&    LE = mapit.Value();
      NCollection_List<TopoDS_Shape>::Iterator itl(LE);
      for (; itl.More(); itl.Next())
        BB.Remove(aWire, itl.Value());
    }
    // Sorting
    if (aWire.Closed())
      ClosedWires.Append(aWire);
    else
      UnclosedWires.Append(aWire);
  }

  while (!UnclosedWires.IsEmpty())
  {
    TopoDS_Wire&  Base = TopoDS::Wire(UnclosedWires(1));
    TopoDS_Vertex Vf, Vl;
    TopExp::Vertices(Base, Vf, Vl);
    if (Vf.IsNull() || Vl.IsNull())
    {
      throw Standard_Failure("BRepFill_OffsetWire::FixHoles(): Wrong wire.");
    }
    gp_Pnt Pf, Pl;
    Pf           = BRep_Tool::Pnt(Vf);
    Pl           = BRep_Tool::Pnt(Vl);
    double DistF = RealLast(), DistL = RealLast();
    int    IndexF = 0, IndexL = 0;
    bool   IsFirstF = false, IsFirstL = false;
    for (int i = 2; i <= UnclosedWires.Length(); i++)
    {
      TopoDS_Wire   aWire = TopoDS::Wire(UnclosedWires(i));
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(aWire, V1, V2);

      if (V1.IsNull() || V2.IsNull())
      {
        throw Standard_Failure("BRepFill_OffsetWire::FixHoles(): Wrong wire.");
      }

      gp_Pnt P1, P2;
      P1          = BRep_Tool::Pnt(V1);
      P2          = BRep_Tool::Pnt(V2);
      double dist = Pf.Distance(P1);
      if (dist < DistF)
      {
        DistF    = dist;
        IndexF   = i;
        IsFirstF = true;
      }
      dist = Pf.Distance(P2);
      if (dist < DistF)
      {
        DistF    = dist;
        IndexF   = i;
        IsFirstF = false;
      }
      dist = Pl.Distance(P1);
      if (dist < DistL)
      {
        DistL    = dist;
        IndexL   = i;
        IsFirstL = true;
      }
      dist = Pl.Distance(P2);
      if (dist < DistL)
      {
        DistL    = dist;
        IndexL   = i;
        IsFirstL = false;
      }
    }
    if (DistF > MaxTol)
      IndexF = 0;
    if (DistL > MaxTol)
      IndexL = 0;
    TopoDS_Wire   theWire;
    TopoDS_Edge   theEdge;
    TopoDS_Vertex theVertex;
    double        CommonTol;
    bool          TryToClose = true;
    if (DistF <= MaxTol && DistL <= MaxTol && IndexF == IndexL && IsFirstF == IsFirstL)
    {
      if (DistF < DistL)
      {
        DistL = RealLast();
        IndexL++;
      }
      else
      {
        DistF = RealLast();
        IndexF++;
      }
      TryToClose = false;
    }
    if (DistF <= MaxTol)
    {
      theWire = TopoDS::Wire(UnclosedWires(IndexF));
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(theWire, V1, V2);
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        VEmap;
      TopExp::MapShapesAndAncestors(theWire, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      theEdge = (IsFirstF) ? TopoDS::Edge(VEmap.FindFromKey(V1).First())
                           : TopoDS::Edge(VEmap.FindFromKey(V2).First());
      TopoDS_Iterator it(theWire);
      for (; it.More(); it.Next())
      {
        TopoDS_Edge anEdge = TopoDS::Edge(it.Value());
        if (IsFirstF)
          anEdge.Reverse();
        if (!anEdge.IsSame(theEdge))
          BB.Add(Base, anEdge);
      }
      theVertex = (IsFirstF) ? V1 : V2;
      CommonTol = std::max(BRep_Tool::Tolerance(Vf), BRep_Tool::Tolerance(theVertex));
      if (DistF <= CommonTol)
      {
        theEdge.Free(true);
        Vf.Orientation(theVertex.Orientation());
        BB.Remove(theEdge, theVertex);
        BB.Add(theEdge, Vf);
        BB.UpdateVertex(Vf, CommonTol);
        if (IsFirstF)
          theEdge.Reverse();
        BB.Add(Base, theEdge);
      }
      else
      {
        if (IsFirstF)
          theEdge.Reverse();
        BB.Add(Base, theEdge);
        // Creating new edge from theVertex to Vf
        TopoDS_Edge NewEdge = BRepLib_MakeEdge(theVertex, Vf);
        BB.Add(Base, NewEdge);
      }
    }
    if (DistL <= MaxTol && IndexL != IndexF)
    {
      theWire = TopoDS::Wire(UnclosedWires(IndexL));
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(theWire, V1, V2);
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        VEmap;
      TopExp::MapShapesAndAncestors(theWire, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      theEdge = (IsFirstL) ? TopoDS::Edge(VEmap.FindFromKey(V1).First())
                           : TopoDS::Edge(VEmap.FindFromKey(V2).First());
      TopoDS_Iterator it(theWire);
      for (; it.More(); it.Next())
      {
        TopoDS_Edge anEdge = TopoDS::Edge(it.Value());
        if (!IsFirstL)
          anEdge.Reverse();
        if (!anEdge.IsSame(theEdge))
          BB.Add(Base, anEdge);
      }
      theVertex = (IsFirstL) ? V1 : V2;
      CommonTol = std::max(BRep_Tool::Tolerance(Vl), BRep_Tool::Tolerance(theVertex));
      if (DistL <= CommonTol)
      {
        theEdge.Free(true);
        Vl.Orientation(theVertex.Orientation());
        BB.Remove(theEdge, theVertex);
        BB.Add(theEdge, Vl);
        BB.UpdateVertex(Vl, CommonTol);
        if (!IsFirstL)
          theEdge.Reverse();
        BB.Add(Base, theEdge);
      }
      else
      {
        if (!IsFirstL)
          theEdge.Reverse();
        BB.Add(Base, theEdge);
        // Creating new edge from Vl to theVertex
        TopoDS_Edge NewEdge = BRepLib_MakeEdge(Vl, theVertex);
        BB.Add(Base, NewEdge);
      }
    }
    // Check if it is possible to close resulting wire
    if (TryToClose)
    {
      TopExp::Vertices(Base, Vf, Vl);
      CommonTol = std::max(BRep_Tool::Tolerance(Vf), BRep_Tool::Tolerance(Vl));
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        VEmap;
      TopExp::MapShapesAndAncestors(Base, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      TopoDS_Edge Efirst, Elast;
      Efirst      = TopoDS::Edge(VEmap.FindFromKey(Vf).First());
      Elast       = TopoDS::Edge(VEmap.FindFromKey(Vl).First());
      Pf          = BRep_Tool::Pnt(Vf);
      Pl          = BRep_Tool::Pnt(Vl);
      double Dist = Pf.Distance(Pl);
      if (Dist <= CommonTol)
      {
        Elast.Free(true);
        Vf.Orientation(Vl.Orientation());
        BB.Remove(Elast, Vl);
        BB.Add(Elast, Vf);
        BB.UpdateVertex(Vf, CommonTol);
        Base.Closed(true);
      }
      else if (Dist <= MaxTol)
      {
        // Creating new edge from Vl to Vf
        TopoDS_Edge NewEdge = BRepLib_MakeEdge(Vf, Vl);
        BB.Add(Base, NewEdge);
        Base.Closed(true);
      }
    }
    // Updating sequences ClosedWires and UnclosedWires
    if (DistF <= MaxTol)
      UnclosedWires.Remove(IndexF);
    if (DistL <= MaxTol && IndexL != IndexF)
    {
      if (DistF <= MaxTol && IndexL > IndexF)
        IndexL--;
      UnclosedWires.Remove(IndexL);
    }
    if (Base.Closed())
    {
      ClosedWires.Append(Base);
      UnclosedWires.Remove(1);
    }
    else if (DistF > MaxTol && DistL > MaxTol)
    {
      IsolatedWires.Append(Base);
      UnclosedWires.Remove(1);
    }
  }

  // Updating myShape
  if (ClosedWires.Length() + IsolatedWires.Length() == 1)
  {
    if (!ClosedWires.IsEmpty())
      myShape = ClosedWires.First();
    else
      myShape = IsolatedWires.First();
  }
  else
  {
    TopoDS_Compound R;
    BB.MakeCompound(R);
    int i;
    for (i = 1; i <= ClosedWires.Length(); i++)
      BB.Add(R, ClosedWires(i));
    for (i = 1; i <= IsolatedWires.Length(); i++)
      BB.Add(R, IsolatedWires(i));
    myShape = R;
  }
}

//=======================================================================
// function : CutEdge
// purpose  : Cut edge at the extrema of curvatures and points of inflexion.
//           So, closed circles are cut in two.
//           If <Cuts> is empty, the edge is not modified.
//           The first and the last vertex of the initial edge
//           belong to the first and the last parts respectively.
//=======================================================================
int CutEdge(const TopoDS_Edge&              E,
            const TopoDS_Face&              F,
            int                             ForceCut,
            NCollection_List<TopoDS_Shape>& Cuts)
{
  Cuts.Clear();
  MAT2d_CutCurve                                  Cuter;
  NCollection_Sequence<occ::handle<Geom2d_Curve>> theCurves;
  double                                          f, l;
  occ::handle<Geom2d_Curve>                       C2d;
  occ::handle<Geom2d_TrimmedCurve>                CT2d;
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:25 2002 Begin
  double                  aTol = BRep_Tool::Tolerance(E);
  occ::handle<Geom_Curve> aC;
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:25 2002 End

  TopoDS_Vertex V1, V2, VF, VL;
  TopExp::Vertices(E, V1, V2);
  BRep_Builder B;

  C2d = BRep_Tool::CurveOnSurface(E, F, f, l);
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:54 2002 Begin
  aC = BRep_Tool::Curve(E, f, l);
  //  Modified by Sergey KHROMOV - Wed Mar  6 17:36:54 2002 End
  CT2d = new Geom2d_TrimmedCurve(C2d, f, l);
  // if (E.Orientation() == TopAbs_REVERSED) CT2d->Reverse();

  if (CT2d->BasisCurve()->IsKind(STANDARD_TYPE(Geom2d_Circle)) && (std::abs(f - l) >= M_PI))
  {
    return 0;
  }

  //-------------------------
  // Cut curve.
  //-------------------------
  Cuter.Perform(CT2d);

  //  Modified by Sergey KHROMOV - Thu Nov 16 17:28:29 2000 Begin
  if (ForceCut == 0)
  {
    if (Cuter.UnModified())
    {
      //-----------------------------
      // edge not modified => return.
      //-----------------------------
      return 0;
    }
    else
    {
      for (int k = 1; k <= Cuter.NbCurves(); k++)
        theCurves.Append(Cuter.Value(k));
    }
  }
  else if (ForceCut == 1)
  {
    if (Cuter.UnModified())
    {
      CutCurve(CT2d, 2, theCurves);
    }
    else
    {
      for (int k = 1; k <= Cuter.NbCurves(); k++)

        theCurves.Append(Cuter.Value(k));
    }
  }
  else if (ForceCut == 2)
  {
    if (Cuter.UnModified())
    {
      CutCurve(CT2d, 3, theCurves);
    }
    else
    {
      if (Cuter.NbCurves() == 2)
      {

        occ::handle<Geom2d_TrimmedCurve> CC = Cuter.Value(1);

        if (CC->LastParameter() > (l + f) / 2.)
        {
          CutCurve(CC, 2, theCurves);
          theCurves.Append(Cuter.Value(2));
        }
        else
        {
          theCurves.Append(CC);
          CutCurve(Cuter.Value(2), 2, theCurves);
        }
      }
      else
      {
        for (int k = 1; k <= Cuter.NbCurves(); k++)
          theCurves.Append(Cuter.Value(k));
      }
    }
  }

  //  Modified by Sergey KHROMOV - Thu Nov 16 17:28:37 2000 End

  //--------------------------------------
  // Creation of cut edges.
  //--------------------------------------
  VF = V1;

  for (int k = 1; k <= theCurves.Length(); k++)
  {

    occ::handle<Geom2d_TrimmedCurve> CC = occ::down_cast<Geom2d_TrimmedCurve>(theCurves.Value(k));

    if (k == theCurves.Length())
    {
      VL = V2;
    }
    else
    {
      //  Modified by Sergey KHROMOV - Wed Mar  6 17:38:02 2002 Begin
      gp_Pnt P = aC->Value(CC->LastParameter());

      VL = BRepLib_MakeVertex(P);
      B.UpdateVertex(VL, aTol);
      //  Modified by Sergey KHROMOV - Wed Mar  6 17:38:05 2002 End
    }
    TopoDS_Shape aLocalShape = E.EmptyCopied();
    TopoDS_Edge  NE          = TopoDS::Edge(aLocalShape);
    //      TopoDS_Edge NE = TopoDS::Edge(E.EmptyCopied());
    NE.Orientation(TopAbs_FORWARD);
    B.Add(NE, VF.Oriented(TopAbs_FORWARD));
    B.Add(NE, VL.Oriented(TopAbs_REVERSED));
    B.Range(NE, CC->FirstParameter(), CC->LastParameter());
    Cuts.Append(NE.Oriented(E.Orientation()));
    VF = VL;
  }

  return theCurves.Length();
}

//  Modified by Sergey KHROMOV - Thu Nov 16 17:27:56 2000 Begin
//=================================================================================================

void CutCurve(const occ::handle<Geom2d_TrimmedCurve>&          C,
              const int                                        nbParts,
              NCollection_Sequence<occ::handle<Geom2d_Curve>>& theCurves)
{
  occ::handle<Geom2d_TrimmedCurve> TrimC;
  double                           UF, UL, UC;
  double                           Step;
  gp_Pnt2d                         PF, PL, PC;
  constexpr double                 PTol  = Precision::PConfusion() * 10;
  constexpr double                 Tol   = Precision::Confusion() * 10;
  bool                             YaCut = false;

  UF = C->FirstParameter();
  UL = C->LastParameter();
  PF = C->Value(UF);
  PL = C->Value(UL);

  Step = (UL - UF) / nbParts;

  for (int i = 1; i < nbParts; i++)
  {

    UC = UF + i * Step;
    PC = C->Value(UC);

    if (UC - UF > PTol && PC.Distance(PF) > Tol)
    {
      if (UL - UC < PTol || PL.Distance(PC) < Tol)
        continue;

      TrimC = new Geom2d_TrimmedCurve(C, UF, UC);
      theCurves.Append(TrimC);
      UF    = UC;
      PF    = PC;
      YaCut = true;
    }
  }
  if (YaCut)
  {
    TrimC = new Geom2d_TrimmedCurve(C, UF, UL);
    theCurves.Append(TrimC);
  }
  else
    theCurves.Append(C);
}

//  Modified by Sergey KHROMOV - Thu Nov 16 17:28:13 2000 End

//=================================================================================================

void MakeCircle(const TopoDS_Edge&                                                        E,
                const TopoDS_Vertex&                                                      V,
                const TopoDS_Face&                                                        F,
                const double                                                              Offset,
                NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& Map,
                const occ::handle<Geom_Plane>&                                            RefPlane)
{
  // evaluate the Axis of the Circle.
  double                    f, l;
  occ::handle<Geom2d_Curve> GC = BRep_Tool::CurveOnSurface(E, F, f, l);
  gp_Vec2d                  DX;
  gp_Pnt2d                  P;

  if (E.Orientation() == TopAbs_FORWARD)
  {
    GC->D1(l, P, DX);
    DX.Reverse();
  }
  else
    GC->D1(f, P, DX);

  gp_Ax2d                    Axis(P, gp_Dir2d(DX));
  occ::handle<Geom2d_Circle> Circ = new Geom2d_Circle(Axis, std::abs(Offset), Offset < 0.);

  // Bind the edges in my Map.
  TopoDS_Edge                    OE = BRepLib_MakeEdge(Circ, RefPlane);
  NCollection_List<TopoDS_Shape> LL;

  LL.Append(OE);
  Map.Add(V, LL);

#ifdef OCCT_DEBUG
#endif
}

//=================================================================================================

void MakeOffset(const TopoDS_Edge&                                                        E,
                const TopoDS_Face&                                                        F,
                const double                                                              Offset,
                NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>>& Map,
                const occ::handle<Geom_Plane>&                                            RefPlane,
                const bool             IsOpenResult,
                const GeomAbs_JoinType theJoinType,
                const TopoDS_Vertex*   Ends)
{
  double f, l;
  double anOffset = Offset;

  if (E.Orientation() == TopAbs_FORWARD)
    anOffset *= -1;

  occ::handle<Geom2d_Curve> G2d = BRep_Tool::CurveOnSurface(E, F, f, l);
  occ::handle<Geom2d_Curve> G2dOC;

  bool ToExtendFirstPar = true;
  bool ToExtendLastPar  = true;
  if (IsOpenResult)
  {
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(E, V1, V2);
    if (V1.IsSame(Ends[0]) || V1.IsSame(Ends[1]))
      ToExtendFirstPar = false;
    if (V2.IsSame(Ends[0]) || V2.IsSame(Ends[1]))
      ToExtendLastPar = false;
  }

  Geom2dAdaptor_Curve AC(G2d, f, l);
  if (AC.GetType() == GeomAbs_Circle)
  {
    // if the offset is greater otr equal to the radius and the side of the
    // concavity of the circle => edge null.
    gp_Circ2d C1(AC.Circle());
    gp_Ax22d  axes(C1.Axis());
    gp_Dir2d  Xd      = axes.XDirection();
    gp_Dir2d  Yd      = axes.YDirection();
    double    Crossed = Xd.X() * Yd.Y() - Xd.Y() * Yd.X();
    double    Signe   = (Crossed > 0.) ? -1. : 1.;

    if (anOffset * Signe < AC.Circle().Radius() - Precision::Confusion())
    {

      occ::handle<Geom2dAdaptor_Curve> AHC = new Geom2dAdaptor_Curve(G2d);
      Adaptor2d_OffsetCurve            Off(AHC, anOffset);
      occ::handle<Geom2d_Circle>       CC = new Geom2d_Circle(Off.Circle());

      double Delta = 2 * M_PI - l + f;
      if (theJoinType == GeomAbs_Arc)
      {
        if (ToExtendFirstPar)
          f -= 0.2 * Delta;
        if (ToExtendLastPar)
          l += 0.2 * Delta;
      }
      else // GeomAbs_Intersection
      {
        if (ToExtendFirstPar && ToExtendLastPar)
        {
          double old_l = l;
          f            = old_l + Delta / 2.;
          l            = f + 2 * M_PI;
        }
        else if (ToExtendFirstPar)
        {
          f = l;
          l = f + 2 * M_PI;
        }
        else if (ToExtendLastPar)
        {
          l = f + 2 * M_PI;
        }
      }
      G2dOC = new Geom2d_TrimmedCurve(CC, f, l);
    }
  }
  else if (AC.GetType() == GeomAbs_Line)
  {
    occ::handle<Geom2dAdaptor_Curve> AHC = new Geom2dAdaptor_Curve(G2d);
    Adaptor2d_OffsetCurve            Off(AHC, anOffset);
    occ::handle<Geom2d_Line>         CC    = new Geom2d_Line(Off.Line());
    double                           Delta = (l - f);
    if (ToExtendFirstPar)
    {
      if (theJoinType == GeomAbs_Arc)
        f -= Delta;
      else // GeomAbs_Intersection
        f = -Precision::Infinite();
    }
    if (ToExtendLastPar)
    {
      if (theJoinType == GeomAbs_Arc)
        l += Delta;
      else // GeomAbs_Intersection
        l = Precision::Infinite();
    }
    G2dOC = new Geom2d_TrimmedCurve(CC, f, l);
  }
  else
  {

    occ::handle<Geom2d_TrimmedCurve> G2dT = new Geom2d_TrimmedCurve(G2d, f, l);
    G2dOC                                 = new Geom2d_OffsetCurve(G2dT, anOffset);
  }

  // Bind the edges in my Map.
  if (!G2dOC.IsNull())
  {
    TopoDS_Edge OE = BRepLib_MakeEdge(G2dOC, RefPlane);
    OE.Orientation(E.Orientation());
    NCollection_List<TopoDS_Shape> LL;
    LL.Append(OE);
    Map.Add(E, LL);

#ifdef OCCT_DEBUG
#endif
  }
}

//=================================================================================================

bool VertexFromNode(const occ::handle<MAT_Node>&                              aNode,
                    const double                                              Offset,
                    gp_Pnt2d&                                                 PN,
                    NCollection_DataMap<occ::handle<MAT_Node>, TopoDS_Shape>& MapNodeVertex,
                    TopoDS_Vertex&                                            VN)
{
  bool             Status;
  constexpr double Tol = Precision::Confusion();
  BRep_Builder     B;

  if (!aNode->Infinite() && std::abs(aNode->Distance() - Offset) < Tol)
  {
    //------------------------------------------------
    // the Node gives a vertex on the offset
    //------------------------------------------------
    if (MapNodeVertex.IsBound(aNode))
    {
      VN = TopoDS::Vertex(MapNodeVertex(aNode));
    }
    else
    {
      gp_Pnt P(PN.X(), PN.Y(), 0.);
      B.MakeVertex(VN);
      B.UpdateVertex(VN, P, Precision::Confusion());
      MapNodeVertex.Bind(aNode, VN);
    }
    Status = true;
  }
  else
    Status = false;

  return Status;
}

//=================================================================================================

void StoreInMap(
  const TopoDS_Shape&                                                              V1,
  const TopoDS_Shape&                                                              V2,
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& MapVV)
{
  TopoDS_Shape OldV = V1, NewV = V2;
  int          i;

  if (MapVV.Contains(V2))
    NewV = MapVV.FindFromKey(V2);

  if (MapVV.Contains(V1))
    MapVV.ChangeFromKey(V1) = NewV;

  for (i = 1; i <= MapVV.Extent(); i++)
    if (MapVV(i).IsSame(V1))
      MapVV(i) = NewV;

  MapVV.Add(V1, NewV);
}

//=================================================================================================

void TrimEdge(
  const TopoDS_Edge&                                                               E,
  const TopoDS_Shape&                                                              ProE,
  const TopoDS_Face&                                                               AllSpine,
  const NCollection_List<TopoDS_Shape>&                                            Detromp,
  NCollection_Sequence<TopoDS_Shape>&                                              TheVer,
  NCollection_Sequence<double>&                                                    ThePar,
  NCollection_Sequence<TopoDS_Shape>&                                              S,
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& MapVV,
  const int                                                                        IndOfE)
{
  bool         Change = true;
  BRep_Builder TheBuilder;
  S.Clear();

  //-----------------------------------------------------------
  // Parse two sequences depending on the parameter on the edge.
  //-----------------------------------------------------------
  while (Change)
  {
    Change = false;
    for (int i = 1; i < ThePar.Length(); i++)
    {
      if (ThePar.Value(i) > ThePar.Value(i + 1))
      {
        ThePar.Exchange(i, i + 1);
        TheVer.Exchange(i, i + 1);
        Change = true;
      }
    }
  }

  //----------------------------------------------------------
  // If a vertex is not in the proofing, it is eliminated.
  //----------------------------------------------------------
  if (!BRep_Tool::Degenerated(E))
  {
    for (int k = 1; k <= TheVer.Length(); k++)
    {
      if (DoubleOrNotInside(Detromp, TopoDS::Vertex(TheVer.Value(k))))
      {
        TheVer.Remove(k);
        ThePar.Remove(k);
        k--;
      }
    }
  }

  //----------------------------------------------------------
  // If a vertex_double appears twice in the proofing
  // the vertex is removed.
  // otherwise preserve only one of its representations.
  //----------------------------------------------------------
  if (!BRep_Tool::Degenerated(E))
  {
    constexpr double aParTol = 2.0 * Precision::PConfusion();
    for (int k = 1; k < TheVer.Length(); k++)
    {
      if (TheVer.Value(k).IsSame(TheVer.Value(k + 1))
          || std::abs(ThePar.Value(k) - ThePar.Value(k + 1)) <= aParTol)
      {

        if (k + 1 == TheVer.Length())
        {
          StoreInMap(TheVer(k), TheVer(k + 1), MapVV);
          TheVer.Remove(k);
          ThePar.Remove(k);
        }
        else
        {
          StoreInMap(TheVer(k + 1), TheVer(k), MapVV);
          TheVer.Remove(k + 1);
          ThePar.Remove(k + 1);
        }
        /*
        if ( DoubleOrNotInside (Detromp,
        TopoDS::Vertex(TheVer.Value(k)))) {
        TheVer.Remove(k);
        ThePar.Remove(k);
        k--;
        }
        */
        k--;
      }
    }
  }
  //-----------------------------------------------------------
  // Creation of edges.
  // the number of vertices should be even. The created edges
  // go from a vertex with uneven index i to vertex i+1;
  //-----------------------------------------------------------
  if (IndOfE == 1 || IndOfE == -1) // open result and extreme edges of result
  {
    TopoDS_Shape  aLocalShape = E.EmptyCopied();
    TopoDS_Edge   NewEdge     = TopoDS::Edge(aLocalShape);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(E, V1, V2);
    double                    fpar, lpar;
    occ::handle<Geom_Surface> aPlane;
    TopLoc_Location           aLoc;
    occ::handle<Geom2d_Curve> PCurve;
    BRep_Tool::CurveOnSurface(E, PCurve, aPlane, aLoc, fpar, lpar);
    // BRep_Tool::Range(E, fpar, lpar);

    double TrPar1, TrPar2;
    bool   ToTrimAsOrigin = IsInnerEdge(ProE, AllSpine, TrPar1, TrPar2);

    if (IndOfE == 1) // first edge of open wire
    {
      if (NewEdge.Orientation() == TopAbs_FORWARD)
      {
        if (ToTrimAsOrigin)
        {
          fpar             = TrPar1;
          gp_Pnt2d TrPnt2d = PCurve->Value(fpar);
          gp_Pnt   TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V1 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, V1.Oriented(TopAbs_FORWARD));
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_REVERSED));
        TheBuilder.Range(NewEdge, fpar, ThePar.First());
      }
      else
      {
        if (ToTrimAsOrigin)
        {
          lpar             = TrPar2;
          gp_Pnt2d TrPnt2d = PCurve->Value(lpar);
          gp_Pnt   TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V2 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_REVERSED));
        TheBuilder.Add(NewEdge, V2.Oriented(TopAbs_FORWARD));
        TheBuilder.Range(NewEdge, ThePar.First(), lpar);
      }
    }
    else // last edge of open wire
    {
      if (NewEdge.Orientation() == TopAbs_FORWARD)
      {
        if (ToTrimAsOrigin)
        {
          lpar             = TrPar2;
          gp_Pnt2d TrPnt2d = PCurve->Value(lpar);
          gp_Pnt   TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V2 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_FORWARD));
        TheBuilder.Add(NewEdge, V2.Oriented(TopAbs_REVERSED));
        TheBuilder.Range(NewEdge, ThePar.First(), lpar);
      }
      else
      {
        if (ToTrimAsOrigin)
        {
          fpar             = TrPar1;
          gp_Pnt2d TrPnt2d = PCurve->Value(fpar);
          gp_Pnt   TrPnt(TrPnt2d.X(), TrPnt2d.Y(), 0.);
          V1 = BRepLib_MakeVertex(TrPnt);
        }
        TheBuilder.Add(NewEdge, V1.Oriented(TopAbs_REVERSED));
        TheBuilder.Add(NewEdge, TheVer.First().Oriented(TopAbs_FORWARD));
        TheBuilder.Range(NewEdge, fpar, ThePar.First());
      }
    }
    S.Append(NewEdge);
  }
  else
  {
    for (int k = 1; k < TheVer.Length(); k = k + 2)
    {
      TopoDS_Shape aLocalShape = E.EmptyCopied();
      TopoDS_Edge  NewEdge     = TopoDS::Edge(aLocalShape);
      //    TopoDS_Edge NewEdge = TopoDS::Edge(E.EmptyCopied());

      if (NewEdge.Orientation() == TopAbs_REVERSED)
      {
        TheBuilder.Add(NewEdge, TheVer.Value(k).Oriented(TopAbs_REVERSED));
        TheBuilder.Add(NewEdge, TheVer.Value(k + 1).Oriented(TopAbs_FORWARD));
      }
      else
      {
        TheBuilder.Add(NewEdge, TheVer.Value(k).Oriented(TopAbs_FORWARD));
        TheBuilder.Add(NewEdge, TheVer.Value(k + 1).Oriented(TopAbs_REVERSED));
      }

      TheBuilder.Range(NewEdge, ThePar.Value(k), ThePar.Value(k + 1));

#ifdef OCCT_DEBUG
#endif
      S.Append(NewEdge);
    }
  }
}

//=================================================================================================

static bool IsInnerEdge(const TopoDS_Shape& ProE,
                        const TopoDS_Face&  AllSpine,
                        double&             TrPar1,
                        double&             TrPar2)
{
  if (ProE.ShapeType() != TopAbs_EDGE)
    return false;

  TopoDS_Edge anEdge = TopoDS::Edge(ProE);

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    VEmap;
  TopExp::MapShapesAndAncestors(AllSpine, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
  for (int i = 1; i <= VEmap.Extent(); i++)
  {
    const NCollection_List<TopoDS_Shape>& LE = VEmap(i);
    if (LE.Extent() == 1 && anEdge.IsSame(LE.First()))
      return false;
  }

  BRep_Tool::Range(anEdge, TrPar1, TrPar2);
  return true;
}

//=======================================================================
// function : DoubleOrNotInside
// purpose  : return True if V appears twice in LV or is not inside.
//=======================================================================

bool DoubleOrNotInside(const NCollection_List<TopoDS_Shape>& LV, const TopoDS_Vertex& V)
{
  bool                                     Vu = false;
  NCollection_List<TopoDS_Shape>::Iterator it(LV);

  for (; it.More(); it.Next())
  {
    if (V.IsSame(it.Value()))
    {
      if (Vu)
        return true;
      else
        Vu = true;
    }
  }
  return !Vu;
}

bool IsSmallClosedEdge(const TopoDS_Edge& anEdge, const TopoDS_Vertex& aVertex)
{
  gp_Pnt   PV = BRep_Tool::Pnt(aVertex);
  gp_Pnt2d PV2d, Pfirst, Plast, Pmid;
  PV2d.SetCoord(PV.X(), PV.Y());

  occ::handle<Geom2d_Curve>             PCurve;
  occ::handle<BRep_CurveRepresentation> CurveRep =
    ((occ::down_cast<BRep_TEdge>(anEdge.TShape()))->Curves()).First();
  PCurve = CurveRep->PCurve();

  double fpar = (occ::down_cast<BRep_GCurve>(CurveRep))->First();
  double lpar = (occ::down_cast<BRep_GCurve>(CurveRep))->Last();
  Pfirst      = PCurve->Value(fpar);
  Plast       = PCurve->Value(lpar);
  Pmid        = PCurve->Value((fpar + lpar) * 0.5);

  double theTol = BRep_Tool::Tolerance(aVertex);
  theTol *= 1.5;

  double dist1 = Pfirst.Distance(PV2d);
  double dist2 = Plast.Distance(PV2d);
  double dist3 = Pmid.Distance(PV2d);

  return dist1 <= theTol && dist2 <= theTol && dist3 <= theTol;
}

static void CheckBadEdges(const TopoDS_Face&              Spine,
                          const double                    Offset,
                          const BRepMAT2d_BisectingLocus& Locus,
                          const BRepMAT2d_LinkTopoBilo&   Link,
                          NCollection_List<TopoDS_Shape>& BadEdges)
{

  TopoDS_Face      F       = TopoDS::Face(Spine.Oriented(TopAbs_FORWARD));
  constexpr double eps     = Precision::Confusion();
  double           LimCurv = 1. / Offset;

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;

  for (int ic = 1; ic <= Locus.NumberOfContours(); ic++)
  {
    for (int ie = 1; ie <= Locus.NumberOfElts(ic); ie++)
    {
      const TopoDS_Shape& SE = Link.GeneratingShape(Locus.BasicElt(ic, ie));
      if (SE.ShapeType() == TopAbs_EDGE)
      {

        if (aMap.Contains(SE))
        {
          // std::cout << "Edge is treated second time" << std::endl;
          continue;
        }

        TopoDS_Edge E = TopoDS::Edge(SE);

        double f, l;

        occ::handle<Geom2d_Curve> G2d = BRep_Tool::CurveOnSurface(E, F, f, l);

        Geom2dAdaptor_Curve AC(G2d, f, l);
        GeomAbs_CurveType   aCType = AC.GetType();

        if (aCType != GeomAbs_Line && aCType != GeomAbs_Circle)
        {

          bool reverse = false;
          if (E.Orientation() == TopAbs_FORWARD)
            reverse = true;

          gp_Pnt2d P, Pc;
          gp_Dir2d N;

          Geom2dLProp_CLProps2d aCLProps(G2d, 2, eps);

          aCLProps.SetParameter(f);
          if (!aCLProps.IsTangentDefined())
          {
            BadEdges.Append(SE);
            aMap.Add(SE);
            continue;
          }

          P          = aCLProps.Value();
          double Crv = aCLProps.Curvature();

          if (Crv >= eps)
          {
            aCLProps.Tangent(N);
            double x = N.Y(), y = -N.X();
            N.SetCoord(x, y);
            if (reverse)
              N.Reverse();
            aCLProps.CentreOfCurvature(Pc);
            gp_Vec2d Dir(P, Pc);
            if (N.Dot(Dir) > 0.)
            {
              if (LimCurv <= Crv + eps)
              {
                BadEdges.Append(SE);
                aMap.Add(SE);
                continue;
              }
            }
          }

          aCLProps.SetParameter(l);
          if (!aCLProps.IsTangentDefined())
          {
            BadEdges.Append(SE);
            aMap.Add(SE);
            continue;
          }

          P   = aCLProps.Value();
          Crv = aCLProps.Curvature();

          if (Crv >= eps)
          {
            aCLProps.Tangent(N);
            double x = N.Y(), y = -N.X();
            N.SetCoord(x, y);
            if (reverse)
              N.Reverse();
            aCLProps.CentreOfCurvature(Pc);
            gp_Vec2d Dir(P, Pc);
            if (N.Dot(Dir) > 0.)
            {
              if (LimCurv <= Crv + eps)
              {
                BadEdges.Append(SE);
                aMap.Add(SE);
                continue;
              }
            }
          }
        }
      }
    }
  }
}

//=================================================================================================

static bool PerformCurve(NCollection_Sequence<double>& Parameters,

                         NCollection_Sequence<gp_Pnt>& Points,
                         const Adaptor3d_Curve&        C,
                         const double                  Deflection,
                         const double                  U1,
                         const double                  U2,
                         const double                  EPSILON,
                         const int                     Nbmin)
{
  double UU1 = std::min(U1, U2);
  double UU2 = std::max(U1, U2);

  gp_Pnt Pdeb, Pfin;
  gp_Vec Ddeb, Dfin;
  C.D1(UU1, Pdeb, Ddeb);
  Parameters.Append(UU1);
  Points.Append(Pdeb);

  C.D1(UU2, Pfin, Dfin);

  const double aDelta = UU2 - UU1;
  const double aDist  = Pdeb.Distance(Pfin);

  if ((aDelta / aDist) > 5.0e-14)
  {
    QuasiFleche(C,
                Deflection * Deflection,

                UU1,
                Pdeb,
                Ddeb,
                UU2,
                Pfin,
                Dfin,
                Nbmin,
                EPSILON * EPSILON,
                Parameters,
                Points);
  }

  return true;
}

//=================================================================================================

static void QuasiFleche(const Adaptor3d_Curve& C,

                        const double                  Deflection2,
                        const double                  Udeb,
                        const gp_Pnt&                 Pdeb,
                        const gp_Vec&                 Vdeb,
                        const double                  Ufin,
                        const gp_Pnt&                 Pfin,
                        const gp_Vec&                 Vfin,
                        const int                     Nbmin,
                        const double                  Eps,
                        NCollection_Sequence<double>& Parameters,
                        NCollection_Sequence<gp_Pnt>& Points)
{
  int    Ptslength = Points.Length();
  double Udelta    = Ufin - Udeb;
  gp_Pnt Pdelta;
  gp_Vec Vdelta;
  if (Nbmin > 2)
  {
    Udelta /= (Nbmin - 1);
    C.D1(Udeb + Udelta, Pdelta, Vdelta);
  }
  else
  {
    Pdelta = Pfin;
    Vdelta = Vfin;
  }

  double Norme     = gp_Vec(Pdeb, Pdelta).SquareMagnitude();
  double theFleche = 0;
  bool   flecheok  = false;
  if (Norme > Eps)
  {
    // Evaluation of the arrow by interpolation. See IntWalk_IWalking::TestDeflection
    double N1 = Vdeb.SquareMagnitude();
    double N2 = Vdelta.SquareMagnitude();
    if (N1 > Eps && N2 > Eps)
    {
      double Normediff =

        (Vdeb.Normalized().XYZ() - Vdelta.Normalized().XYZ()).SquareModulus();
      if (Normediff > Eps)
      {

        theFleche = Normediff * Norme / 64.;
        flecheok  = true;
      }
    }
  }
  if (!flecheok)
  {
    gp_Pnt Pmid((Pdeb.XYZ() + Pdelta.XYZ()) / 2.);
    gp_Pnt Pverif(C.Value(Udeb + Udelta / 2.));
    theFleche = Pmid.SquareDistance(Pverif);
  }

  if (theFleche < Deflection2)
  {
    Parameters.Append(Udeb + Udelta);
    Points.Append(Pdelta);
  }
  else
  {
    QuasiFleche(C,
                Deflection2,
                Udeb,
                Pdeb,

                Vdeb,
                Udeb + Udelta,
                Pdelta,
                Vdelta,
                3,
                Eps,
                Parameters,
                Points);
  }

  if (Nbmin > 2)
  {
    QuasiFleche(C,
                Deflection2,
                Udeb + Udelta,
                Pdelta,

                Vdelta,
                Ufin,
                Pfin,
                Vfin,
                Nbmin - (Points.Length() - Ptslength),
                Eps,
                Parameters,
                Points);
  }
}

bool CheckSmallParamOnEdge(const TopoDS_Edge& anEdge)
{
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>& aList =
    ((occ::down_cast<BRep_TEdge>(anEdge.TShape()))->Curves());
  if (!aList.IsEmpty())
  {
    occ::handle<BRep_CurveRepresentation> CRep =
      ((occ::down_cast<BRep_TEdge>(anEdge.TShape()))->Curves()).First();
    double f = (occ::down_cast<BRep_GCurve>(CRep))->First();
    double l = (occ::down_cast<BRep_GCurve>(CRep))->Last();
    if (std::abs(l - f) < Precision::PConfusion())
      return false;
  }
  return true;
}
