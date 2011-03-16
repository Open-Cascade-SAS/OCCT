// File:	StepToGeom_MakePlane.cxx
// Created:	Fri Jul  2 18:22:22 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakePlane.ixx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>

//=============================================================================
// Creation d' un Plane de Geom a partir d' un plane de Step
//=============================================================================

Standard_Boolean StepToGeom_MakePlane::Convert (const Handle(StepGeom_Plane)& SP, Handle(Geom_Plane)& CP)
{
  Handle(Geom_Axis2Placement) A;
  if (StepToGeom_MakeAxis2Placement::Convert(SP->Position(),A))
  {
    CP = new Geom_Plane(A->Ax2());
    return Standard_True;
  }
  return Standard_False;
}
