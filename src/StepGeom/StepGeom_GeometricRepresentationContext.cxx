#include <StepGeom_GeometricRepresentationContext.ixx>


StepGeom_GeometricRepresentationContext::StepGeom_GeometricRepresentationContext ()  {}

void StepGeom_GeometricRepresentationContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType)
{

	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}

void StepGeom_GeometricRepresentationContext::Init(
	const Handle(TCollection_HAsciiString)& aContextIdentifier,
	const Handle(TCollection_HAsciiString)& aContextType,
	const Standard_Integer aCoordinateSpaceDimension)
{
	// --- classe own fields ---
	coordinateSpaceDimension = aCoordinateSpaceDimension;
	// --- classe inherited fields ---
	StepRepr_RepresentationContext::Init(aContextIdentifier, aContextType);
}


void StepGeom_GeometricRepresentationContext::SetCoordinateSpaceDimension(const Standard_Integer aCoordinateSpaceDimension)
{
	coordinateSpaceDimension = aCoordinateSpaceDimension;
}

Standard_Integer StepGeom_GeometricRepresentationContext::CoordinateSpaceDimension() const
{
	return coordinateSpaceDimension;
}
