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
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepShape_RevolvedAreaSolid.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_RevolvedAreaSolid,StepShape_SweptAreaSolid)

StepShape_RevolvedAreaSolid::StepShape_RevolvedAreaSolid ()  {}

void StepShape_RevolvedAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea,
	const Handle(StepGeom_Axis1Placement)& aAxis,
	const Standard_Real aAngle)
{
	// --- classe own fields ---
	axis = aAxis;
	angle = aAngle;
	// --- classe inherited fields ---
	StepShape_SweptAreaSolid::Init(aName, aSweptArea);
}


void StepShape_RevolvedAreaSolid::SetAxis(const Handle(StepGeom_Axis1Placement)& aAxis)
{
	axis = aAxis;
}

Handle(StepGeom_Axis1Placement) StepShape_RevolvedAreaSolid::Axis() const
{
	return axis;
}

void StepShape_RevolvedAreaSolid::SetAngle(const Standard_Real aAngle)
{
	angle = aAngle;
}

Standard_Real StepShape_RevolvedAreaSolid::Angle() const
{
	return angle;
}
