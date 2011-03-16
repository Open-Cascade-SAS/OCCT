#include <StepVisual_SurfaceStyleSilhouette.ixx>


StepVisual_SurfaceStyleSilhouette::StepVisual_SurfaceStyleSilhouette ()  {}

void StepVisual_SurfaceStyleSilhouette::Init(
	const Handle(StepVisual_CurveStyle)& aStyleOfSilhouette)
{
	// --- classe own fields ---
	styleOfSilhouette = aStyleOfSilhouette;
}


void StepVisual_SurfaceStyleSilhouette::SetStyleOfSilhouette(const Handle(StepVisual_CurveStyle)& aStyleOfSilhouette)
{
	styleOfSilhouette = aStyleOfSilhouette;
}

Handle(StepVisual_CurveStyle) StepVisual_SurfaceStyleSilhouette::StyleOfSilhouette() const
{
	return styleOfSilhouette;
}
