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

#include <StepAP214_AutoDesignActualDateAssignment.hxx>
#include <StepAP214_AutoDesignDatedItem.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_DateRole.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignActualDateAssignment, StepBasic_DateAssignment)

StepAP214_AutoDesignActualDateAssignment::StepAP214_AutoDesignActualDateAssignment() = default;

void StepAP214_AutoDesignActualDateAssignment::Init(
  const occ::handle<StepBasic_Date>&                                     aAssignedDate,
  const occ::handle<StepBasic_DateRole>&                                 aRole,
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>>& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_DateAssignment::Init(aAssignedDate, aRole);
}

void StepAP214_AutoDesignActualDateAssignment::SetItems(
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>>& aItems)
{
  items = aItems;
}

occ::handle<NCollection_HArray1<StepAP214_AutoDesignDatedItem>>
  StepAP214_AutoDesignActualDateAssignment::Items() const
{
  return items;
}

StepAP214_AutoDesignDatedItem StepAP214_AutoDesignActualDateAssignment::ItemsValue(
  const int num) const
{
  return items->Value(num);
}

int StepAP214_AutoDesignActualDateAssignment::NbItems() const
{
  return items->Length();
}
