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


#include <Standard_Type.hxx>
#include <StepAP214_AppliedApprovalAssignment.hxx>
#include <StepAP214_ApprovalItem.hxx>
#include <StepBasic_Approval.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AppliedApprovalAssignment,StepBasic_ApprovalAssignment)

StepAP214_AppliedApprovalAssignment::StepAP214_AppliedApprovalAssignment ()  {}

void StepAP214_AppliedApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval,
	const Handle(StepAP214_HArray1OfApprovalItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}

void StepAP214_AppliedApprovalAssignment::SetItems(const Handle(StepAP214_HArray1OfApprovalItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfApprovalItem) StepAP214_AppliedApprovalAssignment::Items() const
{
	return items;
}

StepAP214_ApprovalItem StepAP214_AppliedApprovalAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AppliedApprovalAssignment::NbItems () const
{
	return items->Length();
}
