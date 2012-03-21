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

// sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude(...) function)

#include <StepToGeom_MakeVectorWithMagnitude.ixx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Direction.hxx>
#include <StepToGeom_MakeDirection.hxx>
#include <Geom_Direction.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom a partir d' un Vector de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeVectorWithMagnitude::Convert (const Handle(StepGeom_Vector)& SV, Handle(Geom_VectorWithMagnitude)& CV)
{
  // sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created 
  Handle(Geom_Direction) D;
  if (StepToGeom_MakeDirection::Convert(SV->Orientation(),D))
  {
    const gp_Vec V(D->Dir().XYZ() * SV->Magnitude() * UnitsMethods::LengthFactor());
    CV = new Geom_VectorWithMagnitude(V);
    return Standard_True;
  }
  return Standard_False;
}
