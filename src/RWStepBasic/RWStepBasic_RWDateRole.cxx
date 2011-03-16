
#include <RWStepBasic_RWDateRole.ixx>


RWStepBasic_RWDateRole::RWStepBasic_RWDateRole () {}

void RWStepBasic_RWDateRole::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_DateRole)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"date_role")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the read entity ---


	ent->Init(aName);
}


void RWStepBasic_RWDateRole::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_DateRole)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());
}
