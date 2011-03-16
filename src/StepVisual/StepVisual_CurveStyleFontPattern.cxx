#include <StepVisual_CurveStyleFontPattern.ixx>


StepVisual_CurveStyleFontPattern::StepVisual_CurveStyleFontPattern ()  {}

void StepVisual_CurveStyleFontPattern::Init(
	const Standard_Real aVisibleSegmentLength,
	const Standard_Real aInvisibleSegmentLength)
{
	// --- classe own fields ---
	visibleSegmentLength = aVisibleSegmentLength;
	invisibleSegmentLength = aInvisibleSegmentLength;
}


void StepVisual_CurveStyleFontPattern::SetVisibleSegmentLength(const Standard_Real aVisibleSegmentLength)
{
	visibleSegmentLength = aVisibleSegmentLength;
}

Standard_Real StepVisual_CurveStyleFontPattern::VisibleSegmentLength() const
{
	return visibleSegmentLength;
}

void StepVisual_CurveStyleFontPattern::SetInvisibleSegmentLength(const Standard_Real aInvisibleSegmentLength)
{
	invisibleSegmentLength = aInvisibleSegmentLength;
}

Standard_Real StepVisual_CurveStyleFontPattern::InvisibleSegmentLength() const
{
	return invisibleSegmentLength;
}
