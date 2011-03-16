#include <StepVisual_CurveStyleFont.ixx>


StepVisual_CurveStyleFont::StepVisual_CurveStyleFont ()  {}

void StepVisual_CurveStyleFont::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepVisual_HArray1OfCurveStyleFontPattern)& aPatternList)
{
	// --- classe own fields ---
	name = aName;
	patternList = aPatternList;
}


void StepVisual_CurveStyleFont::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_CurveStyleFont::Name() const
{
	return name;
}

void StepVisual_CurveStyleFont::SetPatternList(const Handle(StepVisual_HArray1OfCurveStyleFontPattern)& aPatternList)
{
	patternList = aPatternList;
}

Handle(StepVisual_HArray1OfCurveStyleFontPattern) StepVisual_CurveStyleFont::PatternList() const
{
	return patternList;
}

Handle(StepVisual_CurveStyleFontPattern) StepVisual_CurveStyleFont::PatternListValue(const Standard_Integer num) const
{
	return patternList->Value(num);
}

Standard_Integer StepVisual_CurveStyleFont::NbPatternList () const
{
	if (patternList.IsNull()) return 0;
	return patternList->Length();
}
