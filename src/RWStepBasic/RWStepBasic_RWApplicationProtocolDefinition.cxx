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


#include <RWStepBasic_RWApplicationProtocolDefinition.ixx>
#include <StepBasic_ApplicationContext.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ApplicationProtocolDefinition.hxx>


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

