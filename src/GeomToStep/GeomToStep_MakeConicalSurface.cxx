// File:	GeomToStep_MakeConicalSurface.cxx
// Created:	Wed Jun 16 18:38:56 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

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
  if (aSemiAngle < 0. || aSemiAngle > PI/2.) {
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
