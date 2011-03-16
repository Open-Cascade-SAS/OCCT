#include <StepShape_ShellBasedSurfaceModel.ixx>


StepShape_ShellBasedSurfaceModel::StepShape_ShellBasedSurfaceModel ()  {}

void StepShape_ShellBasedSurfaceModel::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_ShellBasedSurfaceModel::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfShell)& aSbsmBoundary)
{
	// --- classe own fields ---
	sbsmBoundary = aSbsmBoundary;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_ShellBasedSurfaceModel::SetSbsmBoundary(const Handle(StepShape_HArray1OfShell)& aSbsmBoundary)
{
	sbsmBoundary = aSbsmBoundary;
}

Handle(StepShape_HArray1OfShell) StepShape_ShellBasedSurfaceModel::SbsmBoundary() const
{
	return sbsmBoundary;
}

StepShape_Shell StepShape_ShellBasedSurfaceModel::SbsmBoundaryValue(const Standard_Integer num) const
{
	return sbsmBoundary->Value(num);
}

Standard_Integer StepShape_ShellBasedSurfaceModel::NbSbsmBoundary () const
{
	return sbsmBoundary->Length();
}
