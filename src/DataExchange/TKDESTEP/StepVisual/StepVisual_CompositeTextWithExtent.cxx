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

#include <StepVisual_CompositeTextWithExtent.hxx>
#include <StepVisual_PlanarExtent.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CompositeTextWithExtent, StepVisual_CompositeText)

StepVisual_CompositeTextWithExtent::StepVisual_CompositeTextWithExtent() = default;

void StepVisual_CompositeTextWithExtent::Init(
  const occ::handle<TCollection_HAsciiString>&                        aName,
  const occ::handle<NCollection_HArray1<StepVisual_TextOrCharacter>>& aCollectedText,
  const occ::handle<StepVisual_PlanarExtent>&                         aExtent)
{
  // --- classe own fields ---
  extent = aExtent;
  // --- classe inherited fields ---
  StepVisual_CompositeText::Init(aName, aCollectedText);
}

void StepVisual_CompositeTextWithExtent::SetExtent(
  const occ::handle<StepVisual_PlanarExtent>& aExtent)
{
  extent = aExtent;
}

occ::handle<StepVisual_PlanarExtent> StepVisual_CompositeTextWithExtent::Extent() const
{
  return extent;
}
