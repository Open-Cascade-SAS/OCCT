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
#include <RWStepVisual_RWAreaInSet.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_AreaInSet.hxx>
#include <StepVisual_PresentationArea.hxx>
#include <StepVisual_PresentationSet.hxx>

RWStepVisual_RWAreaInSet::RWStepVisual_RWAreaInSet () {}

void RWStepVisual_RWAreaInSet::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_AreaInSet)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"area_in_set")) return;

	// --- own field : area ---

	Handle(StepVisual_PresentationArea) aArea;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"area", ach, STANDARD_TYPE(StepVisual_PresentationArea), aArea);

	// --- own field : inSet ---

	Handle(StepVisual_PresentationSet) aInSet;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"in_set", ach, STANDARD_TYPE(StepVisual_PresentationSet), aInSet);

	//--- Initialisation of the read entity ---


	ent->Init(aArea, aInSet);
}


void RWStepVisual_RWAreaInSet::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_AreaInSet)& ent) const
{

	// --- own field : area ---

	SW.Send(ent->Area());

	// --- own field : inSet ---

	SW.Send(ent->InSet());
}


void RWStepVisual_RWAreaInSet::Share(const Handle(StepVisual_AreaInSet)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Area());


	iter.GetOneItem(ent->InSet());
}

