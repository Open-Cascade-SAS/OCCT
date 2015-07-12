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
#include <RWStepVisual_RWPresentedItemRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_PresentationRepresentationSelect.hxx>
#include <StepVisual_PresentedItem.hxx>
#include <StepVisual_PresentedItemRepresentation.hxx>

RWStepVisual_RWPresentedItemRepresentation::RWStepVisual_RWPresentedItemRepresentation () {}

void RWStepVisual_RWPresentedItemRepresentation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PresentedItemRepresentation)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"presented_item_representation")) return;

	// --- own fields

	StepVisual_PresentationRepresentationSelect prs;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num,1,"date_time", ach, prs);

	Handle(StepVisual_PresentedItem) pi;
	//szv#4:S4163:12Mar99 `stat1 =` not needed
	data->ReadEntity (num,2,"dated_approval",ach, STANDARD_TYPE(StepVisual_PresentedItem), pi);

	//--- Initialisation of the read entity ---


	ent->Init(prs,pi);
}


void RWStepVisual_RWPresentedItemRepresentation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PresentedItemRepresentation)& ent) const
{

	// --- own field : dimensions ---

	SW.Send(ent->Presentation().Value());
	SW.Send(ent->Item());
}


void RWStepVisual_RWPresentedItemRepresentation::Share(const Handle(StepVisual_PresentedItemRepresentation)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Presentation().Value());
	iter.GetOneItem(ent->Item());
}
