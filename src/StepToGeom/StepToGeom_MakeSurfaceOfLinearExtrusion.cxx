// File:	StepToGeom_MakeSurfaceOfLinearExtrusion.cxx
// Created:	Mon Jul  5 10:04:34 1993
// Author:	Martine LANGLOIS
// sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created (StepToGeom_MakeSurfaceOfLinearExtrusion(...) function)

#include <StepToGeom_MakeSurfaceOfLinearExtrusion.ixx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <StepToGeom_MakeVectorWithMagnitude.hxx>
#include <Geom_Curve.hxx>
#include <StepToGeom_MakeCurve.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

//=============================================================================
// Creation d' une SurfaceOfLinearExtrusion de Geom a partir d' une
// SurfaceOfLinearExtrusion de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeSurfaceOfLinearExtrusion::Convert (const Handle(StepGeom_SurfaceOfLinearExtrusion)& SS,
                                                                   Handle(Geom_SurfaceOfLinearExtrusion)& CS)
{
  Handle(Geom_Curve) C;
  if (StepToGeom_MakeCurve::Convert(SS->SweptCurve(),C))
  {
    // sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created
    Handle(Geom_VectorWithMagnitude) V;
    if (StepToGeom_MakeVectorWithMagnitude::Convert(SS->ExtrusionAxis(),V))
    {
      const gp_Dir D(V->Vec());
      CS = new Geom_SurfaceOfLinearExtrusion(C,D);
      return Standard_True;
    }
  }
  return Standard_False;
}
