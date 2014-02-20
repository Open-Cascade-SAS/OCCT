// Created on: 1993-08-04
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

//:n6 abv 15.02.99: S4132: adding translation of polyline
//:p0 abv 19.02.99: management of 'done' flag improved

#include <StepToGeom_MakeBoundedCurve2d.ixx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepToGeom_MakeTrimmedCurve2d.hxx>
#include <StepToGeom_MakeBSplineCurve2d.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepToGeom_MakePolyline2d.hxx>

//=============================================================================
// Creation d' une BoundedCurve de Geom a partir d' une BoundedCurve de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeBoundedCurve2d::Convert (const Handle(StepGeom_BoundedCurve)& SC, Handle(Geom2d_BoundedCurve)& CC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve))) {
    const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)
      Bspli = Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)::DownCast(SC);
	return StepToGeom_MakeBSplineCurve2d::Convert(Bspli,*((Handle(Geom2d_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots))) {
    const Handle(StepGeom_BSplineCurveWithKnots)
      Bspli = Handle(StepGeom_BSplineCurveWithKnots)::DownCast(SC);
	return StepToGeom_MakeBSplineCurve2d::Convert(Bspli,*((Handle(Geom2d_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve))) {
    const Handle(StepGeom_TrimmedCurve) TC = Handle(StepGeom_TrimmedCurve)::DownCast(SC);
	return StepToGeom_MakeTrimmedCurve2d::Convert(TC,*((Handle(Geom2d_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Polyline))) { //:n6 abv 15 Feb 99
    const Handle(StepGeom_Polyline) PL = Handle(StepGeom_Polyline)::DownCast(SC);
	return StepToGeom_MakePolyline2d::Convert(PL,*((Handle(Geom2d_BSplineCurve)*)&CC));
  }
  return Standard_False;
}
