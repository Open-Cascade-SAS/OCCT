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
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_Direction.hxx>
#include <StepShape_ExtrudedAreaSolid.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_ExtrudedAreaSolid,StepShape_SweptAreaSolid)

StepShape_ExtrudedAreaSolid::StepShape_ExtrudedAreaSolid ()  {}

void StepShape_ExtrudedAreaSolid::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CurveBoundedSurface)& aSweptArea,
	const Handle(StepGeom_Direction)& aExtrudedDirection,
	const Standard_Real aDepth)
{
	// --- classe own fields ---
	extrudedDirection = aExtrudedDirection;
	depth = aDepth;
	// --- classe inherited fields ---
	StepShape_SweptAreaSolid::Init(aName, aSweptArea);
}


void StepShape_ExtrudedAreaSolid::SetExtrudedDirection(const Handle(StepGeom_Direction)& aExtrudedDirection)
{
	extrudedDirection = aExtrudedDirection;
}

Handle(StepGeom_Direction) StepShape_ExtrudedAreaSolid::ExtrudedDirection() const
{
	return extrudedDirection;
}

void StepShape_ExtrudedAreaSolid::SetDepth(const Standard_Real aDepth)
{
	depth = aDepth;
}

Standard_Real StepShape_ExtrudedAreaSolid::Depth() const
{
	return depth;
}
