#include <StepVisual_SurfaceStyleSegmentationCurve.ixx>


StepVisual_SurfaceStyleSegmentationCurve::StepVisual_SurfaceStyleSegmentationCurve ()  {}

void StepVisual_SurfaceStyleSegmentationCurve::Init(
	const Handle(StepVisual_CurveStyle)& aStyleOfSegmentationCurve)
{
	// --- classe own fields ---
	styleOfSegmentationCurve = aStyleOfSegmentationCurve;
}


void StepVisual_SurfaceStyleSegmentationCurve::SetStyleOfSegmentationCurve(const Handle(StepVisual_CurveStyle)& aStyleOfSegmentationCurve)
{
	styleOfSegmentationCurve = aStyleOfSegmentationCurve;
}

Handle(StepVisual_CurveStyle) StepVisual_SurfaceStyleSegmentationCurve::StyleOfSegmentationCurve() const
{
	return styleOfSegmentationCurve;
}
