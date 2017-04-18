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


#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomToStep_MakeConicalSurface.hxx>
#include <GeomToStep_MakeCylindricalSurface.hxx>
#include <GeomToStep_MakeElementarySurface.hxx>
#include <GeomToStep_MakePlane.hxx>
#include <GeomToStep_MakeSphericalSurface.hxx>
#include <GeomToStep_MakeToroidalSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>

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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeElementarySurface::Value() - no result");
  return theElementarySurface;
}
