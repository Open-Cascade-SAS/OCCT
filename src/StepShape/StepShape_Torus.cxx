#include <StepShape_Torus.ixx>


StepShape_Torus::StepShape_Torus ()  {}

void StepShape_Torus::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Torus::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis1Placement)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius)
{
	// --- classe own fields ---
	position = aPosition;
	majorRadius = aMajorRadius;
	minorRadius = aMinorRadius;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Torus::SetPosition(const Handle(StepGeom_Axis1Placement)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis1Placement) StepShape_Torus::Position() const
{
	return position;
}

void StepShape_Torus::SetMajorRadius(const Standard_Real aMajorRadius)
{
	majorRadius = aMajorRadius;
}

Standard_Real StepShape_Torus::MajorRadius() const
{
	return majorRadius;
}

void StepShape_Torus::SetMinorRadius(const Standard_Real aMinorRadius)
{
	minorRadius = aMinorRadius;
}

Standard_Real StepShape_Torus::MinorRadius() const
{
	return minorRadius;
}
