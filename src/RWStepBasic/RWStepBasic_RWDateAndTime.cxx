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


#include <RWStepBasic_RWDateAndTime.ixx>
#include <StepBasic_Date.hxx>
#include <StepBasic_LocalTime.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_DateAndTime.hxx>


RWStepBasic_RWDateAndTime::RWStepBasic_RWDateAndTime () {}

void RWStepBasic_RWDateAndTime::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DateAndTime)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"date_and_time")) return;

	// --- own field : dateComponent ---

	Handle(StepBasic_Date) aDateComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"date_component", ach, STANDARD_TYPE(StepBasic_Date), aDateComponent);

	// --- own field : timeComponent ---

	Handle(StepBasic_LocalTime) aTimeComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"time_component", ach, STANDARD_TYPE(StepBasic_LocalTime), aTimeComponent);

	//--- Initialisation of the read entity ---


	ent->Init(aDateComponent, aTimeComponent);
}


void RWStepBasic_RWDateAndTime::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DateAndTime)& ent) const
{

	// --- own field : dateComponent ---

	SW.Send(ent->DateComponent());

	// --- own field : timeComponent ---

	SW.Send(ent->TimeComponent());
}


void RWStepBasic_RWDateAndTime::Share(const Handle(StepBasic_DateAndTime)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->DateComponent());


	iter.GetOneItem(ent->TimeComponent());
}

