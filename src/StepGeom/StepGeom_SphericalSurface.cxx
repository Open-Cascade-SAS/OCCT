#include <StepGeom_SphericalSurface.ixx>


StepGeom_SphericalSurface::StepGeom_SphericalSurface ()  {}

void StepGeom_SphericalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition)
{

	StepGeom_ElementarySurface::Init(aName, aPosition);
}

void StepGeom_SphericalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aRadius)
{
	// --- classe own fields ---
	radius = aRadius;
	// --- classe inherited fields ---
	StepGeom_ElementarySurface::Init(aName, aPosition);
}


void StepGeom_SphericalSurface::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepGeom_SphericalSurface::Radius() const
{
	return radius;
}
