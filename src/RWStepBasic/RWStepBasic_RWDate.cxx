
#include <RWStepBasic_RWDate.ixx>


RWStepBasic_RWDate::RWStepBasic_RWDate () {}

void RWStepBasic_RWDate::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_Date)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,1,ach,"date")) return;

	// --- own field : yearComponent ---

	Standard_Integer aYearComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"year_component",ach,aYearComponent);

	//--- Initialisation of the read entity ---


	ent->Init(aYearComponent);
}


void RWStepBasic_RWDate::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_Date)& ent) const
{

	// --- own field : yearComponent ---

	SW.Send(ent->YearComponent());
}
