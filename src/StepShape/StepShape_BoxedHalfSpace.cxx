#include <StepShape_BoxedHalfSpace.ixx>


StepShape_BoxedHalfSpace::StepShape_BoxedHalfSpace ()  {}

void StepShape_BoxedHalfSpace::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBaseSurface,
	const Standard_Boolean aAgreementFlag)
{

	StepShape_HalfSpaceSolid::Init(aName, aBaseSurface, aAgreementFlag);
}

void StepShape_BoxedHalfSpace::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBaseSurface,
	const Standard_Boolean aAgreementFlag,
	const Handle(StepShape_BoxDomain)& aEnclosure)
{
	// --- classe own fields ---
	enclosure = aEnclosure;
	// --- classe inherited fields ---
	StepShape_HalfSpaceSolid::Init(aName, aBaseSurface, aAgreementFlag);
}


void StepShape_BoxedHalfSpace::SetEnclosure(const Handle(StepShape_BoxDomain)& aEnclosure)
{
	enclosure = aEnclosure;
}

Handle(StepShape_BoxDomain) StepShape_BoxedHalfSpace::Enclosure() const
{
	return enclosure;
}
