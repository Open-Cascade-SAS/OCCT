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
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalPersonOrganization.hxx>
#include <StepBasic_ApprovalRole.hxx>
#include <StepBasic_PersonOrganizationSelect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ApprovalPersonOrganization,Standard_Transient)

StepBasic_ApprovalPersonOrganization::StepBasic_ApprovalPersonOrganization ()  {}

void StepBasic_ApprovalPersonOrganization::Init(
	const StepBasic_PersonOrganizationSelect& aPersonOrganization,
	const Handle(StepBasic_Approval)& aAuthorizedApproval,
	const Handle(StepBasic_ApprovalRole)& aRole)
{
	// --- classe own fields ---
	personOrganization = aPersonOrganization;
	authorizedApproval = aAuthorizedApproval;
	role = aRole;
}


void StepBasic_ApprovalPersonOrganization::SetPersonOrganization(const StepBasic_PersonOrganizationSelect& aPersonOrganization)
{
	personOrganization = aPersonOrganization;
}

StepBasic_PersonOrganizationSelect StepBasic_ApprovalPersonOrganization::PersonOrganization() const
{
	return personOrganization;
}

void StepBasic_ApprovalPersonOrganization::SetAuthorizedApproval(const Handle(StepBasic_Approval)& aAuthorizedApproval)
{
	authorizedApproval = aAuthorizedApproval;
}

Handle(StepBasic_Approval) StepBasic_ApprovalPersonOrganization::AuthorizedApproval() const
{
	return authorizedApproval;
}

void StepBasic_ApprovalPersonOrganization::SetRole(const Handle(StepBasic_ApprovalRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_ApprovalRole) StepBasic_ApprovalPersonOrganization::Role() const
{
	return role;
}
