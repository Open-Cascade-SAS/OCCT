#include <StepGeom_CompositeCurve.ixx>


StepGeom_CompositeCurve::StepGeom_CompositeCurve ()  {}

void StepGeom_CompositeCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CompositeCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray1OfCompositeCurveSegment)& aSegments,
	const StepData_Logical aSelfIntersect)
{
	// --- classe own fields ---
	segments = aSegments;
	selfIntersect = aSelfIntersect;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_CompositeCurve::SetSegments(const Handle(StepGeom_HArray1OfCompositeCurveSegment)& aSegments)
{
	segments = aSegments;
}

Handle(StepGeom_HArray1OfCompositeCurveSegment) StepGeom_CompositeCurve::Segments() const
{
	return segments;
}

Handle(StepGeom_CompositeCurveSegment) StepGeom_CompositeCurve::SegmentsValue(const Standard_Integer num) const
{
	return segments->Value(num);
}

Standard_Integer StepGeom_CompositeCurve::NbSegments () const
{
	if (segments.IsNull()) return 0;
	return segments->Length();
}

void StepGeom_CompositeCurve::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
	selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_CompositeCurve::SelfIntersect() const
{
	return selfIntersect;
}
