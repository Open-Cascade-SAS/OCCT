#include <StepBasic_DateAssignment.ixx>

void StepBasic_DateAssignment::Init(
	const Handle(StepBasic_Date)& aAssignedDate,
	const Handle(StepBasic_DateRole)& aRole)
{
	// --- classe own fields ---
	assignedDate = aAssignedDate;
	role = aRole;
}


void StepBasic_DateAssignment::SetAssignedDate(const Handle(StepBasic_Date)& aAssignedDate)
{
	assignedDate = aAssignedDate;
}

Handle(StepBasic_Date) StepBasic_DateAssignment::AssignedDate() const
{
	return assignedDate;
}

void StepBasic_DateAssignment::SetRole(const Handle(StepBasic_DateRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_DateRole) StepBasic_DateAssignment::Role() const
{
	return role;
}
