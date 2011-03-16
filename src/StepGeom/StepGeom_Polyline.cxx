#include <StepGeom_Polyline.ixx>


StepGeom_Polyline::StepGeom_Polyline ()  {}

void StepGeom_Polyline::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Polyline::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aPoints)
{
	// --- classe own fields ---
	points = aPoints;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Polyline::SetPoints(const Handle(StepGeom_HArray1OfCartesianPoint)& aPoints)
{
	points = aPoints;
}

Handle(StepGeom_HArray1OfCartesianPoint) StepGeom_Polyline::Points() const
{
	return points;
}

Handle(StepGeom_CartesianPoint) StepGeom_Polyline::PointsValue(const Standard_Integer num) const
{
	return points->Value(num);
}

Standard_Integer StepGeom_Polyline::NbPoints () const
{
	if (points.IsNull()) return 0;
	return points->Length();
}
