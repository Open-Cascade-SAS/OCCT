
#include <RWStepVisual_RWColourSpecification.ixx>


RWStepVisual_RWColourSpecification::RWStepVisual_RWColourSpecification () {}

void RWStepVisual_RWColourSpecification::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_ColourSpecification)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"colour_specification")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepVisual_RWColourSpecification::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_ColourSpecification)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
