
#include <RWStepRepr_RWRepresentationItem.ixx>


RWStepRepr_RWRepresentationItem::RWStepRepr_RWRepresentationItem () {}

void RWStepRepr_RWRepresentationItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepRepr_RepresentationItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepRepr_RWRepresentationItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepRepr_RepresentationItem)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
