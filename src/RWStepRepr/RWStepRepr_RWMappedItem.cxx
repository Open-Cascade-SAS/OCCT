
#include <RWStepRepr_RWMappedItem.ixx>
#include <StepRepr_RepresentationMap.hxx>
#include <StepRepr_RepresentationItem.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepRepr_MappedItem.hxx>


RWStepRepr_RWMappedItem::RWStepRepr_RWMappedItem () {}

void RWStepRepr_RWMappedItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_MappedItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"mapped_item")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : mappingSource ---

	Handle(StepRepr_RepresentationMap) aMappingSource;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"mapping_source", ach, STANDARD_TYPE(StepRepr_RepresentationMap), aMappingSource);

	// --- own field : mappingTarget ---

	Handle(StepRepr_RepresentationItem) aMappingTarget;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"mapping_target", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aMappingTarget);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aMappingSource, aMappingTarget);
}


void RWStepRepr_RWMappedItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_MappedItem)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : mappingSource ---

	SW.Send(ent->MappingSource());

	// --- own field : mappingTarget ---

	SW.Send(ent->MappingTarget());
}


void RWStepRepr_RWMappedItem::Share(const Handle(StepRepr_MappedItem)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->MappingSource());


	iter.GetOneItem(ent->MappingTarget());
}

