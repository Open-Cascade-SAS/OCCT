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


#include <RWStepRepr_RWRepresentationMap.ixx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_Representation.hxx>


#include <Interface_EntityIterator.hxx>


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

