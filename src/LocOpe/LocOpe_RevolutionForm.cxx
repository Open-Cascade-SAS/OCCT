// Created on: 1997-10-20
// Created by: Olga KOULECHOVA
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRepLib_MakeVertex.hxx>
#include <BRepSweep_Revol.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_RevolutionForm.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

//=======================================================================
//function : LocOpe_Revol
//purpose  : 
//=======================================================================
LocOpe_RevolutionForm::LocOpe_RevolutionForm() :  myDone(Standard_False)

{}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_RevolutionForm::Perform(const TopoDS_Shape& Base,
				    const gp_Ax1& Axis,
				    const Standard_Real Angle)
{
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();
  myBase = Base;
  myAngle = Angle;
  myAxis = Axis;
  myAngTra = 0.;
  myIsTrans = Standard_False;
  IntPerf();
}


//=======================================================================
//function : IntPerf
//purpose  : 
//=======================================================================

void LocOpe_RevolutionForm::IntPerf()
{
  TopoDS_Shape theBase = myBase;
  BRepTools_Modifier Modif;
  if (myIsTrans) {
    gp_Trsf T;
    T.SetRotation(myAxis,myAngTra);
    Handle(BRepTools_TrsfModification) modbase = 
      new BRepTools_TrsfModification(T);
    Modif.Init(theBase);
    Modif.Perform(modbase);
    theBase = Modif.ModifiedShape(theBase);
  }
  
  BRepSweep_Revol theRevol(theBase,myAxis,myAngle);

  myFirstShape = theRevol.FirstShape();
  myLastShape = theRevol.LastShape();

  TopExp_Explorer exp;
  if (theBase.ShapeType() == TopAbs_FACE) {
    for (exp.Init(theBase,TopAbs_EDGE);exp.More();exp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      if (!myMap.IsBound(edg)) {
        TopTools_ListOfShape thelist;
	myMap.Bind(edg, thelist);
	TopoDS_Shape desc = theRevol.Shape(edg);
	if (!desc.IsNull()) {
	  myMap(edg).Append(desc);
	}
      }
    }
    myRes = theRevol.Shape();
  }

  else {
    // Cas base != FACE
    TopTools_IndexedDataMapOfShapeListOfShape theEFMap;
    TopExp::MapShapesAndAncestors(theBase,TopAbs_EDGE,TopAbs_FACE,theEFMap);
    TopTools_ListOfShape lfaces;
    Standard_Boolean toremove = Standard_False;
    for (Standard_Integer i=1; i<=theEFMap.Extent(); i++) {
      const TopoDS_Shape& edg = theEFMap.FindKey(i);
      TopTools_ListOfShape thelist1;
      myMap.Bind(edg, thelist1);
      TopoDS_Shape desc = theRevol.Shape(edg);
      if (!desc.IsNull()) {
	if (theEFMap(i).Extent() >= 2) {
	  toremove = Standard_True;
	}
	else {
	  myMap(edg).Append(desc);
	  lfaces.Append(desc);
	}
      }
    }
    if(toremove) {
      // Rajouter les faces de FirstShape et LastShape
      for (exp.Init(myFirstShape,TopAbs_FACE);exp.More();exp.Next()) {
	lfaces.Append(exp.Current());
      }
      for (exp.Init(myLastShape,TopAbs_FACE);exp.More();exp.Next()) {
	lfaces.Append(exp.Current());
      }
      
      LocOpe_BuildShape BS(lfaces);
      myRes = BS.Shape();
    }
    else {
      for (exp.Init(theBase,TopAbs_EDGE);exp.More();exp.Next()) {
	const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
	if (!myMap.IsBound(edg)) {
          TopTools_ListOfShape thelist2;  
	  myMap.Bind(edg, thelist2);
	  TopoDS_Shape desc = theRevol.Shape(edg);
	  if (!desc.IsNull()) {
	    myMap(edg).Append(desc);
	  }
	}
      }
      myRes = theRevol.Shape();
    }
  }

  if (myIsTrans) {
    // m-a-j des descendants
    TopExp_Explorer anExp;
    for (anExp.Init(myBase,TopAbs_EDGE); anExp.More(); anExp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(anExp.Current());
      const TopoDS_Edge& edgbis = TopoDS::Edge(Modif.ModifiedShape(edg));
      if (!edgbis.IsSame(edg) && myMap.IsBound(edgbis)) {
	myMap.Bind(edg,myMap(edgbis));
	myMap.UnBind(edgbis);
      }
    }
  }
  myDone = Standard_True;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_RevolutionForm::Shape () const
{
  if (!myDone) {
    throw StdFail_NotDone();
  }
  return myRes;
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_RevolutionForm::FirstShape () const
{
  return myFirstShape;
}

//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_RevolutionForm::LastShape () const
{
  return myLastShape;
}


//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_RevolutionForm::Shapes (const TopoDS_Shape& S) const
{
  return myMap(S);
}
