#include <StepGeom_Axis2Placement3d.ixx>


StepGeom_Axis2Placement3d::StepGeom_Axis2Placement3d ()  {}

void StepGeom_Axis2Placement3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation)
{

	StepGeom_Placement::Init(aName, aLocation);
}

void StepGeom_Axis2Placement3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasAaxis,
	const Handle(StepGeom_Direction)& aAxis,
	const Standard_Boolean hasArefDirection,
	const Handle(StepGeom_Direction)& aRefDirection)
{
	// --- classe own fields ---
	hasAxis = hasAaxis;
	axis = aAxis;
	hasRefDirection = hasArefDirection;
	refDirection = aRefDirection;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis2Placement3d::SetAxis(const Handle(StepGeom_Direction)& aAxis)
{
	axis = aAxis;
	hasAxis = Standard_True;
}

void StepGeom_Axis2Placement3d::UnSetAxis()
{
	hasAxis = Standard_False;
	axis.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement3d::Axis() const
{
	return axis;
}

Standard_Boolean StepGeom_Axis2Placement3d::HasAxis() const
{
	return hasAxis;
}

void StepGeom_Axis2Placement3d::SetRefDirection(const Handle(StepGeom_Direction)& aRefDirection)
{
	refDirection = aRefDirection;
	hasRefDirection = Standard_True;
}

void StepGeom_Axis2Placement3d::UnSetRefDirection()
{
	hasRefDirection = Standard_False;
	refDirection.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement3d::RefDirection() const
{
	return refDirection;
}

Standard_Boolean StepGeom_Axis2Placement3d::HasRefDirection() const
{
	return hasRefDirection;
}
