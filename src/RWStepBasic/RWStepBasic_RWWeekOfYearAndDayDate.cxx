
#include <RWStepBasic_RWWeekOfYearAndDayDate.ixx>


RWStepBasic_RWWeekOfYearAndDayDate::RWStepBasic_RWWeekOfYearAndDayDate () {}

void RWStepBasic_RWWeekOfYearAndDayDate::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepBasic_WeekOfYearAndDayDate)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"week_of_year_and_day_date")) return;

	// --- inherited field : yearComponent ---

	Standard_Integer aYearComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"year_component",ach,aYearComponent);

	// --- own field : weekComponent ---

	Standard_Integer aWeekComponent;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"week_component",ach,aWeekComponent);

	// --- own field : dayComponent ---

	Standard_Integer aDayComponent;
	Standard_Boolean hasAdayComponent = Standard_True;
	if (data->IsParamDefined(num,3)) {
	  //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	  data->ReadInteger (num,3,"day_component",ach,aDayComponent);
	}
	else {
	  hasAdayComponent = Standard_False;
	  aDayComponent = 0;
	}

	//--- Initialisation of the read entity ---


	ent->Init(aYearComponent, aWeekComponent, hasAdayComponent, aDayComponent);
}


void RWStepBasic_RWWeekOfYearAndDayDate::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepBasic_WeekOfYearAndDayDate)& ent) const
{

	// --- inherited field yearComponent ---

	SW.Send(ent->YearComponent());

	// --- own field : weekComponent ---

	SW.Send(ent->WeekComponent());

	// --- own field : dayComponent ---

	Standard_Boolean hasAdayComponent = ent->HasDayComponent();
	if (hasAdayComponent) {
	  SW.Send(ent->DayComponent());
	}
	else {
	  SW.SendUndef();
	}
}
