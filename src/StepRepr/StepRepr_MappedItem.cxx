#include <StepRepr_MappedItem.ixx>


StepRepr_MappedItem::StepRepr_MappedItem ()  {}

void StepRepr_MappedItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepRepr_MappedItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepRepr_RepresentationMap)& aMappingSource,
	const Handle(StepRepr_RepresentationItem)& aMappingTarget)
{
	// --- classe own fields ---
	mappingSource = aMappingSource;
	mappingTarget = aMappingTarget;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepRepr_MappedItem::SetMappingSource(const Handle(StepRepr_RepresentationMap)& aMappingSource)
{
	mappingSource = aMappingSource;
}

Handle(StepRepr_RepresentationMap) StepRepr_MappedItem::MappingSource() const
{
	return mappingSource;
}

void StepRepr_MappedItem::SetMappingTarget(const Handle(StepRepr_RepresentationItem)& aMappingTarget)
{
	mappingTarget = aMappingTarget;
}

Handle(StepRepr_RepresentationItem) StepRepr_MappedItem::MappingTarget() const
{
	return mappingTarget;
}
