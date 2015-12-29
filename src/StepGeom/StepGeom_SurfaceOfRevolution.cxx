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
#include <StepGeom_Curve.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_SurfaceOfRevolution,StepGeom_SweptSurface)

StepGeom_SurfaceOfRevolution::StepGeom_SurfaceOfRevolution ()  {}

void StepGeom_SurfaceOfRevolution::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aSweptCurve,
	const Handle(StepGeom_Axis1Placement)& aAxisPosition)
{
	// --- classe own fields ---
	axisPosition = aAxisPosition;
	// --- classe inherited fields ---
	StepGeom_SweptSurface::Init(aName, aSweptCurve);
}


void StepGeom_SurfaceOfRevolution::SetAxisPosition(const Handle(StepGeom_Axis1Placement)& aAxisPosition)
{
	axisPosition = aAxisPosition;
}

Handle(StepGeom_Axis1Placement) StepGeom_SurfaceOfRevolution::AxisPosition() const
{
	return axisPosition;
}
