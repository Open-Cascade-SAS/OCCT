
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

