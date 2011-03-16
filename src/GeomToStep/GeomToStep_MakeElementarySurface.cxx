// File:	GeomToStep_MakeElementarySurface.cxx
// Created:	Tue Jun 22 14:28:02 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeElementarySurface.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <GeomToStep_MakeElementarySurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Plane.hxx>
#include <GeomToStep_MakeCylindricalSurface.hxx>
#include <GeomToStep_MakeConicalSurface.hxx>
#include <GeomToStep_MakeSphericalSurface.hxx>
#include <GeomToStep_MakeToroidalSurface.hxx>
#include <GeomToStep_MakePlane.hxx>

//=============================================================================
// Creation d' une ElementarySurface de prostep a partir d' une 
// ElementarySurface de Geom
//=============================================================================

GeomToStep_MakeElementarySurface::GeomToStep_MakeElementarySurface
  ( const Handle(Geom_ElementarySurface)& S)
{
  done = Standard_True;
  if (S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    Handle(Geom_CylindricalSurface) Sur = 
      Handle(Geom_CylindricalSurface)::DownCast(S);
    GeomToStep_MakeCylindricalSurface MkCylindrical(Sur);
    theElementarySurface = MkCylindrical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
    Handle(Geom_ConicalSurface) Sur = 
      Handle(Geom_ConicalSurface)::DownCast(S);
    GeomToStep_MakeConicalSurface MkConical(Sur);
    theElementarySurface = MkConical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    Handle(Geom_SphericalSurface) Sur = 
      Handle(Geom_SphericalSurface)::DownCast(S);
    GeomToStep_MakeSphericalSurface MkSpherical(Sur);
    theElementarySurface = MkSpherical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
    Handle(Geom_ToroidalSurface) Sur = 
      Handle(Geom_ToroidalSurface)::DownCast(S);
    GeomToStep_MakeToroidalSurface MkToroidal(Sur);
    theElementarySurface = MkToroidal.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_Plane))) {
    Handle(Geom_Plane) Sur =	Handle(Geom_Plane)::DownCast(S); 
    GeomToStep_MakePlane MkPlane(Sur);
    theElementarySurface = MkPlane.Value();
  }
  else
    done = Standard_False;
}	 


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_ElementarySurface) &
      GeomToStep_MakeElementarySurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theElementarySurface;
}
