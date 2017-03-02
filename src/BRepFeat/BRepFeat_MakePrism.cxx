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
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI.hxx>
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
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_PntFace.hxx>
#include <LocOpe_Prism.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean BRepFeat_GettraceFEAT();
#endif

static void MajMap(const TopoDS_Shape&, // base
		   const LocOpe_Prism&,
		   TopTools_DataMapOfShapeListOfShape&, // myMap
		   TopoDS_Shape&,  // myFShape
		   TopoDS_Shape&); // myLShape

static Standard_Boolean ToFuse(const TopoDS_Face& ,
			       const TopoDS_Face&);

static Standard_Real HeightMax(const TopoDS_Shape& theSbase,
			       const TopoDS_Face& theSkface,
			       const TopoDS_Shape& theSFrom,
			       const TopoDS_Shape& theSUntil);

static Standard_Integer SensOfPrism(const Handle(Geom_Curve) C,
				    const TopoDS_Shape& Until);

static Handle(Geom_Curve) TestCurve(const TopoDS_Shape&,
				    const gp_Vec&);


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFeat_MakePrism::Init(const TopoDS_Shape& Sbase,
			      const TopoDS_Shape& Pbase,
			      const TopoDS_Face& Skface,
			      const gp_Dir& Direc,
			      const Standard_Integer Mode,
			      const Standard_Boolean Modify)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::Init" << endl;
#endif
  mySkface = Skface;
  SketchFaceValid();
  mySbase  = Sbase;
  BasisShapeValid();
  myPbase  = Pbase;
  mySlface.Clear();
  myDir    = Direc;
  if(Mode == 0) {
    myFuse   = Standard_False;
    myJustFeat = Standard_False;
  }
  else if(Mode == 1) {
    myFuse   = Standard_True;
    myJustFeat = Standard_False;
  }
  else if(Mode == 2) {
    myFuse   = Standard_True;
    myJustFeat = Standard_True;
  }
  else {    
  }
  myModify = Modify;
  myJustGluer = Standard_False;


  //-------------- ifv
  //mySkface.Nullify();
  //-------------- ifv


  myShape.Nullify();
  myNewEdges.Clear();
  myTgtEdges.Clear();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  TopExp_Explorer exp;
  for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
    TopTools_ListOfShape thelist;
    myMap.Bind(exp.Current(), thelist);
    myMap(exp.Current()).Append(exp.Current());
  }
#ifdef OCCT_DEBUG
  if (trc) {
    if (myJustFeat)  cout << " Just Feature" << endl;
    if (myFuse)  cout << " Fuse" << endl;
    if (!myFuse)  cout << " Cut" << endl;
    if (!myModify) cout << " Modify = 0" << endl;
  }
#endif 
}


//=======================================================================
//function : Add
//purpose  : add elements of sliding (edge on face)
//=======================================================================

void BRepFeat_MakePrism::Add(const TopoDS_Edge& E,
			     const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::Add(Edge,face)" << endl;
#endif
  TopExp_Explorer exp;
  for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
    if (exp.Current().IsSame(F)) {
      break;
    }
  }
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  
  for (exp.Init(myPbase,TopAbs_EDGE);exp.More();exp.Next()) {
    if (exp.Current().IsSame(E)) {
      break;
    }
  }
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  
  if (!mySlface.IsBound(F)) {
    TopTools_ListOfShape thelist1;
    mySlface.Bind(F, thelist1);
  }
  TopTools_ListIteratorOfListOfShape itl(mySlface(F));
  for (; itl.More();itl.Next()) {
    if (itl.Value().IsSame(E)) {
      break;
    }
  }
  if (!itl.More()) {
    mySlface(F).Append(E);
  }
}


//=======================================================================
//function : Perform
//purpose  : construction of prism of length Length and 
//           call of reconstruction topo
//=======================================================================

void BRepFeat_MakePrism::Perform(const Standard_Real Length)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::Perform(Length)" << endl;
#endif
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil.Nullify();
  ShapeUntilValid();
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  gp_Vec V(Length*myDir);
  
//construction of prism of height Length

  LocOpe_Prism thePrism(myPbase,V);
  TopoDS_Shape VraiPrism = thePrism.Shape();
  
// management of descendants
  MajMap(myPbase,thePrism,myMap,myFShape,myLShape);
  

  myGShape = VraiPrism;    // the primitive
  GeneratedShapeValid();
  
  TopoDS_Face FFace;
  
  Standard_Boolean found = Standard_False;

// try to detect the faces of gluing 
// in case if the top of the prism is tangent to the initial shape

  if(!mySkface.IsNull() || !mySlface.IsEmpty()) {
    if(myLShape.ShapeType() == TopAbs_WIRE) {
      TopExp_Explorer ex1(VraiPrism, TopAbs_FACE);
      for(; ex1.More(); ex1.Next()) {
        TopExp_Explorer ex2(ex1.Current(), TopAbs_WIRE);
        for(; ex2.More(); ex2.Next()) {
          if(ex2.Current().IsSame(myLShape)) {
            FFace = TopoDS::Face(ex1.Current());
            found = Standard_True;
            break;
          }
        }
        if(found) break;
      }
    }
    
    TopExp_Explorer exp(mySbase, TopAbs_FACE);
    for(; exp.More(); exp.Next()) {
      const TopoDS_Face& ff = TopoDS::Face(exp.Current());
      if(ToFuse(ff, FFace)) {
        TopTools_DataMapOfShapeListOfShape sl;
        if(!FFace.IsSame(myPbase) && BRepFeat::IsInside(ff, FFace)) 
          break;
      }
    }
  }

// management of faces of gluing given by the user

  GluedFacesValid();

  if(!myGluedF.IsEmpty()) {   // case gluing
    myJustGluer = Standard_True;
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();    
    GlobalPerform();  // topological reconstruction 
  }

// if there is no gluing  -> call of ope topo
  if(!myJustGluer) {
    if(myFuse == 1 && !myJustFeat) {
      BRepAlgoAPI_Fuse f(mySbase, myGShape);
      myShape = f.Shape();
      UpdateDescendants(f, myShape, Standard_False);
      Done();
    }
    else if(myFuse == 0) {
      BRepAlgoAPI_Cut c(mySbase, myGShape);
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
    else {
      myShape = myGShape;
      Done();
    }
  }
}


//=======================================================================
//function : Perform
//purpose  : construction of prism oriented at the face Until, sufficiently 
//           long; call of topological reconstruction          
//=======================================================================

void BRepFeat_MakePrism::Perform(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::Perform(Until)" << endl;
#endif
  if (Until.IsNull()) {
    throw Standard_ConstructionError();
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trf = TransformShapeFU(1);  
  ShapeUntilValid();
  Handle(Geom_Curve) C = TestCurve(myPbase,myDir);  
  Standard_Integer sens = SensOfPrism(C, mySUntil);
  Standard_Real Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec V(2*sens*Height*myDir);

// construction of long prism
  LocOpe_Prism thePrism(myPbase,V);
  TopoDS_Shape VraiPrism = thePrism.Shape();

// in case of support of face Until
  if(!Trf) {    
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);    
    myGShape = VraiPrism;
    GeneratedShapeValid();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else {    // until support -> passage to topological operations
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);    
    TColGeom_SequenceOfCurve scur;
    scur.Clear();    
    scur.Append(C);

// direction of the prism depending on Until

    LocOpe_CSIntersector ASI(mySUntil);
    ASI.Perform(scur);
    TopAbs_Orientation Or;
    if (ASI.IsDone() && ASI.NbPoints(1) >=1) {
      if (myFuse == 1) {
        Or = ASI.Point(1, 1).Orientation();
      }
      else {
        Or = ASI.Point(1, ASI.NbPoints(1)).Orientation();
      }
      if(sens==-1) Or=TopAbs::Reverse(Or);
      TopoDS_Face FUntil = ASI.Point(1,1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull()) B.Add(Comp,S);
      BRepAlgoAPI_Cut trP(VraiPrism,Comp);
      UpdateDescendants(trP, trP.Shape(), Standard_False);
      //
      TopExp_Explorer ex(trP.Shape(), TopAbs_SOLID);
      TopoDS_Shape Cutsh = ex.Current();
      if (myFuse == 1 && !myJustFeat) {
        BRepAlgoAPI_Fuse f(mySbase, Cutsh);
        myShape = f.Shape();
        UpdateDescendants(f, myShape, Standard_False);
        Done();
      }
      else if(myFuse == 0) {
        BRepAlgoAPI_Cut c(mySbase, Cutsh);
        myShape = c.Shape();
        UpdateDescendants(c, myShape, Standard_False);
        Done();
      }
      else {
        myShape = Cutsh;
        Done();
      }
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : construction of a sufficiently long and properly oriented prism
//           call of topological reconstruction
//=======================================================================

void BRepFeat_MakePrism::Perform(const TopoDS_Shape& From,
				 const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::Perform(From,Until)" << endl;
#endif
  if (From.IsNull() || Until.IsNull()) {
    throw Standard_ConstructionError();
  }

  if (!mySkface.IsNull()) {
    if (From.IsSame(mySkface)) {
      myJustGluer = Standard_True;
      Perform(Until);
      if (myJustGluer) return;
    }
    else if (Until.IsSame(mySkface)) {
      myJustGluer = Standard_True;
      Perform(From);
      if (myJustGluer) return;
    } 
  }
  
  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionFU;
  PerfSelectionValid();
  
  TopExp_Explorer exp(From, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  mySFrom = From;
  Standard_Boolean Trff = TransformShapeFU(0);
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trfu = TransformShapeFU(1);
  ShapeUntilValid();  
  if(Trfu != Trff) {
    NotDone();
    myStatusError = BRepFeat_IncTypes;
    return;
  }

// length depending on bounding boxes

  Standard_Real Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  Handle(Geom_Curve) C = TestCurve(myPbase,myDir);  
  Standard_Integer sens;  // direction of prism
  Standard_Integer tran;  // transfer of prism
  if(From.IsSame(Until)) {
    sens = 1;
    tran = -1;
  }
  else {
    sens = SensOfPrism(C, mySUntil);
    tran = sens*SensOfPrism(C, mySFrom);
  }
  LocOpe_Prism thePrism;   
  if(tran < 0) {
    gp_Vec Vtra(-3*Height*sens/2.*myDir);
    thePrism.Perform(myPbase,3*sens*Height*myDir,Vtra);
  }
  else {
    thePrism.Perform(myPbase,2*sens*Height*myDir);
  }
  TopoDS_Shape VraiPrism = thePrism.Shape();
  
  if(!Trff) {
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);
    
    myGShape = VraiPrism;
    GeneratedShapeValid();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else {    // case until support -> topological operation
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);    
    TColGeom_SequenceOfCurve scur;
    scur.Clear();    
    scur.Append(C);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySFrom);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU, OrF;
    TopoDS_Face FFrom, FUntil;
    Standard_Real ParF, ParU;
    if (ASI1.IsDone() && ASI1.NbPoints(1) >=1) {
      if (myFuse == 1) {
	      OrU = ASI1.Point(1,1).Orientation();
      }
      else {
	      OrU = ASI1.Point(1,ASI1.NbPoints(1)).Orientation();
      }
      if(sens==-1) OrU = TopAbs::Reverse(OrU);
      FUntil = ASI1.Point(1,1).Face();
      ParU = ASI1.Point(1,1).Parameter();
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NoIntersectU;
      return;
    }
    if (ASI2.IsDone() && ASI2.NbPoints(1) >=1) {
      OrF = ASI2.Point(1,1).Orientation();
      if(sens==1) OrF = TopAbs::Reverse(OrF);
      FFrom = ASI2.Point(1,1).Face();
      ParF = ASI2.Point(1,1).Parameter();
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NoIntersectF;
      return;
    }
    if(tran > 0 && (Abs(ParU) < Abs(ParF)))
    {
      TopAbs_Orientation Or;
      Or = OrU;
      OrU = OrF;
      OrF = Or;
    }
    //
    TopTools_ListOfShape aLTools;
    TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!S.IsNull()) {
      aLTools.Append(S);
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NullToolU;
      return;
    }
    TopoDS_Solid SS = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!SS.IsNull()) {
      aLTools.Append(SS);
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NullToolF;
      return;
    }
    //
    TopTools_ListOfShape aLObj;
    aLObj.Append(VraiPrism);
    //
    BRepAlgoAPI_Cut trP;
    trP.SetArguments(aLObj);
    trP.SetTools(aLTools);
    trP.Build();
    UpdateDescendants(trP, trP.Shape(), Standard_False);
    if(myFuse == 1 && !myJustFeat) {
      BRepAlgoAPI_Fuse f(mySbase, trP.Shape());
      myShape = f.Shape();
      UpdateDescendants(f, myShape, Standard_False);
      Done();
    }
    else if(myFuse == 0) {
      BRepAlgoAPI_Cut c(mySbase, trP.Shape());
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
    else {
      myShape = trP.Shape();
      Done();
    }
  }
}

//=======================================================================
//function : PerformUntilEnd
//purpose  : construction of a prism and reconstruction
//=======================================================================

void BRepFeat_MakePrism::PerformUntilEnd()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::PerformUntilEnd()" << endl;
#endif
  myPerfSelection = BRepFeat_SelectionSh;
  PerfSelectionValid();
  myGluedF.Clear();
  mySUntil.Nullify();
  ShapeUntilValid();
  mySFrom.Nullify();
  ShapeFromValid();
  Standard_Real Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec V(2*Height*myDir);
  
  LocOpe_Prism thePrism(myPbase,V);
  TopoDS_Shape VraiPrism = thePrism.Shape();
  
  MajMap(myPbase,thePrism,myMap,myFShape,myLShape);
  
  myGShape = VraiPrism;
  GeneratedShapeValid();
  GluedFacesValid();

  if(myFuse == 0) {
    BRepAlgoAPI_Cut c(mySbase, myGShape);
    if (c.IsDone()) {
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
  }
  else {
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
}

//=======================================================================
//function : PerformFromEnd
//purpose  : 
//=======================================================================

void BRepFeat_MakePrism::PerformFromEnd(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::PerformFromEnd(From,Until)" << endl;
#endif
  if (Until.IsNull()) {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull() && Until.IsSame(mySkface)) {
    myDir.Reverse();
    PerformUntilEnd();
    return;
  }
  TopExp_Explorer exp;
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  myPerfSelection = BRepFeat_SelectionShU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trf = TransformShapeFU(1);
  ShapeUntilValid();
  Handle(Geom_Curve) C = TestCurve(myPbase,myDir);  
  Standard_Integer sens = SensOfPrism(C, mySUntil);
  Standard_Real Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec Vtra(-3*Height*sens/2.*myDir);
  gp_Vec Vect(3*sens*Height*myDir);
  LocOpe_Prism thePrism(myPbase,Vect,Vtra);
  TopoDS_Shape VraiPrism = thePrism.Shape();
  
  if(!Trf) {  // case face until 
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);
    myGShape = VraiPrism;
    GeneratedShapeValid();
    myGluedF.Clear();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else {   // case support
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);    
    TColGeom_SequenceOfCurve scur;
    scur.Clear();    
    scur.Append(C);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySbase);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU = TopAbs_FORWARD, OrF = TopAbs_FORWARD;
    TopoDS_Face FUntil, FFrom;
    if (ASI1.IsDone() && ASI1.NbPoints(1) >=1) {
      OrU = ASI1.Point(1,1).Orientation();
      if(sens==-1) {
	OrU = TopAbs::Reverse(OrU);
      }
      FUntil = ASI1.Point(1,1).Face();
    }
    if (ASI2.IsDone() && ASI2.NbPoints(1) >=1) {
      OrF = ASI2.Point(1,1).Orientation();
//      if(sens==1) OrF = TopAbs::Reverse(OrF);
      FFrom = ASI2.Point(1 ,1).Face();
      Handle(Geom_Surface) S = BRep_Tool::Surface(FFrom);
      if (S->DynamicType() == 
	  STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
	S = Handle(Geom_RectangularTrimmedSurface)::
	  DownCast(S)->BasisSurface();
      }
      BRepLib_MakeFace fac(S, Precision::Confusion());
      mySFrom = fac.Face();  
      Trf = TransformShapeFU(0);
      FFrom = TopoDS::Face(mySFrom);
    }

    TopTools_ListOfShape aLTools;
    TopoDS_Solid Sol = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!Sol.IsNull()) {
      aLTools.Append(Sol);
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NullToolU;
      return;
    }

    TopoDS_Solid Sol1 = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!Sol1.IsNull()) {
      aLTools.Append(Sol1);
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NullToolF;
      return;
    }
    //
    TopTools_ListOfShape aLObj;
    aLObj.Append(VraiPrism);
    //
    BRepAlgoAPI_Cut trP;
    trP.SetArguments(aLObj);
    trP.SetTools(aLTools);
    trP.Build();
    //
    UpdateDescendants(trP, trP.Shape(), Standard_False);
    if(myFuse == 1 && !myJustFeat) {
      BRepAlgoAPI_Fuse f(mySbase, trP.Shape());
      myShape = f.Shape();
      UpdateDescendants(f, myShape, Standard_False);
      Done();
    }
    else if(myFuse == 0) {
      BRepAlgoAPI_Cut c(mySbase, trP.Shape());
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
    else {
      myShape = trP.Shape();
      Done();
    }
  }
}

//=======================================================================
//function : PerformThruAll
//purpose  : 
//=======================================================================

void BRepFeat_MakePrism::PerformThruAll()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::PerformThruAll()" << endl;
#endif
  mySUntil.Nullify();
  ShapeUntilValid();
  mySFrom.Nullify();
  ShapeFromValid();
  if(myFuse == 0) {
    myPerfSelection = BRepFeat_NoSelection;
  }
  else {
    myPerfSelection = BRepFeat_SelectionSh;
  }
  PerfSelectionValid();
  myGluedF.Clear();
  GluedFacesValid();

  Standard_Real Height = HeightMax(mySbase, mySkface, mySFrom, mySUntil);
  gp_Vec V(3*Height*myDir);
  gp_Vec Vtra(-3*Height/2.*myDir);
  LocOpe_Prism thePrism(myPbase,V,Vtra);
  TopoDS_Shape VraiPrism = thePrism.Shape();
  MajMap(myPbase,thePrism,myMap,myFShape,myLShape);

  myGShape = VraiPrism;
  GeneratedShapeValid();  

  if(myFuse == 0) {
    BRepAlgoAPI_Cut c(mySbase, myGShape);
    if (c.IsDone()) {
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
  }
  else {
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
}

//=======================================================================
//function : PerformUntilHauteur
//purpose  : 
//=======================================================================

void BRepFeat_MakePrism::PerformUntilHeight(const TopoDS_Shape& Until,
                                            const Standard_Real Length)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePrism::PerformUntilHeight(Until,Length)" << endl;
#endif
  if (Until.IsNull()) {
    Perform(Length);
  }
  if(Length == 0) {
    Perform(Until);
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trf = TransformShapeFU(1);
  ShapeUntilValid();
  Handle(Geom_Curve) C = TestCurve(myPbase,myDir);  
  Standard_Integer sens = SensOfPrism(C, mySUntil);
  gp_Vec V(sens*Length*myDir);
  LocOpe_Prism thePrism(myPbase,V);
  TopoDS_Shape VraiPrism = thePrism.Shape();

  if(!Trf) {
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);
    
    myGShape = VraiPrism;
    GeneratedShapeValid();
    GluedFacesValid();
    thePrism.Curves(myCurves);
    myBCurve = thePrism.BarycCurve();
    GlobalPerform();
  }
  else {
    MajMap(myPbase,thePrism,myMap,myFShape,myLShape);    
    TColGeom_SequenceOfCurve scur;
    scur.Clear();    
    scur.Append(C);
    LocOpe_CSIntersector ASI(mySUntil);
    ASI.Perform(scur);
    TopAbs_Orientation Or;
    if (ASI.IsDone() && ASI.NbPoints(1) >=1) {
      if (myFuse == 1) {
	Or = ASI.Point(1,1).Orientation();
      }
      else {
	Or = ASI.Point(1,ASI.NbPoints(1)).Orientation();
      }
      if(sens==-1) Or=TopAbs::Reverse(Or);
      TopoDS_Face FUntil = ASI.Point(1,1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull()) B.Add(Comp,S);

      BRepAlgoAPI_Cut trP(VraiPrism,Comp);
      UpdateDescendants(trP, trP.Shape(), Standard_False);
      if(myFuse == 1 && !myJustFeat) {
        BRepAlgoAPI_Fuse f(mySbase, trP.Shape());
        myShape = f.Shape();
        UpdateDescendants(f, myShape, Standard_False);
        Done();
      }
      else if(myFuse == 0) {
        BRepAlgoAPI_Cut c(mySbase, trP.Shape());
        myShape = c.Shape();
        UpdateDescendants(c, myShape, Standard_False);
        Done();
      }
      else {
        myShape = trP.Shape();
        Done();
      }
    }
  }
}

//=======================================================================
//function : Curves
//purpose  : sequence of curves parallel to the axis of prism 
//=======================================================================

void BRepFeat_MakePrism::Curves(TColGeom_SequenceOfCurve& scur)
{
  scur = myCurves;
}

//=======================================================================
//function : BarycCurve
//purpose  : curve parallel to the axis of the prism passing through the center  
//           of masses
//=======================================================================

Handle(Geom_Curve) BRepFeat_MakePrism::BarycCurve()
{
  return myBCurve;
}


//=======================================================================
//function : HeightMax
//purpose  : Calculate the height of the prism following the parameters of
//           bounding box
//=======================================================================

static Standard_Real HeightMax(const TopoDS_Shape& theSbase,
			       const TopoDS_Face&  theSkface,
			       const TopoDS_Shape& theSFrom,
			       const TopoDS_Shape& theSUntil)
{
  Bnd_Box Box;
  BRepBndLib::Add(theSbase,Box);
  BRepBndLib::Add(theSkface,Box);
  if(!theSFrom.IsNull()) {
    Standard_Boolean FacRevolInfini = Standard_False;
    TopExp_Explorer exp;
    exp.Init(theSFrom, TopAbs_EDGE);
    for(; exp.More(); exp.Next()) {
      TopExp_Explorer exp1;
      exp1.Init(exp.Current(), TopAbs_VERTEX);
      if(!exp1.More()) {
	FacRevolInfini = Standard_True;
	break;
      }
    }
    if(!FacRevolInfini)
      BRepBndLib::Add(theSFrom,Box);
  }
  if(!theSUntil.IsNull()) 
  {
    Standard_Boolean FacRevolInfini = Standard_False;
    TopExp_Explorer exp;
    exp.Init(theSUntil, TopAbs_EDGE);
    for(; exp.More(); exp.Next()) 
    {
      TopExp_Explorer exp1;
      exp1.Init(exp.Current(), TopAbs_VERTEX);
      if(!exp1.More()) 
      {
        FacRevolInfini = Standard_True;
        break;
      }
    }
    if(!FacRevolInfini)
      BRepBndLib::Add(theSUntil,Box);
  }

  Standard_Real c[6];

  Box.Get(c[0],c[2],c[4],c[1],c[3],c[5]);
  Standard_Real parmin=c[0], parmax = c[0];
  for(Standard_Integer i = 0 ; i < 6; i++) {
    if(c[i] > parmax) parmax = c[i];
    if(c[i] < parmin ) parmin = c[i];    
  }
//#ifndef OCCT_DEBUG
  Standard_Real Height = fabs(2.*(parmax - parmin));
//#else
//  Standard_Real Height = abs(2.*(parmax - parmin));
//#endif
  return(Height);
}


//=======================================================================
//function : SensOfPrism
//purpose  : Direction of the prism depending on the shape Until
//=======================================================================
Standard_Integer SensOfPrism(const Handle(Geom_Curve) C,
			     const TopoDS_Shape& Until)
{
  LocOpe_CSIntersector ASI1(Until);
  TColGeom_SequenceOfCurve scur;
  scur.Append(C);
  ASI1.Perform(scur);
  Standard_Integer sens = 1;
  if(ASI1.IsDone() && ASI1.NbPoints(1) >= 1) {
    if(ASI1.Point(1, 1).Parameter() < 0. && 
       ASI1.Point(1, ASI1.NbPoints(1)).Parameter() < 0.) {
      sens = -1;
    }
  }
  else if(BRepFeat::ParametricBarycenter(Until,C) < 0) {
      sens = -1;
  }
  else {}
  return sens;
}

//=======================================================================
//function : MajMap
//purpose  : management of descendants
//=======================================================================

static void MajMap(const TopoDS_Shape& theB,
		   const LocOpe_Prism& theP,
		   TopTools_DataMapOfShapeListOfShape& theMap, // myMap
		   TopoDS_Shape& theFShape,  // myFShape
		   TopoDS_Shape& theLShape) // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(),TopAbs_WIRE);
  if (exp.More()) {
    theFShape = exp.Current();
    TopTools_ListOfShape thelist2;
    theMap.Bind(theFShape, thelist2);
    for (exp.Init(theP.FirstShape(),TopAbs_FACE);exp.More();exp.Next()) {
      theMap(theFShape).Append(exp.Current());
    }
  }
  
  exp.Init(theP.LastShape(),TopAbs_WIRE);
  if (exp.More()) {
    theLShape = exp.Current();
    TopTools_ListOfShape thelist3;
    theMap.Bind(theLShape, thelist3);
    for (exp.Init(theP.LastShape(),TopAbs_FACE);exp.More();exp.Next()) {
      theMap(theLShape).Append(exp.Current());
    }
  }

  for (exp.Init(theB,TopAbs_EDGE); exp.More(); exp.Next()) {
    if (!theMap.IsBound(exp.Current())) {
      TopTools_ListOfShape thelist4;
      theMap.Bind(exp.Current(), thelist4);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}


//=======================================================================
//function : MajMap
//purpose  : management of descendants
//=======================================================================

static Handle(Geom_Curve) TestCurve(const TopoDS_Shape& Base,
				    const gp_Vec& V)
{
  gp_Pnt bar(0., 0., 0.);
  TColgp_SequenceOfPnt spt;
  LocOpe::SampleEdges(Base,spt);
  for (Standard_Integer jj=1;jj<=spt.Length(); jj++) {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  gp_Ax1 newAx(bar,V);
  Handle(Geom_Line) theLin = new Geom_Line(newAx);
  return theLin;
}




//=======================================================================
//function : ToFuse
//purpose  : face SameDomaine or not
//=======================================================================

static Standard_Boolean ToFuse (const TopoDS_Face& F1, const TopoDS_Face& F2)
{
  if (F1.IsNull() || F2.IsNull()) {
    return Standard_False;
  }

  Handle(Geom_Surface) S1,S2;
  TopLoc_Location loc1, loc2;
  Handle(Standard_Type) typS1,typS2;
  const Standard_Real tollin = Precision::Confusion();
  const Standard_Real tolang = Precision::Angular();

  S1 = BRep_Tool::Surface(F1,loc1);
  S2 = BRep_Tool::Surface(F2,loc2);

  typS1 = S1->DynamicType();
  typS2 = S2->DynamicType();

  if (typS1 == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S1 =  Handle(Geom_RectangularTrimmedSurface)::DownCast (S1)->BasisSurface();
    typS1 = S1->DynamicType();
  }

  if (typS2 == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S2 =  Handle(Geom_RectangularTrimmedSurface)::DownCast (S2)->BasisSurface();
    typS2 = S2->DynamicType();
  }

  if (typS1 != typS2) {
    return Standard_False;
  }


  Standard_Boolean ValRet = Standard_False;
  if (typS1 == STANDARD_TYPE(Geom_Plane)) {
    gp_Pln pl1( Handle(Geom_Plane)::DownCast (S1)->Pln());
    gp_Pln pl2( Handle(Geom_Plane)::DownCast (S2)->Pln());

    // apply locations
    if (! loc1.IsIdentity())
      pl1.Transform (loc1.Transformation());
    if (! loc2.IsIdentity())
      pl2.Transform (loc2.Transformation());

    if (pl1.Position().IsCoplanar(pl2.Position(),tollin,tolang)) {
      ValRet = Standard_True;
    }
  }

  return ValRet;
}






