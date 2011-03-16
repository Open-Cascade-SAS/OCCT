#include <StepVisual_TextStyle.ixx>


StepVisual_TextStyle::StepVisual_TextStyle ()  {}

void StepVisual_TextStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_TextStyleForDefinedFont)& aCharacterAppearance)
{
	// --- classe own fields ---
	name = aName;
	characterAppearance = aCharacterAppearance;
}


void StepVisual_TextStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_TextStyle::Name() const
{
	return name;
}

void StepVisual_TextStyle::SetCharacterAppearance(const Handle(StepVisual_TextStyleForDefinedFont)& aCharacterAppearance)
{
	characterAppearance = aCharacterAppearance;
}

Handle(StepVisual_TextStyleForDefinedFont) StepVisual_TextStyle::CharacterAppearance() const
{
	return characterAppearance;
}
