#include <StepVisual_PresentationStyleByContext.ixx>


StepVisual_PresentationStyleByContext::StepVisual_PresentationStyleByContext ()  {}

void StepVisual_PresentationStyleByContext::Init(
	const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles)
{

	StepVisual_PresentationStyleAssignment::Init(aStyles);
}

void StepVisual_PresentationStyleByContext::Init(
	const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles,
	const StepVisual_StyleContextSelect& aStyleContext)
{
	// --- classe own fields ---
	styleContext = aStyleContext;
	// --- classe inherited fields ---
	StepVisual_PresentationStyleAssignment::Init(aStyles);
}


void StepVisual_PresentationStyleByContext::SetStyleContext(const StepVisual_StyleContextSelect& aStyleContext)
{
	styleContext = aStyleContext;
}

StepVisual_StyleContextSelect StepVisual_PresentationStyleByContext::StyleContext() const
{
	return styleContext;
}
