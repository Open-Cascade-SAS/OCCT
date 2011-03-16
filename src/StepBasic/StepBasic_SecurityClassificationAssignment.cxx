#include <StepBasic_SecurityClassificationAssignment.ixx>

void StepBasic_SecurityClassificationAssignment::Init(
	const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification)
{
	// --- classe own fields ---
	assignedSecurityClassification = aAssignedSecurityClassification;
}


void StepBasic_SecurityClassificationAssignment::SetAssignedSecurityClassification(const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification)
{
	assignedSecurityClassification = aAssignedSecurityClassification;
}

Handle(StepBasic_SecurityClassification) StepBasic_SecurityClassificationAssignment::AssignedSecurityClassification() const
{
	return assignedSecurityClassification;
}
