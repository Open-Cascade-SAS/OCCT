#include <StepShape_BrepWithVoids.ixx>


StepShape_BrepWithVoids::StepShape_BrepWithVoids ()  {}

void StepShape_BrepWithVoids::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aOuter)
{

	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}

void StepShape_BrepWithVoids::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aOuter,
	const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	// --- classe own fields ---
	voids = aVoids;
	// --- classe inherited fields ---
	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}


void StepShape_BrepWithVoids::SetVoids(const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	voids = aVoids;
}

Handle(StepShape_HArray1OfOrientedClosedShell) StepShape_BrepWithVoids::Voids() const
{
	return voids;
}

Handle(StepShape_OrientedClosedShell) StepShape_BrepWithVoids::VoidsValue(const Standard_Integer num) const
{
	return voids->Value(num);
}

Standard_Integer StepShape_BrepWithVoids::NbVoids () const
{
	if (voids.IsNull()) return 0;
	return voids->Length();
}
