// File:	StepToGeom_MakeAxisPlacemant.cxx
// Created:	Fri Aug 26 12:11:31 1994
// Author:	Frederic MAUPAS
// sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used

#include <StepToGeom_MakeAxisPlacement.ixx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <StepToGeom_MakeDirection2d.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Direction.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>

//=============================================================================
// Creation d' un AxisPlacement de Geom2d a partir d' un axis2_placement_3d
// de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeAxisPlacement::Convert
  (const Handle(StepGeom_Axis2Placement2d)& SA,
   Handle(Geom2d_AxisPlacement)& CA)
{
  Handle(Geom2d_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint2d::Convert(SA->Location(),P))
  {
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    gp_Dir2d Vxgp(1.,0.);
    if (SA->HasRefDirection()) {
      Handle(Geom2d_Direction) Vx;
      if (StepToGeom_MakeDirection2d::Convert(SA->RefDirection(),Vx))
        Vxgp = Vx->Dir2d();
    }

    CA = new Geom2d_AxisPlacement(P->Pnt2d(),Vxgp);
    return Standard_True;
  }
  return Standard_False;
}
