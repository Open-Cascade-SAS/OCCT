#include <StepVisual_ColourSpecification.ixx>


StepVisual_ColourSpecification::StepVisual_ColourSpecification ()  {}

void StepVisual_ColourSpecification::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepVisual_ColourSpecification::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_ColourSpecification::Name() const
{
	return name;
}
