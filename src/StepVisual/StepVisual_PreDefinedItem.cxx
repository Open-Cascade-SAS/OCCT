#include <StepVisual_PreDefinedItem.ixx>


StepVisual_PreDefinedItem::StepVisual_PreDefinedItem ()  {}

void StepVisual_PreDefinedItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepVisual_PreDefinedItem::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_PreDefinedItem::Name() const
{
	return name;
}
