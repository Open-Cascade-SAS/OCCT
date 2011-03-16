#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.ixx>

#include <StepGeom_GeometricRepresentationContext.hxx>

#include <StepRepr_GlobalUnitAssignedContext.hxx>


StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext ()  {}

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext,
	const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedContext)
{
	// --- classe own fields ---
	geometricRepresentationContext = aGeometricRepresentationContext;
	globalUnitAssignedContext = aGlobalUnitAssignedContext;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Standard_Integer aCoordinateSpaceDimension,
	const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
	// --- classe inherited fields ---

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);

	// --- ANDOR componant fields ---

	geometricRepresentationContext = new StepGeom_GeometricRepresentationContext();
	geometricRepresentationContext->Init(aContextIdentifier, aContextType, aCoordinateSpaceDimension);

	// --- ANDOR componant fields ---

	globalUnitAssignedContext = new StepRepr_GlobalUnitAssignedContext();
	globalUnitAssignedContext->Init(aContextIdentifier, aContextType, aUnits);
}


void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::SetGeometricRepresentationContext(const Handle(StepGeom_GeometricRepresentationContext)& aGeometricRepresentationContext)
{
	geometricRepresentationContext = aGeometricRepresentationContext;
}

Handle(StepGeom_GeometricRepresentationContext) StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::GeometricRepresentationContext() const
{
	return geometricRepresentationContext;
}

void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::SetGlobalUnitAssignedContext(const Handle(StepRepr_GlobalUnitAssignedContext)& aGlobalUnitAssignedContext)
{
	globalUnitAssignedContext = aGlobalUnitAssignedContext;
}

Handle(StepRepr_GlobalUnitAssignedContext) StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::GlobalUnitAssignedContext() const
{
	return globalUnitAssignedContext;
}

	//--- Specific Methods for AND classe field access ---


void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::SetCoordinateSpaceDimension(const Standard_Integer aCoordinateSpaceDimension)
{
	geometricRepresentationContext->SetCoordinateSpaceDimension(aCoordinateSpaceDimension);
}

Standard_Integer StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::CoordinateSpaceDimension() const
{
	return geometricRepresentationContext->CoordinateSpaceDimension();
}

	//--- Specific Methods for AND classe field access ---


void StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::SetUnits(const Handle(StepBasic_HArray1OfNamedUnit)& aUnits)
{
	globalUnitAssignedContext->SetUnits(aUnits);
}

Handle(StepBasic_HArray1OfNamedUnit) StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::Units() const
{
	return globalUnitAssignedContext->Units();
}

Handle(StepBasic_NamedUnit) StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::UnitsValue(const Standard_Integer num) const
{
	return globalUnitAssignedContext->UnitsValue(num);
}

Standard_Integer StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext::NbUnits () const
{
	return globalUnitAssignedContext->NbUnits();
}
