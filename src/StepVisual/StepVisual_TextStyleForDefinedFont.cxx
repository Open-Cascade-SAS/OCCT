#include <StepVisual_TextStyleForDefinedFont.ixx>


StepVisual_TextStyleForDefinedFont::StepVisual_TextStyleForDefinedFont ()  {}

void StepVisual_TextStyleForDefinedFont::Init(
	const Handle(StepVisual_Colour)& aTextColour)
{
	// --- classe own fields ---
	textColour = aTextColour;
}


void StepVisual_TextStyleForDefinedFont::SetTextColour(const Handle(StepVisual_Colour)& aTextColour)
{
	textColour = aTextColour;
}

Handle(StepVisual_Colour) StepVisual_TextStyleForDefinedFont::TextColour() const
{
	return textColour;
}
