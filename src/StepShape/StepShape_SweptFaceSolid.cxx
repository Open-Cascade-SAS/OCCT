#include <StepShape_SweptFaceSolid.ixx>


StepShape_SweptFaceSolid::StepShape_SweptFaceSolid ()  {}

void StepShape_SweptFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

  StepRepr_RepresentationItem::Init(aName);
}

void StepShape_SweptFaceSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_FaceSurface)& aSweptArea)
{
  // --- classe own fields ---
  sweptArea = aSweptArea;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(aName);
}


void StepShape_SweptFaceSolid::SetSweptFace(const Handle(StepShape_FaceSurface)& aSweptArea)
{
  sweptArea = aSweptArea;
}

Handle(StepShape_FaceSurface) StepShape_SweptFaceSolid::SweptFace() const
{
  return sweptArea;
}
