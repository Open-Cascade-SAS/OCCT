//:S4134: abv 03.03.99: implementation of DraughtingPreDefinedColour modified

#include <RWStepVisual_RWDraughtingPreDefinedColour.ixx>
#include <StepVisual_PreDefinedItem.hxx>


RWStepVisual_RWDraughtingPreDefinedColour::RWStepVisual_RWDraughtingPreDefinedColour () {}

void RWStepVisual_RWDraughtingPreDefinedColour::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_DraughtingPreDefinedColour)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"draughting_pre_defined_colour has not 1 parameter(s)")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->GetPreDefinedItem()->Init(aName);
}


void RWStepVisual_RWDraughtingPreDefinedColour::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_DraughtingPreDefinedColour)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->GetPreDefinedItem()->Name());
}
