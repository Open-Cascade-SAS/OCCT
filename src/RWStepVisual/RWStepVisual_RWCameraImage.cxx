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


#include <RWStepVisual_RWCameraImage.ixx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationItem.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_CameraImage.hxx>


RWStepVisual_RWCameraImage::RWStepVisual_RWCameraImage () {}

void RWStepVisual_RWCameraImage::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CameraImage)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"camera_image")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : mappingSource ---

	Handle(StepRepr_RepresentationMap) aMappingSource;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"mapping_source", ach, STANDARD_TYPE(StepRepr_RepresentationMap), aMappingSource);

	// --- inherited field : mappingTarget ---

	Handle(StepRepr_RepresentationItem) aMappingTarget;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"mapping_target", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aMappingTarget);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aMappingSource, aMappingTarget);
}


void RWStepVisual_RWCameraImage::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CameraImage)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field mappingSource ---

	SW.Send(ent->MappingSource());

	// --- inherited field mappingTarget ---

	SW.Send(ent->MappingTarget());
}


void RWStepVisual_RWCameraImage::Share(const Handle(StepVisual_CameraImage)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->MappingSource());


	iter.GetOneItem(ent->MappingTarget());
}

