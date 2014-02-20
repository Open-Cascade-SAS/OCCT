// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
