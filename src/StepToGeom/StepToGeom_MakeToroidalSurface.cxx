// Created on: 1993-07-05
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


#include <StepToGeom_MakeToroidalSurface.ixx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une ToroidalSurface de Geom a partir d' une 
// ToroidalSurface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeToroidalSurface::Convert (const Handle(StepGeom_ToroidalSurface)& SS, Handle(Geom_ToroidalSurface)& CS)
{
  Handle(Geom_Axis2Placement) A;
  if (StepToGeom_MakeAxis2Placement::Convert(SS->Position(),A))
  {
    const Standard_Real LF = UnitsMethods::LengthFactor();
    CS = new Geom_ToroidalSurface(A->Ax2(), Abs(SS->MajorRadius() * LF), Abs(SS->MinorRadius() * LF));
    return Standard_True;
  }
  return Standard_False;
}
