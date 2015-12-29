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
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Axis2Placement3d,StepGeom_Placement)

StepGeom_Axis2Placement3d::StepGeom_Axis2Placement3d ()  {}

void StepGeom_Axis2Placement3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasAaxis,
	const Handle(StepGeom_Direction)& aAxis,
	const Standard_Boolean hasArefDirection,
	const Handle(StepGeom_Direction)& aRefDirection)
{
	// --- classe own fields ---
	hasAxis = hasAaxis;
	axis = aAxis;
	hasRefDirection = hasArefDirection;
	refDirection = aRefDirection;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis2Placement3d::SetAxis(const Handle(StepGeom_Direction)& aAxis)
{
	axis = aAxis;
	hasAxis = Standard_True;
}

void StepGeom_Axis2Placement3d::UnSetAxis()
{
	hasAxis = Standard_False;
	axis.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement3d::Axis() const
{
	return axis;
}

Standard_Boolean StepGeom_Axis2Placement3d::HasAxis() const
{
	return hasAxis;
}

void StepGeom_Axis2Placement3d::SetRefDirection(const Handle(StepGeom_Direction)& aRefDirection)
{
	refDirection = aRefDirection;
	hasRefDirection = Standard_True;
}

void StepGeom_Axis2Placement3d::UnSetRefDirection()
{
	hasRefDirection = Standard_False;
	refDirection.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement3d::RefDirection() const
{
	return refDirection;
}

Standard_Boolean StepGeom_Axis2Placement3d::HasRefDirection() const
{
	return hasRefDirection;
}
