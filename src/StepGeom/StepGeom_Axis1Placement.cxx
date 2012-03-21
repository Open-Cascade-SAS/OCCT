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

#include <StepGeom_Axis1Placement.ixx>


StepGeom_Axis1Placement::StepGeom_Axis1Placement ()  {}

void StepGeom_Axis1Placement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation)
{

	StepGeom_Placement::Init(aName, aLocation);
}

void StepGeom_Axis1Placement::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasAaxis,
	const Handle(StepGeom_Direction)& aAxis)
{
	// --- classe own fields ---
	hasAxis = hasAaxis;
	axis = aAxis;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis1Placement::SetAxis(const Handle(StepGeom_Direction)& aAxis)
{
	axis = aAxis;
	hasAxis = Standard_True;
}

void StepGeom_Axis1Placement::UnSetAxis()
{
	hasAxis = Standard_False;
	axis.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis1Placement::Axis() const
{
	return axis;
}

Standard_Boolean StepGeom_Axis1Placement::HasAxis() const
{
	return hasAxis;
}
