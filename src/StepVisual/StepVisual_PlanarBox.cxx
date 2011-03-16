#include <StepVisual_PlanarBox.ixx>


StepVisual_PlanarBox::StepVisual_PlanarBox ()  {}

void StepVisual_PlanarBox::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aSizeInX,
	const Standard_Real aSizeInY)
{

	StepVisual_PlanarExtent::Init(aName, aSizeInX, aSizeInY);
}

void StepVisual_PlanarBox::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aSizeInX,
	const Standard_Real aSizeInY,
	const StepGeom_Axis2Placement& aPlacement)
{
	// --- classe own fields ---
	placement = aPlacement;
	// --- classe inherited fields ---
	StepVisual_PlanarExtent::Init(aName, aSizeInX, aSizeInY);
}


void StepVisual_PlanarBox::SetPlacement(const StepGeom_Axis2Placement& aPlacement)
{
	placement = aPlacement;
}

StepGeom_Axis2Placement StepVisual_PlanarBox::Placement() const
{
	return placement;
}
