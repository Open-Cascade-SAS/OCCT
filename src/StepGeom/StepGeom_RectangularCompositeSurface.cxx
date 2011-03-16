#include <StepGeom_RectangularCompositeSurface.ixx>


StepGeom_RectangularCompositeSurface::StepGeom_RectangularCompositeSurface ()  {}

void StepGeom_RectangularCompositeSurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_RectangularCompositeSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_HArray2OfSurfacePatch)& aSegments)
{
	// --- classe own fields ---
	segments = aSegments;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_RectangularCompositeSurface::SetSegments(const Handle(StepGeom_HArray2OfSurfacePatch)& aSegments)
{
	segments = aSegments;
}

Handle(StepGeom_HArray2OfSurfacePatch) StepGeom_RectangularCompositeSurface::Segments() const
{
	return segments;
}

Handle(StepGeom_SurfacePatch) StepGeom_RectangularCompositeSurface::SegmentsValue(const Standard_Integer num1,const Standard_Integer num2) const
{
	return segments->Value(num1,num2);
}

Standard_Integer StepGeom_RectangularCompositeSurface::NbSegmentsI () const
{
	if (segments.IsNull()) return 0;
	return segments->UpperRow() - segments->LowerRow() + 1;
}

Standard_Integer StepGeom_RectangularCompositeSurface::NbSegmentsJ () const
{
	if (segments.IsNull()) return 0;
	return segments->UpperCol() - segments->LowerCol() + 1;
}
