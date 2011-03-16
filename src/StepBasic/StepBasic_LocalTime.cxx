#include <StepBasic_LocalTime.ixx>


StepBasic_LocalTime::StepBasic_LocalTime ()  {}

void StepBasic_LocalTime::Init(
	const Standard_Integer aHourComponent,
	const Standard_Boolean hasAminuteComponent,
	const Standard_Integer aMinuteComponent,
	const Standard_Boolean hasAsecondComponent,
	const Standard_Real aSecondComponent,
	const Handle(StepBasic_CoordinatedUniversalTimeOffset)& aZone)
{
	// --- classe own fields ---
	hourComponent = aHourComponent;
	hasMinuteComponent = hasAminuteComponent;
	minuteComponent = aMinuteComponent;
	hasSecondComponent = hasAsecondComponent;
	secondComponent = aSecondComponent;
	zone = aZone;
}


void StepBasic_LocalTime::SetHourComponent(const Standard_Integer aHourComponent)
{
	hourComponent = aHourComponent;
}

Standard_Integer StepBasic_LocalTime::HourComponent() const
{
	return hourComponent;
}

void StepBasic_LocalTime::SetMinuteComponent(const Standard_Integer aMinuteComponent)
{
	minuteComponent = aMinuteComponent;
	hasMinuteComponent = Standard_True;
}

void StepBasic_LocalTime::UnSetMinuteComponent()
{
	hasMinuteComponent = Standard_False;
}

Standard_Integer StepBasic_LocalTime::MinuteComponent() const
{
	return minuteComponent;
}

Standard_Boolean StepBasic_LocalTime::HasMinuteComponent() const
{
	return hasMinuteComponent;
}

void StepBasic_LocalTime::SetSecondComponent(const Standard_Real aSecondComponent)
{
	secondComponent = aSecondComponent;
	hasSecondComponent = Standard_True;
}

void StepBasic_LocalTime::UnSetSecondComponent()
{
	hasSecondComponent = Standard_False;
}

Standard_Real StepBasic_LocalTime::SecondComponent() const
{
	return secondComponent;
}

Standard_Boolean StepBasic_LocalTime::HasSecondComponent() const
{
	return hasSecondComponent;
}

void StepBasic_LocalTime::SetZone(const Handle(StepBasic_CoordinatedUniversalTimeOffset)& aZone)
{
	zone = aZone;
}

Handle(StepBasic_CoordinatedUniversalTimeOffset) StepBasic_LocalTime::Zone() const
{
	return zone;
}
