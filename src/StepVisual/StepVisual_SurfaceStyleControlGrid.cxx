#include <StepVisual_SurfaceStyleControlGrid.ixx>


StepVisual_SurfaceStyleControlGrid::StepVisual_SurfaceStyleControlGrid ()  {}

void StepVisual_SurfaceStyleControlGrid::Init(
	const Handle(StepVisual_CurveStyle)& aStyleOfControlGrid)
{
	// --- classe own fields ---
	styleOfControlGrid = aStyleOfControlGrid;
}


void StepVisual_SurfaceStyleControlGrid::SetStyleOfControlGrid(const Handle(StepVisual_CurveStyle)& aStyleOfControlGrid)
{
	styleOfControlGrid = aStyleOfControlGrid;
}

Handle(StepVisual_CurveStyle) StepVisual_SurfaceStyleControlGrid::StyleOfControlGrid() const
{
	return styleOfControlGrid;
}
