
#include <RWStepBasic_RWApplicationContext.ixx>


RWStepBasic_RWApplicationContext::RWStepBasic_RWApplicationContext () {}

void RWStepBasic_RWApplicationContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_ApplicationContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"application_context")) return;

	// --- own field : application ---

	Handle(TCollection_HAsciiString) aApplication;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"application",ach,aApplication);

	//--- Initialisation of the read entity ---


	ent->Init(aApplication);
}


void RWStepBasic_RWApplicationContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_ApplicationContext)& ent) const
{

	// --- own field : application ---

	SW.Send(ent->Application());
}
