#include <StepVisual_SurfaceStyleUsage.ixx>


StepVisual_SurfaceStyleUsage::StepVisual_SurfaceStyleUsage ()  {}

void StepVisual_SurfaceStyleUsage::Init(
	const StepVisual_SurfaceSide aSide,
	const Handle(StepVisual_SurfaceSideStyle)& aStyle)
{
	// --- classe own fields ---
	side = aSide;
	style = aStyle;
}


void StepVisual_SurfaceStyleUsage::SetSide(const StepVisual_SurfaceSide aSide)
{
	side = aSide;
}

StepVisual_SurfaceSide StepVisual_SurfaceStyleUsage::Side() const
{
	return side;
}

void StepVisual_SurfaceStyleUsage::SetStyle(const Handle(StepVisual_SurfaceSideStyle)& aStyle)
{
	style = aStyle;
}

Handle(StepVisual_SurfaceSideStyle) StepVisual_SurfaceStyleUsage::Style() const
{
	return style;
}
