#include <StepRepr_RepresentationItem.ixx>


StepRepr_RepresentationItem::StepRepr_RepresentationItem ()  {}

void StepRepr_RepresentationItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{
	// --- classe own fields ---
	name = aName;
}


void StepRepr_RepresentationItem::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationItem::Name() const
{
	return name;
}
