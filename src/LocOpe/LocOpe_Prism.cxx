// Created on: 1996-09-04
// Created by: Olga PILLOT
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <LocOpe_Prism.ixx>


#include <BRepSweep_Prism.hxx>

#include <TopExp_Explorer.hxx>
#include <LocOpe_BuildShape.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <BRepLib_MakeVertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <LocOpe.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>


//=======================================================================
//function : LocOpe_Prism
//purpose  : 
//=======================================================================

LocOpe_Prism::LocOpe_Prism(): myDone(Standard_False)
{}

//=======================================================================
//function : LocOpe_Prism
//purpose  : 
//=======================================================================

LocOpe_Prism::LocOpe_Prism(const TopoDS_Shape&  Base,
			   const gp_Vec& V):
       myBase(Base), myVec(V), myIsTrans(Standard_False)

{
  IntPerf();
}

//=======================================================================
//function : LocOpe_Prism
//purpose  : 
//=======================================================================

LocOpe_Prism::LocOpe_Prism(const TopoDS_Shape&  Base,
			   const gp_Vec& V,
			   const gp_Vec& Vtra):
       myBase(Base), myVec(V), myTra(Vtra), myIsTrans(Standard_True)

{
  IntPerf();

}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_Prism::Perform(const TopoDS_Shape& Base,
			   const gp_Vec& V)
{
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();

  myBase = Base;
  myVec = V;
  myIsTrans = Standard_False;
  IntPerf();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void LocOpe_Prism::Perform(const TopoDS_Shape& Base,
			   const gp_Vec& V,
			   const gp_Vec& Vtra)
{
  myMap.Clear();
  myFirstShape.Nullify();
  myLastShape.Nullify();
  myBase.Nullify();
  myRes.Nullify();

  myBase = Base;
  myVec = V;
  myTra = Vtra;
  myIsTrans = Standard_True;
  IntPerf();
}


//=======================================================================
//function : IntPerf
//purpose  : 
//=======================================================================

void LocOpe_Prism::IntPerf()
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
  
  BRepSweep_Prism thePrism(theBase,myVec);

  myFirstShape = thePrism.FirstShape();
  myLastShape = thePrism.LastShape();

  TopExp_Explorer exp;
  if (theBase.ShapeType() == TopAbs_FACE) {
    for (exp.Init(theBase,TopAbs_EDGE);exp.More();exp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      if (!myMap.IsBound(edg)) {
        TopTools_ListOfShape thelist;
	myMap.Bind(edg, thelist);
	TopoDS_Shape desc = thePrism.Shape(edg);
	if (!desc.IsNull()) {
	  myMap(edg).Append(desc);
	}
      }
    }
    myRes = thePrism.Shape();
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
      TopoDS_Shape desc = thePrism.Shape(edg);
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
	  TopoDS_Shape desc = thePrism.Shape(edg);
	  if (!desc.IsNull()) {
	    myMap(edg).Append(desc);
	  }
	}
      }
      myRes = thePrism.Shape();
    }
  }

  if (myIsTrans) {
    // m-a-j des descendants
    TopExp_Explorer exp;
    for (exp.Init(myBase,TopAbs_EDGE);exp.More();exp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
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

const TopoDS_Shape& LocOpe_Prism::Shape () const
{
  if (!myDone) {
    StdFail_NotDone::Raise();
  }
  return myRes;
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_Prism::FirstShape () const
{
  return myFirstShape;
}

//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& LocOpe_Prism::LastShape () const
{
  return myLastShape;
}


//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_Prism::Shapes (const TopoDS_Shape& S) const
{
  return myMap(S);
}


//=======================================================================
//function : Curves
//purpose  : 
//=======================================================================

void LocOpe_Prism::Curves(TColGeom_SequenceOfCurve& Scurves) const
{
  Scurves.Clear();
  TColgp_SequenceOfPnt spt;
  LocOpe::SampleEdges(myFirstShape,spt);
  Standard_Real height = 
    Sqrt(myVec.X()*myVec.X()+myVec.Y()*myVec.Y()+myVec.Z()*myVec.Z());
  Standard_Real u1 = -2*height;
  Standard_Real u2 =  2*height;

  for (Standard_Integer jj=1;jj<=spt.Length(); jj++) {
    gp_Ax1 theAx(spt(jj),myVec);
    Handle(Geom_Line) theLin = new Geom_Line(theAx);
    Handle(Geom_TrimmedCurve) trlin = 
      new Geom_TrimmedCurve(theLin, u1, u2, Standard_True);   
    Scurves.Append(trlin);
  }
}

//=======================================================================
//function : BarycCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) LocOpe_Prism::BarycCurve() const
{
  gp_Pnt bar(0., 0., 0.);
  TColgp_SequenceOfPnt spt;
  LocOpe::SampleEdges(myFirstShape,spt);
  for (Standard_Integer jj=1;jj<=spt.Length(); jj++) {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());
  gp_Ax1 newAx(bar,myVec);
  Handle(Geom_Line) theLin = new Geom_Line(newAx);
  return theLin;
}

