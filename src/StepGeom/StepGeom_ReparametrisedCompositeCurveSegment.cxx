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
#include <StepGeom_Curve.hxx>
#include <StepGeom_ReparametrisedCompositeCurveSegment.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_ReparametrisedCompositeCurveSegment,StepGeom_CompositeCurveSegment)

StepGeom_ReparametrisedCompositeCurveSegment::StepGeom_ReparametrisedCompositeCurveSegment ()  {}

void StepGeom_ReparametrisedCompositeCurveSegment::Init(
	const StepGeom_TransitionCode aTransition,
	const Standard_Boolean aSameSense,
	const Handle(StepGeom_Curve)& aParentCurve,
	const Standard_Real aParamLength)
{
	// --- classe own fields ---
	paramLength = aParamLength;
	// --- classe inherited fields ---
	StepGeom_CompositeCurveSegment::Init(aTransition, aSameSense, aParentCurve);
}


void StepGeom_ReparametrisedCompositeCurveSegment::SetParamLength(const Standard_Real aParamLength)
{
	paramLength = aParamLength;
}

Standard_Real StepGeom_ReparametrisedCompositeCurveSegment::ParamLength() const
{
	return paramLength;
}
