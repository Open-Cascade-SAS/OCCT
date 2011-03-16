
#include <RWStepVisual_RWPreDefinedItem.ixx>


RWStepVisual_RWPreDefinedItem::RWStepVisual_RWPreDefinedItem () {}

void RWStepVisual_RWPreDefinedItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PreDefinedItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"pre_defined_item")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepVisual_RWPreDefinedItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PreDefinedItem)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
