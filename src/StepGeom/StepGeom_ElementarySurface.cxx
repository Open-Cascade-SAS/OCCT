#include <StepGeom_ElementarySurface.ixx>


StepGeom_ElementarySurface::StepGeom_ElementarySurface ()  {}

void StepGeom_ElementarySurface::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_ElementarySurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition)
{
	// --- classe own fields ---
	position = aPosition;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_ElementarySurface::SetPosition(const Handle(StepGeom_Axis2Placement3d)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis2Placement3d) StepGeom_ElementarySurface::Position() const
{
	return position;
}
