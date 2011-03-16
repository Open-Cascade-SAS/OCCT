
#include <RWStepRepr_RWDescriptiveRepresentationItem.ixx>


RWStepRepr_RWDescriptiveRepresentationItem::RWStepRepr_RWDescriptiveRepresentationItem () {}

void RWStepRepr_RWDescriptiveRepresentationItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_DescriptiveRepresentationItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"descriptive_representation_item")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : description ---

	Handle(TCollection_HAsciiString) aDescription;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadString (num,2,"description",ach,aDescription);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDescription);
}


void RWStepRepr_RWDescriptiveRepresentationItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_DescriptiveRepresentationItem)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : description ---

	SW.Send(ent->Description());
}
