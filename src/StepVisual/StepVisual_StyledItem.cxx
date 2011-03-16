#include <StepVisual_StyledItem.ixx>


StepVisual_StyledItem::StepVisual_StyledItem ()  {}

void StepVisual_StyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_StyledItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles,
	const Handle(StepRepr_RepresentationItem)& aItem)
{
	// --- classe own fields ---
	styles = aStyles;
	item = aItem;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_StyledItem::SetStyles(const Handle(StepVisual_HArray1OfPresentationStyleAssignment)& aStyles)
{
	styles = aStyles;
}

Handle(StepVisual_HArray1OfPresentationStyleAssignment) StepVisual_StyledItem::Styles() const
{
	return styles;
}

Handle(StepVisual_PresentationStyleAssignment) StepVisual_StyledItem::StylesValue(const Standard_Integer num) const
{
	return styles->Value(num);
}

Standard_Integer StepVisual_StyledItem::NbStyles () const
{
	if (styles.IsNull()) return 0;
	return styles->Length();
}

void StepVisual_StyledItem::SetItem(const Handle(StepRepr_RepresentationItem)& aItem)
{
	item = aItem;
}

Handle(StepRepr_RepresentationItem) StepVisual_StyledItem::Item() const
{
	return item;
}
