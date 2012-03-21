// Created on: 1994-08-26
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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
