// File:	GeomToStep_MakeSurfaceOfLinearExtrusion.cxx
// Created:	Thu Jun 17 12:28:45 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeSurfaceOfLinearExtrusion.ixx>
#include <GeomToStep_MakeVector.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Vec.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Vector.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une surface_of_linear_extrusion de prostep a partir d' une
// SurfaceOfLinearExtrusion de Geom
//=============================================================================

GeomToStep_MakeSurfaceOfLinearExtrusion::GeomToStep_MakeSurfaceOfLinearExtrusion
  ( const Handle(Geom_SurfaceOfLinearExtrusion)& S )
	
{
  Handle(StepGeom_SurfaceOfLinearExtrusion) Surf;
  Handle(StepGeom_Curve) aSweptCurve;
  Handle(StepGeom_Vector) aExtrusionAxis;
  
  GeomToStep_MakeCurve MkCurve(S->BasisCurve());
  GeomToStep_MakeVector MkVector(gp_Vec(S->Direction()));

  aSweptCurve = MkCurve.Value();
  aExtrusionAxis = MkVector.Value();

  Surf = new StepGeom_SurfaceOfLinearExtrusion;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Surf->Init(name, aSweptCurve, aExtrusionAxis);
  theSurfaceOfLinearExtrusion = Surf;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SurfaceOfLinearExtrusion) &
      GeomToStep_MakeSurfaceOfLinearExtrusion::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSurfaceOfLinearExtrusion;
}
