#include <StepGeom_Circle.ixx>


StepGeom_Circle::StepGeom_Circle ()  {}

void StepGeom_Circle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{

	StepGeom_Conic::Init(aName, aPosition);
}

void StepGeom_Circle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition,
	const Standard_Real aRadius)
{
	// --- classe own fields ---
	radius = aRadius;
	// --- classe inherited fields ---
	StepGeom_Conic::Init(aName, aPosition);
}


void StepGeom_Circle::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepGeom_Circle::Radius() const
{
	return radius;
}
