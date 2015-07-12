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
#include <RWStepVisual_RWCameraModelD3.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepVisual_CameraModelD3.hxx>
#include <StepVisual_ViewVolume.hxx>

RWStepVisual_RWCameraModelD3::RWStepVisual_RWCameraModelD3 () {}

void RWStepVisual_RWCameraModelD3::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CameraModelD3)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"camera_model_d3")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : viewReferenceSystem ---

	Handle(StepGeom_Axis2Placement3d) aViewReferenceSystem;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"view_reference_system", ach, STANDARD_TYPE(StepGeom_Axis2Placement3d), aViewReferenceSystem);

	// --- own field : perspectiveOfVolume ---

	Handle(StepVisual_ViewVolume) aPerspectiveOfVolume;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"perspective_of_volume", ach, STANDARD_TYPE(StepVisual_ViewVolume), aPerspectiveOfVolume);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aViewReferenceSystem, aPerspectiveOfVolume);
}


void RWStepVisual_RWCameraModelD3::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CameraModelD3)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : viewReferenceSystem ---

	SW.Send(ent->ViewReferenceSystem());

	// --- own field : perspectiveOfVolume ---

	SW.Send(ent->PerspectiveOfVolume());
}


void RWStepVisual_RWCameraModelD3::Share(const Handle(StepVisual_CameraModelD3)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->ViewReferenceSystem());


	iter.GetOneItem(ent->PerspectiveOfVolume());
}

