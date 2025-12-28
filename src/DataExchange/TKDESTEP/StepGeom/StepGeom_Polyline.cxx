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

#include <StepGeom_Polyline.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Polyline, StepGeom_BoundedCurve)

StepGeom_Polyline::StepGeom_Polyline() {}

void StepGeom_Polyline::Init(
  const occ::handle<TCollection_HAsciiString>&                                  aName,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aPoints)
{
  // --- classe own fields ---
  points = aPoints;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Polyline::SetPoints(
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aPoints)
{
  points = aPoints;
}

occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> StepGeom_Polyline::Points()
  const
{
  return points;
}

occ::handle<StepGeom_CartesianPoint> StepGeom_Polyline::PointsValue(const int num) const
{
  return points->Value(num);
}

int StepGeom_Polyline::NbPoints() const
{
  if (points.IsNull())
    return 0;
  return points->Length();
}
