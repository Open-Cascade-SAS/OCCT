// Created on: 1993-06-21
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomToStep_MakeBoundedCurve.hxx>
#include <GeomToStep_MakeConic.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Line.hxx>

//=============================================================================
// Creation d' une Curve de prostep a partir d' une Curve de Geom
//=============================================================================
GeomToStep_MakeCurve::GeomToStep_MakeCurve ( const Handle(Geom_Curve)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom_Line))) {
    Handle(Geom_Line) L = Handle(Geom_Line)::DownCast(C);
    GeomToStep_MakeLine MkLine(L);
    theCurve = MkLine.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Conic))) {
    Handle(Geom_Conic) L = Handle(Geom_Conic)::DownCast(C);
    GeomToStep_MakeConic MkConic(L);
    theCurve = MkConic.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) T = Handle(Geom_TrimmedCurve)::DownCast(C);
    Handle(Geom_Curve) B = T->BasisCurve();
//    TANT PIS, on passe la courbe de base ...
    if (B->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
      Handle(Geom_BSplineCurve) BS = Handle(Geom_BSplineCurve)::DownCast
	(B->Copy());
      BS->Segment (T->FirstParameter(),T->LastParameter());
      B = BS;
    }
    else if (B->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
      Handle(Geom_BezierCurve) BZ = Handle(Geom_BezierCurve)::DownCast
	(B->Copy());
      BZ->Segment (T->FirstParameter(),T->LastParameter());
      B = BZ;
    }
    else {
#ifdef OCCT_DEBUG
      cout<<"GeomToStep_MakeCurve, TrimmedCurve, BasisCurve is transferred not trimmed"<<endl;
      cout<<"BasisCurve Type : "<<B->DynamicType()->Name()<<endl;
#endif
    }
    GeomToStep_MakeCurve MkBasisC(B);
    theCurve = MkBasisC.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
    Handle(Geom_BoundedCurve) L = Handle(Geom_BoundedCurve)::DownCast(C);
    GeomToStep_MakeBoundedCurve MkBoundedC(L);
    theCurve = MkBoundedC.Value();
  }
  else
    done = Standard_False;
}	 

//=============================================================================
// Creation d'une Curve de prostep a partir d' une Curve de Geom2d
//=============================================================================

GeomToStep_MakeCurve::GeomToStep_MakeCurve ( const Handle(Geom2d_Curve)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom2d_Line))) {
    Handle(Geom2d_Line) L = Handle(Geom2d_Line)::DownCast(C);
    GeomToStep_MakeLine MkLine(L);
    theCurve = MkLine.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Conic))) {

    // ----------------------------------------------------------------------
    // A Circle of an Ellipse can be indirect. An indirect Axis in not 
    // mappable onto STEP. Then to avoid changing the topology, the Circle 
    // or the Ellipse are converted into BSpline Curves
    // ----------------------------------------------------------------------
    
    if (C->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
      Handle(Geom2d_Circle) theC2d = Handle(Geom2d_Circle)::DownCast(C);
      gp_Circ2d C2d = theC2d->Circ2d();
      if (!C2d.IsDirect()) {
#ifdef OCCT_DEBUG
	cout << "Warning : Circle converted to BSpline." << endl;
#endif
	Handle(Geom2d_BSplineCurve) aBSplineCurve2d = 
	  Geom2dConvert::CurveToBSplineCurve(theC2d);
        const Handle(Geom2d_BoundedCurve)& aBC2d = aBSplineCurve2d; // to avoid ambiguity
	GeomToStep_MakeBoundedCurve MkBoundedC(aBC2d);
	theCurve = MkBoundedC.Value();
      }
      else {
	Handle(Geom2d_Conic) L = Handle(Geom2d_Conic)::DownCast(C);
	GeomToStep_MakeConic MkConic(L);
	theCurve = MkConic.Value();
      }
    }
    else if (C->IsKind(STANDARD_TYPE(Geom2d_Ellipse))) {
      Handle(Geom2d_Ellipse) theE2d = Handle(Geom2d_Ellipse)::DownCast(C);
      gp_Elips2d E2d = theE2d->Elips2d();
      if (!E2d.IsDirect()) {
#ifdef OCCT_DEBUG
	cout << "Warning : Ellipse converted to BSpline." << endl;
#endif
	Handle(Geom2d_BSplineCurve) aBSplineCurve2d = 
	  Geom2dConvert::CurveToBSplineCurve(theE2d);
        const Handle(Geom2d_BoundedCurve)& aBC2d = aBSplineCurve2d; // to avoid ambiguity
	GeomToStep_MakeBoundedCurve MkBoundedC(aBC2d);
	theCurve = MkBoundedC.Value();
      }
      else {
	Handle(Geom2d_Conic) L = Handle(Geom2d_Conic)::DownCast(C);
	GeomToStep_MakeConic MkConic(L);
	theCurve = MkConic.Value();
      }
    }
    else {
      Handle(Geom2d_Conic) L = Handle(Geom2d_Conic)::DownCast(C);
      GeomToStep_MakeConic MkConic(L);
      theCurve = MkConic.Value();
    }
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve))) {
    Handle(Geom2d_BoundedCurve) L = Handle(Geom2d_BoundedCurve)::DownCast(C);
    GeomToStep_MakeBoundedCurve MkBoundedC(L);
    theCurve = MkBoundedC.Value();
  }
  else
    done = Standard_False;
} 


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Curve) &
      GeomToStep_MakeCurve::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeCurve::Value() - no result");
  return theCurve;
}
