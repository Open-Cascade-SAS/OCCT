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

#include <Standard_Type.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_SurfacePatch.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_SurfacePatch, Standard_Transient)

StepGeom_SurfacePatch::StepGeom_SurfacePatch() {}

void StepGeom_SurfacePatch::Init(const occ::handle<StepGeom_BoundedSurface>& aParentSurface,
                                 const StepGeom_TransitionCode          aUTransition,
                                 const StepGeom_TransitionCode          aVTransition,
                                 const bool                 aUSense,
                                 const bool                 aVSense)
{
  // --- classe own fields ---
  parentSurface = aParentSurface;
  uTransition   = aUTransition;
  vTransition   = aVTransition;
  uSense        = aUSense;
  vSense        = aVSense;
}

void StepGeom_SurfacePatch::SetParentSurface(const occ::handle<StepGeom_BoundedSurface>& aParentSurface)
{
  parentSurface = aParentSurface;
}

occ::handle<StepGeom_BoundedSurface> StepGeom_SurfacePatch::ParentSurface() const
{
  return parentSurface;
}

void StepGeom_SurfacePatch::SetUTransition(const StepGeom_TransitionCode aUTransition)
{
  uTransition = aUTransition;
}

StepGeom_TransitionCode StepGeom_SurfacePatch::UTransition() const
{
  return uTransition;
}

void StepGeom_SurfacePatch::SetVTransition(const StepGeom_TransitionCode aVTransition)
{
  vTransition = aVTransition;
}

StepGeom_TransitionCode StepGeom_SurfacePatch::VTransition() const
{
  return vTransition;
}

void StepGeom_SurfacePatch::SetUSense(const bool aUSense)
{
  uSense = aUSense;
}

bool StepGeom_SurfacePatch::USense() const
{
  return uSense;
}

void StepGeom_SurfacePatch::SetVSense(const bool aVSense)
{
  vSense = aVSense;
}

bool StepGeom_SurfacePatch::VSense() const
{
  return vSense;
}
