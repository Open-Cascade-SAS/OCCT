#include <StepVisual_ContextDependentOverRidingStyledItem.ixx>


StepVisual_ContextDependentOverRidingStyledItem::StepVisual_ContextDependentOverRidingStyledItem ()  {}

void StepVisual_ContextDependentOverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem,
	const Handle(StepVisual_StyledItem)& aOverRiddenStyle)
{

	StepVisual_OverRidingStyledItem::Init(aName, aStyles, aItem, aOverRiddenStyle);
}

void StepVisual_ContextDependentOverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem,
	const Handle(StepVisual_StyledItem)& aOverRiddenStyle,
	const Handle(StepVisual_HArray1OfStyleContextSelect)& aStyleContext)
{
	// --- classe own fields ---
	styleContext = aStyleContext;
	// --- classe inherited fields ---
	StepVisual_OverRidingStyledItem::Init(aName, aStyles, aItem, aOverRiddenStyle);
}


void StepVisual_ContextDependentOverRidingStyledItem::SetStyleContext(const Handle(StepVisual_HArray1OfStyleContextSelect)& aStyleContext)
{
	styleContext = aStyleContext;
}

Handle(StepVisual_HArray1OfStyleContextSelect) StepVisual_ContextDependentOverRidingStyledItem::StyleContext() const
{
	return styleContext;
}

StepVisual_StyleContextSelect StepVisual_ContextDependentOverRidingStyledItem::StyleContextValue(const Standard_Integer num) const
{
	return styleContext->Value(num);
}

Standard_Integer StepVisual_ContextDependentOverRidingStyledItem::NbStyleContext () const
{
	return styleContext->Length();
}
