#include <StepShape_PolyLoop.ixx>


StepShape_PolyLoop::StepShape_PolyLoop ()  {}

void StepShape_PolyLoop::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_PolyLoop::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aPolygon)
{
	// --- classe own fields ---
	polygon = aPolygon;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_PolyLoop::SetPolygon(const Handle(StepGeom_HArray1OfCartesianPoint)& aPolygon)
{
	polygon = aPolygon;
}

Handle(StepGeom_HArray1OfCartesianPoint) StepShape_PolyLoop::Polygon() const
{
	return polygon;
}

Handle(StepGeom_CartesianPoint) StepShape_PolyLoop::PolygonValue(const Standard_Integer num) const
{
	return polygon->Value(num);
}

Standard_Integer StepShape_PolyLoop::NbPolygon () const
{
	if (polygon.IsNull()) return 0;
	return polygon->Length();
}
