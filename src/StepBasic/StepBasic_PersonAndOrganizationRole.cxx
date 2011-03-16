#include <StepBasic_PersonAndOrganizationRole.ixx>


StepBasic_PersonAndOrganizationRole::StepBasic_PersonAndOrganizationRole ()  {}

void StepBasic_PersonAndOrganizationRole::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepBasic_PersonAndOrganizationRole::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_PersonAndOrganizationRole::Name() const
{
	return name;
}
