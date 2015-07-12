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
#include <RWStepRepr_RWRepresentationMap.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationMap.hxx>

RWStepRepr_RWRepresentationMap::RWStepRepr_RWRepresentationMap () {}

void RWStepRepr_RWRepresentationMap::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_RepresentationMap)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"representation_map")) return;

	// --- own field : mappingOrigin ---

	Handle(StepRepr_RepresentationItem) aMappingOrigin;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"mapping_origin", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aMappingOrigin);

	// --- own field : mappedRepresentation ---

	Handle(StepRepr_Representation) aMappedRepresentation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"mapped_representation", ach, STANDARD_TYPE(StepRepr_Representation), aMappedRepresentation);

	//--- Initialisation of the read entity ---


	ent->Init(aMappingOrigin, aMappedRepresentation);
}


void RWStepRepr_RWRepresentationMap::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_RepresentationMap)& ent) const
{

	// --- own field : mappingOrigin ---

	SW.Send(ent->MappingOrigin());

	// --- own field : mappedRepresentation ---

	SW.Send(ent->MappedRepresentation());
}


void RWStepRepr_RWRepresentationMap::Share(const Handle(StepRepr_RepresentationMap)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->MappingOrigin());


	iter.GetOneItem(ent->MappedRepresentation());
}

