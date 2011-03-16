#include <StepBasic_DateRole.ixx>


StepBasic_DateRole::StepBasic_DateRole ()  {}

void StepBasic_DateRole::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepBasic_DateRole::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_DateRole::Name() const
{
	return name;
}
