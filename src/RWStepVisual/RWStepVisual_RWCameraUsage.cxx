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
#include <RWStepVisual_RWCameraUsage.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_CameraUsage.hxx>

RWStepVisual_RWCameraUsage::RWStepVisual_RWCameraUsage () {}

void RWStepVisual_RWCameraUsage::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CameraUsage)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"camera_usage")) return;

	// --- inherited field : mappingOrigin ---

	Handle(StepRepr_RepresentationItem) aMappingOrigin;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadEntity(num, 1,"mapping_origin", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aMappingOrigin);

	// --- inherited field : mappedRepresentation ---

	Handle(StepRepr_Representation) aMappedRepresentation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"mapped_representation", ach, STANDARD_TYPE(StepRepr_Representation), aMappedRepresentation);

	//--- Initialisation of the read entity ---


	ent->Init(aMappingOrigin, aMappedRepresentation);
}


void RWStepVisual_RWCameraUsage::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CameraUsage)& ent) const
{

	// --- inherited field mappingOrigin ---

	SW.Send(ent->MappingOrigin());

	// --- inherited field mappedRepresentation ---

	SW.Send(ent->MappedRepresentation());
}


void RWStepVisual_RWCameraUsage::Share(const Handle(StepVisual_CameraUsage)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->MappingOrigin());


	iter.GetOneItem(ent->MappedRepresentation());
}

