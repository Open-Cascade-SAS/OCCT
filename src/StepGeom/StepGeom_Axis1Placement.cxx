#include <StepGeom_Axis1Placement.ixx>


StepGeom_Axis1Placement::StepGeom_Axis1Placement ()  {}

void StepGeom_Axis1Placement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation)
{

	StepGeom_Placement::Init(aName, aLocation);
}

void StepGeom_Axis1Placement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasAaxis,
	const Handle(StepGeom_Direction)& aAxis)
{
	// --- classe own fields ---
	hasAxis = hasAaxis;
	axis = aAxis;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis1Placement::SetAxis(const Handle(StepGeom_Direction)& aAxis)
{
	axis = aAxis;
	hasAxis = Standard_True;
}

void StepGeom_Axis1Placement::UnSetAxis()
{
	hasAxis = Standard_False;
	axis.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis1Placement::Axis() const
{
	return axis;
}

Standard_Boolean StepGeom_Axis1Placement::HasAxis() const
{
	return hasAxis;
}
