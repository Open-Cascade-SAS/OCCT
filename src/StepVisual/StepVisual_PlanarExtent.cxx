#include <StepVisual_PlanarExtent.ixx>


StepVisual_PlanarExtent::StepVisual_PlanarExtent ()  {}

void StepVisual_PlanarExtent::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepVisual_PlanarExtent::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Real aSizeInX,
	const Standard_Real aSizeInY)
{
	// --- classe own fields ---
	sizeInX = aSizeInX;
	sizeInY = aSizeInY;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepVisual_PlanarExtent::SetSizeInX(const Standard_Real aSizeInX)
{
	sizeInX = aSizeInX;
}

Standard_Real StepVisual_PlanarExtent::SizeInX() const
{
	return sizeInX;
}

void StepVisual_PlanarExtent::SetSizeInY(const Standard_Real aSizeInY)
{
	sizeInY = aSizeInY;
}

Standard_Real StepVisual_PlanarExtent::SizeInY() const
{
	return sizeInY;
}
