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

//gka 05.03.99 S4134 upgrade from CD to DIS
#include <RWStepBasic_RWProductDefinition.ixx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionContext.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepBasic_ProductDefinition.hxx>


RWStepBasic_RWProductDefinition::RWStepBasic_RWProductDefinition () {}

void RWStepBasic_RWProductDefinition::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductDefinition)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"product_definition")) return;

	// --- own field : id ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	if (data->IsParamDefined (num,2)) { //gka 05.03.99 S4134 upgrade from CD to DIS
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	  data->ReadString (num,2,"description",ach,aDescription);
	}
	// --- own field : formation ---

	Handle(StepBasic_ProductDefinitionFormation) aFormation;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"formation", ach, STANDARD_TYPE(StepBasic_ProductDefinitionFormation), aFormation);

	// --- own field : frameOfReference ---

	Handle(StepBasic_ProductDefinitionContext) aFrameOfReference;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"frame_of_reference", ach, STANDARD_TYPE(StepBasic_ProductDefinitionContext), aFrameOfReference);

	//--- Initialisation of the read entity ---


	ent->Init(aId, aDescription, aFormation, aFrameOfReference);
}


void RWStepBasic_RWProductDefinition::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductDefinition)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());

	// --- own field : description ---

	SW.Send(ent->Description());

	// --- own field : formation ---

	SW.Send(ent->Formation());

	// --- own field : frameOfReference ---

	SW.Send(ent->FrameOfReference());
}


void RWStepBasic_RWProductDefinition::Share(const Handle(StepBasic_ProductDefinition)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Formation());


	iter.GetOneItem(ent->FrameOfReference());
}

