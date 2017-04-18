// Created on: 1993-06-15
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Geom2d_AxisPlacement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <GeomToStep_MakeAxis1Placement.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <GeomToStep_MakeDirection.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2d.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' un axis1_placement de prostep a partir d' un Ax1 de gp
//=============================================================================
GeomToStep_MakeAxis1Placement::GeomToStep_MakeAxis1Placement( const gp_Ax1& A)
{
#include "GeomToStep_MakeAxis1Placement_gen.pxx"
}
//=============================================================================
// Creation d' un axis1_placement de prostep a partir d' un Ax2d de gp
//=============================================================================

GeomToStep_MakeAxis1Placement::GeomToStep_MakeAxis1Placement( const gp_Ax2d& A)
{
#include "GeomToStep_MakeAxis1Placement_gen.pxx"
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
#include "GeomToStep_MakeAxis1Placement_gen.pxx"
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
#include "GeomToStep_MakeAxis1Placement_gen.pxx"
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Axis1Placement) &
      GeomToStep_MakeAxis1Placement::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeAxis1Placement::Value() - no result");
  return theAxis1Placement;
}
