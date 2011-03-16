#include <StepBasic_Date.ixx>


StepBasic_Date::StepBasic_Date ()  {}

void StepBasic_Date::Init(
	const Standard_Integer aYearComponent)
{
	// --- classe own fields ---
	yearComponent = aYearComponent;
}


void StepBasic_Date::SetYearComponent(const Standard_Integer aYearComponent)
{
	yearComponent = aYearComponent;
}

Standard_Integer StepBasic_Date::YearComponent() const
{
	return yearComponent;
}
