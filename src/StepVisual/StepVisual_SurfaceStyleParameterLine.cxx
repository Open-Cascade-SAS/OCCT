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
