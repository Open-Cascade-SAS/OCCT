#include <StepRepr_RepresentationRelationship.ixx>


StepRepr_RepresentationRelationship::StepRepr_RepresentationRelationship ()  {}

void StepRepr_RepresentationRelationship::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TCollection_HAsciiString)& aDescription,
	const Handle(StepRepr_Representation)& aRep1,
	const Handle(StepRepr_Representation)& aRep2)
{
	// --- classe own fields ---
	name = aName;
	description = aDescription;
	rep1 = aRep1;
	rep2 = aRep2;
}


void StepRepr_RepresentationRelationship::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationRelationship::Name() const
{
	return name;
}

void StepRepr_RepresentationRelationship::SetDescription(const Handle(TCollection_HAsciiString)& aDescription)
{
	description = aDescription;
}

Handle(TCollection_HAsciiString) StepRepr_RepresentationRelationship::Description() const
{
	return description;
}

void StepRepr_RepresentationRelationship::SetRep1(const Handle(StepRepr_Representation)& aRep1)
{
	rep1 = aRep1;
}

Handle(StepRepr_Representation) StepRepr_RepresentationRelationship::Rep1() const
{
	return rep1;
}

void StepRepr_RepresentationRelationship::SetRep2(const Handle(StepRepr_Representation)& aRep2)
{
	rep2 = aRep2;
}

Handle(StepRepr_Representation) StepRepr_RepresentationRelationship::Rep2() const
{
	return rep2;
}
