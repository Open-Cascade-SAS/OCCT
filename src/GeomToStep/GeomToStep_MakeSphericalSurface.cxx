// File:	GeomToStep_MakeSphericalSurface.cxx
// Created:	Thu Jun 17 12:10:20 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeSphericalSurface.ixx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_SphericalSurface.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une conical_surface de prostep a partir d' une SphericalSurface
// de Geom
//=============================================================================

GeomToStep_MakeSphericalSurface::GeomToStep_MakeSphericalSurface
  ( const Handle(Geom_SphericalSurface)& S )
	
{
  Handle(StepGeom_SphericalSurface) Surf;
  Handle(StepGeom_Axis2Placement3d) aPosition;
  Standard_Real aRadius;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(S->Position());
  aPosition = MkAxis2.Value();
  aRadius = S->Radius();
  Surf = new StepGeom_SphericalSurface;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Surf->Init(name, aPosition, aRadius/UnitsMethods::LengthFactor());
  theSphericalSurface = Surf;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SphericalSurface) &
      GeomToStep_MakeSphericalSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSphericalSurface;
}
