// Created on: 1995-11-02
// Created by: Jing Cheng MEI
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


#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : BRepBuilderAPI_FindPlane
//purpose  : 
//=======================================================================
BRepBuilderAPI_FindPlane::BRepBuilderAPI_FindPlane() 
{
}

//=======================================================================
//function : BRepBuilderAPI_FindPlane
//purpose  : 
//=======================================================================

BRepBuilderAPI_FindPlane::BRepBuilderAPI_FindPlane(const TopoDS_Shape&    S, 
				     const Standard_Real    Tol)
{
  Init(S,Tol);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepBuilderAPI_FindPlane::Init(const TopoDS_Shape&    S, 
			     const Standard_Real    Tol)
{
  Standard_Real tolerance = Tol;
  myPlane.Nullify();

  // compute the tolerance
  TopExp_Explorer ex;
  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
    Standard_Real t = BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (t > tolerance) tolerance = t;
  }
  
  Standard_Real tol2 = tolerance*tolerance;
  // try to find an analytical curve and calculate points
  TopLoc_Location loc;
  Standard_Real first, last;
  Standard_Boolean found = Standard_False;  
  Handle(Geom_Plane) P;
  TColgp_SequenceOfPnt points;
  Standard_Integer nbPnts;

  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
    Handle(Geom_Curve) c3d = 
      BRep_Tool::Curve(TopoDS::Edge(ex.Current()), loc, first, last);

    if (!c3d.IsNull()) {
      Handle(Geom_Curve) c3dptr = 
	Handle(Geom_Curve)::DownCast(c3d->Transformed(loc.Transformation()));

      Handle(Standard_Type) cType = c3dptr->DynamicType();
      
      if (cType == STANDARD_TYPE(Geom_Line)) {
	nbPnts = 3;
      }
      else if ((cType == STANDARD_TYPE(Geom_Circle)) ||
	       (cType == STANDARD_TYPE(Geom_Ellipse)) ||
	       (cType == STANDARD_TYPE(Geom_Parabola)) ||
	       (cType == STANDARD_TYPE(Geom_Hyperbola))) {
	nbPnts = 4; 
	if (!found) {
	  found = Standard_True;
	  Handle(Geom_Conic) Co = Handle(Geom_Conic)::DownCast(c3dptr);
	  P = new Geom_Plane(gp_Ax3(Co->Position()));
	}
      }
      else if (cType == STANDARD_TYPE(Geom_BezierCurve)) {
	Handle(Geom_BezierCurve) Co = 
	  Handle(Geom_BezierCurve)::DownCast(c3dptr);
	nbPnts = Co->NbPoles();
      }
      else if (cType == STANDARD_TYPE(Geom_BSplineCurve)) {
	Handle(Geom_BSplineCurve) Co = 
	  Handle(Geom_BSplineCurve)::DownCast(c3dptr);
	nbPnts = Co->NbPoles();
      }
      else {
	nbPnts = 10;
      }
      
      gp_Pnt p0;
      for (Standard_Integer i=1; i<=nbPnts; i++) {
	if (i == 1) {
	  c3dptr->D0(first, p0);
	}
	else if (i == nbPnts) {
	  c3dptr->D0(last, p0);
	}
	else {
	  c3dptr->D0(first+(last-first)/(nbPnts-1)*(i-1), p0);
	}
	points.Append(p0);
      }
    }
  }

  if (!found) {
    // try to find a plane with the points
    if (points.Length() > 2) {    

      Standard_Real disMax = 0.0;
      gp_Pnt p0 = points(1);
      gp_Pnt p1;
      for (Standard_Integer i=2; i<=points.Length(); i++) {
	Standard_Real dist = p0.SquareDistance(points(i));
	if (dist > disMax) {
	  disMax = dist;
	  p1 = points(i); // it will be faster to store the point, otherwise it is necessary to find a value in a sequence
	}
      }

      if (disMax > tol2) {
	gp_Vec V1(p0, p1), V3;
	Standard_Real proMax = 0.0;
	gp_Pnt p2 = p0 ;
	for (Standard_Integer j=2; j<=points.Length(); j++) {
	  V3 = V1^gp_Vec(p0, points(j));
	  Standard_Real pro = V3.SquareMagnitude();
	  if (pro > proMax) {
	    proMax = pro;
	    p2 = points(j);
	  }
	}
	
	if (p0.SquareDistance(p2) > tol2) {
	  gp_Dir D1(V1), D2(gp_Vec(p0, p2));
	  if (!D1.IsParallel(D2, Precision::Angular())) {
	    P = new Geom_Plane(gp_Ax3(p0, D1.Crossed(D2), D1));
	    found = Standard_True;
	  }
	}
      }
    }
  }
  
  if (found) {
    // test if all points are on the plane
    const gp_Pln& pln = P->Pln();
    for (Standard_Integer i=1; i<=points.Length(); i++) {
      if (pln.SquareDistance(points(i)) > tol2) {
	found = Standard_False;
	break;
      }
    } 
  }
  
  if (found) {
    myPlane = P;
  }
}

//=======================================================================
//function : Found
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_FindPlane::Found() const 
{
  return !myPlane.IsNull();
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

Handle(Geom_Plane) BRepBuilderAPI_FindPlane::Plane() const 
{
  return myPlane;
}

