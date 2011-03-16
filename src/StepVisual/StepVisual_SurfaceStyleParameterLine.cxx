#include <StepVisual_SurfaceStyleParameterLine.ixx>


StepVisual_SurfaceStyleParameterLine::StepVisual_SurfaceStyleParameterLine ()  {}

void StepVisual_SurfaceStyleParameterLine::Init(
	const Handle(StepVisual_CurveStyle)& aStyleOfParameterLines,
	const Handle(StepVisual_HArray1OfDirectionCountSelect)& aDirectionCounts)
{
	// --- classe own fields ---
	styleOfParameterLines = aStyleOfParameterLines;
	directionCounts = aDirectionCounts;
}


void StepVisual_SurfaceStyleParameterLine::SetStyleOfParameterLines(const Handle(StepVisual_CurveStyle)& aStyleOfParameterLines)
{
	styleOfParameterLines = aStyleOfParameterLines;
}

Handle(StepVisual_CurveStyle) StepVisual_SurfaceStyleParameterLine::StyleOfParameterLines() const
{
	return styleOfParameterLines;
}

void StepVisual_SurfaceStyleParameterLine::SetDirectionCounts(const Handle(StepVisual_HArray1OfDirectionCountSelect)& aDirectionCounts)
{
	directionCounts = aDirectionCounts;
}

Handle(StepVisual_HArray1OfDirectionCountSelect) StepVisual_SurfaceStyleParameterLine::DirectionCounts() const
{
	return directionCounts;
}

StepVisual_DirectionCountSelect StepVisual_SurfaceStyleParameterLine::DirectionCountsValue(const Standard_Integer num) const
{
	return directionCounts->Value(num);
}

Standard_Integer StepVisual_SurfaceStyleParameterLine::NbDirectionCounts () const
{
	return directionCounts->Length();
}
