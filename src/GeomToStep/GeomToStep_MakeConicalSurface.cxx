// Created on: 1993-06-16
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


#include <GeomToStep_MakeConicalSurface.ixx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_ConicalSurface.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <TCollection_HAsciiString.hxx>

#include <Standard_DomainError.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une conical_surface de prostep a partir d' une ConicalSurface
// de Geom
//=============================================================================

GeomToStep_MakeConicalSurface::GeomToStep_MakeConicalSurface
  ( const Handle(Geom_ConicalSurface)& CS )
	
{
  Handle(StepGeom_ConicalSurface) CSstep = new StepGeom_ConicalSurface;
  Handle(StepGeom_Axis2Placement3d) aPosition;
  Standard_Real aRadius, aSemiAngle;
  
  GeomToStep_MakeAxis2Placement3d MkAxis(CS->Position());
  aPosition = MkAxis.Value();
  aRadius = CS->RefRadius();
  aSemiAngle = CS->SemiAngle();
  if (aSemiAngle < 0. || aSemiAngle > M_PI/2.) {
    Standard_DomainError::Raise("Conicalsurface not STEP conformant");
  }
  
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  CSstep->Init(name, aPosition, aRadius / UnitsMethods::LengthFactor(), aSemiAngle);
  theConicalSurface = CSstep;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_ConicalSurface) &
      GeomToStep_MakeConicalSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theConicalSurface;
}
