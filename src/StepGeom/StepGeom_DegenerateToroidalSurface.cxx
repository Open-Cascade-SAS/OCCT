#include <StepGeom_DegenerateToroidalSurface.ixx>


StepGeom_DegenerateToroidalSurface::StepGeom_DegenerateToroidalSurface ()  {}

void StepGeom_DegenerateToroidalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius)
{

	StepGeom_ToroidalSurface::Init(aName, aPosition, aMajorRadius, aMinorRadius);
}

void StepGeom_DegenerateToroidalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius,
	const Standard_Boolean aSelectOuter)
{
	// --- classe own fields ---
	selectOuter = aSelectOuter;
	// --- classe inherited fields ---
	StepGeom_ToroidalSurface::Init(aName, aPosition, aMajorRadius, aMinorRadius);
}


void StepGeom_DegenerateToroidalSurface::SetSelectOuter(const Standard_Boolean aSelectOuter)
{
	selectOuter = aSelectOuter;
}

Standard_Boolean StepGeom_DegenerateToroidalSurface::SelectOuter() const
{
	return selectOuter;
}
