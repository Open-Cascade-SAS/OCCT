// Created on: 1993-06-17
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


#include <Geom_SphericalSurface.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeSphericalSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeSphericalSurface::Value() - no result");
  return theSphericalSurface;
}
