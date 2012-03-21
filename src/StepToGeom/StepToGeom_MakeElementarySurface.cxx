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

//:p0 abv 19.02.99: management of 'done' flag improved

#include <StepToGeom_MakeElementarySurface.ixx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepGeom_Plane.hxx>
#include <StepToGeom_MakeCylindricalSurface.hxx>
#include <StepToGeom_MakeConicalSurface.hxx>
#include <StepToGeom_MakeSphericalSurface.hxx>
#include <StepToGeom_MakeToroidalSurface.hxx>
#include <StepToGeom_MakePlane.hxx>

//=============================================================================
// Creation d' une ElementarySurface de Geom a partir d' une 
// ElementarySurface de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeElementarySurface::Convert (const Handle(StepGeom_ElementarySurface)& SS, Handle(Geom_ElementarySurface)& CS)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_Plane))) {
    const Handle(StepGeom_Plane) Sur = Handle(StepGeom_Plane)::DownCast(SS);
	return StepToGeom_MakePlane::Convert(Sur,*((Handle(Geom_Plane)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_CylindricalSurface))) {
    const Handle(StepGeom_CylindricalSurface) Sur = Handle(StepGeom_CylindricalSurface)::DownCast(SS);
    return StepToGeom_MakeCylindricalSurface::Convert(Sur,*((Handle(Geom_CylindricalSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_ConicalSurface))) {
    const Handle(StepGeom_ConicalSurface) Sur = Handle(StepGeom_ConicalSurface)::DownCast(SS);
	return StepToGeom_MakeConicalSurface::Convert(Sur,*((Handle(Geom_ConicalSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SphericalSurface))) {
    const Handle(StepGeom_SphericalSurface) Sur = Handle(StepGeom_SphericalSurface)::DownCast(SS);
    return StepToGeom_MakeSphericalSurface::Convert(Sur,*((Handle(Geom_SphericalSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_ToroidalSurface))) {
    const Handle(StepGeom_ToroidalSurface) Sur = Handle(StepGeom_ToroidalSurface)::DownCast(SS);
    return StepToGeom_MakeToroidalSurface::Convert(Sur,*((Handle(Geom_ToroidalSurface)*)&CS));
  }
  return Standard_False;
}	 
