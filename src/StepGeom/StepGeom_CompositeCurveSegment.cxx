// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepGeom_CompositeCurveSegment.ixx>


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
