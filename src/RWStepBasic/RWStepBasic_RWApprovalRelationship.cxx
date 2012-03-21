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

//gka 05.03.99 S4134 upgrade from CD to DIS
#include <RWStepBasic_RWApprovalRelationship.ixx>
#include <StepBasic_Approval.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ApprovalRelationship.hxx>


RWStepBasic_RWApprovalRelationship::RWStepBasic_RWApprovalRelationship () {}

void RWStepBasic_RWApprovalRelationship::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApprovalRelationship)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"approval_relationship")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescription);
	}
	// --- own field : relatingApproval ---

	Handle(StepBasic_Approval) aRelatingApproval;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"relating_approval", ach, STANDARD_TYPE(StepBasic_Approval), aRelatingApproval);

	// --- own field : relatedApproval ---

	Handle(StepBasic_Approval) aRelatedApproval;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"related_approval", ach, STANDARD_TYPE(StepBasic_Approval), aRelatedApproval);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescription, aRelatingApproval, aRelatedApproval);
}


void RWStepBasic_RWApprovalRelationship::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApprovalRelationship)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : relatingApproval ---

	SW.Send(ent->RelatingApproval());

	// --- own field : relatedApproval ---

	SW.Send(ent->RelatedApproval());
}


void RWStepBasic_RWApprovalRelationship::Share(const Handle(StepBasic_ApprovalRelationship)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->RelatingApproval());


	iter.GetOneItem(ent->RelatedApproval());
}

