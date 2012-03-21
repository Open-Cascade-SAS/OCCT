// Created on: 1993-08-04
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

// sln 23.10.2001. CTS23496: Direction is not created if it has null magnitude (StepToGeom_MakeDirection2d(...) function)

#include <StepToGeom_MakeDirection2d.ixx>
#include <StepGeom_Direction.hxx>

//=============================================================================
// Creation d' un Direction de Geom2d a partir d' un Direction de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeDirection2d::Convert (const Handle(StepGeom_Direction)& SD, Handle(Geom2d_Direction)& CD)
{
  if (SD->NbDirectionRatios() >= 2)
  {
    const Standard_Real X = SD->DirectionRatiosValue(1);
    const Standard_Real Y = SD->DirectionRatiosValue(2);
    // sln 23.10.2001. CTS23496: Direction is not created if it has null magnitude
    if(gp_XY(X,Y).SquareModulus() > gp::Resolution()*gp::Resolution())
    {
      CD = new Geom2d_Direction(X, Y);
      return Standard_True;
    }
  }
  return Standard_False;   
}
