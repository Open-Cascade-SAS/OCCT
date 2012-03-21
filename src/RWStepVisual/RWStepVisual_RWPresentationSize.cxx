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


#include <RWStepVisual_RWPresentationSize.ixx>
#include <StepVisual_PresentationSizeAssignmentSelect.hxx>
#include <StepVisual_PlanarBox.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_PresentationSize.hxx>


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

