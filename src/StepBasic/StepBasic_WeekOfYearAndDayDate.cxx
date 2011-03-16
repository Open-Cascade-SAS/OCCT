#include <StepBasic_WeekOfYearAndDayDate.ixx>


StepBasic_WeekOfYearAndDayDate::StepBasic_WeekOfYearAndDayDate ()  {}

void StepBasic_WeekOfYearAndDayDate::Init(
	const Standard_Integer aYearComponent)
{

	StepBasic_Date::Init(aYearComponent);
}

void StepBasic_WeekOfYearAndDayDate::Init(
	const Standard_Integer aYearComponent,
	const Standard_Integer aWeekComponent,
	const Standard_Boolean hasAdayComponent,
	const Standard_Integer aDayComponent)
{
	// --- classe own fields ---
	weekComponent = aWeekComponent;
	hasDayComponent = hasAdayComponent;
	dayComponent = aDayComponent;
	// --- classe inherited fields ---
	StepBasic_Date::Init(aYearComponent);
}


void StepBasic_WeekOfYearAndDayDate::SetWeekComponent(const Standard_Integer aWeekComponent)
{
	weekComponent = aWeekComponent;
}

Standard_Integer StepBasic_WeekOfYearAndDayDate::WeekComponent() const
{
	return weekComponent;
}

void StepBasic_WeekOfYearAndDayDate::SetDayComponent(const Standard_Integer aDayComponent)
{
	dayComponent = aDayComponent;
	hasDayComponent = Standard_True;
}

void StepBasic_WeekOfYearAndDayDate::UnSetDayComponent()
{
	hasDayComponent = Standard_False;
}

Standard_Integer StepBasic_WeekOfYearAndDayDate::DayComponent() const
{
	return dayComponent;
}

Standard_Boolean StepBasic_WeekOfYearAndDayDate::HasDayComponent() const
{
	return hasDayComponent;
}
