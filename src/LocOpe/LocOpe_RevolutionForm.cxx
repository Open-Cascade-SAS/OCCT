// Created on: 1997-10-20
// Created by: Olga KOULECHOVA
// Copyright (c) 1997-1999 Matra Datavision
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



#include <LocOpe_RevolutionForm.ixx>

#include <BRepSweep_Revol.hxx>

#include <TopExp_Explorer.hxx>
#include <LocOpe_BuildShape.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <gp_Circ.hxx>
#include <gp_Vec.hxx>

#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>


#include <BRepLib_MakeVertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <LocOpe.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>


#ifdef DEB
static Standard_Boolean FindCircle(const gp_Ax1&,
				   const gp_Pnt&,
				   gp_Circ&);
#endif

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

const TopoDS_Shape& LocOpe_RevolutionForm::Shape () const
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

//=======================================================================
//function : FincCircle
//purpose  : 
//=======================================================================

#ifdef DEB
static Standard_Boolean FindCircle(const gp_Ax1& Ax,
				   const gp_Pnt& Pt,
				   gp_Circ& Ci)
{

  const gp_Dir& Dax = Ax.Direction();
  gp_Vec OP(Ax.Location(),Pt);

  Standard_Real prm = OP.Dot(Dax);

  gp_Pnt prj(Ax.Location().XYZ().Added(prm*Dax.XYZ()));
  gp_Vec axx(prj,Pt);
  Standard_Real Radius = axx.Magnitude();
  if (Radius < Precision::Confusion()) {
    return Standard_False;
  }
  Ci.SetRadius(Radius);
  Ci.SetPosition(gp_Ax2(prj,Dax,axx));
  return Standard_True;
}
#endif
