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
#include <StepShape_RightCircularCylinder.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_RightCircularCylinder,StepGeom_GeometricRepresentationItem)

StepShape_RightCircularCylinder::StepShape_RightCircularCylinder ()  {}

void StepShape_RightCircularCylinder::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis1Placement)& aPosition,
	const Standard_Real aHeight,
	const Standard_Real aRadius)
{
	// --- classe own fields ---
	position = aPosition;
	height = aHeight;
	radius = aRadius;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_RightCircularCylinder::SetPosition(const Handle(StepGeom_Axis1Placement)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis1Placement) StepShape_RightCircularCylinder::Position() const
{
	return position;
}

void StepShape_RightCircularCylinder::SetHeight(const Standard_Real aHeight)
{
	height = aHeight;
}

Standard_Real StepShape_RightCircularCylinder::Height() const
{
	return height;
}

void StepShape_RightCircularCylinder::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepShape_RightCircularCylinder::Radius() const
{
	return radius;
}
