#include <StepRepr_GlobalUncertaintyAssignedContext.ixx>


StepRepr_GlobalUncertaintyAssignedContext::StepRepr_GlobalUncertaintyAssignedContext ()  {}

void StepRepr_GlobalUncertaintyAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepRepr_GlobalUncertaintyAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty)
{
	// --- classe own fields ---
	uncertainty = aUncertainty;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepRepr_GlobalUncertaintyAssignedContext::SetUncertainty(const Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit)& aUncertainty)
{
	uncertainty = aUncertainty;
}

Handle(StepBasic_HArray1OfUncertaintyMeasureWithUnit) StepRepr_GlobalUncertaintyAssignedContext::Uncertainty() const
{
	return uncertainty;
}

Handle(StepBasic_UncertaintyMeasureWithUnit) StepRepr_GlobalUncertaintyAssignedContext::UncertaintyValue(const Standard_Integer num) const
{
	return uncertainty->Value(num);
}

Standard_Integer StepRepr_GlobalUncertaintyAssignedContext::NbUncertainty () const
{
	if (uncertainty.IsNull()) return 0;
	return uncertainty->Length();
}
