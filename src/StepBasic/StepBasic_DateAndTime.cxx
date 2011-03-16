#include <StepBasic_DateAndTime.ixx>


StepBasic_DateAndTime::StepBasic_DateAndTime ()  {}

void StepBasic_DateAndTime::Init(
	const Handle(StepBasic_Date)& aDateComponent,
	const Handle(StepBasic_LocalTime)& aTimeComponent)
{
	// --- classe own fields ---
	dateComponent = aDateComponent;
	timeComponent = aTimeComponent;
}


void StepBasic_DateAndTime::SetDateComponent(const Handle(StepBasic_Date)& aDateComponent)
{
	dateComponent = aDateComponent;
}

Handle(StepBasic_Date) StepBasic_DateAndTime::DateComponent() const
{
	return dateComponent;
}

void StepBasic_DateAndTime::SetTimeComponent(const Handle(StepBasic_LocalTime)& aTimeComponent)
{
	timeComponent = aTimeComponent;
}

Handle(StepBasic_LocalTime) StepBasic_DateAndTime::TimeComponent() const
{
	return timeComponent;
}
