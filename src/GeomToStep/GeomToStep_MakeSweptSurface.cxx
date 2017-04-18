// Created on: 1993-06-22
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <GeomToStep_MakeSurfaceOfLinearExtrusion.hxx>
#include <GeomToStep_MakeSurfaceOfRevolution.hxx>
#include <GeomToStep_MakeSweptSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <StepGeom_SweptSurface.hxx>
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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeSweptSurface::Value() - no result");
  return theSweptSurface;
}
