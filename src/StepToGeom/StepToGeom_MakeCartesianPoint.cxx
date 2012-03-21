// Created on: 1993-07-02
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


#include <StepToGeom_MakeCartesianPoint.ixx>
#include <StepGeom_CartesianPoint.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un CartesianPoint de Geom a partir d' un CartesianPoint de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCartesianPoint::Convert
  (const Handle(StepGeom_CartesianPoint)& SP,
   Handle(Geom_CartesianPoint)& CP)
{
  if (SP->NbCoordinates() == 3)
  {
    const Standard_Real LF = UnitsMethods::LengthFactor();
    const Standard_Real X = SP->CoordinatesValue(1) * LF;
    const Standard_Real Y = SP->CoordinatesValue(2) * LF;
    const Standard_Real Z = SP->CoordinatesValue(3) * LF;
    CP = new Geom_CartesianPoint(X, Y, Z);
    return Standard_True;
  }
  return Standard_False;
}
