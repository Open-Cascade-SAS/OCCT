#include <StepShape_ExtrudedFaceSolid.ixx>


StepShape_ExtrudedFaceSolid::StepShape_ExtrudedFaceSolid ()  {}

void StepShape_ExtrudedFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea)
{

  StepShape_SweptFaceSolid::Init(aName, aSweptArea);
}

void StepShape_ExtrudedFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea,
	const Handle(StepGeom_Direction)& aExtrudedDirection,
	const Standard_Real aDepth)
{
  // --- classe own fields ---
  extrudedDirection = aExtrudedDirection;
  depth = aDepth;
  // --- classe inherited fields ---
  StepShape_SweptFaceSolid::Init(aName, aSweptArea);
}


void StepShape_ExtrudedFaceSolid::SetExtrudedDirection(const Handle(StepGeom_Direction)& aExtrudedDirection)
{
  extrudedDirection = aExtrudedDirection;
}

Handle(StepGeom_Direction) StepShape_ExtrudedFaceSolid::ExtrudedDirection() const
{
  return extrudedDirection;
}

void StepShape_ExtrudedFaceSolid::SetDepth(const Standard_Real aDepth)
{
  depth = aDepth;
}

Standard_Real StepShape_ExtrudedFaceSolid::Depth() const
{
  return depth;
}
