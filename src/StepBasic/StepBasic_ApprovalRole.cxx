#include <StepBasic_ApprovalRole.ixx>


StepBasic_ApprovalRole::StepBasic_ApprovalRole ()  {}

void StepBasic_ApprovalRole::Init(
	const Handle(TCollection_HAsciiString)& aRole)
{
	// --- classe own fields ---
	role = aRole;
}


void StepBasic_ApprovalRole::SetRole(const Handle(TCollection_HAsciiString)& aRole)
{
	role = aRole;
}

Handle(TCollection_HAsciiString) StepBasic_ApprovalRole::Role() const
{
	return role;
}
