#include <StepRepr_RepresentationMap.ixx>


StepRepr_RepresentationMap::StepRepr_RepresentationMap ()  {}

void StepRepr_RepresentationMap::Init(
	const Handle(StepRepr_RepresentationItem)& aMappingOrigin,
	const Handle(StepRepr_Representation)& aMappedRepresentation)
{
	// --- classe own fields ---
	mappingOrigin = aMappingOrigin;
	mappedRepresentation = aMappedRepresentation;
}


void StepRepr_RepresentationMap::SetMappingOrigin(const Handle(StepRepr_RepresentationItem)& aMappingOrigin)
{
	mappingOrigin = aMappingOrigin;
}

Handle(StepRepr_RepresentationItem) StepRepr_RepresentationMap::MappingOrigin() const
{
	return mappingOrigin;
}

void StepRepr_RepresentationMap::SetMappedRepresentation(const Handle(StepRepr_Representation)& aMappedRepresentation)
{
	mappedRepresentation = aMappedRepresentation;
}

Handle(StepRepr_Representation) StepRepr_RepresentationMap::MappedRepresentation() const
{
	return mappedRepresentation;
}
