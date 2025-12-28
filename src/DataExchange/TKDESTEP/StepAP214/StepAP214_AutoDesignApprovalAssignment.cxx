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

#include <StepAP214_AutoDesignApprovalAssignment.hxx>
#include <StepAP214_AutoDesignGeneralOrgItem.hxx>
#include <StepBasic_Approval.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignApprovalAssignment, StepBasic_ApprovalAssignment)

StepAP214_AutoDesignApprovalAssignment::StepAP214_AutoDesignApprovalAssignment() {}

void StepAP214_AutoDesignApprovalAssignment::Init(
  const occ::handle<StepBasic_Approval>&                                      aAssignedApproval,
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignGeneralOrgItem>>& aItems)
{
  // --- classe own fields ---
  items = aItems;
  // --- classe inherited fields ---
  StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}

void StepAP214_AutoDesignApprovalAssignment::SetItems(
  const occ::handle<NCollection_HArray1<StepAP214_AutoDesignGeneralOrgItem>>& aItems)
{
  items = aItems;
}

occ::handle<NCollection_HArray1<StepAP214_AutoDesignGeneralOrgItem>>
  StepAP214_AutoDesignApprovalAssignment::Items() const
{
  return items;
}

StepAP214_AutoDesignGeneralOrgItem StepAP214_AutoDesignApprovalAssignment::ItemsValue(
  const int num) const
{
  return items->Value(num);
}

int StepAP214_AutoDesignApprovalAssignment::NbItems() const
{
  return items->Length();
}
