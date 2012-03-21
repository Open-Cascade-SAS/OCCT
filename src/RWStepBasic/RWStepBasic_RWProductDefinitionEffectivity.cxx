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


#include <RWStepBasic_RWProductDefinitionEffectivity.ixx>

#include <Interface_EntityIterator.hxx>


#include <StepBasic_ProductDefinitionEffectivity.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <TCollection_HAsciiString.hxx>


RWStepBasic_RWProductDefinitionEffectivity::RWStepBasic_RWProductDefinitionEffectivity () {}

void RWStepBasic_RWProductDefinitionEffectivity::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ProductDefinitionEffectivity)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"product_definition_effectivity")) return;

	// --- inherited field : product_data_type ---

	Handle(TCollection_HAsciiString) aId;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"id",ach,aId);

	// --- own field : kind ---

	Handle(StepBasic_ProductDefinitionRelationship) aUsage;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity (num, 2,"kind", ach, STANDARD_TYPE(StepBasic_ProductDefinitionRelationship), aUsage);

	//--- Initialisation of the read entity ---


	ent->Init(aId,aUsage);
}


void RWStepBasic_RWProductDefinitionEffectivity::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ProductDefinitionEffectivity)& ent) const
{

	// --- own field : id ---

	SW.Send(ent->Id());
	SW.Send(ent->Usage());
}


void RWStepBasic_RWProductDefinitionEffectivity::Share(const Handle(StepBasic_ProductDefinitionEffectivity)& ent, Interface_EntityIterator& iter) const
{
  iter.AddItem(ent->Usage());
}
