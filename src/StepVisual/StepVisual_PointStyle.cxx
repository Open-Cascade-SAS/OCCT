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
#include <StepVisual_MarkerSelect.hxx>
#include <StepVisual_PointStyle.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_PointStyle,Standard_Transient)

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
