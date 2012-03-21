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

// sln 22.10.2001. CTS23496: If problems with creation of axis's direction occur default direction is used (StepToGeom_MakeAxis1Placement(...) function)

#include <StepToGeom_MakeAxis1Placement.ixx>
#include <StepGeom_Direction.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeDirection.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Direction.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Axis1Placement.hxx>

//=============================================================================
// Creation d' un Ax1Placement de Geom a partir d' un axis1_placement de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeAxis1Placement::Convert (const Handle(StepGeom_Axis1Placement)& SA, Handle(Geom_Axis1Placement)& CA)
{
  Handle(Geom_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint::Convert(SA->Location(),P))
  {
    // sln 22.10.2001. CTS23496: If problems with creation of axis direction occur default direction is used
    gp_Dir D(0.,0.,1.);
    if (SA->HasAxis())
    {
      Handle(Geom_Direction) D1;
      if (StepToGeom_MakeDirection::Convert(SA->Axis(),D1))
        D = D1->Dir();
    }
    CA = new Geom_Axis1Placement(P->Pnt(),D);
    return Standard_True;
  }
  return Standard_False;
}
