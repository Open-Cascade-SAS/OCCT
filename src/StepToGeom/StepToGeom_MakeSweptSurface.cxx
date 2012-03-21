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
