#include <StepBasic_DateAndTimeAssignment.ixx>

void StepBasic_DateAndTimeAssignment::Init(
	const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime,
	const Handle(StepBasic_DateTimeRole)& aRole)
{
	// --- classe own fields ---
	assignedDateAndTime = aAssignedDateAndTime;
	role = aRole;
}


void StepBasic_DateAndTimeAssignment::SetAssignedDateAndTime(const Handle(StepBasic_DateAndTime)& aAssignedDateAndTime)
{
	assignedDateAndTime = aAssignedDateAndTime;
}

Handle(StepBasic_DateAndTime) StepBasic_DateAndTimeAssignment::AssignedDateAndTime() const
{
	return assignedDateAndTime;
}

void StepBasic_DateAndTimeAssignment::SetRole(const Handle(StepBasic_DateTimeRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_DateTimeRole) StepBasic_DateAndTimeAssignment::Role() const
{
	return role;
}
