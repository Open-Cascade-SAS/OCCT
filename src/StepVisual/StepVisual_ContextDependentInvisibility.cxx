#include <StepVisual_ContextDependentInvisibility.ixx>


StepVisual_ContextDependentInvisibility::StepVisual_ContextDependentInvisibility ()  {}

void StepVisual_ContextDependentInvisibility::Init(
	const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems)
{

	StepVisual_Invisibility::Init(aInvisibleItems);
}

void StepVisual_ContextDependentInvisibility::Init(
	const Handle(StepVisual_HArray1OfInvisibleItem)& aInvisibleItems,
	const StepVisual_InvisibilityContext& aPresentationContext)
{
	// --- classe own fields ---
	presentationContext = aPresentationContext;
	// --- classe inherited fields ---
	StepVisual_Invisibility::Init(aInvisibleItems);
}


void StepVisual_ContextDependentInvisibility::SetPresentationContext(const StepVisual_InvisibilityContext& aPresentationContext)
{
	presentationContext = aPresentationContext;
}

StepVisual_InvisibilityContext StepVisual_ContextDependentInvisibility::PresentationContext() const
{
	return presentationContext;
}
