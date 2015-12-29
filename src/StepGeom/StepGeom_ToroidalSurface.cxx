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
#include <StepGeom_ToroidalSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_ToroidalSurface,StepGeom_ElementarySurface)

StepGeom_ToroidalSurface::StepGeom_ToroidalSurface ()  {}

void StepGeom_ToroidalSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius)
{
	// --- classe own fields ---
	majorRadius = aMajorRadius;
	minorRadius = aMinorRadius;
	// --- classe inherited fields ---
	StepGeom_ElementarySurface::Init(aName, aPosition);
}


void StepGeom_ToroidalSurface::SetMajorRadius(const Standard_Real aMajorRadius)
{
	majorRadius = aMajorRadius;
}

Standard_Real StepGeom_ToroidalSurface::MajorRadius() const
{
	return majorRadius;
}

void StepGeom_ToroidalSurface::SetMinorRadius(const Standard_Real aMinorRadius)
{
	minorRadius = aMinorRadius;
}

Standard_Real StepGeom_ToroidalSurface::MinorRadius() const
{
	return minorRadius;
}
