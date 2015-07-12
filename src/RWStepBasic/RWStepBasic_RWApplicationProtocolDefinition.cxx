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
#include <RWStepBasic_RWApplicationProtocolDefinition.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWApplicationProtocolDefinition::RWStepBasic_RWApplicationProtocolDefinition () {}

void RWStepBasic_RWApplicationProtocolDefinition::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApplicationProtocolDefinition)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"application_protocol_definition")) return;

	// --- own field : status ---

	Handle(TCollection_HAsciiString) aStatus;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"status",ach,aStatus);

	// --- own field : applicationInterpretedModelSchemaName ---

	Handle(TCollection_HAsciiString) aApplicationInterpretedModelSchemaName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"application_interpreted_model_schema_name",ach,aApplicationInterpretedModelSchemaName);

	// --- own field : applicationProtocolYear ---

	Standard_Integer aApplicationProtocolYear;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadInteger (num,3,"application_protocol_year",ach,aApplicationProtocolYear);

	// --- own field : application ---

	Handle(StepBasic_ApplicationContext) aApplication;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"application", ach, STANDARD_TYPE(StepBasic_ApplicationContext), aApplication);

	//--- Initialisation of the read entity ---


	ent->Init(aStatus, aApplicationInterpretedModelSchemaName, aApplicationProtocolYear, aApplication);
}


void RWStepBasic_RWApplicationProtocolDefinition::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApplicationProtocolDefinition)& ent) const
{

	// --- own field : status ---

	SW.Send(ent->Status());

	// --- own field : applicationInterpretedModelSchemaName ---

	SW.Send(ent->ApplicationInterpretedModelSchemaName());

	// --- own field : applicationProtocolYear ---

	SW.Send(ent->ApplicationProtocolYear());

	// --- own field : application ---

	SW.Send(ent->Application());
}


void RWStepBasic_RWApplicationProtocolDefinition::Share(const Handle(StepBasic_ApplicationProtocolDefinition)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Application());
}

