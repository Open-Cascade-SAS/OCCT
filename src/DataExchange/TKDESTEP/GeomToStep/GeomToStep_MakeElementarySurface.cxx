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
#include <StepData_Factors.hxx>
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
GeomToStep_MakeElementarySurface::GeomToStep_MakeElementarySurface(
  const occ::handle<Geom_ElementarySurface>& S,
  const StepData_Factors&                    theLocalFactors)
{
  done = true;
  if (S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
  {
    occ::handle<Geom_CylindricalSurface> Sur = occ::down_cast<Geom_CylindricalSurface>(S);
    GeomToStep_MakeCylindricalSurface    MkCylindrical(Sur, theLocalFactors);
    theElementarySurface = MkCylindrical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
  {
    occ::handle<Geom_ConicalSurface> Sur = occ::down_cast<Geom_ConicalSurface>(S);
    GeomToStep_MakeConicalSurface    MkConical(Sur, theLocalFactors);
    theElementarySurface = MkConical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    occ::handle<Geom_SphericalSurface> Sur = occ::down_cast<Geom_SphericalSurface>(S);
    GeomToStep_MakeSphericalSurface    MkSpherical(Sur, theLocalFactors);
    theElementarySurface = MkSpherical.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
  {
    occ::handle<Geom_ToroidalSurface> Sur = occ::down_cast<Geom_ToroidalSurface>(S);
    GeomToStep_MakeToroidalSurface    MkToroidal(Sur, theLocalFactors);
    theElementarySurface = MkToroidal.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_Plane)))
  {
    occ::handle<Geom_Plane> Sur = occ::down_cast<Geom_Plane>(S);
    GeomToStep_MakePlane    MkPlane(Sur, theLocalFactors);
    theElementarySurface = MkPlane.Value();
  }
  else
    done = false;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_ElementarySurface>& GeomToStep_MakeElementarySurface::Value() const
{
  StdFail_NotDone_Raise_if(!done, "GeomToStep_MakeElementarySurface::Value() - no result");
  return theElementarySurface;
}
