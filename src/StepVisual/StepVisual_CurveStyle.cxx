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
