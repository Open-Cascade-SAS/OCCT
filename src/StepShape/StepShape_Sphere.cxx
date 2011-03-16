#include <StepShape_Sphere.ixx>


StepShape_Sphere::StepShape_Sphere ()  {}

void StepShape_Sphere::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Sphere::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aRadius,
	const Handle(StepGeom_Point)& aCentre)
{
	// --- classe own fields ---
	radius = aRadius;
	centre = aCentre;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Sphere::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepShape_Sphere::Radius() const
{
	return radius;
}

void StepShape_Sphere::SetCentre(const Handle(StepGeom_Point)& aCentre)
{
	centre = aCentre;
}

Handle(StepGeom_Point) StepShape_Sphere::Centre() const
{
	return centre;
}
