#include <StepBasic_OrganizationRole.ixx>


StepBasic_OrganizationRole::StepBasic_OrganizationRole ()  {}

void StepBasic_OrganizationRole::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepBasic_OrganizationRole::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_OrganizationRole::Name() const
{
	return name;
}
