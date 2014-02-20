// Created on: 1993-07-05
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

#include <StepToGeom_MakeSweptSurface.ixx>
#include <StepGeom_SweptSurface.hxx>
#include <StepToGeom_MakeSweptSurface.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <StepToGeom_MakeSurfaceOfLinearExtrusion.hxx>
#include <StepToGeom_MakeSurfaceOfRevolution.hxx>

//=============================================================================
// Creation d' une SweptSurface de prostep a partir d' une 
// SweptSurface de Geom
//=============================================================================

Standard_Boolean StepToGeom_MakeSweptSurface::Convert (const Handle(StepGeom_SweptSurface)& SS, Handle(Geom_SweptSurface)& CS)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceOfLinearExtrusion))) {
    const Handle(StepGeom_SurfaceOfLinearExtrusion) Sur = Handle(StepGeom_SurfaceOfLinearExtrusion)::DownCast(SS);
    return StepToGeom_MakeSurfaceOfLinearExtrusion::Convert(Sur,*((Handle(Geom_SurfaceOfLinearExtrusion)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceOfRevolution))) {
    const Handle(StepGeom_SurfaceOfRevolution) Sur = Handle(StepGeom_SurfaceOfRevolution)::DownCast(SS);
    return StepToGeom_MakeSurfaceOfRevolution::Convert(Sur,*((Handle(Geom_SurfaceOfRevolution)*)&CS));
  }
  return Standard_False;
}	 
