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
#include <StepGeom_Hyperbola.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Hyperbola,StepGeom_Conic)

StepGeom_Hyperbola::StepGeom_Hyperbola ()  {}

void StepGeom_Hyperbola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition,
	const Standard_Real aSemiAxis,
	const Standard_Real aSemiImagAxis)
{
	// --- classe own fields ---
	semiAxis = aSemiAxis;
	semiImagAxis = aSemiImagAxis;
	// --- classe inherited fields ---
	StepGeom_Conic::Init(aName, aPosition);
}


void StepGeom_Hyperbola::SetSemiAxis(const Standard_Real aSemiAxis)
{
	semiAxis = aSemiAxis;
}

Standard_Real StepGeom_Hyperbola::SemiAxis() const
{
	return semiAxis;
}

void StepGeom_Hyperbola::SetSemiImagAxis(const Standard_Real aSemiImagAxis)
{
	semiImagAxis = aSemiImagAxis;
}

Standard_Real StepGeom_Hyperbola::SemiImagAxis() const
{
	return semiImagAxis;
}
