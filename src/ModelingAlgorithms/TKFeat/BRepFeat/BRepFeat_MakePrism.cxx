// Created on: 1996-02-13
// Created by: Jacques GOUSSARD
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
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepFeat.hxx>
#include <BRepFeat_MakePrism.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_PntFace.hxx>
#include <LocOpe_Prism.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#ifdef OCCT_DEBUG
extern bool BRepFeat_GettraceFEAT();
#endif

static void MajMap(const TopoDS_Shape&, // base
                   const LocOpe_Prism&,
                   NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&, // myMap
                   TopoDS_Shape&,                       // myFShape
                   TopoDS_Shape&);                      // myLShape

static bool ToFuse(const TopoDS_Face&, const TopoDS_Face&);

static double HeightMax(const TopoDS_Shape& theSbase,
                               const TopoDS_Face&  theSkface,
                               const TopoDS_Shape& theSFrom,
                               const TopoDS_Shape& theSUntil);

static int SensOfPrism(const occ::handle<Geom_Curve>& C, const TopoDS_Shape& Until);

static occ::handle<Geom_Curve> TestCurve(const TopoDS_Shape&, const gp_Vec&);

//=================================================================================================

void BRepFeat_MakePrism::Init(const TopoDS_Shape&    Sbase,
                              const TopoDS_Shape&    Pbase,
                              const TopoDS_Face&     Skface,
                              const gp_Dir&          Direc,
                              const int Mode,
                              const bool Modify)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::Init" << std::endl;
#endif
  mySkface = Skface;
  SketchFaceValid();
  mySbase = Sbase;
  BasisShapeValid();
  myPbase = Pbase;
  mySlface.Clear();
  myDir = Direc;
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
  myNewEdges.Clear();
  myTgtEdges.Clear();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  TopExp_Explorer exp;
  for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
  {
    NCollection_List<TopoDS_Shape> thelist;
    myMap.Bind(exp.Current(), thelist);
    myMap(exp.Current()).Append(exp.Current());
  }
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
  }
#endif
}

//=======================================================================
// function : Add
// purpose  : add elements of sliding (edge on face)
//=======================================================================

void BRepFeat_MakePrism::Add(const TopoDS_Edge& E, const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::Add(Edge,face)" << std::endl;
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
    NCollection_List<TopoDS_Shape> thelist1;
    mySlface.Bind(F, thelist1);
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

//=======================================================================
// function : Perform
// purpose  : construction of prism of length Length and
//           call of reconstruction topo
//=======================================================================

void BRepFeat_MakePrism::Perform(const double Length)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::Perform(Length)" << std::endl;
#endif
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil.Nullify();
  ShapeUntilValid();
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  gp_Vec V(Length * myDir);

  // construction of prism of height Length

  LocOpe_Prism        thePrism(myPbase, V);
  const TopoDS_Shape& VraiPrism = thePrism.Shape();

  // management of descendants
  MajMap(myPbase, thePrism, myMap, myFShape, myLShape);

  myGShape = VraiPrism; // the primitive
  GeneratedShapeValid();

  TopoDS_Face FFace;

  bool found = false;

  // try to detect the faces of gluing
  // in case if the top of the prism is tangent to the initial shape

  if (!mySkface.IsNull() || !mySlface.IsEmpty())
  {
    if (myLShape.ShapeType() == TopAbs_WIRE)
    {
      TopExp_Explorer ex1(VraiPrism, TopAbs_FACE);
      for (; ex1.More(); ex1.Next())
      {
        TopExp_Explorer ex2(ex1.Current(), TopAbs_WIRE);
        for (; ex2.More(); ex2.Next())
        {
          if (ex2.Current().IsSame(myLShape))
          {
            FFace = TopoDS::Face(ex1.Current());
            found = true;
            break;
          }
        }
        if (found)
          break;
      }
    }

    TopExp_Explorer exp(mySbase, TopAbs_FACE);
    for (; exp.More(); exp.Next())
    {
      const TopoDS_Face& ff = TopoDS::Face(exp.Current());
      if (ToFuse(ff, FFace))
      {
        NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> sl;
        if (!FFace.IsSame(myPbase) && BRepFeat::IsInside(ff, FFace))
          break;
      }
    }
  }

  // management of faces of gluing given by the user

  GluedFacesValid();

  if (!myGluedF.IsEmpty())
  { // case gluing
    myJustGluer = true;
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform(); // topological reconstruction
  }

  // if there is no gluing  -> call of ope topo
  if (!myJustGluer)
  {
    if (myFuse == 1 && !myJustFeat)
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
// purpose  : construction of prism oriented at the face Until, sufficiently
//           long; call of topological reconstruction
//=======================================================================

void BRepFeat_MakePrism::Perform(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::Perform(Until)" << std::endl;
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
  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil             = Until;
  bool Trf = TransformShapeFU(1);
  ShapeUntilValid();
  occ::handle<Geom_Curve> C      = TestCurve(myPbase, myDir);
  int   sens   = SensOfPrism(C, mySUntil);
  double      Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec             V(2 * sens * Height * myDir);

  // construction of long prism
  LocOpe_Prism        thePrism(myPbase, V);
  const TopoDS_Shape& VraiPrism = thePrism.Shape();

  // in case of support of face Until
  if (!Trf)
  {
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);
    myGShape = VraiPrism;
    GeneratedShapeValid();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else
  { // until support -> passage to topological operations
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C);

    // direction of the prism depending on Until

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
      if (sens == -1)
        Or = TopAbs::Reverse(Or);
      TopoDS_Face  FUntil = ASI.Point(1, 1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull())
        B.Add(Comp, S);
      BRepAlgoAPI_Cut trP(VraiPrism, Comp);
      UpdateDescendants(trP, trP.Shape(), false);
      //
      TopExp_Explorer     ex(trP.Shape(), TopAbs_SOLID);
      const TopoDS_Shape& Cutsh = ex.Current();
      if (myFuse == 1 && !myJustFeat)
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
}

//=======================================================================
// function : Perform
// purpose  : construction of a sufficiently long and properly oriented prism
//           call of topological reconstruction
//=======================================================================

void BRepFeat_MakePrism::Perform(const TopoDS_Shape& From, const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::Perform(From,Until)" << std::endl;
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
  mySFrom               = From;
  bool Trff = TransformShapeFU(0);
  ShapeFromValid();
  mySUntil              = Until;
  bool Trfu = TransformShapeFU(1);
  ShapeUntilValid();
  if (Trfu != Trff)
  {
    NotDone();
    myStatusError = BRepFeat_IncTypes;
    return;
  }

  // length depending on bounding boxes

  double      Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  occ::handle<Geom_Curve> C      = TestCurve(myPbase, myDir);
  int   sens; // direction of prism
  int   tran; // transfer of prism
  if (From.IsSame(Until))
  {
    sens = 1;
    tran = -1;
  }
  else
  {
    sens = SensOfPrism(C, mySUntil);
    tran = sens * SensOfPrism(C, mySFrom);
  }
  LocOpe_Prism thePrism;
  if (tran < 0)
  {
    gp_Vec Vtra(-3 * Height * sens / 2. * myDir);
    thePrism.Perform(myPbase, 3 * sens * Height * myDir, Vtra);
  }
  else
  {
    thePrism.Perform(myPbase, 2 * sens * Height * myDir);
  }
  TopoDS_Shape VraiPrism = thePrism.Shape();

  if (!Trff)
  {
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);

    myGShape = VraiPrism;
    GeneratedShapeValid();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else
  { // case until support -> topological operation
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySFrom);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU, OrF;
    TopoDS_Face        FFrom, FUntil;
    double      ParF, ParU;
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
      if (sens == -1)
        OrU = TopAbs::Reverse(OrU);
      FUntil = ASI1.Point(1, 1).Face();
      ParU   = ASI1.Point(1, 1).Parameter();
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
      if (sens == 1)
        OrF = TopAbs::Reverse(OrF);
      FFrom = ASI2.Point(1, 1).Face();
      ParF  = ASI2.Point(1, 1).Parameter();
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NoIntersectF;
      return;
    }
    if (tran > 0 && (std::abs(ParU) < std::abs(ParF)))
    {
      TopAbs_Orientation Or;
      Or  = OrU;
      OrU = OrF;
      OrF = Or;
    }
    //
    NCollection_List<TopoDS_Shape> aLTools;
    TopoDS_Solid         S = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!S.IsNull())
    {
      aLTools.Append(S);
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolU;
      return;
    }
    TopoDS_Solid SS = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!SS.IsNull())
    {
      aLTools.Append(SS);
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolF;
      return;
    }
    //
    NCollection_List<TopoDS_Shape> aLObj;
    aLObj.Append(VraiPrism);
    //
    BRepAlgoAPI_Cut trP;
    trP.SetArguments(aLObj);
    trP.SetTools(aLTools);
    trP.Build();
    UpdateDescendants(trP, trP.Shape(), false);
    if (myFuse == 1 && !myJustFeat)
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
// function : PerformUntilEnd
// purpose  : construction of a prism and reconstruction
//=======================================================================

void BRepFeat_MakePrism::PerformUntilEnd()
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::PerformUntilEnd()" << std::endl;
#endif
  myPerfSelection = BRepFeat_SelectionSh;
  PerfSelectionValid();
  myGluedF.Clear();
  mySUntil.Nullify();
  ShapeUntilValid();
  mySFrom.Nullify();
  ShapeFromValid();
  double Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec        V(2 * Height * myDir);

  LocOpe_Prism        thePrism(myPbase, V);
  const TopoDS_Shape& VraiPrism = thePrism.Shape();

  MajMap(myPbase, thePrism, myMap, myFShape, myLShape);

  myGShape = VraiPrism;
  GeneratedShapeValid();
  GluedFacesValid();

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
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
}

//=================================================================================================

void BRepFeat_MakePrism::PerformFromEnd(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::PerformFromEnd(From,Until)" << std::endl;
#endif
  if (Until.IsNull())
  {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull() && Until.IsSame(mySkface))
  {
    myDir.Reverse();
    PerformUntilEnd();
    return;
  }
  TopExp_Explorer exp;
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  myPerfSelection = BRepFeat_SelectionShU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil             = Until;
  bool Trf = TransformShapeFU(1);
  ShapeUntilValid();
  occ::handle<Geom_Curve>  C      = TestCurve(myPbase, myDir);
  int    sens   = SensOfPrism(C, mySUntil);
  double       Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec              Vtra(-3 * Height * sens / 2. * myDir);
  gp_Vec              Vect(3 * sens * Height * myDir);
  LocOpe_Prism        thePrism(myPbase, Vect, Vtra);
  const TopoDS_Shape& VraiPrism = thePrism.Shape();

  if (!Trf)
  { // case face until
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);
    myGShape = VraiPrism;
    GeneratedShapeValid();
    myGluedF.Clear();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else
  { // case support
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySbase);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU = TopAbs_FORWARD, OrF = TopAbs_FORWARD;
    TopoDS_Face        FUntil, FFrom;
    if (ASI1.IsDone() && ASI1.NbPoints(1) >= 1)
    {
      OrU = ASI1.Point(1, 1).Orientation();
      if (sens == -1)
      {
        OrU = TopAbs::Reverse(OrU);
      }
      FUntil = ASI1.Point(1, 1).Face();
    }
    if (ASI2.IsDone() && ASI2.NbPoints(1) >= 1)
    {
      OrF = ASI2.Point(1, 1).Orientation();
      //      if(sens==1) OrF = TopAbs::Reverse(OrF);
      FFrom                  = ASI2.Point(1, 1).Face();
      occ::handle<Geom_Surface> S = BRep_Tool::Surface(FFrom);
      if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
      }
      BRepLib_MakeFace fac(S, Precision::Confusion());
      mySFrom = fac.Face();
      Trf     = TransformShapeFU(0);
      FFrom   = TopoDS::Face(mySFrom);
    }

    NCollection_List<TopoDS_Shape> aLTools;
    TopoDS_Solid         Sol = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!Sol.IsNull())
    {
      aLTools.Append(Sol);
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolU;
      return;
    }

    TopoDS_Solid Sol1 = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!Sol1.IsNull())
    {
      aLTools.Append(Sol1);
    }
    else
    {
      NotDone();
      myStatusError = BRepFeat_NullToolF;
      return;
    }
    //
    NCollection_List<TopoDS_Shape> aLObj;
    aLObj.Append(VraiPrism);
    //
    BRepAlgoAPI_Cut trP;
    trP.SetArguments(aLObj);
    trP.SetTools(aLTools);
    trP.Build();
    //
    UpdateDescendants(trP, trP.Shape(), false);
    if (myFuse == 1 && !myJustFeat)
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

//=================================================================================================

void BRepFeat_MakePrism::PerformThruAll()
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::PerformThruAll()" << std::endl;
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
  myGluedF.Clear();
  GluedFacesValid();

  double       Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec              V(3 * Height * myDir);
  gp_Vec              Vtra(-3 * Height / 2. * myDir);
  LocOpe_Prism        thePrism(myPbase, V, Vtra);
  const TopoDS_Shape& VraiPrism = thePrism.Shape();
  MajMap(myPbase, thePrism, myMap, myFShape, myLShape);

  myGShape = VraiPrism;
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
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
}

//=================================================================================================

void BRepFeat_MakePrism::PerformUntilHeight(const TopoDS_Shape& Until, const double Length)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakePrism::PerformUntilHeight(Until,Length)" << std::endl;
#endif
  if (Until.IsNull())
  {
    Perform(Length);
  }
  if (Length == 0)
  {
    Perform(Until);
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More())
  {
    throw Standard_ConstructionError();
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil             = Until;
  bool Trf = TransformShapeFU(1);
  ShapeUntilValid();
  occ::handle<Geom_Curve>  C    = TestCurve(myPbase, myDir);
  int    sens = SensOfPrism(C, mySUntil);
  gp_Vec              V(sens * Length * myDir);
  LocOpe_Prism        thePrism(myPbase, V);
  const TopoDS_Shape& VraiPrism = thePrism.Shape();

  if (!Trf)
  {
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);

    myGShape = VraiPrism;
    GeneratedShapeValid();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else
  {
    MajMap(myPbase, thePrism, myMap, myFShape, myLShape);
    NCollection_Sequence<occ::handle<Geom_Curve>> scur;
    scur.Clear();
    scur.Append(C);
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
      if (sens == -1)
        Or = TopAbs::Reverse(Or);
      TopoDS_Face  FUntil = ASI.Point(1, 1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull())
        B.Add(Comp, S);

      BRepAlgoAPI_Cut trP(VraiPrism, Comp);
      UpdateDescendants(trP, trP.Shape(), false);
      if (myFuse == 1 && !myJustFeat)
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
// purpose  : sequence of curves parallel to the axis of prism
//=======================================================================

void BRepFeat_MakePrism::Curves(NCollection_Sequence<occ::handle<Geom_Curve>>& scur)
{
  scur = myCurves;
}

//=======================================================================
// function : BarycCurve
// purpose  : curve parallel to the axis of the prism passing through the center
//           of masses
//=======================================================================

occ::handle<Geom_Curve> BRepFeat_MakePrism::BarycCurve()
{
  return myBCurve;
}

//=======================================================================
// function : HeightMax
// purpose  : Calculate the height of the prism following the parameters of
//           bounding box
//=======================================================================

static double HeightMax(const TopoDS_Shape& theSbase,
                               const TopoDS_Face&  theSkface,
                               const TopoDS_Shape& theSFrom,
                               const TopoDS_Shape& theSUntil)
{
  Bnd_Box Box;
  BRepBndLib::Add(theSbase, Box);
  BRepBndLib::Add(theSkface, Box);
  if (!theSFrom.IsNull())
  {
    bool FacRevolInfini = false;
    TopExp_Explorer  exp;
    exp.Init(theSFrom, TopAbs_EDGE);
    for (; exp.More(); exp.Next())
    {
      TopExp_Explorer exp1;
      exp1.Init(exp.Current(), TopAbs_VERTEX);
      if (!exp1.More())
      {
        FacRevolInfini = true;
        break;
      }
    }
    if (!FacRevolInfini)
      BRepBndLib::Add(theSFrom, Box);
  }
  if (!theSUntil.IsNull())
  {
    bool FacRevolInfini = false;
    TopExp_Explorer  exp;
    exp.Init(theSUntil, TopAbs_EDGE);
    for (; exp.More(); exp.Next())
    {
      TopExp_Explorer exp1;
      exp1.Init(exp.Current(), TopAbs_VERTEX);
      if (!exp1.More())
      {
        FacRevolInfini = true;
        break;
      }
    }
    if (!FacRevolInfini)
      BRepBndLib::Add(theSUntil, Box);
  }

  double c[6];

  Box.Get(c[0], c[2], c[4], c[1], c[3], c[5]);
  double parmin = c[0], parmax = c[0];
  for (int i = 0; i < 6; i++)
  {
    if (c[i] > parmax)
      parmax = c[i];
    if (c[i] < parmin)
      parmin = c[i];
  }
  // #ifndef OCCT_DEBUG
  double Height = fabs(2. * (parmax - parmin));
  // #else
  //   double Height = abs(2.*(parmax - parmin));
  // #endif
  return (Height);
}

//=======================================================================
// function : SensOfPrism
// purpose  : Direction of the prism depending on the shape Until
//=======================================================================
int SensOfPrism(const occ::handle<Geom_Curve>& C, const TopoDS_Shape& Until)
{
  LocOpe_CSIntersector     ASI1(Until);
  NCollection_Sequence<occ::handle<Geom_Curve>> scur;
  scur.Append(C);
  ASI1.Perform(scur);
  int sens = 1;
  if (ASI1.IsDone() && ASI1.NbPoints(1) >= 1)
  {
    if (ASI1.Point(1, 1).Parameter() + Precision::Confusion() < 0.
        && ASI1.Point(1, ASI1.NbPoints(1)).Parameter() + Precision::Confusion() < 0.)
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

//=======================================================================
// function : MajMap
// purpose  : management of descendants
//=======================================================================

static void MajMap(const TopoDS_Shape&                 theB,
                   const LocOpe_Prism&                 theP,
                   NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMap,    // myMap
                   TopoDS_Shape&                       theFShape, // myFShape
                   TopoDS_Shape&                       theLShape)                       // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(), TopAbs_WIRE);
  if (exp.More())
  {
    theFShape = exp.Current();
    NCollection_List<TopoDS_Shape> thelist2;
    theMap.Bind(theFShape, thelist2);
    for (exp.Init(theP.FirstShape(), TopAbs_FACE); exp.More(); exp.Next())
    {
      theMap(theFShape).Append(exp.Current());
    }
  }

  exp.Init(theP.LastShape(), TopAbs_WIRE);
  if (exp.More())
  {
    theLShape = exp.Current();
    NCollection_List<TopoDS_Shape> thelist3;
    theMap.Bind(theLShape, thelist3);
    for (exp.Init(theP.LastShape(), TopAbs_FACE); exp.More(); exp.Next())
    {
      theMap(theLShape).Append(exp.Current());
    }
  }

  for (exp.Init(theB, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (!theMap.IsBound(exp.Current()))
    {
      NCollection_List<TopoDS_Shape> thelist4;
      theMap.Bind(exp.Current(), thelist4);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}

//=======================================================================
// function : MajMap
// purpose  : management of descendants
//=======================================================================

static occ::handle<Geom_Curve> TestCurve(const TopoDS_Shape& Base, const gp_Vec& V)
{
  gp_Pnt               bar(0., 0., 0.);
  NCollection_Sequence<gp_Pnt> spt;
  LocOpe::SampleEdges(Base, spt);
  for (int jj = 1; jj <= spt.Length(); jj++)
  {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  gp_Ax1            newAx(bar, V);
  occ::handle<Geom_Line> theLin = new Geom_Line(newAx);
  return theLin;
}

//=======================================================================
// function : ToFuse
// purpose  : face SameDomaine or not
//=======================================================================

static bool ToFuse(const TopoDS_Face& F1, const TopoDS_Face& F2)
{
  if (F1.IsNull() || F2.IsNull())
  {
    return false;
  }

  occ::handle<Geom_Surface>    S1, S2;
  TopLoc_Location         loc1, loc2;
  occ::handle<Standard_Type>   typS1, typS2;
  constexpr double tollin = Precision::Confusion();
  constexpr double tolang = Precision::Angular();

  S1 = BRep_Tool::Surface(F1, loc1);
  S2 = BRep_Tool::Surface(F2, loc2);

  typS1 = S1->DynamicType();
  typS2 = S2->DynamicType();

  if (typS1 == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S1    = occ::down_cast<Geom_RectangularTrimmedSurface>(S1)->BasisSurface();
    typS1 = S1->DynamicType();
  }

  if (typS2 == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S2    = occ::down_cast<Geom_RectangularTrimmedSurface>(S2)->BasisSurface();
    typS2 = S2->DynamicType();
  }

  if (typS1 != typS2)
  {
    return false;
  }

  bool ValRet = false;
  if (typS1 == STANDARD_TYPE(Geom_Plane))
  {
    gp_Pln pl1(occ::down_cast<Geom_Plane>(S1)->Pln());
    gp_Pln pl2(occ::down_cast<Geom_Plane>(S2)->Pln());

    // apply locations
    if (!loc1.IsIdentity())
      pl1.Transform(loc1.Transformation());
    if (!loc2.IsIdentity())
      pl2.Transform(loc2.Transformation());

    if (pl1.Position().IsCoplanar(pl2.Position(), tollin, tolang))
    {
      ValRet = true;
    }
  }

  return ValRet;
}
