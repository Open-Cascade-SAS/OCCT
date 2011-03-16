#include <StepGeom_Vector.ixx>


StepGeom_Vector::StepGeom_Vector ()  {}

void StepGeom_Vector::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Vector::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Direction)& aOrientation,
	const Standard_Real aMagnitude)
{
	// --- classe own fields ---
	orientation = aOrientation;
	magnitude = aMagnitude;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Vector::SetOrientation(const Handle(StepGeom_Direction)& aOrientation)
{
	orientation = aOrientation;
}

Handle(StepGeom_Direction) StepGeom_Vector::Orientation() const
{
	return orientation;
}

void StepGeom_Vector::SetMagnitude(const Standard_Real aMagnitude)
{
	magnitude = aMagnitude;
}

Standard_Real StepGeom_Vector::Magnitude() const
{
	return magnitude;
}
