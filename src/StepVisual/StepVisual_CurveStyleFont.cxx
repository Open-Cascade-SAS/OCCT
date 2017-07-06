// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepVisual_CurveStyleFont.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CurveStyleFont,Standard_Transient)

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
