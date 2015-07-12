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
#include <RWStepBasic_RWDateAndTime.hxx>
#include <StepBasic_Date.hxx>
#include <StepBasic_DateAndTime.hxx>
#include <StepBasic_LocalTime.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

