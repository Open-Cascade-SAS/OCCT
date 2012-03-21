// Created on: 1993-08-04
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
