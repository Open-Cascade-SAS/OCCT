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

#include <StepGeom_EvaluatedDegeneratePcurve.ixx>


StepGeom_EvaluatedDegeneratePcurve::StepGeom_EvaluatedDegeneratePcurve ()  {}

void StepGeom_EvaluatedDegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve)
{

	StepGeom_DegeneratePcurve::Init(aName, aBasisSurface, aReferenceToCurve);
}

void StepGeom_EvaluatedDegeneratePcurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aBasisSurface,
	const Handle(StepRepr_DefinitionalRepresentation)& aReferenceToCurve,
	const Handle(StepGeom_CartesianPoint)& aEquivalentPoint)
{
	// --- classe own fields ---
	equivalentPoint = aEquivalentPoint;
	// --- classe inherited fields ---
	StepGeom_DegeneratePcurve::Init(aName, aBasisSurface, aReferenceToCurve);
}


void StepGeom_EvaluatedDegeneratePcurve::SetEquivalentPoint(const Handle(StepGeom_CartesianPoint)& aEquivalentPoint)
{
	equivalentPoint = aEquivalentPoint;
}

Handle(StepGeom_CartesianPoint) StepGeom_EvaluatedDegeneratePcurve::EquivalentPoint() const
{
	return equivalentPoint;
}
