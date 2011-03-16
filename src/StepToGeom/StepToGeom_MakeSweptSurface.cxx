// File:	StepToGeom_MakeSweptSurface.cxx
// Created:	Mon Jul  5 10:52:45 1993
// Author:	Martine LANGLOIS
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
