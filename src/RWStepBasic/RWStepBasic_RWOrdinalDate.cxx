
#include <RWStepBasic_RWOrdinalDate.ixx>


RWStepBasic_RWOrdinalDate::RWStepBasic_RWOrdinalDate () {}

void RWStepBasic_RWOrdinalDate::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_OrdinalDate)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"ordinal_date")) return;

	// --- inherited field : yearComponent ---

	Standard_Integer aYearComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"year_component",ach,aYearComponent);

	// --- own field : dayComponent ---

	Standard_Integer aDayComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"day_component",ach,aDayComponent);

	//--- Initialisation of the read entity ---


	ent->Init(aYearComponent, aDayComponent);
}


void RWStepBasic_RWOrdinalDate::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_OrdinalDate)& ent) const
{

	// --- inherited field yearComponent ---

	SW.Send(ent->YearComponent());

	// --- own field : dayComponent ---

	SW.Send(ent->DayComponent());
}
