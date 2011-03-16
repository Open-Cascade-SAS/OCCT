#include <StepGeom_PointOnCurve.ixx>


StepGeom_PointOnCurve::StepGeom_PointOnCurve ()  {}

void StepGeom_PointOnCurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_PointOnCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aBasisCurve,
	const Standard_Real aPointParameter)
{
	// --- classe own fields ---
	basisCurve = aBasisCurve;
	pointParameter = aPointParameter;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_PointOnCurve::SetBasisCurve(const Handle(StepGeom_Curve)& aBasisCurve)
{
	basisCurve = aBasisCurve;
}

Handle(StepGeom_Curve) StepGeom_PointOnCurve::BasisCurve() const
{
	return basisCurve;
}

void StepGeom_PointOnCurve::SetPointParameter(const Standard_Real aPointParameter)
{
	pointParameter = aPointParameter;
}

Standard_Real StepGeom_PointOnCurve::PointParameter() const
{
	return pointParameter;
}
