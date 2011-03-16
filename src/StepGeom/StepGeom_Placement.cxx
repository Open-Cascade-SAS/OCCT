#include <StepGeom_Placement.ixx>


StepGeom_Placement::StepGeom_Placement ()  {}

void StepGeom_Placement::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Placement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation)
{
	// --- classe own fields ---
	location = aLocation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Placement::SetLocation(const Handle(StepGeom_CartesianPoint)& aLocation)
{
	location = aLocation;
}

Handle(StepGeom_CartesianPoint) StepGeom_Placement::Location() const
{
	return location;
}
