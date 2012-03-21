// Created on: 1993-06-22
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


#include <GeomToStep_MakeElementarySurface.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <GeomToStep_MakeElementarySurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Plane.hxx>
#include <GeomToStep_MakeCylindricalSurface.hxx>
#include <GeomToStep_MakeConicalSurface.hxx>
#include <GeomToStep_MakeSphericalSurface.hxx>
#include <GeomToStep_MakeToroidalSurface.hxx>
#include <GeomToStep_MakePlane.hxx>

//=============================================================================
// Creation d' une ElementarySurface de prostep a partir d' une 
// ElementarySurface de Geom
//=============================================================================

GeomToStep_MakeElementarySurface::GeomToStep_MakeElementarySurface
  ( const Handle(Geom_ElementarySurface)& S)
{
  done = Standard_True;
  if (S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    Handle(Geom_CylindricalSurface) Sur = 
      Handle(Geom_CylindricalSurface)::DownCast(S);
    GeomToStep_MakeCylindricalSurface MkCylindrical(Sur);
    theElementarySurface = MkCylindrical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
    Handle(Geom_ConicalSurface) Sur = 
      Handle(Geom_ConicalSurface)::DownCast(S);
    GeomToStep_MakeConicalSurface MkConical(Sur);
    theElementarySurface = MkConical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    Handle(Geom_SphericalSurface) Sur = 
      Handle(Geom_SphericalSurface)::DownCast(S);
    GeomToStep_MakeSphericalSurface MkSpherical(Sur);
    theElementarySurface = MkSpherical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
    Handle(Geom_ToroidalSurface) Sur = 
      Handle(Geom_ToroidalSurface)::DownCast(S);
    GeomToStep_MakeToroidalSurface MkToroidal(Sur);
    theElementarySurface = MkToroidal.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_Plane))) {
    Handle(Geom_Plane) Sur =	Handle(Geom_Plane)::DownCast(S); 
    GeomToStep_MakePlane MkPlane(Sur);
    theElementarySurface = MkPlane.Value();
  }
  else
    done = Standard_False;
}	 


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_ElementarySurface) &
      GeomToStep_MakeElementarySurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theElementarySurface;
}
