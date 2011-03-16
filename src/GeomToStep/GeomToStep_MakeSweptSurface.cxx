// File:	GeomToStep_MakeSweptSurface.cxx
// Created:	Tue Jun 22 14:42:00 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeSweptSurface.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_SweptSurface.hxx>
#include <GeomToStep_MakeSweptSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomToStep_MakeSurfaceOfLinearExtrusion.hxx>
#include <GeomToStep_MakeSurfaceOfRevolution.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une SweptSurface de prostep a partir d' une 
// SweptSurface de Geom
//=============================================================================

GeomToStep_MakeSweptSurface::GeomToStep_MakeSweptSurface
  ( const Handle(Geom_SweptSurface)& S)
{
  done = Standard_True;
  if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) Sur = 
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S);
    GeomToStep_MakeSurfaceOfLinearExtrusion MkLinear(Sur);
    theSweptSurface = MkLinear.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) Sur = 
      Handle(Geom_SurfaceOfRevolution)::DownCast(S);
    GeomToStep_MakeSurfaceOfRevolution MkRevol(Sur);
    theSweptSurface = MkRevol.Value();
  }
  else
    done = Standard_False;
}	 


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SweptSurface) &
      GeomToStep_MakeSweptSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSweptSurface;
}
