// File:	StepToGeom_MakeParabola.cxx
// Created:	Thu Sep  8 09:00:36 1994
// Author:	Frederic MAUPAS

#include <StepToGeom_MakeParabola.ixx>
#include <StepGeom_Parabola.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <Geom_Axis2Placement.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un Parabola de Geom a partir d' un Parabola de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeParabola::Convert
    (const Handle(StepGeom_Parabola)& SC,
     Handle(Geom_Parabola)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A;
    if (StepToGeom_MakeAxis2Placement::Convert
          (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()),A))
	{
      CC = new Geom_Parabola(A->Ax2(), SC->FocalDist() * UnitsMethods::LengthFactor());
      return Standard_True;
    }
  }
  return Standard_False;
}
