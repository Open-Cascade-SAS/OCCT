#include <StepVisual_CompositeTextWithExtent.ixx>


StepVisual_CompositeTextWithExtent::StepVisual_CompositeTextWithExtent ()  {}

void StepVisual_CompositeTextWithExtent::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText)
{

	StepVisual_CompositeText::Init(aName, aCollectedText);
}

void StepVisual_CompositeTextWithExtent::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText,
	const Handle(StepVisual_PlanarExtent)& aExtent)
{
	// --- classe own fields ---
	extent = aExtent;
	// --- classe inherited fields ---
	StepVisual_CompositeText::Init(aName, aCollectedText);
}


void StepVisual_CompositeTextWithExtent::SetExtent(const Handle(StepVisual_PlanarExtent)& aExtent)
{
	extent = aExtent;
}

Handle(StepVisual_PlanarExtent) StepVisual_CompositeTextWithExtent::Extent() const
{
	return extent;
}
