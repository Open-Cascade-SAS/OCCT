// File:	StepToGeom_MakeParabola2d.cxx
// Created:	Thu Sep  1 13:57:24 1994
// Author:	Frederic MAUPAS

#include <StepToGeom_MakeParabola2d.ixx>
#include <StepGeom_Parabola.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <Geom2d_AxisPlacement.hxx>
#include <StepToGeom_MakeAxisPlacement.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax22d.hxx>

//=============================================================================
// Creation d' un Parabola de Geom2d a partir d' un Parabola de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeParabola2d::Convert
    (const Handle(StepGeom_Parabola)& SC,
     Handle(Geom2d_Parabola)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1) {
    Handle(Geom2d_AxisPlacement) A1;
    if (StepToGeom_MakeAxisPlacement::Convert
          (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()),A1))
    {
      const gp_Ax22d A( A1->Ax2d() );
      CC = new Geom2d_Parabola(A, SC->FocalDist());
      return Standard_True;
    }
  }
  return Standard_False;
}
