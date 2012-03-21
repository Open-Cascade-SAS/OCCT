// Created on: 1993-06-21
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <GeomToStep_MakeBoundedCurve.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <GeomToStep_MakeBoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GeomConvert.hxx>
#include <Geom2dConvert.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnots.hxx>
#include <GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>


//=============================================================================
// Creation d' une BoundedCurve de prostep a partir d' une BoundedCurve de Geom
//=============================================================================

GeomToStep_MakeBoundedCurve::GeomToStep_MakeBoundedCurve ( const Handle(Geom_BoundedCurve)& C)
{
  done = Standard_True;

  if (C->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) Bspli = Handle(Geom_BSplineCurve)::DownCast(C);
    // UPDATE FMA 1-04-96
    if (C->IsPeriodic()) {
      Handle(Geom_BSplineCurve) newBspli = 
	Handle(Geom_BSplineCurve)::DownCast(Bspli->Copy());
      newBspli->SetNotPeriodic();
      Bspli = newBspli;
    }
    if ( Bspli->IsRational() ) {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
    Handle(Geom_BezierCurve) Cur = Handle(Geom_BezierCurve)::DownCast(C);
    Handle(Geom_BSplineCurve) Bspli = GeomConvert::CurveToBSplineCurve(Cur);
    if ( Bspli->IsRational() ) {
      GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli);
      theBoundedCurve = MkRatBSplineC.Value();
    }
    else {
      GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
      theBoundedCurve = MkBSplineC.Value();
    }
  }
  else {
#ifdef DEB
    cout<<"MakeBoundedCurve, type : "<<C->DynamicType()->Name()<<" not processed"<<endl;
#endif
    done = Standard_False;
  }
}
	 
//=============================================================================
// Creation d' une BoundedCurve de prostep a partir d' une BoundedCurve de
// Geom2d
//=============================================================================

GeomToStep_MakeBoundedCurve::GeomToStep_MakeBoundedCurve ( const Handle(Geom2d_BoundedCurve)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      Handle(Geom2d_BSplineCurve) Bspli = 
	Handle(Geom2d_BSplineCurve)::DownCast(C);
      // UPDATE FMA 1-04-96
      if (C->IsPeriodic()) {
	Handle(Geom2d_BSplineCurve) newBspli = 
	  Handle(Geom2d_BSplineCurve)::DownCast(Bspli->Copy());
	newBspli->SetNotPeriodic();
	Bspli = newBspli;
      }
      if ( Bspli->IsRational() ) {
	GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve MkRatBSplineC(Bspli);
	theBoundedCurve = MkRatBSplineC.Value();
      }
      else {
	GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
	theBoundedCurve = MkBSplineC.Value();
      }
    }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
    Handle(Geom2d_BezierCurve) Cur = Handle(Geom2d_BezierCurve)::DownCast(C);
    Handle(Geom2d_BSplineCurve) Bspli = 
      Geom2dConvert::CurveToBSplineCurve(Cur);
    GeomToStep_MakeBSplineCurveWithKnots MkBSplineC(Bspli);
    theBoundedCurve = MkBSplineC.Value();
    }
  else
    done = Standard_False;
  
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BoundedCurve) &
      GeomToStep_MakeBoundedCurve::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theBoundedCurve;
}
