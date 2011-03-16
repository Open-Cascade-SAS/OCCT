// File:	GeomToStep_MakeSurfaceOfRevolution.cxx
// Created:	Thu Jun 17 12:45:12 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeSurfaceOfRevolution.ixx>
#include <GeomToStep_MakeAxis1Placement.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <gp_Ax1.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une surface_of_revolution de prostep a partir d' une
// SurfaceOfRevolution de Geom
//=============================================================================

GeomToStep_MakeSurfaceOfRevolution::GeomToStep_MakeSurfaceOfRevolution
  ( const Handle(Geom_SurfaceOfRevolution)& S )
	
{
  Handle(StepGeom_SurfaceOfRevolution) Surf;
  Handle(StepGeom_Curve) aSweptCurve;
  Handle(StepGeom_Axis1Placement) aAxisPosition;
  
  GeomToStep_MakeCurve MkSwept(S->BasisCurve());
  GeomToStep_MakeAxis1Placement MkAxis1(S->Axis());
  aSweptCurve = MkSwept.Value();
  aAxisPosition = MkAxis1.Value();
  Surf = new StepGeom_SurfaceOfRevolution;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Surf->Init(name, aSweptCurve, aAxisPosition);
  theSurfaceOfRevolution = Surf;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SurfaceOfRevolution) &
      GeomToStep_MakeSurfaceOfRevolution::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSurfaceOfRevolution;
}
