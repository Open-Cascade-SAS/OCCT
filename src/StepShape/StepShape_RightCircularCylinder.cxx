#include <StepShape_RightCircularCylinder.ixx>


StepShape_RightCircularCylinder::StepShape_RightCircularCylinder ()  {}

void StepShape_RightCircularCylinder::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_RightCircularCylinder::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis1Placement)& aPosition,
	const Standard_Real aHeight,
	const Standard_Real aRadius)
{
	// --- classe own fields ---
	position = aPosition;
	height = aHeight;
	radius = aRadius;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_RightCircularCylinder::SetPosition(const Handle(StepGeom_Axis1Placement)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis1Placement) StepShape_RightCircularCylinder::Position() const
{
	return position;
}

void StepShape_RightCircularCylinder::SetHeight(const Standard_Real aHeight)
{
	height = aHeight;
}

Standard_Real StepShape_RightCircularCylinder::Height() const
{
	return height;
}

void StepShape_RightCircularCylinder::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepShape_RightCircularCylinder::Radius() const
{
	return radius;
}
