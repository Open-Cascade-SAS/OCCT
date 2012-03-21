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


#include <RWStepBasic_RWPersonAndOrganization.ixx>
#include <StepBasic_Person.hxx>
#include <StepBasic_Organization.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_PersonAndOrganization.hxx>


RWStepBasic_RWPersonAndOrganization::RWStepBasic_RWPersonAndOrganization () {}

void RWStepBasic_RWPersonAndOrganization::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_PersonAndOrganization)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"person_and_organization")) return;

	// --- own field : thePerson ---

	Handle(StepBasic_Person) aThePerson;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"the_person", ach, STANDARD_TYPE(StepBasic_Person), aThePerson);

	// --- own field : theOrganization ---

	Handle(StepBasic_Organization) aTheOrganization;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 2,"the_organization", ach, STANDARD_TYPE(StepBasic_Organization), aTheOrganization);

	//--- Initialisation of the read entity ---


	ent->Init(aThePerson, aTheOrganization);
}


void RWStepBasic_RWPersonAndOrganization::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_PersonAndOrganization)& ent) const
{

	// --- own field : thePerson ---

	SW.Send(ent->ThePerson());

	// --- own field : theOrganization ---

	SW.Send(ent->TheOrganization());
}


void RWStepBasic_RWPersonAndOrganization::Share(const Handle(StepBasic_PersonAndOrganization)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ThePerson());


	iter.GetOneItem(ent->TheOrganization());
}

