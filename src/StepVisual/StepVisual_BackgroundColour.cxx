#include <StepVisual_BackgroundColour.ixx>


StepVisual_BackgroundColour::StepVisual_BackgroundColour ()  {}

void StepVisual_BackgroundColour::Init(
	const StepVisual_AreaOrView& aPresentation)
{
	// --- classe own fields ---
	presentation = aPresentation;
}


void StepVisual_BackgroundColour::SetPresentation(const StepVisual_AreaOrView& aPresentation)
{
	presentation = aPresentation;
}

StepVisual_AreaOrView StepVisual_BackgroundColour::Presentation() const
{
	return presentation;
}
