#include <StepVisual_CompositeText.ixx>


StepVisual_CompositeText::StepVisual_CompositeText ()  {}

void StepVisual_CompositeText::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_CompositeText::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText)
{
	// --- classe own fields ---
	collectedText = aCollectedText;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_CompositeText::SetCollectedText(const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText)
{
	collectedText = aCollectedText;
}

Handle(StepVisual_HArray1OfTextOrCharacter) StepVisual_CompositeText::CollectedText() const
{
	return collectedText;
}

StepVisual_TextOrCharacter StepVisual_CompositeText::CollectedTextValue(const Standard_Integer num) const
{
	return collectedText->Value(num);
}

Standard_Integer StepVisual_CompositeText::NbCollectedText () const
{
	return collectedText->Length();
}
