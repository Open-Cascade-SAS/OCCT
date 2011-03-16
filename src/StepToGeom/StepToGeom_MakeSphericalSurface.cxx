// File:	StepToGeom_MakeSphericalSurface.cxx
// Created:	Fri Jul  2 18:35:52 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakeSphericalSurface.ixx>
#include <StepGeom_SphericalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une SphericalSurface de Geom a partir d' une 
// SphericalSurface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeSphericalSurface::Convert (const Handle(StepGeom_SphericalSurface)& SS, Handle(Geom_SphericalSurface)& CS)
{
  Handle(Geom_Axis2Placement) A;
  if (StepToGeom_MakeAxis2Placement::Convert(SS->Position(),A))
  {
    CS = new Geom_SphericalSurface(A->Ax2(), SS->Radius() * UnitsMethods::LengthFactor());
    return Standard_True;
  }
  return Standard_False;
}
