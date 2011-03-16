// File:	GeomToStep_MakeCylindricalSurface.cxx
// Created:	Thu Jun 17 10:33:15 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeCylindricalSurface.ixx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <TCollection_HAsciiString.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une conical_surface de prostep a partir d' une 
// CylindricalSurface de Geom
//=============================================================================

GeomToStep_MakeCylindricalSurface::GeomToStep_MakeCylindricalSurface
  ( const Handle(Geom_CylindricalSurface)& CS )
	
{
  Handle(StepGeom_CylindricalSurface) CSstep;
  Handle(StepGeom_Axis2Placement3d) aPosition;
  Standard_Real aRadius;
  
  GeomToStep_MakeAxis2Placement3d MkAxis2(CS->Position());
  aPosition = MkAxis2.Value();
  aRadius = CS->Radius();
  CSstep = new StepGeom_CylindricalSurface;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  CSstep->Init(name, aPosition, aRadius / UnitsMethods::LengthFactor());
  theCylindricalSurface = CSstep;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_CylindricalSurface) &
      GeomToStep_MakeCylindricalSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theCylindricalSurface;
}
