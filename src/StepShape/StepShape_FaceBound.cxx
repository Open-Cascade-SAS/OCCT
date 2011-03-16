#include <StepShape_FaceBound.ixx>


StepShape_FaceBound::StepShape_FaceBound ()  {}

void StepShape_FaceBound::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_FaceBound::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Loop)& aBound,
	const Standard_Boolean aOrientation)
{
	// --- classe own fields ---
	bound = aBound;
	orientation = aOrientation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_FaceBound::SetBound(const Handle(StepShape_Loop)& aBound)
{
	bound = aBound;
}

Handle(StepShape_Loop) StepShape_FaceBound::Bound() const
{
	return bound;
}

void StepShape_FaceBound::SetOrientation(const Standard_Boolean aOrientation)
{
	orientation = aOrientation;
}

Standard_Boolean StepShape_FaceBound::Orientation() const
{
	return orientation;
}
