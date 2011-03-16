// File:	StepToGeom_MakeConic2d.cxx
// Created:	Fri Aug 26 11:38:37 1994
// Author:	Frederic MAUPAS
//:p0 abv 19.02.99: management of 'done' flag improved

#include <StepToGeom_MakeConic2d.ixx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepToGeom_MakeCircle2d.hxx>
#include <StepToGeom_MakeEllipse2d.hxx>
#include <StepToGeom_MakeHyperbola2d.hxx>
#include <StepToGeom_MakeParabola2d.hxx>

//=============================================================================
// Creation d' une Conic de Geom2d a partir d' une Conic de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeConic2d::Convert
    (const Handle(StepGeom_Conic)& SC,
     Handle(Geom2d_Conic)& CC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Circle))) {
    const Handle(StepGeom_Circle) Cir = Handle(StepGeom_Circle)::DownCast(SC);
	return StepToGeom_MakeCircle2d::Convert(Cir,*((Handle(Geom2d_Circle)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
    const Handle(StepGeom_Ellipse) Ell = Handle(StepGeom_Ellipse)::DownCast(SC);
	return StepToGeom_MakeEllipse2d::Convert(Ell,*((Handle(Geom2d_Ellipse)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Hyperbola))) {
    const Handle(StepGeom_Hyperbola) Hyp = Handle(StepGeom_Hyperbola)::DownCast(SC);
	return StepToGeom_MakeHyperbola2d::Convert(Hyp,*((Handle(Geom2d_Hyperbola)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Parabola))) {
    const Handle(StepGeom_Parabola) Par = Handle(StepGeom_Parabola)::DownCast(SC);
	return StepToGeom_MakeParabola2d::Convert(Par,*((Handle(Geom2d_Parabola)*)&CC));
  }
  // Attention : Other conic shall be implemented !
  return Standard_False;
}
