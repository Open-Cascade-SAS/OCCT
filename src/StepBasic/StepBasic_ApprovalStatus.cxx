#include <StepBasic_ApprovalStatus.ixx>


StepBasic_ApprovalStatus::StepBasic_ApprovalStatus ()  {}

void StepBasic_ApprovalStatus::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepBasic_ApprovalStatus::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_ApprovalStatus::Name() const
{
	return name;
}
