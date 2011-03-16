#include <StepShape_HalfSpaceSolid.ixx>


StepShape_HalfSpaceSolid::StepShape_HalfSpaceSolid ()  {}

void StepShape_HalfSpaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_HalfSpaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBaseSurface,
	const Standard_Boolean aAgreementFlag)
{
	// --- classe own fields ---
	baseSurface = aBaseSurface;
	agreementFlag = aAgreementFlag;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_HalfSpaceSolid::SetBaseSurface(const Handle(StepGeom_Surface)& aBaseSurface)
{
	baseSurface = aBaseSurface;
}

Handle(StepGeom_Surface) StepShape_HalfSpaceSolid::BaseSurface() const
{
	return baseSurface;
}

void StepShape_HalfSpaceSolid::SetAgreementFlag(const Standard_Boolean aAgreementFlag)
{
	agreementFlag = aAgreementFlag;
}

Standard_Boolean StepShape_HalfSpaceSolid::AgreementFlag() const
{
	return agreementFlag;
}
