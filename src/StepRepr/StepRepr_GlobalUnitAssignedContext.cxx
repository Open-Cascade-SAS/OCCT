#include <StepRepr_GlobalUnitAssignedContext.ixx>


StepRepr_GlobalUnitAssignedContext::StepRepr_GlobalUnitAssignedContext ()  {}

void StepRepr_GlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepRepr_GlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
	// --- classe own fields ---
	units = aUnits;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepRepr_GlobalUnitAssignedContext::SetUnits(const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
	units = aUnits;
}

Handle(StepBasic_HArray1OfNamedUnit) StepRepr_GlobalUnitAssignedContext::Units() const
{
	return units;
}

Handle(StepBasic_NamedUnit) StepRepr_GlobalUnitAssignedContext::UnitsValue(const Standard_Integer num) const
{
	return units->Value(num);
}

Standard_Integer StepRepr_GlobalUnitAssignedContext::NbUnits () const
{
	if (units.IsNull()) return 0;
	return units->Length();
}
