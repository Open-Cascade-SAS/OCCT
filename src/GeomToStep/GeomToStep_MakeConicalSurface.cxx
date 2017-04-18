// Created on: 1993-06-16
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


#include <Geom_ConicalSurface.hxx>
#include <GeomToStep_MakeAxis2Placement3d.hxx>
#include <GeomToStep_MakeConicalSurface.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <TCollection_HAsciiString.hxx>
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
  if (aSemiAngle < 0. || aSemiAngle > M_PI/2.) {
    throw Standard_DomainError("Conicalsurface not STEP conformant");
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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeConicalSurface::Value() - no result");
  return theConicalSurface;
}
