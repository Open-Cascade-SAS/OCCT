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
#include <StepGeom_Axis2Placement.hxx>
#include <StepGeom_Parabola.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Parabola,StepGeom_Conic)

StepGeom_Parabola::StepGeom_Parabola ()  {}

void StepGeom_Parabola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition,
	const Standard_Real aFocalDist)
{
	// --- classe own fields ---
	focalDist = aFocalDist;
	// --- classe inherited fields ---
	StepGeom_Conic::Init(aName, aPosition);
}


void StepGeom_Parabola::SetFocalDist(const Standard_Real aFocalDist)
{
	focalDist = aFocalDist;
}

Standard_Real StepGeom_Parabola::FocalDist() const
{
	return focalDist;
}
