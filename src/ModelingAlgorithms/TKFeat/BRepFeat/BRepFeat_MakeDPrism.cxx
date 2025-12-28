// Created on: 1996-09-03
// Created by: Olga KOULECHOVA
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

#include <Bnd_Box.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepFeat.hxx>
#include <BRepFeat_MakeDPrism.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <LocOpe.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_DPrism.hxx>
#include <LocOpe_PntFace.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <NCollection_Sequence.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

#ifdef OCCT_DEBUG
extern bool BRepFeat_GettraceFEAT();
#endif

static void MajMap(const TopoDS_Shape&,
                   const LocOpe_DPrism&,
                   NCollection_DataMap<TopoDS_Shape,
                                       NCollection_List<TopoDS_Shape>,
                                       TopTools_ShapeMapHasher>&, // myMap
                   TopoDS_Shape&,                                 // myFShape
                   TopoDS_Shape&);                                // myLShape

static double HeightMax(const TopoDS_Shape& theSbase,
                        const TopoDS_Face&  theSkface,
                        const TopoDS_Shape& theSFrom,
                        const TopoDS_Shape& theSUntil);

static int SensOfPrism(const occ::handle<Geom_Curve>& C, const TopoDS_Shape& Until);

static occ::handle<Geom_Curve> TestCurve(const TopoDS_Face&);

//=================================================================================================

void BRepFeat_MakeDPrism::Init(const TopoDS_Shape& Sbase,
                               const TopoDS_Face&  Pbase,
                               const TopoDS_Face&  Skface,
                               const double        Angle,
                               const int           Mode,
                               const bool          Modify)

{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::Init" << std::endl;
#endif
  mySkface = Skface;
  SketchFaceValid();
  mySbase = Sbase;
  BasisShapeValid();
  myPbase = Pbase;
  mySlface.Clear();
  if (Mode == 0)
  {
    myFuse     = false;
    myJustFeat = false;
  }
  else if (Mode == 1)
  {
    myFuse     = true;
    myJustFeat = false;
  }
  else if (Mode == 2)
  {
    myFuse     = true;
    myJustFeat = true;
  }
  else
  {
  }
  myModify    = Modify;
  myJustGluer = false;

  //-------------- ifv
  // mySkface.Nullify();
  //-------------- ifv

  myShape.Nullify();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  myTopEdges.Clear();
  myLatEdges.Clear();
  TopExp_Explorer exp;
  for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
  {
    NCollection_List<TopoDS_Shape> thelist;
    myMap.Bind(exp.Current(), thelist);
    myMap(exp.Current()).Append(exp.Current());
  }
  myAngle = Angle;
#ifdef OCCT_DEBUG
  if (trc)
  {
    if (myJustFeat)
      std::cout << " Just Feature" << std::endl;
    if (myFuse)
      std::cout << " Fuse" << std::endl;
    if (!myFuse)
      std::cout << " Cut" << std::endl;
    if (!myModify)
      std::cout << " Modify = 0" << std::endl;
    //    std::cout <<" Angle = " << myAngle << std::endl;
  }
#endif
}

//=======================================================================
// function : Add
// purpose  : add sliding faces and edges
//=======================================================================

void BRepFeat_MakeDPrism::Add(const TopoDS_Edge& E, const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::Add(Edge,face)" << std::endl;
#endif
  TopExp_Explorer exp;
  for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(F))
    {
      break;
    }
  }
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }

  for (exp.Init(myPbase, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(E))
    {
      break;
    }
  }
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }

  if (!mySlface.IsBound(F))
  {
    NCollection_List<TopoDS_Shape> thelist;
    mySlface.Bind(F, thelist);
  }
  NCollection_List<TopoDS_Shape>::Iterator itl(mySlface(F));
  for (; itl.More(); itl.Next())
  {
    if (itl.Value().IsSame(E))
    {
      break;
    }
  }
  if (!itl.More())
  {
    mySlface(F).Append(E);
  }
}

//=================================================================================================

void BRepFeat_MakeDPrism::Perform(const double Height)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::Perform(Height)" << std::endl;
#endif
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil.Nullify();
  ShapeUntilValid();
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();

  double theheight = Height / cos(myAngle);
  //  myPbase.Orientation(TopAbs_FORWARD);

  LocOpe_DPrism       theDPrism(myPbase, theheight, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();

  MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);

  myGShape = VraiDPrism;
  GeneratedShapeValid();
  const TopoDS_Shape& Base = theDPrism.FirstShape();
  TopExp_Explorer     exp(Base, TopAbs_FACE);
  TopoDS_Face         theBase = TopoDS::Face(exp.Current());
  exp.Next();
  if (exp.More())
  {
    NotDone();
    myStatusError = BRepFeat_InvFirstShape;
    return;
  }

  // management of gluing faces

  GluedFacesValid();

  if (!myGluedF.IsEmpty())
  { // case gluing
    myJustGluer = true;
    theDPrism.Curves(myCurves);
    myBCurve = theDPrism.BarycCurve();
    GlobalPerform();
  }

  // if there is no gluing -> call topological operations
  if (!myJustGluer)
  {
    if (myFuse == 1)
    {
      BRepAlgoAPI_Fuse f(mySbase, myGShape);
      myShape = f.Shape();
      UpdateDescendants(f, myShape, false);
      Done();
    }
    else if (myFuse == 0)
    {
      BRepAlgoAPI_Cut c(mySbase, myGShape);
      myShape = c.Shape();
      UpdateDescendants(c, myShape, false);
      Done();
    }
    else
    {
      myShape = myGShape;
      Done();
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : feature limited by the shape Until
//=======================================================================

void BRepFeat_MakeDPrism::Perform(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::Perform(Until)" << std::endl;
#endif
  if (Until.IsNull())
  {
    throw Standard_ConstructionError();
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  //  myPbase.Orientation(TopAbs_FORWARD);

  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  bool Trf = TransformShapeFU(1);
  ShapeUntilValid();
  occ::handle<Geom_Curve> C    = TestCurve(myPbase);
  int                     sens = SensOfPrism(C, mySUntil);

  BRep_Builder        bB;
  double              Height = sens * HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  LocOpe_DPrism       theDPrism(myPbase, Height, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();
  if (!Trf)
  {
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    myGShape = VraiDPrism;
    GeneratedShapeValid();
    const TopoDS_Shape& Base = theDPrism.FirstShape();
    exp.Init(Base, TopAbs_FACE);
    TopoDS_Face theBase = TopoDS::Face(exp.Current());
    exp.Next();
    if (exp.More())
    {
      NotDone();
      myStatusError = BRepFeat_InvFirstShape;
      return;
    }

    GluedFacesValid();
    theDPrism.Curves(myCurves);
    myBCurve = theDPrism.BarycCurve();
    GlobalPerform();
  }
  else
  {
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    occ::handle<Geom_Curve> C1;
    if (sens == -1)
    {
      C1 = C->Reversed();
    }
    else
    {
      C1 = C;
    }

    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C1);
    LocOpe_CSIntersector ASI(mySUntil);
    ASI.Perform(scur);
    TopAbs_Orientation Or;
    if (ASI.IsDone() && ASI.NbPoints(1) >= 1)
    {
      if (myFuse == 1)
      {
        Or = ASI.Point(1, 1).Orientation();
      }
      else
      {
        Or = ASI.Point(1, ASI.NbPoints(1)).Orientation();
      }
      //      double prm = ASI.Point(1,1).Parameter();
      //      if(prm < 0) Or = TopAbs::Reverse(Or);
      TopoDS_Face  FUntil = ASI.Point(1, 1).Face();
      TopoDS_Shape Comp;
      bB.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull())
        bB.Add(Comp, S);

      BRepAlgoAPI_Cut trP(VraiDPrism, Comp);
      UpdateDescendants(trP, trP.Shape(), false);

      TopExp_Explorer     ex(trP.Shape(), TopAbs_SOLID);
      const TopoDS_Shape& Cutsh = ex.Current();
      if (myFuse == 1)
      {
        BRepAlgoAPI_Fuse f(mySbase, Cutsh);
        myShape = f.Shape();
        UpdateDescendants(f, myShape, false);
        Done();
      }
      else if (myFuse == 0)
      {
        BRepAlgoAPI_Cut c(mySbase, Cutsh);
        myShape = c.Shape();
        UpdateDescendants(c, myShape, false);
        Done();
      }
      else
      {
        myShape = Cutsh;
        Done();
      }
    }
  }
  NCollection_List<TopoDS_Shape>::Iterator ited(myNewEdges);
  for (; ited.More(); ited.Next())
  {
    const TopoDS_Edge& ledg = TopoDS::Edge(ited.Value());
    if (!BRepAlgo::IsValid(ledg))
    {
      bB.SameRange(ledg, false);
      bB.SameParameter(ledg, false);
      BRepLib::SameParameter(ledg, BRep_Tool::Tolerance(ledg));
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : feature limited by two shapes
//=======================================================================

void BRepFeat_MakeDPrism::Perform(const TopoDS_Shape& From, const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::Perform(From,Until)" << std::endl;
#endif
  if (From.IsNull() || Until.IsNull())
  {
    throw Standard_ConstructionError();
  }

  if (!mySkface.IsNull())
  {
    if (From.IsSame(mySkface))
    {
      myJustGluer = true;
      Perform(Until);
      if (myJustGluer)
        return;
    }
    else if (Until.IsSame(mySkface))
    {
      myJustGluer = true;
      Perform(From);
      if (myJustGluer)
        return;
    }
  }
  //  myPbase.Orientation(TopAbs_FORWARD);

  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionFU;
  PerfSelectionValid();

  TopExp_Explorer exp(From, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  mySFrom   = From;
  bool Trff = TransformShapeFU(0);
  ShapeFromValid();
  mySUntil  = Until;
  bool Trfu = TransformShapeFU(1);
  ShapeUntilValid();
  if (Trfu != Trff)
  {
    NotDone();
    myStatusError = BRepFeat_IncTypes;
    return;
  }
  occ::handle<Geom_Curve> C = TestCurve(myPbase);
  int                     sens;
  if (From.IsSame(Until))
  {
    sens = 1;
  }
  else
  {
    sens = SensOfPrism(C, mySUntil);
  }

  double              Height = sens * HeightMax(mySbase, myPbase, mySFrom, mySUntil);
  LocOpe_DPrism       theDPrism(myPbase, Height, Height, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();

  if (!Trff)
  {
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);

    // Make systematically dprism
    myGShape = VraiDPrism;
    GeneratedShapeValid();

    // management of gluing faces
    //	  mySbase, myPbase, mySlface, theDPrism, myGluedF);
    GluedFacesValid();
    theDPrism.Curves(myCurves);
    myBCurve = theDPrism.BarycCurve();

    // topologic reconstruction
    GlobalPerform();
  }
  else
  {
    // management of descendants
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    occ::handle<Geom_Curve> C1;
    if (sens == -1)
    {
      C1 = C->Reversed();
    }
    else
    {
      C1 = C;
    }
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C1);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySFrom);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU, OrF;
    TopoDS_Face        FFrom, FUntil;
    // direction of dprism
    if (ASI1.IsDone() && ASI1.NbPoints(1) >= 1)
    {
      if (myFuse == 1)
      {
        OrU = ASI1.Point(1, 1).Orientation();
      }
      else
      {
        OrU = ASI1.Point(1, ASI1.NbPoints(1)).Orientation();
      }
      //      double prm = ASI1.Point(1,1).Parameter();
      //      if(prm < 0) OrU = TopAbs::Reverse(OrU);
      FUntil = ASI1.Point(1, 1).Face();
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NoIntersectU;
      return;
    }
    if (ASI2.IsDone() && ASI2.NbPoints(1) >= 1)
    {
      OrF = ASI2.Point(1, 1).Orientation();
      //      double prm = ASI2.Point(1,1).Parameter();
      OrF   = TopAbs::Reverse(OrF);
      FFrom = ASI2.Point(1, 1).Face();
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NoIntersectF;
      return;
    }
    TopoDS_Shape Comp;
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Comp));
    TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!S.IsNull())
      B.Add(Comp, S);
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolU;
      return;
    }
    TopoDS_Solid SS = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!SS.IsNull())
      B.Add(Comp, SS);
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolF;
      return;
    }

    BRepAlgoAPI_Cut trP(VraiDPrism, Comp);

    if (myFuse == 1)
    {
      BRepAlgoAPI_Fuse f(mySbase, trP.Shape());
      myShape = f.Shape();
      UpdateDescendants(f, myShape, false);
      Done();
    }
    //
    else if (myFuse == 0)
    {
      BRepAlgoAPI_Cut c(mySbase, trP.Shape());
      myShape = c.Shape();
      UpdateDescendants(c, myShape, false);
      Done();
    }
    else
    {
      myShape = trP.Shape();
      Done();
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : feature semi-infinie
//=======================================================================

void BRepFeat_MakeDPrism::PerformUntilEnd()
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::PerformUntilEnd()" << std::endl;
#endif
  myPerfSelection = BRepFeat_SelectionSh;
  PerfSelectionValid();
  myGluedF.Clear();
  mySUntil.Nullify();
  ShapeUntilValid();
  mySFrom.Nullify();
  ShapeFromValid();
  double Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  //  myPbase.Orientation(TopAbs_FORWARD);

  LocOpe_DPrism       theDPrism(myPbase, Height, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();

  MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);

  myGShape = VraiDPrism;
  GeneratedShapeValid();
  GluedFacesValid();
  theDPrism.Curves(myCurves);
  myBCurve = theDPrism.BarycCurve();
  GlobalPerform();
}

//=======================================================================
// function : PerformFromEnd
// purpose  : feature semiinfinite limited by the shape Until from the other side
//=======================================================================

void BRepFeat_MakeDPrism::PerformFromEnd(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::PerformFromEnd(From,Until)" << std::endl;
#endif
  if (Until.IsNull())
  {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull() && Until.IsSame(mySkface))
  {
    PerformUntilEnd();
    return;
  }
  TopExp_Explorer exp;
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  //  myPbase.Orientation(TopAbs_FORWARD);
  myPerfSelection = BRepFeat_SelectionShU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  bool Trf = TransformShapeFU(1);
  ShapeUntilValid();
  occ::handle<Geom_Curve> C      = TestCurve(myPbase);
  int                     sens   = SensOfPrism(C, mySUntil);
  double                  Height = sens * HeightMax(mySbase, mySkface, mySFrom, mySUntil);

  LocOpe_DPrism       theDPrism(myPbase, Height, Height, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();
  if (VraiDPrism.IsNull())
  {
    NotDone();
    myStatusError = BRepFeat_NullRealTool;
    return;
  }

  if (!Trf)
  { // case finite face
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    myGShape = VraiDPrism;
    GeneratedShapeValid();
    myGluedF.Clear();
    GluedFacesValid();
    theDPrism.Curves(myCurves);
    myBCurve = theDPrism.BarycCurve();
    GlobalPerform();
  }
  else
  { // case support
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    occ::handle<Geom_Curve> C2;
    if (sens == -1)
    {
      C2 = C->Reversed();
    }
    else
    {
      C2 = C;
    }
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C2);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySbase);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU = TopAbs_FORWARD, OrF = TopAbs_FORWARD;
    TopoDS_Face        FUntil, FFrom;
    if (ASI1.IsDone() && ASI1.NbPoints(1) >= 1)
    {
      OrU        = ASI1.Point(1, 1).Orientation();
      double prm = ASI1.Point(1, 1).Parameter();
      if (prm < 0)
        OrU = TopAbs::Reverse(OrU);
      FUntil = ASI1.Point(1, 1).Face();
    }

    if (ASI2.IsDone() && ASI2.NbPoints(1) >= 1)
    {
      int    jj  = ASI2.NbPoints(1);
      double prm = ASI2.Point(1, 1).Parameter();
      FFrom      = ASI2.Point(1, 1).Face();
      OrF        = ASI2.Point(1, 1).Orientation();
      OrF        = TopAbs::Reverse(OrF);
      for (int iii = 1; iii <= jj; iii++)
      {
        if (ASI2.Point(1, iii).Parameter() < prm)
        {
          prm   = ASI2.Point(1, iii).Parameter();
          FFrom = ASI2.Point(1, iii).Face();
          OrF   = ASI2.Point(1, iii).Orientation();
          OrF   = TopAbs::Reverse(OrF);
        }
      }
      occ::handle<Geom_Surface> S = BRep_Tool::Surface(FFrom);
      if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
      }
      BRepLib_MakeFace fac(S, Precision::Confusion());
      mySFrom = fac.Face();
      Trf     = TransformShapeFU(0);
      //      FFrom = TopoDS::Face(mySFrom);
    }

    TopoDS_Shape Comp;
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Comp));
    TopoDS_Solid Sol = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!Sol.IsNull())
      B.Add(Comp, Sol);
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolU;
      return;
    }

    TopoDS_Solid Sol1 = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!Sol1.IsNull())
      B.Add(Comp, Sol1);
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolF;
      return;
    }

    BRepAlgoAPI_Cut trP(VraiDPrism, Comp);
    if (myFuse == 1)
    {
      BRepAlgoAPI_Fuse f(mySbase, trP.Shape());
      myShape = f.Shape();
      UpdateDescendants(f, myShape, false);
      Done();
    }
    else if (myFuse == 0)
    {
      BRepAlgoAPI_Cut c(mySbase, trP.Shape());
      myShape = c.Shape();
      UpdateDescendants(c, myShape, false);
      Done();
    }
    else
    {
      myShape = trP.Shape();
      Done();
    }
  }
}

//=======================================================================
// function : PerformThruAll
// purpose  : feature throughout the entire initial shape
//=======================================================================

void BRepFeat_MakeDPrism::PerformThruAll()
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::PerformThruAll()" << std::endl;
#endif
  mySUntil.Nullify();
  ShapeUntilValid();
  mySFrom.Nullify();
  ShapeFromValid();
  if (myFuse == 0)
  {
    myPerfSelection = BRepFeat_NoSelection;
  }
  else
  {
    myPerfSelection = BRepFeat_SelectionSh;
  }

  PerfSelectionValid();
  //  myPbase.Orientation(TopAbs_FORWARD);
  myGluedF.Clear();
  GluedFacesValid();

  double              Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  LocOpe_DPrism       theDPrism(myPbase, Height, Height, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();
  MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);

  myGShape = VraiDPrism;
  GeneratedShapeValid();

  if (myFuse == 0)
  {
    BRepAlgoAPI_Cut c(mySbase, myGShape);
    if (c.IsDone())
    {
      myShape = c.Shape();
      UpdateDescendants(c, myShape, false);
      Done();
    }
  }
  else
  {
    theDPrism.Curves(myCurves);
    myBCurve = theDPrism.BarycCurve();
    GlobalPerform();
  }
}

//=======================================================================
// function : PerformUntilHeight
// purpose  : feature until the shape is of the given height
//=======================================================================

void BRepFeat_MakeDPrism::PerformUntilHeight(const TopoDS_Shape& Until, const double Height)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::PerformUntilHeight(Until,Height)" << std::endl;
#endif
  if (Until.IsNull())
  {
    Perform(Height);
  }
  if (Height == 0)
  {
    Perform(Until);
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  //  myPbase.Orientation(TopAbs_FORWARD);
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  bool Trf = TransformShapeFU(1);
  ShapeUntilValid();
  occ::handle<Geom_Curve> C    = TestCurve(myPbase);
  int                     sens = SensOfPrism(C, mySUntil);

  LocOpe_DPrism       theDPrism(myPbase, sens * Height, myAngle);
  const TopoDS_Shape& VraiDPrism = theDPrism.Shape();

  if (!Trf)
  { // case face finished
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    myGShape = VraiDPrism;
    GeneratedShapeValid();
    const TopoDS_Shape& Base = theDPrism.FirstShape();
    exp.Init(Base, TopAbs_FACE);
    TopoDS_Face theBase = TopoDS::Face(exp.Current());
    exp.Next();
    if (exp.More())
    {
      NotDone();
      myStatusError = BRepFeat_InvFirstShape;
      return;
    }

    GluedFacesValid();
    theDPrism.Curves(myCurves);
    myBCurve = theDPrism.BarycCurve();
    GlobalPerform();
  }
  else
  { // case support
    MajMap(myPbase, theDPrism, myMap, myFShape, myLShape);
    occ::handle<Geom_Curve> C1;
    if (sens == -1)
    {
      C1 = C->Reversed();
    }
    else
    {
      C1 = C;
    }
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C1);
    LocOpe_CSIntersector ASI(mySUntil);
    ASI.Perform(scur);
    TopAbs_Orientation Or;
    if (ASI.IsDone() && ASI.NbPoints(1) >= 1)
    {
      if (myFuse == 1)
      {
        Or = ASI.Point(1, 1).Orientation();
      }
      else
      {
        Or = ASI.Point(1, ASI.NbPoints(1)).Orientation();
      }
      //      double prm = ASI.Point(1,1).Parameter();
      //      if(prm < 0) Or = TopAbs::Reverse(Or);
      TopoDS_Face  FUntil = ASI.Point(1, 1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull())
        B.Add(Comp, S);
      BRepAlgoAPI_Cut trP(VraiDPrism, Comp);
      if (myFuse == 1)
      {
        BRepAlgoAPI_Fuse f(mySbase, trP.Shape());
        myShape = f.Shape();
        UpdateDescendants(f, myShape, false);
        Done();
      }
      else if (myFuse == 0)
      {
        BRepAlgoAPI_Cut c(mySbase, trP.Shape());
        myShape = c.Shape();
        UpdateDescendants(c, myShape, false);
        Done();
      }
      else
      {
        myShape = trP.Shape();
        Done();
      }
    }
  }
}

//=======================================================================
// function : Curves
// purpose  : curves parallel to the axis of the prism
//=======================================================================

void BRepFeat_MakeDPrism::Curves(NCollection_Sequence<occ::handle<Geom_Curve>>& scur)
{
  scur = myCurves;
}

//============================================================================
// function : BRepFeat_BossEgdes
// purpose: Determination of TopEdges and LatEdges.
//          sig = 1 -> TopEdges = FirstShape of the DPrism
//          sig = 2 -> TOpEdges = LastShape of the DPrism
//============================================================================
void BRepFeat_MakeDPrism::BossEdges(const int signature)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeDPrism::BossEdges (integer)" << std::endl;
#endif
  NCollection_List<TopoDS_Shape> theLastShape;
  theLastShape.Clear();
  if (signature == 1 || signature == -1)
  {
    theLastShape = FirstShape();
  }
  else if (signature == 2 || signature == -2)
  {
    theLastShape = LastShape();
  }
  else
  {
    return;
  }

  // Edges Top
  NCollection_List<TopoDS_Shape>::Iterator itLS;
  TopExp_Explorer                          ExpE;
  for (itLS.Initialize(theLastShape); itLS.More(); itLS.Next())
  {
    const TopoDS_Face& FF = TopoDS::Face(itLS.Value());
    for (ExpE.Init(FF, TopAbs_EDGE); ExpE.More(); ExpE.Next())
    {
      const TopoDS_Edge& EE = TopoDS::Edge(ExpE.Current());
      myTopEdges.Append(EE);
    }
  }

  // Edges Bottom
  if (signature < 0)
  {
    // Attention check if TgtEdges is important
    myLatEdges = NewEdges();
  }
  else if (signature > 0)
  {
    if (!myShape.IsNull())
    {
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapE;
      bool                                                   Found;

      TopExp_Explorer ExpF;
      for (ExpF.Init(myShape, TopAbs_FACE); ExpF.More(); ExpF.Next())
      {
        Found                 = false;
        const TopoDS_Face& FF = TopoDS::Face(ExpF.Current());
        for (itLS.Initialize(theLastShape); itLS.More(); itLS.Next())
        {
          const TopoDS_Face& TopFace = TopoDS::Face(itLS.Value());
          if (!FF.IsSame(TopFace))
          {
            for (ExpE.Init(FF, TopAbs_EDGE); ExpE.More() && !Found; ExpE.Next())
            {
              const TopoDS_Edge& E1 = TopoDS::Edge(ExpE.Current());
              TopoDS_Vertex      V1, V2;
              TopExp::Vertices(E1, V1, V2);
              NCollection_List<TopoDS_Shape>::Iterator it(myTopEdges);
              for (; it.More() && !Found; it.Next())
              {
                TopoDS_Edge   E2 = TopoDS::Edge(it.Value());
                TopoDS_Vertex VT1, VT2;
                TopExp::Vertices(E2, VT1, VT2);

                if (V1.IsSame(VT1) || V1.IsSame(VT2) || V2.IsSame(VT1) || V2.IsSame(VT2))
                {
                  Found = true;
                  TopExp_Explorer ExpE2;
                  for (ExpE2.Init(FF, TopAbs_EDGE); ExpE2.More(); ExpE2.Next())
                  {
                    const TopoDS_Edge& E3 = TopoDS::Edge(ExpE2.Current());
                    if (MapE.Contains(E3))
                    {
                      MapE.Remove(E3);
                    }
                    else
                    {
                      MapE.Add(E3);
                    }
                  }
                }
              }
            }
          }
        }
      }

      NCollection_List<TopoDS_Shape>::Iterator it(myTopEdges);
      for (; it.More(); it.Next())
      {
        if (MapE.Contains(it.Value()))
        {
          MapE.Remove(it.Value());
        }
      }

      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itMap;
      for (itMap.Initialize(MapE); itMap.More(); itMap.Next())
      {
        if (!BRep_Tool::Degenerated(TopoDS::Edge(itMap.Key())))
          myLatEdges.Append(itMap.Key());
      }
    }
  }
}

//============================================================================
// function : BRepFeat_TopEgdes
// Purpose: Returns the list of TopoDS Edges of the top of the boss
//============================================================================
const NCollection_List<TopoDS_Shape>& BRepFeat_MakeDPrism::TopEdges()
{
  return myTopEdges;
}

//============================================================================
// function : BRepFeat_LatEgdes
// Purpose: Returns the list of TopoDS Edges of the top of the boss
//============================================================================
const NCollection_List<TopoDS_Shape>& BRepFeat_MakeDPrism::LatEdges()
{
  return myLatEdges;
}

//=======================================================================
// function : BarycCurve
// purpose  : passe par le centre de masses de la primitive
//=======================================================================

occ::handle<Geom_Curve> BRepFeat_MakeDPrism::BarycCurve()
{
  return myBCurve;
}

//=======================================================================
// function : HeightMax
// purpose  : Calculate the height of the prism following the parameters of the bounding box
//=======================================================================

static double HeightMax(const TopoDS_Shape& theSbase,  // shape initial
                        const TopoDS_Face&  theSkface, // face de sketch
                        const TopoDS_Shape& theSFrom,  // shape from
                        const TopoDS_Shape& theSUntil) // shape until
{
  Bnd_Box Box;
  BRepBndLib::Add(theSbase, Box);
  BRepBndLib::Add(theSkface, Box);
  if (!theSFrom.IsNull())
  {
    BRepBndLib::Add(theSFrom, Box);
  }
  if (!theSUntil.IsNull())
  {
    BRepBndLib::Add(theSUntil, Box);
  }
  double c[6];

  Box.Get(c[0], c[2], c[4], c[1], c[3], c[5]);
  //  double parmin=c[0], parmax = c[0];
  //  for(int i = 0 ; i < 6; i++) {
  //    if(c[i] > parmax) parmax = c[i];
  //    if(c[i] < parmin ) parmin = c[i];
  //  }
  //  double Height = abs(2.*(parmax - parmin));
  //  return(2.*Height);
  // #ifndef OCCT_DEBUG
  double par = std::max(std::max(fabs(c[1] - c[0]), fabs(c[3] - c[2])), fabs(c[5] - c[4]));
  // #else
  //   double par = std::max(  std::max( abs(c[1] - c[0]), abs(c[3] - c[2]) ), abs(c[5] -
  //   c[4]) );
  // #endif
#ifdef OCCT_DEBUG
  std::cout << "Height = > " << par << std::endl;
#endif
  return par;
}

//=======================================================================
// function : SensOfPrism
// purpose  : determine the direction of prism generation
//=======================================================================
int SensOfPrism(const occ::handle<Geom_Curve>& C, const TopoDS_Shape& Until)
{
  LocOpe_CSIntersector                          ASI1(Until);
  NCollection_Sequence<occ::handle<Geom_Curve>> scur;
  scur.Append(C);
  ASI1.Perform(scur);
  int sens = 1;
  if (ASI1.IsDone() && ASI1.NbPoints(1) >= 1)
  {
    int    nb   = ASI1.NbPoints(1);
    double prm1 = ASI1.Point(1, 1).Parameter();
    double prm2 = ASI1.Point(1, nb).Parameter();
    if (prm1 < 0. && prm2 < 0.)
    {
      sens = -1;
    }
  }
  else if (BRepFeat::ParametricBarycenter(Until, C) < 0)
  {
    sens = -1;
  }
  else
  {
  }
  return sens;
}

//=================================================================================================

static void MajMap(
  const TopoDS_Shape&  theB,
  const LocOpe_DPrism& theP,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                theMap,    // myMap
  TopoDS_Shape& theFShape, // myFShape
  TopoDS_Shape& theLShape) // myLShape
{
  TopExp_Explorer exp;
  if (!theP.FirstShape().IsNull())
  {
    exp.Init(theP.FirstShape(), TopAbs_WIRE);
    if (exp.More())
    {
      theFShape = exp.Current();
      NCollection_List<TopoDS_Shape> thelist;
      theMap.Bind(theFShape, thelist);
      for (exp.Init(theP.FirstShape(), TopAbs_FACE); exp.More(); exp.Next())
      {
        theMap(theFShape).Append(exp.Current());
      }
    }
  }

  if (!theP.LastShape().IsNull())
  {
    exp.Init(theP.LastShape(), TopAbs_WIRE);
    if (exp.More())
    {
      theLShape = exp.Current();
      NCollection_List<TopoDS_Shape> thelist1;
      theMap.Bind(theLShape, thelist1);
      for (exp.Init(theP.LastShape(), TopAbs_FACE); exp.More(); exp.Next())
      {
        theMap(theLShape).Append(exp.Current());
      }
    }
  }

  for (exp.Init(theB, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (!theMap.IsBound(exp.Current()))
    {
      const TopoDS_Edge&             edg = TopoDS::Edge(exp.Current());
      NCollection_List<TopoDS_Shape> thelist2;
      theMap.Bind(edg, thelist2);
      theMap(edg) = theP.Shapes(edg);
    }
  }
}

//=================================================================================================

static occ::handle<Geom_Curve> TestCurve(const TopoDS_Face& Base)
{
  gp_Pnt                       bar(0., 0., 0.);
  NCollection_Sequence<gp_Pnt> spt;
  LocOpe::SampleEdges(Base, spt);
  for (int jj = 1; jj <= spt.Length(); jj++)
  {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  occ::handle<Geom_Surface> s = BRep_Tool::Surface(Base);
  if (s->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    s = occ::down_cast<Geom_RectangularTrimmedSurface>(s)->BasisSurface();
  }
  occ::handle<Geom_Plane> P = occ::down_cast<Geom_Plane>(s);
  if (P.IsNull())
  {
    occ::handle<Geom_Curve> toto;
    return toto;
  }
  gp_Pln                 pp = P->Pln();
  gp_Dir                 Normale(pp.Position().XDirection() ^ pp.Position().YDirection());
  gp_Ax1                 theAx(bar, Normale);
  occ::handle<Geom_Line> theLin = new Geom_Line(theAx);
  return theLin;
}
