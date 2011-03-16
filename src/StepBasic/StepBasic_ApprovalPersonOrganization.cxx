#include <StepBasic_ApprovalPersonOrganization.ixx>


StepBasic_ApprovalPersonOrganization::StepBasic_ApprovalPersonOrganization ()  {}

void StepBasic_ApprovalPersonOrganization::Init(
	const StepBasic_PersonOrganizationSelect& aPersonOrganization,
	const Handle(StepBasic_Approval)& aAuthorizedApproval,
	const Handle(StepBasic_ApprovalRole)& aRole)
{
	// --- classe own fields ---
	personOrganization = aPersonOrganization;
	authorizedApproval = aAuthorizedApproval;
	role = aRole;
}


void StepBasic_ApprovalPersonOrganization::SetPersonOrganization(const StepBasic_PersonOrganizationSelect& aPersonOrganization)
{
	personOrganization = aPersonOrganization;
}

StepBasic_PersonOrganizationSelect StepBasic_ApprovalPersonOrganization::PersonOrganization() const
{
	return personOrganization;
}

void StepBasic_ApprovalPersonOrganization::SetAuthorizedApproval(const Handle(StepBasic_Approval)& aAuthorizedApproval)
{
	authorizedApproval = aAuthorizedApproval;
}

Handle(StepBasic_Approval) StepBasic_ApprovalPersonOrganization::AuthorizedApproval() const
{
	return authorizedApproval;
}

void StepBasic_ApprovalPersonOrganization::SetRole(const Handle(StepBasic_ApprovalRole)& aRole)
{
	role = aRole;
}

Handle(StepBasic_ApprovalRole) StepBasic_ApprovalPersonOrganization::Role() const
{
	return role;
}
