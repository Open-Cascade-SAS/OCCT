// Created on: 1993-07-02
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

//:p0 abv 19.02.99: management of 'done' flag improved

#include <StepToGeom_MakeConic.ixx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_Circle.hxx>
#include <StepToGeom_MakeCircle.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepToGeom_MakeEllipse.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepToGeom_MakeHyperbola.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepToGeom_MakeParabola.hxx>

//=============================================================================
// Creation d' une Conic de Geom a partir d' une Conic de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeConic::Convert
    (const Handle(StepGeom_Conic)& SC,
     Handle(Geom_Conic)& CC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Circle))) {
    const Handle(StepGeom_Circle) Cir = Handle(StepGeom_Circle)::DownCast(SC);
	return StepToGeom_MakeCircle::Convert(Cir,*((Handle(Geom_Circle)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
    const Handle(StepGeom_Ellipse) Ell = Handle(StepGeom_Ellipse)::DownCast(SC);
	return StepToGeom_MakeEllipse::Convert(Ell,*((Handle(Geom_Ellipse)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Hyperbola))) {
    const Handle(StepGeom_Hyperbola) Hyp = Handle(StepGeom_Hyperbola)::DownCast(SC);
	return StepToGeom_MakeHyperbola::Convert(Hyp,*((Handle(Geom_Hyperbola)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Parabola))) {
    const Handle(StepGeom_Parabola) Par = Handle(StepGeom_Parabola)::DownCast(SC);
	return StepToGeom_MakeParabola::Convert(Par,*((Handle(Geom_Parabola)*)&CC));
  }
  // Attention : Other conic shall be implemented !
  return Standard_False;
}
