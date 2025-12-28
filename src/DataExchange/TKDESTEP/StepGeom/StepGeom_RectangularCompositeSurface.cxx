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

#include <StepGeom_RectangularCompositeSurface.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_RectangularCompositeSurface, StepGeom_BoundedSurface)

StepGeom_RectangularCompositeSurface::StepGeom_RectangularCompositeSurface() = default;

void StepGeom_RectangularCompositeSurface::Init(
  const occ::handle<TCollection_HAsciiString>&                                aName,
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_SurfacePatch>>>& aSegments)
{
  // --- classe own fields ---
  segments = aSegments;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_RectangularCompositeSurface::SetSegments(
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_SurfacePatch>>>& aSegments)
{
  segments = aSegments;
}

occ::handle<NCollection_HArray2<occ::handle<StepGeom_SurfacePatch>>>
  StepGeom_RectangularCompositeSurface::Segments() const
{
  return segments;
}

occ::handle<StepGeom_SurfacePatch> StepGeom_RectangularCompositeSurface::SegmentsValue(
  const int num1,
  const int num2) const
{
  return segments->Value(num1, num2);
}

int StepGeom_RectangularCompositeSurface::NbSegmentsI() const
{
  if (segments.IsNull())
    return 0;
  return segments->UpperRow() - segments->LowerRow() + 1;
}

int StepGeom_RectangularCompositeSurface::NbSegmentsJ() const
{
  if (segments.IsNull())
    return 0;
  return segments->UpperCol() - segments->LowerCol() + 1;
}
