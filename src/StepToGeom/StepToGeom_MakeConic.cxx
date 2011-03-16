// File:	StepToGeom_MakeConic.cxx
// Created:	Fri Jul  2 16:51:20 1993
// Author:	Martine LANGLOIS
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
