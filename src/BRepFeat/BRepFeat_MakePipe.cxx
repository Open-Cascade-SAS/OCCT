// Created on: 1996-09-03
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
#include <BRepFeat.hxx>
#include <BRepFeat_MakePipe.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_Pipe.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean BRepFeat_GettraceFEAT();
#endif

static void MajMap(const TopoDS_Shape&, // base
		   LocOpe_Pipe&,
		   TopTools_DataMapOfShapeListOfShape&, // myMap
		   TopoDS_Shape&,  // myFShape
		   TopoDS_Shape&); // myLShape

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFeat_MakePipe::Init(const TopoDS_Shape& Sbase,
			     const TopoDS_Shape& Pbase,
			     const TopoDS_Face& Skface,
			     const TopoDS_Wire& Spine,
			     const Standard_Integer Mode,
			     const Standard_Boolean Modify)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePipe::Init" << endl;
#endif
  mySbase  = Sbase;
  BasisShapeValid();
  mySkface = Skface;
  SketchFaceValid();
  myPbase  = Pbase;
  mySlface.Clear();
  mySpine   = Spine;
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
//purpose  : add faces of gluing
//=======================================================================

void BRepFeat_MakePipe::Add(const TopoDS_Edge& E,
			     const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePipe::Add(Edge,face)" << endl;
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
    mySlface.Bind(F,thelist1);
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
//purpose  : 
//=======================================================================

void BRepFeat_MakePipe::Perform()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePipe::Perform()" << endl;
#endif
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil.Nullify();
  ShapeUntilValid();
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  TopoDS_Shape theBase = myPbase;
  LocOpe_Pipe thePipe(mySpine,theBase);
  TopoDS_Shape VraiPipe = thePipe.Shape();
  MajMap(myPbase,thePipe,myMap,myFShape,myLShape);
  myGShape = VraiPipe;
  GeneratedShapeValid();

  GluedFacesValid();

  if(myGluedF.IsEmpty()) {
    if(myFuse == 1) {
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
  else {
    myFShape = thePipe.FirstShape();
    TColgp_SequenceOfPnt spt;
    LocOpe::SampleEdges(myFShape,spt); 
    myCurves = thePipe.Curves(spt);
    myBCurve = thePipe.BarycCurve();
    GlobalPerform();
  }
}


//=======================================================================
//function : Perform
//purpose  : till shape Until
//=======================================================================

void BRepFeat_MakePipe::Perform(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePipe::Perform(Until)" << endl;
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
  TransformShapeFU(1);
  ShapeUntilValid();
  LocOpe_Pipe thePipe(mySpine,myPbase);
  TopoDS_Shape VraiTuyau = thePipe.Shape();
  MajMap(myPbase,thePipe,myMap,myFShape,myLShape);
  myGShape = VraiTuyau;
  GeneratedShapeValid();

  GluedFacesValid();

  myFShape = thePipe.FirstShape();
  TColgp_SequenceOfPnt spt;
  LocOpe::SampleEdges(myFShape,spt); 
  myCurves = thePipe.Curves(spt);
  myBCurve = thePipe.BarycCurve();
  GlobalPerform();
}


//=======================================================================
//function : Perform
//purpose  : between From and Until
//=======================================================================

void BRepFeat_MakePipe::Perform(const TopoDS_Shape& From,
				const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakePipe::Perform(From,Until)" << endl;
#endif
  if (From.IsNull() || Until.IsNull()) {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull()) {
    if (From.IsSame(mySkface)) {
      Perform(Until);
      return;
    }
    else if (Until.IsSame(mySkface)) {
      Perform(From);
      return;
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
  TransformShapeFU(0);
  ShapeFromValid();
  mySUntil = Until;
  TransformShapeFU(1);
  ShapeUntilValid();  
  LocOpe_Pipe thePipe(mySpine,myPbase);
  TopoDS_Shape VraiTuyau = thePipe.Shape();
  MajMap(myPbase,thePipe,myMap,myFShape,myLShape);
  myGShape = VraiTuyau;
  GeneratedShapeValid();

	//	mySbase, myPbase, mySlface, thePipe, myGluedF);
  GluedFacesValid();

  myFShape = thePipe.FirstShape();
  TColgp_SequenceOfPnt spt;
  LocOpe::SampleEdges(myFShape,spt); 
  myCurves = thePipe.Curves(spt);
  myBCurve = thePipe.BarycCurve();
  GlobalPerform();
}


//=======================================================================
//function : Curves
//purpose  : curves parallel to the generating wire of the pipe
//=======================================================================

void BRepFeat_MakePipe::Curves(TColGeom_SequenceOfCurve& scur)
{
  scur = myCurves;
}

//=======================================================================
//function : BarycCurve
//purpose  : pass through the center of mass
//=======================================================================

Handle(Geom_Curve) BRepFeat_MakePipe::BarycCurve()
{
  return myBCurve;
}

//=======================================================================
//function : MajMap
//purpose  : management of descendants
//=======================================================================

static void MajMap(const TopoDS_Shape& theB,
		   LocOpe_Pipe& theP,
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














