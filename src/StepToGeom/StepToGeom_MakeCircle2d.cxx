// File:	StepToGeom_MakeCircle2d.cxx
// Created:	Fri Aug 26 11:45:44 1994
// Author:	Frederic MAUPAS

#include <StepToGeom_MakeCircle2d.ixx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <Geom2d_AxisPlacement.hxx>
#include <StepToGeom_MakeAxisPlacement.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax22d.hxx>

//=============================================================================
// Creation d' un Circle de Geom2d a partir d' un Circle de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCircle2d::Convert
    (const Handle(StepGeom_Circle)& SC,
     Handle(Geom2d_Circle)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1) {
    Handle(Geom2d_AxisPlacement) A1;
    if (StepToGeom_MakeAxisPlacement::Convert
          (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()),A1))
    {
      const gp_Ax22d A( A1->Ax2d() );
      CC = new Geom2d_Circle(A, SC->Radius());
      return Standard_True;
    }
  }
  return Standard_False;
}
