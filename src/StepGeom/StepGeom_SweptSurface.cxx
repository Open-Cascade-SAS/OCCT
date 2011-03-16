#include <StepGeom_SweptSurface.ixx>


StepGeom_SweptSurface::StepGeom_SweptSurface ()  {}

void StepGeom_SweptSurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_SweptSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aSweptCurve)
{
	// --- classe own fields ---
	sweptCurve = aSweptCurve;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_SweptSurface::SetSweptCurve(const Handle(StepGeom_Curve)& aSweptCurve)
{
	sweptCurve = aSweptCurve;
}

Handle(StepGeom_Curve) StepGeom_SweptSurface::SweptCurve() const
{
	return sweptCurve;
}
