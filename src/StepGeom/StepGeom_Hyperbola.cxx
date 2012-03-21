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

#include <StepGeom_Hyperbola.ixx>


StepGeom_Hyperbola::StepGeom_Hyperbola ()  {}

void StepGeom_Hyperbola::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const StepGeom_Axis2Placement& aPosition)
{

	StepGeom_Conic::Init(aName, aPosition);
}

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
