#include <StepShape_ExtrudedAreaSolid.ixx>


StepShape_ExtrudedAreaSolid::StepShape_ExtrudedAreaSolid ()  {}

void StepShape_ExtrudedAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea)
{

	StepShape_SweptAreaSolid::Init(aName, aSweptArea);
}

void StepShape_ExtrudedAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea,
	const Handle(StepGeom_Direction)& aExtrudedDirection,
	const Standard_Real aDepth)
{
	// --- classe own fields ---
	extrudedDirection = aExtrudedDirection;
	depth = aDepth;
	// --- classe inherited fields ---
	StepShape_SweptAreaSolid::Init(aName, aSweptArea);
}


void StepShape_ExtrudedAreaSolid::SetExtrudedDirection(const Handle(StepGeom_Direction)& aExtrudedDirection)
{
	extrudedDirection = aExtrudedDirection;
}

Handle(StepGeom_Direction) StepShape_ExtrudedAreaSolid::ExtrudedDirection() const
{
	return extrudedDirection;
}

void StepShape_ExtrudedAreaSolid::SetDepth(const Standard_Real aDepth)
{
	depth = aDepth;
}

Standard_Real StepShape_ExtrudedAreaSolid::Depth() const
{
	return depth;
}
