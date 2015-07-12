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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWPersonAndOrganization.hxx>
#include <StepBasic_Organization.hxx>
#include <StepBasic_Person.hxx>
#include <StepBasic_PersonAndOrganization.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

