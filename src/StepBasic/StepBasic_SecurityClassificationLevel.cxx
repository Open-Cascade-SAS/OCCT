#include <StepBasic_SecurityClassificationLevel.ixx>


StepBasic_SecurityClassificationLevel::StepBasic_SecurityClassificationLevel ()  {}

void StepBasic_SecurityClassificationLevel::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepBasic_SecurityClassificationLevel::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepBasic_SecurityClassificationLevel::Name() const
{
	return name;
}
