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

#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_CartesianTransformationOperator3d,
                           StepGeom_CartesianTransformationOperator)

StepGeom_CartesianTransformationOperator3d::StepGeom_CartesianTransformationOperator3d() = default;

void StepGeom_CartesianTransformationOperator3d::Init(
  const occ::handle<TCollection_HAsciiString>& aName,
  const bool                                   hasAaxis1,
  const occ::handle<StepGeom_Direction>&       aAxis1,
  const bool                                   hasAaxis2,
  const occ::handle<StepGeom_Direction>&       aAxis2,
  const occ::handle<StepGeom_CartesianPoint>&  aLocalOrigin,
  const bool                                   hasAscale,
  const double                                 aScale,
  const bool                                   hasAaxis3,
  const occ::handle<StepGeom_Direction>&       aAxis3)
{
  // --- classe own fields ---
  hasAxis3 = hasAaxis3;
  axis3    = aAxis3;
  // --- classe inherited fields ---
  StepGeom_CartesianTransformationOperator::Init(aName,
                                                 hasAaxis1,
                                                 aAxis1,
                                                 hasAaxis2,
                                                 aAxis2,
                                                 aLocalOrigin,
                                                 hasAscale,
                                                 aScale);
}

void StepGeom_CartesianTransformationOperator3d::SetAxis3(
  const occ::handle<StepGeom_Direction>& aAxis3)
{
  axis3    = aAxis3;
  hasAxis3 = true;
}

void StepGeom_CartesianTransformationOperator3d::UnSetAxis3()
{
  hasAxis3 = false;
  axis3.Nullify();
}

occ::handle<StepGeom_Direction> StepGeom_CartesianTransformationOperator3d::Axis3() const
{
  return axis3;
}

bool StepGeom_CartesianTransformationOperator3d::HasAxis3() const
{
  return hasAxis3;
}
