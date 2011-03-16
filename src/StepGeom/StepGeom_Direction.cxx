#include <StepGeom_Direction.ixx>


StepGeom_Direction::StepGeom_Direction ()  {}

void StepGeom_Direction::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Direction::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(TColStd_HArray1OfReal)& aDirectionRatios)
{
	// --- classe own fields ---
	directionRatios = aDirectionRatios;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Direction::SetDirectionRatios(const Handle(TColStd_HArray1OfReal)& aDirectionRatios)
{
	directionRatios = aDirectionRatios;
}

Handle(TColStd_HArray1OfReal) StepGeom_Direction::DirectionRatios() const
{
	return directionRatios;
}

Standard_Real StepGeom_Direction::DirectionRatiosValue(const Standard_Integer num) const
{
	return directionRatios->Value(num);
}

Standard_Integer StepGeom_Direction::NbDirectionRatios () const
{
	return directionRatios->Length();
}
