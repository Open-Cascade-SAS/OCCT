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

#include <StepVisual_PointStyle.ixx>


StepVisual_PointStyle::StepVisual_PointStyle ()  {}

void StepVisual_PointStyle::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepVisual_MarkerSelect& aMarker,
	const StepBasic_SizeSelect& aMarkerSize,
	const Handle(StepVisual_Colour)& aMarkerColour)
{
	// --- classe own fields ---
	name = aName;
	marker = aMarker;
	markerSize = aMarkerSize;
	markerColour = aMarkerColour;
}


void StepVisual_PointStyle::SetName(const Handle(TCollection_HAsciiString)& aName)
{
	name = aName;
}

Handle(TCollection_HAsciiString) StepVisual_PointStyle::Name() const
{
	return name;
}

void StepVisual_PointStyle::SetMarker(const StepVisual_MarkerSelect& aMarker)
{
	marker = aMarker;
}

StepVisual_MarkerSelect StepVisual_PointStyle::Marker() const
{
	return marker;
}

void StepVisual_PointStyle::SetMarkerSize(const StepBasic_SizeSelect& aMarkerSize)
{
	markerSize = aMarkerSize;
}

StepBasic_SizeSelect StepVisual_PointStyle::MarkerSize() const
{
	return markerSize;
}

void StepVisual_PointStyle::SetMarkerColour(const Handle(StepVisual_Colour)& aMarkerColour)
{
	markerColour = aMarkerColour;
}

Handle(StepVisual_Colour) StepVisual_PointStyle::MarkerColour() const
{
	return markerColour;
}
