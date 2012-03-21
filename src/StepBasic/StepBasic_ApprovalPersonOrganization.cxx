// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepBasic_ApprovalPersonOrganization.ixx>


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
