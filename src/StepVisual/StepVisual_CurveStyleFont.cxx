// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
