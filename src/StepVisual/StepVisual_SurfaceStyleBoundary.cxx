#include <StepVisual_SurfaceStyleBoundary.ixx>


StepVisual_SurfaceStyleBoundary::StepVisual_SurfaceStyleBoundary ()  {}

void StepVisual_SurfaceStyleBoundary::Init(
	const Handle(StepVisual_CurveStyle)& aStyleOfBoundary)
{
	// --- classe own fields ---
	styleOfBoundary = aStyleOfBoundary;
}


void StepVisual_SurfaceStyleBoundary::SetStyleOfBoundary(const Handle(StepVisual_CurveStyle)& aStyleOfBoundary)
{
	styleOfBoundary = aStyleOfBoundary;
}

Handle(StepVisual_CurveStyle) StepVisual_SurfaceStyleBoundary::StyleOfBoundary() const
{
	return styleOfBoundary;
}
