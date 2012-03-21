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

#include <StepShape_Torus.ixx>


StepShape_Torus::StepShape_Torus ()  {}

void StepShape_Torus::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_Torus::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis1Placement)& aPosition,
	const Standard_Real aMajorRadius,
	const Standard_Real aMinorRadius)
{
	// --- classe own fields ---
	position = aPosition;
	majorRadius = aMajorRadius;
	minorRadius = aMinorRadius;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_Torus::SetPosition(const Handle(StepGeom_Axis1Placement)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis1Placement) StepShape_Torus::Position() const
{
	return position;
}

void StepShape_Torus::SetMajorRadius(const Standard_Real aMajorRadius)
{
	majorRadius = aMajorRadius;
}

Standard_Real StepShape_Torus::MajorRadius() const
{
	return majorRadius;
}

void StepShape_Torus::SetMinorRadius(const Standard_Real aMinorRadius)
{
	minorRadius = aMinorRadius;
}

Standard_Real StepShape_Torus::MinorRadius() const
{
	return minorRadius;
}
