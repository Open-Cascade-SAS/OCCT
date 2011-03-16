#include <StepBasic_OrganizationAssignment.ixx>

void StepBasic_OrganizationAssignment::Init(
	const Handle(StepBasic_Organization)& aAssignedOrganization,
	const Handle(StepBasic_OrganizationRole)& aRole)
{
	// --- classe own fields ---
	assignedOrganization = aAssignedOrganization;
	role = aRole;
}


void StepBasic_OrganizationAssignment::SetAssignedOrganization(const Handle(StepBasic_Organization)& aAssignedOrganization)
{
	assignedOrganization = aAssignedOrganization;
}

Handle(StepBasic_Organization) StepBasic_OrganizationAssignment::AssignedOrganization() const
{
	return assignedOrganization;
}

void StepBasic_OrganizationAssignment::SetRole(const Handle(StepBasic_OrganizationRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_OrganizationRole) StepBasic_OrganizationAssignment::Role() const
{
	return role;
}
