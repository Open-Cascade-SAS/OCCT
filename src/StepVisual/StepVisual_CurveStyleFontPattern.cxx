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
#include <StepVisual_CurveStyleFontPattern.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CurveStyleFontPattern,Standard_Transient)

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
