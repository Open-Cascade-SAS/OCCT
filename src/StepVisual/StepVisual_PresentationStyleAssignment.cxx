#include <StepVisual_PresentationStyleAssignment.ixx>


StepVisual_PresentationStyleAssignment::StepVisual_PresentationStyleAssignment ()  {}

void StepVisual_PresentationStyleAssignment::Init(
	const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles)
{
	// --- classe own fields ---
	styles = aStyles;
}


void StepVisual_PresentationStyleAssignment::SetStyles(const Handle(StepVisual_HArray1OfPresentationStyleSelect)& aStyles)
{
	styles = aStyles;
}

Handle(StepVisual_HArray1OfPresentationStyleSelect) StepVisual_PresentationStyleAssignment::Styles() const
{
	return styles;
}

StepVisual_PresentationStyleSelect StepVisual_PresentationStyleAssignment::StylesValue(const Standard_Integer num) const
{
	return styles->Value(num);
}

Standard_Integer StepVisual_PresentationStyleAssignment::NbStyles () const
{
	return styles->Length();
}
