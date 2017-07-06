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
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_Curve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_CompositeCurveSegment,Standard_Transient)

StepGeom_CompositeCurveSegment::StepGeom_CompositeCurveSegment ()  {}

void StepGeom_CompositeCurveSegment::Init(
	const StepGeom_TransitionCode aTransition,
	const Standard_Boolean aSameSense,
	const Handle(StepGeom_Curve)& aParentCurve)
{
	// --- classe own fields ---
	transition = aTransition;
	sameSense = aSameSense;
	parentCurve = aParentCurve;
}


void StepGeom_CompositeCurveSegment::SetTransition(const StepGeom_TransitionCode aTransition)
{
	transition = aTransition;
}

StepGeom_TransitionCode StepGeom_CompositeCurveSegment::Transition() const
{
	return transition;
}

void StepGeom_CompositeCurveSegment::SetSameSense(const Standard_Boolean aSameSense)
{
	sameSense = aSameSense;
}

Standard_Boolean StepGeom_CompositeCurveSegment::SameSense() const
{
	return sameSense;
}

void StepGeom_CompositeCurveSegment::SetParentCurve(const Handle(StepGeom_Curve)& aParentCurve)
{
	parentCurve = aParentCurve;
}

Handle(StepGeom_Curve) StepGeom_CompositeCurveSegment::ParentCurve() const
{
	return parentCurve;
}
