#include <StepBasic_PersonAndOrganizationAssignment.ixx>

void StepBasic_PersonAndOrganizationAssignment::Init(
	const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization,
	const Handle(StepBasic_PersonAndOrganizationRole)& aRole)
{
	// --- classe own fields ---
	assignedPersonAndOrganization = aAssignedPersonAndOrganization;
	role = aRole;
}


void StepBasic_PersonAndOrganizationAssignment::SetAssignedPersonAndOrganization(const Handle(StepBasic_PersonAndOrganization)& aAssignedPersonAndOrganization)
{
	assignedPersonAndOrganization = aAssignedPersonAndOrganization;
}

Handle(StepBasic_PersonAndOrganization) StepBasic_PersonAndOrganizationAssignment::AssignedPersonAndOrganization() const
{
	return assignedPersonAndOrganization;
}

void StepBasic_PersonAndOrganizationAssignment::SetRole(const Handle(StepBasic_PersonAndOrganizationRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_PersonAndOrganizationRole) StepBasic_PersonAndOrganizationAssignment::Role() const
{
	return role;
}
