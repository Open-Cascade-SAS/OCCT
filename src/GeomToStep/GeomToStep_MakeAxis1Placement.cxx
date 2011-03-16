// File:	GeomToStep_MakeAxis1Placement.cxx
// Created:	Tue Jun 15 18:35:17 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeAxis1Placement.ixx>
#include <GeomToStep_MakeDirection.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2d.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom2d_AxisPlacement.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' un axis1_placement de prostep a partir d' un Ax1 de gp
//=============================================================================

GeomToStep_MakeAxis1Placement::GeomToStep_MakeAxis1Placement( const gp_Ax1& A)
{
#include <GeomToStep_MakeAxis1Placement_gen.pxx>
}
//=============================================================================
// Creation d' un axis1_placement de prostep a partir d' un Ax2d de gp
//=============================================================================

GeomToStep_MakeAxis1Placement::GeomToStep_MakeAxis1Placement( const gp_Ax2d& A)
{
#include <GeomToStep_MakeAxis1Placement_gen.pxx>
}

//=============================================================================
// Creation d' un axis1_placement de prostep a partir d' un Ax1Placement de
// Geom
//=============================================================================

GeomToStep_MakeAxis1Placement::GeomToStep_MakeAxis1Placement
  ( const Handle(Geom_Axis1Placement)& Axis1)
{
  gp_Ax1 A;
  A = Axis1->Ax1();
#include<GeomToStep_MakeAxis1Placement_gen.pxx>
}

//=============================================================================
// Creation d' un axis1_placement de prostep a partir d' un AxPlacement de
// Geom2d
//=============================================================================

GeomToStep_MakeAxis1Placement::GeomToStep_MakeAxis1Placement
  ( const Handle(Geom2d_AxisPlacement)& Axis1)
{
  gp_Ax2d A;
  A = Axis1->Ax2d();
#include<GeomToStep_MakeAxis1Placement_gen.pxx>
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Axis1Placement) &
      GeomToStep_MakeAxis1Placement::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theAxis1Placement;
}
