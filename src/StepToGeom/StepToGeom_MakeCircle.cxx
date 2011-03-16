// File:	StepToGeom_MakeCircle.cxx
// Created:	Fri Jul  2 16:37:06 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakeCircle.ixx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <Geom_Axis2Placement.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un Circle de Geom a partir d' un Circle de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCircle::Convert
    (const Handle(StepGeom_Circle)& SC,
     Handle(Geom_Circle)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A;
    if (StepToGeom_MakeAxis2Placement::Convert 
          (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()),A))
    {
      CC = new Geom_Circle(A->Ax2(),SC->Radius() * UnitsMethods::LengthFactor());
      return Standard_True;
	}
  }
  return Standard_False;
}
