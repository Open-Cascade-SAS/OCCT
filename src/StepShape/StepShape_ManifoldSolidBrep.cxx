#include <StepShape_ManifoldSolidBrep.ixx>


StepShape_ManifoldSolidBrep::StepShape_ManifoldSolidBrep ()  {}

void StepShape_ManifoldSolidBrep::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_ManifoldSolidBrep::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aOuter)
{
	// --- classe own fields ---
	outer = aOuter;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_ManifoldSolidBrep::SetOuter(const Handle(StepShape_ClosedShell)& aOuter)
{
	outer = aOuter;
}

Handle(StepShape_ClosedShell) StepShape_ManifoldSolidBrep::Outer() const
{
	return outer;
}
