#include <StepBasic_Approval.ixx>


StepBasic_Approval::StepBasic_Approval ()  {}

void StepBasic_Approval::Init(
	const Handle(StepBasic_ApprovalStatus)& aStatus,
	const Handle(TCollection_HAsciiString)& aLevel)
{
	// --- classe own fields ---
	status = aStatus;
	level = aLevel;
}


void StepBasic_Approval::SetStatus(const Handle(StepBasic_ApprovalStatus)& aStatus)
{
	status = aStatus;
}

Handle(StepBasic_ApprovalStatus) StepBasic_Approval::Status() const
{
	return status;
}

void StepBasic_Approval::SetLevel(const Handle(TCollection_HAsciiString)& aLevel)
{
	level = aLevel;
}

Handle(TCollection_HAsciiString) StepBasic_Approval::Level() const
{
	return level;
}
