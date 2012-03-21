// Created on: 1993-06-15
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
