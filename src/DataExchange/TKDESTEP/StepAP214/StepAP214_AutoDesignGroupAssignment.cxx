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

#include <StepAP214_AutoDesignGroupAssignment.hxx>
#include <StepAP214_AutoDesignGroupedItem.hxx>
#include <StepBasic_Group.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignGroupAssignment, StepBasic_GroupAssignment)

StepAP214_AutoDesignGroupAssignment::StepAP214_AutoDesignGroupAssignment() {}

void StepAP214_AutoDesignGroupAssignment::Init(
  const occ::handle<StepBasic_Group>&                                      aAssignedGroup,
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignGroupedItem>>& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_GroupAssignment::Init(aAssignedGroup);
}

void StepAP214_AutoDesignGroupAssignment::SetItems(
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignGroupedItem>>& aItems)
{
  items = aItems;
}

occ::handle<NCollection_HArray1<StepAP214_AutoDesignGroupedItem>>
  StepAP214_AutoDesignGroupAssignment::Items() const
{
  return items;
}

StepAP214_AutoDesignGroupedItem StepAP214_AutoDesignGroupAssignment::ItemsValue(const int num) const
{
  return items->Value(num);
}

int StepAP214_AutoDesignGroupAssignment::NbItems() const
{
  return items->Length();
}
