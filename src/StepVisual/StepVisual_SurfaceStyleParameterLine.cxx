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
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_DirectionCountSelect.hxx>
#include <StepVisual_SurfaceStyleParameterLine.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleParameterLine,Standard_Transient)

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
