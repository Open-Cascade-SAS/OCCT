#include <StepBasic_CalendarDate.ixx>


StepBasic_CalendarDate::StepBasic_CalendarDate ()  {}

void StepBasic_CalendarDate::Init(
	const Standard_Integer aYearComponent)
{

	StepBasic_Date::Init(aYearComponent);
}

void StepBasic_CalendarDate::Init(
	const Standard_Integer aYearComponent,
	const Standard_Integer aDayComponent,
	const Standard_Integer aMonthComponent)
{
	// --- classe own fields ---
	dayComponent = aDayComponent;
	monthComponent = aMonthComponent;
	// --- classe inherited fields ---
	StepBasic_Date::Init(aYearComponent);
}


void StepBasic_CalendarDate::SetDayComponent(const Standard_Integer aDayComponent)
{
	dayComponent = aDayComponent;
}

Standard_Integer StepBasic_CalendarDate::DayComponent() const
{
	return dayComponent;
}

void StepBasic_CalendarDate::SetMonthComponent(const Standard_Integer aMonthComponent)
{
	monthComponent = aMonthComponent;
}

Standard_Integer StepBasic_CalendarDate::MonthComponent() const
{
	return monthComponent;
}
