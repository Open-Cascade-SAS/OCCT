
#include <RWStepShape_RWSolidModel.ixx>


RWStepShape_RWSolidModel::RWStepShape_RWSolidModel () {}

void RWStepShape_RWSolidModel::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_SolidModel)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"solid_model")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepShape_RWSolidModel::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_SolidModel)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());
}
