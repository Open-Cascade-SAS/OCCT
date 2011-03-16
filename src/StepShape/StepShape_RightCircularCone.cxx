#include <StepShape_RightCircularCone.ixx>


StepShape_RightCircularCone::StepShape_RightCircularCone ()  {}

void StepShape_RightCircularCone::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_RightCircularCone::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis1Placement)& aPosition,
	const Standard_Real aHeight,
	const Standard_Real aRadius,
	const Standard_Real aSemiAngle)
{
	// --- classe own fields ---
	position = aPosition;
	height = aHeight;
	radius = aRadius;
	semiAngle = aSemiAngle;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_RightCircularCone::SetPosition(const Handle(StepGeom_Axis1Placement)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis1Placement) StepShape_RightCircularCone::Position() const
{
	return position;
}

void StepShape_RightCircularCone::SetHeight(const Standard_Real aHeight)
{
	height = aHeight;
}

Standard_Real StepShape_RightCircularCone::Height() const
{
	return height;
}

void StepShape_RightCircularCone::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepShape_RightCircularCone::Radius() const
{
	return radius;
}

void StepShape_RightCircularCone::SetSemiAngle(const Standard_Real aSemiAngle)
{
	semiAngle = aSemiAngle;
}

Standard_Real StepShape_RightCircularCone::SemiAngle() const
{
	return semiAngle;
}
