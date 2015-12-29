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
#include <StepGeom_ConicalSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_ConicalSurface,StepGeom_ElementarySurface)

StepGeom_ConicalSurface::StepGeom_ConicalSurface ()  {}

void StepGeom_ConicalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aRadius,
	const Standard_Real aSemiAngle)
{
	// --- classe own fields ---
	radius = aRadius;
	semiAngle = aSemiAngle;
	// --- classe inherited fields ---
	StepGeom_ElementarySurface::Init(aName, aPosition);
}


void StepGeom_ConicalSurface::SetRadius(const Standard_Real aRadius)
{
	radius = aRadius;
}

Standard_Real StepGeom_ConicalSurface::Radius() const
{
	return radius;
}

void StepGeom_ConicalSurface::SetSemiAngle(const Standard_Real aSemiAngle)
{
	semiAngle = aSemiAngle;
}

Standard_Real StepGeom_ConicalSurface::SemiAngle() const
{
	return semiAngle;
}
