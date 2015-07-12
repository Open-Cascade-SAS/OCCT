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
#include <RWStepBasic_RWProductDefinitionContext.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWProductDefinitionContext::RWStepBasic_RWProductDefinitionContext () {}

void RWStepBasic_RWProductDefinitionContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductDefinitionContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"product_definition_context")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : frameOfReference ---

	Handle(StepBasic_ApplicationContext) aFrameOfReference;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"frame_of_reference", ach, STANDARD_TYPE(StepBasic_ApplicationContext), aFrameOfReference);

	// --- own field : lifeCycleStage ---

	Handle(TCollection_HAsciiString) aLifeCycleStage;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,3,"life_cycle_stage",ach,aLifeCycleStage);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aFrameOfReference, aLifeCycleStage);
}


void RWStepBasic_RWProductDefinitionContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductDefinitionContext)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field frameOfReference ---

	SW.Send(ent->FrameOfReference());

	// --- own field : lifeCycleStage ---

	SW.Send(ent->LifeCycleStage());
}


void RWStepBasic_RWProductDefinitionContext::Share(const Handle(StepBasic_ProductDefinitionContext)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->FrameOfReference());
}

