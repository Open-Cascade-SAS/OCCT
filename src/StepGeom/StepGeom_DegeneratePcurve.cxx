#include <StepGeom_DegeneratePcurve.ixx>


StepGeom_DegeneratePcurve::StepGeom_DegeneratePcurve ()  {}

void StepGeom_DegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_DegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve)
{
	// --- classe own fields ---
	basisSurface = aBasisSurface;
	referenceToCurve = aReferenceToCurve;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_DegeneratePcurve::SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface)
{
	basisSurface = aBasisSurface;
}

Handle(StepGeom_Surface) StepGeom_DegeneratePcurve::BasisSurface() const
{
	return basisSurface;
}

void StepGeom_DegeneratePcurve::SetReferenceToCurve(const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve)
{
	referenceToCurve = aReferenceToCurve;
}

Handle(StepRepr_DefinitionalRepresentation) StepGeom_DegeneratePcurve::ReferenceToCurve() const
{
	return referenceToCurve;
}
