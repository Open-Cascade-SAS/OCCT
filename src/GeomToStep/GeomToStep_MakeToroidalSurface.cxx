// File:	GeomToStep_MakeToroidalSurface.cxx
// Created:	Thu Jun 17 14:47:08 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeToroidalSurface.ixx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une toroidal_surface de prostep a partir d' une ToroidalSurface
// de Geom
//=============================================================================

GeomToStep_MakeToroidalSurface::GeomToStep_MakeToroidalSurface
  ( const Handle(Geom_ToroidalSurface)& S )
	
{
  Handle(StepGeom_ToroidalSurface) Surf;
  Handle(StepGeom_Axis2Placement3d) aPosition;
  Standard_Real aMajorRadius, aMinorRadius;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(S->Position());
  aPosition = MkAxis2.Value();
  aMajorRadius = S->MajorRadius();
  aMinorRadius = S->MinorRadius();
  Surf = new StepGeom_ToroidalSurface;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Standard_Real fact = UnitsMethods::LengthFactor();
  Surf->Init(name, aPosition, aMajorRadius/fact, aMinorRadius/fact);
  theToroidalSurface = Surf;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_ToroidalSurface) &
      GeomToStep_MakeToroidalSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theToroidalSurface;
}
