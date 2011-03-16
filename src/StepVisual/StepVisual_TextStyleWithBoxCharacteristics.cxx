#include <StepVisual_TextStyleWithBoxCharacteristics.ixx>


StepVisual_TextStyleWithBoxCharacteristics::StepVisual_TextStyleWithBoxCharacteristics ()  {}

void StepVisual_TextStyleWithBoxCharacteristics::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_TextStyleForDefinedFont)& aCharacterAppearance)
{

	StepVisual_TextStyle::Init(aName, aCharacterAppearance);
}

void StepVisual_TextStyleWithBoxCharacteristics::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_TextStyleForDefinedFont)& aCharacterAppearance,
	const Handle(StepVisual_HArray1OfBoxCharacteristicSelect)& aCharacteristics)
{
	// --- classe own fields ---
	characteristics = aCharacteristics;
	// --- classe inherited fields ---
	StepVisual_TextStyle::Init(aName, aCharacterAppearance);
}


void StepVisual_TextStyleWithBoxCharacteristics::SetCharacteristics(const Handle(StepVisual_HArray1OfBoxCharacteristicSelect)& aCharacteristics)
{
	characteristics = aCharacteristics;
}

Handle(StepVisual_HArray1OfBoxCharacteristicSelect) StepVisual_TextStyleWithBoxCharacteristics::Characteristics() const
{
	return characteristics;
}

StepVisual_BoxCharacteristicSelect StepVisual_TextStyleWithBoxCharacteristics::CharacteristicsValue(const Standard_Integer num) const
{
	return characteristics->Value(num);
}

Standard_Integer StepVisual_TextStyleWithBoxCharacteristics::NbCharacteristics () const
{
	return characteristics->Length();
}
