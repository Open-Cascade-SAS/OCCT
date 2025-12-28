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

#include <StepShape_PolyLoop.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_PolyLoop, StepShape_Loop)

StepShape_PolyLoop::StepShape_PolyLoop() {}

void StepShape_PolyLoop::Init(const occ::handle<TCollection_HAsciiString>&         aName,
                              const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aPolygon)
{
  // --- classe own fields ---
  polygon = aPolygon;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_PolyLoop::SetPolygon(const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aPolygon)
{
  polygon = aPolygon;
}

occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> StepShape_PolyLoop::Polygon() const
{
  return polygon;
}

occ::handle<StepGeom_CartesianPoint> StepShape_PolyLoop::PolygonValue(const int num) const
{
  return polygon->Value(num);
}

int StepShape_PolyLoop::NbPolygon() const
{
  if (polygon.IsNull())
    return 0;
  return polygon->Length();
}
