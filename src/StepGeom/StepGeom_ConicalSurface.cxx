#include <StepGeom_ConicalSurface.ixx>


StepGeom_ConicalSurface::StepGeom_ConicalSurface ()  {}

void StepGeom_ConicalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition)
{

	StepGeom_ElementarySurface::Init(aName, aPosition);
}

void StepGeom_ConicalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aRadius,
	const Standard_Real aSemiAngle)
{
	// --- classe own fields ---
	radius = aRadius;
	semiAngle = aSemiAngle;
	// --- classe inherited fields ---
	StepGeom_ElementarySurface::Init(aName, aPosition);
}


void StepGeom_ConicalSurface::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepGeom_ConicalSurface::Radius() const
{
	return radius;
}

void StepGeom_ConicalSurface::SetSemiAngle(const Standard_Real aSemiAngle)
{
	semiAngle = aSemiAngle;
}

Standard_Real StepGeom_ConicalSurface::SemiAngle() const
{
	return semiAngle;
}
