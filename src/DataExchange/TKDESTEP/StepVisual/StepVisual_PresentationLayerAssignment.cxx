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

#include <StepVisual_PresentationLayerAssignment.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_PresentationLayerAssignment, Standard_Transient)

StepVisual_PresentationLayerAssignment::StepVisual_PresentationLayerAssignment() = default;

void StepVisual_PresentationLayerAssignment::Init(
  const occ::handle<TCollection_HAsciiString>&                    aName,
  const occ::handle<TCollection_HAsciiString>&                    aDescription,
  const occ::handle<NCollection_HArray1<StepVisual_LayeredItem>>& aAssignedItems)
{
  // --- classe own fields ---
  name          = aName;
  description   = aDescription;
  assignedItems = aAssignedItems;
}

void StepVisual_PresentationLayerAssignment::SetName(
  const occ::handle<TCollection_HAsciiString>& aName)
{
  name = aName;
}

occ::handle<TCollection_HAsciiString> StepVisual_PresentationLayerAssignment::Name() const
{
  return name;
}

void StepVisual_PresentationLayerAssignment::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  description = aDescription;
}

occ::handle<TCollection_HAsciiString> StepVisual_PresentationLayerAssignment::Description() const
{
  return description;
}

void StepVisual_PresentationLayerAssignment::SetAssignedItems(
  const occ::handle<NCollection_HArray1<StepVisual_LayeredItem>>& aAssignedItems)
{
  assignedItems = aAssignedItems;
}

occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> StepVisual_PresentationLayerAssignment::
  AssignedItems() const
{
  return assignedItems;
}

StepVisual_LayeredItem StepVisual_PresentationLayerAssignment::AssignedItemsValue(
  const int num) const
{
  return assignedItems->Value(num);
}

int StepVisual_PresentationLayerAssignment::NbAssignedItems() const
{
  return assignedItems.IsNull() ? 0 : assignedItems->Length();
}
