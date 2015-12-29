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
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepShape_RightAngularWedge.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_RightAngularWedge,StepGeom_GeometricRepresentationItem)

StepShape_RightAngularWedge::StepShape_RightAngularWedge ()  {}

void StepShape_RightAngularWedge::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aX,
	const Standard_Real aY,
	const Standard_Real aZ,
	const Standard_Real aLtx)
{
	// --- classe own fields ---
	position = aPosition;
	x = aX;
	y = aY;
	z = aZ;
	ltx = aLtx;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_RightAngularWedge::SetPosition(const Handle(StepGeom_Axis2Placement3d)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis2Placement3d) StepShape_RightAngularWedge::Position() const
{
	return position;
}

void StepShape_RightAngularWedge::SetX(const Standard_Real aX)
{
	x = aX;
}

Standard_Real StepShape_RightAngularWedge::X() const
{
	return x;
}

void StepShape_RightAngularWedge::SetY(const Standard_Real aY)
{
	y = aY;
}

Standard_Real StepShape_RightAngularWedge::Y() const
{
	return y;
}

void StepShape_RightAngularWedge::SetZ(const Standard_Real aZ)
{
	z = aZ;
}

Standard_Real StepShape_RightAngularWedge::Z() const
{
	return z;
}

void StepShape_RightAngularWedge::SetLtx(const Standard_Real aLtx)
{
	ltx = aLtx;
}

Standard_Real StepShape_RightAngularWedge::Ltx() const
{
	return ltx;
}
