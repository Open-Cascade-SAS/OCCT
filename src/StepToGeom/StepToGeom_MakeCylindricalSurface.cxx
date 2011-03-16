// File:	StepToGeom_MakeCylindricalSurface.cxx
// Created:	Fri Jul  2 17:20:46 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakeCylindricalSurface.ixx>
#include <Geom_CylindricalSurface.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une CylindricalSurface de Geom a partir d' une 
// CylindricalSurface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCylindricalSurface::Convert (const Handle(StepGeom_CylindricalSurface)& SS, Handle(Geom_CylindricalSurface)& CS)
{
  Handle(Geom_Axis2Placement) A;
  if (StepToGeom_MakeAxis2Placement::Convert(SS->Position(),A))
  {
    CS = new Geom_CylindricalSurface(A->Ax2(), SS->Radius() * UnitsMethods::LengthFactor());
    return Standard_True;
  }
  return Standard_False;
}
