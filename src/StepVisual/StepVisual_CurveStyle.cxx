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
#include <StepBasic_SizeSelect.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_CurveStyleFontSelect.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CurveStyle,Standard_Transient)

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
