#include <StepVisual_FillAreaStyleColour.ixx>


StepVisual_FillAreaStyleColour::StepVisual_FillAreaStyleColour ()  {}

void StepVisual_FillAreaStyleColour::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_Colour)& aFillColour)
{
	// --- classe own fields ---
	name = aName;
	fillColour = aFillColour;
}


void StepVisual_FillAreaStyleColour::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_FillAreaStyleColour::Name() const
{
	return name;
}

void StepVisual_FillAreaStyleColour::SetFillColour(const Handle(StepVisual_Colour)& aFillColour)
{
	fillColour = aFillColour;
}

Handle(StepVisual_Colour) StepVisual_FillAreaStyleColour::FillColour() const
{
	return fillColour;
}
