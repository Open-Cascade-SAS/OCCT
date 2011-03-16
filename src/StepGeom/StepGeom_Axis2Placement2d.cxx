#include <StepGeom_Axis2Placement2d.ixx>


StepGeom_Axis2Placement2d::StepGeom_Axis2Placement2d ()  {}

void StepGeom_Axis2Placement2d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation)
{

	StepGeom_Placement::Init(aName, aLocation);
}

void StepGeom_Axis2Placement2d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasArefDirection,
	const Handle(StepGeom_Direction)& aRefDirection)
{
	// --- classe own fields ---
	hasRefDirection = hasArefDirection;
	refDirection = aRefDirection;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis2Placement2d::SetRefDirection(const Handle(StepGeom_Direction)& aRefDirection)
{
	refDirection = aRefDirection;
	hasRefDirection = Standard_True;
}

void StepGeom_Axis2Placement2d::UnSetRefDirection()
{
	hasRefDirection = Standard_False;
	refDirection.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement2d::RefDirection() const
{
	return refDirection;
}

Standard_Boolean StepGeom_Axis2Placement2d::HasRefDirection() const
{
	return hasRefDirection;
}
