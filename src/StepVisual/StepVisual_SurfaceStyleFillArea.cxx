#include <StepVisual_SurfaceStyleFillArea.ixx>


StepVisual_SurfaceStyleFillArea::StepVisual_SurfaceStyleFillArea ()  {}

void StepVisual_SurfaceStyleFillArea::Init(
	const Handle(StepVisual_FillAreaStyle)& aFillArea)
{
	// --- classe own fields ---
	fillArea = aFillArea;
}


void StepVisual_SurfaceStyleFillArea::SetFillArea(const Handle(StepVisual_FillAreaStyle)& aFillArea)
{
	fillArea = aFillArea;
}

Handle(StepVisual_FillAreaStyle) StepVisual_SurfaceStyleFillArea::FillArea() const
{
	return fillArea;
}
