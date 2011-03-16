#include <StepShape_SweptAreaSolid.ixx>


StepShape_SweptAreaSolid::StepShape_SweptAreaSolid ()  {}

void StepShape_SweptAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_SweptAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea)
{
	// --- classe own fields ---
	sweptArea = aSweptArea;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_SweptAreaSolid::SetSweptArea(const Handle(StepGeom_CurveBoundedSurface)& aSweptArea)
{
	sweptArea = aSweptArea;
}

Handle(StepGeom_CurveBoundedSurface) StepShape_SweptAreaSolid::SweptArea() const
{
	return sweptArea;
}
