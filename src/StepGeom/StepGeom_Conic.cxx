#include <StepGeom_Conic.ixx>


StepGeom_Conic::StepGeom_Conic ()  {}

void StepGeom_Conic::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_Conic::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{
	// --- classe own fields ---
	position = aPosition;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Conic::SetPosition(const StepGeom_Axis2Placement& aPosition)
{
	position = aPosition;
}

StepGeom_Axis2Placement StepGeom_Conic::Position() const
{
	return position;
}
