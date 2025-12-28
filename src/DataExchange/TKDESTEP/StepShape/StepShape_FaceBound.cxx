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

#include <StepShape_FaceBound.hxx>
#include <StepShape_Loop.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_FaceBound, StepShape_TopologicalRepresentationItem)

StepShape_FaceBound::StepShape_FaceBound() {}

void StepShape_FaceBound::Init(const occ::handle<TCollection_HAsciiString>& aName,
                               const occ::handle<StepShape_Loop>&           aBound,
                               const bool                                   aOrientation)
{
  // --- classe own fields ---
  bound       = aBound;
  orientation = aOrientation;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_FaceBound::SetBound(const occ::handle<StepShape_Loop>& aBound)
{
  bound = aBound;
}

occ::handle<StepShape_Loop> StepShape_FaceBound::Bound() const
{
  return bound;
}

void StepShape_FaceBound::SetOrientation(const bool aOrientation)
{
  orientation = aOrientation;
}

bool StepShape_FaceBound::Orientation() const
{
  return orientation;
}
