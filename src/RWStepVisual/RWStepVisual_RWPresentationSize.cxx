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
#include <RWStepVisual_RWPresentationSize.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_PresentationSize.hxx>
#include <StepVisual_PresentationSizeAssignmentSelect.hxx>

RWStepVisual_RWPresentationSize::RWStepVisual_RWPresentationSize () {}

void RWStepVisual_RWPresentationSize::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PresentationSize)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"presentation_size")) return;

	// --- own field : unit ---

	StepVisual_PresentationSizeAssignmentSelect aUnit;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num,1,"unit",ach,aUnit);

	// --- own field : size ---

	Handle(StepVisual_PlanarBox) aSize;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"size", ach, STANDARD_TYPE(StepVisual_PlanarBox), aSize);

	//--- Initialisation of the read entity ---


	ent->Init(aUnit, aSize);
}


void RWStepVisual_RWPresentationSize::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PresentationSize)& ent) const
{

	// --- own field : unit ---

	SW.Send(ent->Unit().Value());

	// --- own field : size ---

	SW.Send(ent->Size());
}


void RWStepVisual_RWPresentationSize::Share(const Handle(StepVisual_PresentationSize)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Unit().Value());


	iter.GetOneItem(ent->Size());
}

