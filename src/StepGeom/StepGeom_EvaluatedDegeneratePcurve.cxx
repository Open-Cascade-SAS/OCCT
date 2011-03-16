#include <StepGeom_EvaluatedDegeneratePcurve.ixx>


StepGeom_EvaluatedDegeneratePcurve::StepGeom_EvaluatedDegeneratePcurve ()  {}

void StepGeom_EvaluatedDegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve)
{

	StepGeom_DegeneratePcurve::Init(aName, aBasisSurface, aReferenceToCurve);
}

void StepGeom_EvaluatedDegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve,
	const Handle(StepGeom_CartesianPoint)& aEquivalentPoint)
{
	// --- classe own fields ---
	equivalentPoint = aEquivalentPoint;
	// --- classe inherited fields ---
	StepGeom_DegeneratePcurve::Init(aName, aBasisSurface, aReferenceToCurve);
}


void StepGeom_EvaluatedDegeneratePcurve::SetEquivalentPoint(const Handle(StepGeom_CartesianPoint)& aEquivalentPoint)
{
	equivalentPoint = aEquivalentPoint;
}

Handle(StepGeom_CartesianPoint) StepGeom_EvaluatedDegeneratePcurve::EquivalentPoint() const
{
	return equivalentPoint;
}
