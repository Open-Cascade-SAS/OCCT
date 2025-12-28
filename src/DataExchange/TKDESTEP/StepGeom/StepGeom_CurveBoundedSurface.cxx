// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_CurveBoundedSurface, StepGeom_BoundedSurface)

//=================================================================================================

StepGeom_CurveBoundedSurface::StepGeom_CurveBoundedSurface() {}

//=================================================================================================

void StepGeom_CurveBoundedSurface::Init(
  const occ::handle<TCollection_HAsciiString>&                      aRepresentationItem_Name,
  const occ::handle<StepGeom_Surface>&                              aBasisSurface,
  const occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>>& aBoundaries,
  const bool                                                        aImplicitOuter)
{
  StepGeom_BoundedSurface::Init(aRepresentationItem_Name);

  theBasisSurface = aBasisSurface;

  theBoundaries = aBoundaries;

  theImplicitOuter = aImplicitOuter;
}

//=================================================================================================

occ::handle<StepGeom_Surface> StepGeom_CurveBoundedSurface::BasisSurface() const
{
  return theBasisSurface;
}

//=================================================================================================

void StepGeom_CurveBoundedSurface::SetBasisSurface(
  const occ::handle<StepGeom_Surface>& aBasisSurface)
{
  theBasisSurface = aBasisSurface;
}

//=================================================================================================

occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>> StepGeom_CurveBoundedSurface::
  Boundaries() const
{
  return theBoundaries;
}

//=================================================================================================

void StepGeom_CurveBoundedSurface::SetBoundaries(
  const occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>>& aBoundaries)
{
  theBoundaries = aBoundaries;
}

//=================================================================================================

bool StepGeom_CurveBoundedSurface::ImplicitOuter() const
{
  return theImplicitOuter;
}

//=================================================================================================

void StepGeom_CurveBoundedSurface::SetImplicitOuter(const bool aImplicitOuter)
{
  theImplicitOuter = aImplicitOuter;
}
