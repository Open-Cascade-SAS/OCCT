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
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Axis1Placement,StepGeom_Placement)

StepGeom_Axis1Placement::StepGeom_Axis1Placement ()  {}

void StepGeom_Axis1Placement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasAaxis,
	const Handle(StepGeom_Direction)& aAxis)
{
	// --- classe own fields ---
	hasAxis = hasAaxis;
	axis = aAxis;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis1Placement::SetAxis(const Handle(StepGeom_Direction)& aAxis)
{
	axis = aAxis;
	hasAxis = Standard_True;
}

void StepGeom_Axis1Placement::UnSetAxis()
{
	hasAxis = Standard_False;
	axis.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis1Placement::Axis() const
{
	return axis;
}

Standard_Boolean StepGeom_Axis1Placement::HasAxis() const
{
	return hasAxis;
}
