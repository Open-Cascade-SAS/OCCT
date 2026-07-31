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

#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_SphericalSurface, StepGeom_ElementarySurface)

StepGeom_SphericalSurface::StepGeom_SphericalSurface() = default;

void StepGeom_SphericalSurface::Init(const occ::handle<TCollection_HAsciiString>&  aName,
                                     const occ::handle<StepGeom_Axis2Placement3d>& aPosition,
                                     const double                                  aRadius)
{
  // --- classe own fields ---
  radius = aRadius;
  // --- classe inherited fields ---
  StepGeom_ElementarySurface::Init(aName, aPosition);
}

void StepGeom_SphericalSurface::SetRadius(const double aRadius)
{
  radius = aRadius;
}

double StepGeom_SphericalSurface::Radius() const
{
  return radius;
}
