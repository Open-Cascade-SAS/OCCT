#include <StepBasic_DateTimeRole.ixx>


StepBasic_DateTimeRole::StepBasic_DateTimeRole ()  {}

void StepBasic_DateTimeRole::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepBasic_DateTimeRole::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_DateTimeRole::Name() const
{
	return name;
}
