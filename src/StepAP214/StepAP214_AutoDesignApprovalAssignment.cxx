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
#include <StepAP214_AutoDesignApprovalAssignment.hxx>
#include <StepAP214_AutoDesignGeneralOrgItem.hxx>
#include <StepBasic_Approval.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepAP214_AutoDesignApprovalAssignment,StepBasic_ApprovalAssignment)

StepAP214_AutoDesignApprovalAssignment::StepAP214_AutoDesignApprovalAssignment ()  {}

void StepAP214_AutoDesignApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval,
	const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	// --- classe own fields ---
	items = aItems;
	// --- classe inherited fields ---
	StepBasic_ApprovalAssignment::Init(aAssignedApproval);
}

void StepAP214_AutoDesignApprovalAssignment::SetItems(const Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem)& aItems)
{
	items = aItems;
}

Handle(StepAP214_HArray1OfAutoDesignGeneralOrgItem) StepAP214_AutoDesignApprovalAssignment::Items() const
{
	return items;
}

StepAP214_AutoDesignGeneralOrgItem StepAP214_AutoDesignApprovalAssignment::ItemsValue(const Standard_Integer num) const
{
	return items->Value(num);
}

Standard_Integer StepAP214_AutoDesignApprovalAssignment::NbItems () const
{
	return items->Length();
}
