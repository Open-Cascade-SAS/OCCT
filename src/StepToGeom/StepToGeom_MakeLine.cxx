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

// sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created (StepToGeom_MakeLine(...) function)

#include <StepToGeom_MakeLine.ixx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeVectorWithMagnitude.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

//=============================================================================
// Creation d' une Line de Geom a partir d' une Line de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeLine::Convert (const Handle(StepGeom_Line)& SC, Handle(Geom_Line)& CC)
{
  Handle(Geom_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint::Convert(SC->Pnt(),P))
  {
    // sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created 
    Handle(Geom_VectorWithMagnitude) D;
    if (StepToGeom_MakeVectorWithMagnitude::Convert(SC->Dir(),D))
    {
      const gp_Dir V(D->Vec());
      CC = new Geom_Line(P->Pnt(), V);
      return Standard_True;
    }
  }
  return Standard_False;
}
