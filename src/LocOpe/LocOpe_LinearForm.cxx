// Created on: 1996-09-04
// Created by: Olga PILLOT
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepSweep_Prism.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_LinearForm.hxx>
#include <Standard_NoSuchObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void LocOpe_LinearForm::Perform(const TopoDS_Shape& Base,
				const gp_Vec& V,
				const gp_Pnt& Pnt1,
				const gp_Pnt& Pnt2)
				
{
  myIsTrans = Standard_False;
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();

  myBase = Base;
  myVec = V;
  
//myEdge = E;
  myPnt1 = Pnt1;
  myPnt2 = Pnt2;

  IntPerf();
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_LinearForm::Perform(const TopoDS_Shape& Base,
				const gp_Vec& V,
				const gp_Vec& Vectra,
				const gp_Pnt& Pnt1,
				const gp_Pnt& Pnt2)
				
{
  myIsTrans = Standard_True;
  myTra = Vectra;
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();

  myBase = Base;
  myVec = V;
  
//myEdge = E;
  myPnt1 = Pnt1;
  myPnt2 = Pnt2;


  IntPerf();
}

//=======================================================================
//function : IntPerf
//purpose  : 
//=======================================================================

void LocOpe_LinearForm::IntPerf()
{
  TopoDS_Shape theBase = myBase;
  BRepTools_Modifier Modif;

  if (myIsTrans) {
    gp_Trsf T;
    T.SetTranslation(myTra);
    Handle(BRepTools_TrsfModification) modbase = 
      new BRepTools_TrsfModification(T);
    Modif.Init(theBase);
    Modif.Perform(modbase);
    theBase = Modif.ModifiedShape(theBase);
  }

  BRepSweep_Prism myPrism(theBase, myVec);

  myFirstShape = myPrism.FirstShape();
  myLastShape = myPrism.LastShape();

  TopExp_Explorer exp;
  if (theBase.ShapeType() == TopAbs_FACE) {
    for (exp.Init(theBase,TopAbs_EDGE);exp.More();exp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      if (!myMap.IsBound(edg)) {
        TopTools_ListOfShape thelist;
	myMap.Bind(edg, thelist);
	TopoDS_Shape desc = myPrism.Shape(edg);
	if (!desc.IsNull()) {
	  myMap(edg).Append(desc);
	}
      }
    }
    myRes = myPrism.Shape();
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
      TopoDS_Shape desc = myPrism.Shape(edg);
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
	  TopoDS_Shape desc = myPrism.Shape(edg);
	  if (!desc.IsNull()) {
	    myMap(edg).Append(desc);
	  }
	}
      }
      myRes = myPrism.Shape();
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

const TopoDS_Shape& LocOpe_LinearForm::Shape () const
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

const TopoDS_Shape& LocOpe_LinearForm::FirstShape () const
{
  return myFirstShape;
}

//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_LinearForm::LastShape () const
{
  return myLastShape;
}


//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_LinearForm::Shapes (const TopoDS_Shape& S) const
{
  return myMap(S);
}














































