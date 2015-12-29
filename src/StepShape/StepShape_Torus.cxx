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
#include <StepGeom_Axis1Placement.hxx>
#include <StepShape_Torus.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_Torus,StepGeom_GeometricRepresentationItem)

StepShape_Torus::StepShape_Torus ()  {}

void StepShape_Torus::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis1Placement)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius)
{
	// --- classe own fields ---
	position = aPosition;
	majorRadius = aMajorRadius;
	minorRadius = aMinorRadius;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Torus::SetPosition(const Handle(StepGeom_Axis1Placement)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis1Placement) StepShape_Torus::Position() const
{
	return position;
}

void StepShape_Torus::SetMajorRadius(const Standard_Real aMajorRadius)
{
	majorRadius = aMajorRadius;
}

Standard_Real StepShape_Torus::MajorRadius() const
{
	return majorRadius;
}

void StepShape_Torus::SetMinorRadius(const Standard_Real aMinorRadius)
{
	minorRadius = aMinorRadius;
}

Standard_Real StepShape_Torus::MinorRadius() const
{
	return minorRadius;
}
