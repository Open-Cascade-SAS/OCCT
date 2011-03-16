#include <StepBasic_OrdinalDate.ixx>


StepBasic_OrdinalDate::StepBasic_OrdinalDate ()  {}

void StepBasic_OrdinalDate::Init(
	const Standard_Integer aYearComponent)
{

	StepBasic_Date::Init(aYearComponent);
}

void StepBasic_OrdinalDate::Init(
	const Standard_Integer aYearComponent,
	const Standard_Integer aDayComponent)
{
	// --- classe own fields ---
	dayComponent = aDayComponent;
	// --- classe inherited fields ---
	StepBasic_Date::Init(aYearComponent);
}


void StepBasic_OrdinalDate::SetDayComponent(const Standard_Integer aDayComponent)
{
	dayComponent = aDayComponent;
}

Standard_Integer StepBasic_OrdinalDate::DayComponent() const
{
	return dayComponent;
}
