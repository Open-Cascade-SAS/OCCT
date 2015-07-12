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
#include <RWStepRepr_RWMappedItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_MappedItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>

RWStepRepr_RWMappedItem::RWStepRepr_RWMappedItem () {}

void RWStepRepr_RWMappedItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_MappedItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"mapped_item")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : mappingSource ---

	Handle(StepRepr_RepresentationMap) aMappingSource;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"mapping_source", ach, STANDARD_TYPE(StepRepr_RepresentationMap), aMappingSource);

	// --- own field : mappingTarget ---

	Handle(StepRepr_RepresentationItem) aMappingTarget;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"mapping_target", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aMappingTarget);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aMappingSource, aMappingTarget);
}


void RWStepRepr_RWMappedItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_MappedItem)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : mappingSource ---

	SW.Send(ent->MappingSource());

	// --- own field : mappingTarget ---

	SW.Send(ent->MappingTarget());
}


void RWStepRepr_RWMappedItem::Share(const Handle(StepRepr_MappedItem)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->MappingSource());


	iter.GetOneItem(ent->MappingTarget());
}

