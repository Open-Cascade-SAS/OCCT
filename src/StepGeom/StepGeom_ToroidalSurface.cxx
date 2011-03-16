#include <StepGeom_ToroidalSurface.ixx>


StepGeom_ToroidalSurface::StepGeom_ToroidalSurface ()  {}

void StepGeom_ToroidalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition)
{

	StepGeom_ElementarySurface::Init(aName, aPosition);
}

void StepGeom_ToroidalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius)
{
	// --- classe own fields ---
	majorRadius = aMajorRadius;
	minorRadius = aMinorRadius;
	// --- classe inherited fields ---
	StepGeom_ElementarySurface::Init(aName, aPosition);
}


void StepGeom_ToroidalSurface::SetMajorRadius(const Standard_Real aMajorRadius)
{
	majorRadius = aMajorRadius;
}

Standard_Real StepGeom_ToroidalSurface::MajorRadius() const
{
	return majorRadius;
}

void StepGeom_ToroidalSurface::SetMinorRadius(const Standard_Real aMinorRadius)
{
	minorRadius = aMinorRadius;
}

Standard_Real StepGeom_ToroidalSurface::MinorRadius() const
{
	return minorRadius;
}
