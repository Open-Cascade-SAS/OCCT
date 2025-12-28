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

#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_CompositeCurve, StepGeom_BoundedCurve)

StepGeom_CompositeCurve::StepGeom_CompositeCurve() {}

void StepGeom_CompositeCurve::Init(
  const occ::handle<TCollection_HAsciiString>&                                         aName,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CompositeCurveSegment>>>& aSegments,
  const StepData_Logical aSelfIntersect)
{
  // --- classe own fields ---
  segments      = aSegments;
  selfIntersect = aSelfIntersect;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CompositeCurve::SetSegments(
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CompositeCurveSegment>>>& aSegments)
{
  segments = aSegments;
}

occ::handle<NCollection_HArray1<occ::handle<StepGeom_CompositeCurveSegment>>>
  StepGeom_CompositeCurve::Segments() const
{
  return segments;
}

occ::handle<StepGeom_CompositeCurveSegment> StepGeom_CompositeCurve::SegmentsValue(
  const int num) const
{
  return segments->Value(num);
}

int StepGeom_CompositeCurve::NbSegments() const
{
  if (segments.IsNull())
    return 0;
  return segments->Length();
}

void StepGeom_CompositeCurve::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
  selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_CompositeCurve::SelfIntersect() const
{
  return selfIntersect;
}
