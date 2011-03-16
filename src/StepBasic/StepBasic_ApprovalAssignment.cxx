#include <StepBasic_ApprovalAssignment.ixx>

void StepBasic_ApprovalAssignment::Init(
	const Handle(StepBasic_Approval)& aAssignedApproval)
{
	// --- classe own fields ---
	assignedApproval = aAssignedApproval;
}


void StepBasic_ApprovalAssignment::SetAssignedApproval(const Handle(StepBasic_Approval)& aAssignedApproval)
{
	assignedApproval = aAssignedApproval;
}

Handle(StepBasic_Approval) StepBasic_ApprovalAssignment::AssignedApproval() const
{
	return assignedApproval;
}
