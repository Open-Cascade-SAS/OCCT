#include <StepRepr_DescriptiveRepresentationItem.ixx>


StepRepr_DescriptiveRepresentationItem::StepRepr_DescriptiveRepresentationItem ()  {}

void StepRepr_DescriptiveRepresentationItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepRepr_DescriptiveRepresentationItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription)
{
	// --- classe own fields ---
	description = aDescription;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepRepr_DescriptiveRepresentationItem::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_DescriptiveRepresentationItem::Description() const
{
	return description;
}
