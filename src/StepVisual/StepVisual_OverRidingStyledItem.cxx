#include <StepVisual_OverRidingStyledItem.ixx>


StepVisual_OverRidingStyledItem::StepVisual_OverRidingStyledItem ()  {}

void StepVisual_OverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem)
{

	StepVisual_StyledItem::Init(aName, aStyles, aItem);
}

void StepVisual_OverRidingStyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem,
	const Handle(StepVisual_StyledItem)& aOverRiddenStyle)
{
	// --- classe own fields ---
	overRiddenStyle = aOverRiddenStyle;
	// --- classe inherited fields ---
	StepVisual_StyledItem::Init(aName, aStyles, aItem);
}


void StepVisual_OverRidingStyledItem::SetOverRiddenStyle(const Handle(StepVisual_StyledItem)& aOverRiddenStyle)
{
	overRiddenStyle = aOverRiddenStyle;
}

Handle(StepVisual_StyledItem) StepVisual_OverRidingStyledItem::OverRiddenStyle() const
{
	return overRiddenStyle;
}
