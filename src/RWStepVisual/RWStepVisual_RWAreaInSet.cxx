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


#include <RWStepVisual_RWAreaInSet.ixx>
#include <StepVisual_PresentationArea.hxx>
#include <StepVisual_PresentationSet.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_AreaInSet.hxx>


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

