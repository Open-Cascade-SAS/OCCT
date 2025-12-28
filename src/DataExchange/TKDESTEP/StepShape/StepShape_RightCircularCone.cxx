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

#include <StepGeom_Axis1Placement.hxx>
#include <StepShape_RightCircularCone.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_RightCircularCone, StepGeom_GeometricRepresentationItem)

StepShape_RightCircularCone::StepShape_RightCircularCone() {}

void StepShape_RightCircularCone::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                       const occ::handle<StepGeom_Axis1Placement>&  aPosition,
                                       const double                                 aHeight,
                                       const double                                 aRadius,
                                       const double                                 aSemiAngle)
{
  // --- classe own fields ---
  position  = aPosition;
  height    = aHeight;
  radius    = aRadius;
  semiAngle = aSemiAngle;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_RightCircularCone::SetPosition(const occ::handle<StepGeom_Axis1Placement>& aPosition)
{
  position = aPosition;
}

occ::handle<StepGeom_Axis1Placement> StepShape_RightCircularCone::Position() const
{
  return position;
}

void StepShape_RightCircularCone::SetHeight(const double aHeight)
{
  height = aHeight;
}

double StepShape_RightCircularCone::Height() const
{
  return height;
}

void StepShape_RightCircularCone::SetRadius(const double aRadius)
{
  radius = aRadius;
}

double StepShape_RightCircularCone::Radius() const
{
  return radius;
}

void StepShape_RightCircularCone::SetSemiAngle(const double aSemiAngle)
{
  semiAngle = aSemiAngle;
}

double StepShape_RightCircularCone::SemiAngle() const
{
  return semiAngle;
}
