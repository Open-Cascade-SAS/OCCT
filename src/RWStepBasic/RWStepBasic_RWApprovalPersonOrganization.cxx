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


#include <RWStepBasic_RWApprovalPersonOrganization.ixx>
#include <StepBasic_PersonOrganizationSelect.hxx>
#include <StepBasic_Approval.hxx>
#include <StepBasic_ApprovalRole.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ApprovalPersonOrganization.hxx>


RWStepBasic_RWApprovalPersonOrganization::RWStepBasic_RWApprovalPersonOrganization () {}

void RWStepBasic_RWApprovalPersonOrganization::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApprovalPersonOrganization)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"approval_person_organization")) return;

	// --- own field : personOrganization ---

	StepBasic_PersonOrganizationSelect aPersonOrganization;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num,1,"person_organization",ach,aPersonOrganization);

	// --- own field : authorizedApproval ---

	Handle(StepBasic_Approval) aAuthorizedApproval;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"authorized_approval", ach, STANDARD_TYPE(StepBasic_Approval), aAuthorizedApproval);

	// --- own field : role ---

	Handle(StepBasic_ApprovalRole) aRole;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"role", ach, STANDARD_TYPE(StepBasic_ApprovalRole), aRole);

	//--- Initialisation of the read entity ---


	ent->Init(aPersonOrganization, aAuthorizedApproval, aRole);
}


void RWStepBasic_RWApprovalPersonOrganization::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApprovalPersonOrganization)& ent) const
{

	// --- own field : personOrganization ---

	SW.Send(ent->PersonOrganization().Value());

	// --- own field : authorizedApproval ---

	SW.Send(ent->AuthorizedApproval());

	// --- own field : role ---

	SW.Send(ent->Role());
}


void RWStepBasic_RWApprovalPersonOrganization::Share(const Handle(StepBasic_ApprovalPersonOrganization)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->PersonOrganization().Value());


	iter.GetOneItem(ent->AuthorizedApproval());


	iter.GetOneItem(ent->Role());
}

