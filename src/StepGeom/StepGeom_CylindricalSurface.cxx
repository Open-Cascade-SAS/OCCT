#include <StepGeom_CylindricalSurface.ixx>


StepGeom_CylindricalSurface::StepGeom_CylindricalSurface ()  {}

void StepGeom_CylindricalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition)
{

	StepGeom_ElementarySurface::Init(aName, aPosition);
}

void StepGeom_CylindricalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aRadius)
{
	// --- classe own fields ---
	radius = aRadius;
	// --- classe inherited fields ---
	StepGeom_ElementarySurface::Init(aName, aPosition);
}


void StepGeom_CylindricalSurface::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepGeom_CylindricalSurface::Radius() const
{
	return radius;
}
