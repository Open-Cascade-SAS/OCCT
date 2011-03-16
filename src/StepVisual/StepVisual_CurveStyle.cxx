#include <StepVisual_CurveStyle.ixx>


StepVisual_CurveStyle::StepVisual_CurveStyle ()  {}

void StepVisual_CurveStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepVisual_CurveStyleFontSelect& aCurveFont,
	const StepBasic_SizeSelect& aCurveWidth,
	const Handle(StepVisual_Colour)& aCurveColour)
{
	// --- classe own fields ---
	name = aName;
	curveFont = aCurveFont;
	curveWidth = aCurveWidth;
	curveColour = aCurveColour;
}


void StepVisual_CurveStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_CurveStyle::Name() const
{
	return name;
}

void StepVisual_CurveStyle::SetCurveFont(const StepVisual_CurveStyleFontSelect& aCurveFont)
{
	curveFont = aCurveFont;
}

StepVisual_CurveStyleFontSelect StepVisual_CurveStyle::CurveFont() const
{
	return curveFont;
}

void StepVisual_CurveStyle::SetCurveWidth(const StepBasic_SizeSelect& aCurveWidth)
{
	curveWidth = aCurveWidth;
}

StepBasic_SizeSelect StepVisual_CurveStyle::CurveWidth() const
{
	return curveWidth;
}

void StepVisual_CurveStyle::SetCurveColour(const Handle(StepVisual_Colour)& aCurveColour)
{
	curveColour = aCurveColour;
}

Handle(StepVisual_Colour) StepVisual_CurveStyle::CurveColour() const
{
	return curveColour;
}
