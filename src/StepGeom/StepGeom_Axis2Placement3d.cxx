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

#include <StepGeom_Axis2Placement3d.ixx>


StepGeom_Axis2Placement3d::StepGeom_Axis2Placement3d ()  {}

void StepGeom_Axis2Placement3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation)
{

	StepGeom_Placement::Init(aName, aLocation);
}

void StepGeom_Axis2Placement3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aLocation,
	const Standard_Boolean hasAaxis,
	const Handle(StepGeom_Direction)& aAxis,
	const Standard_Boolean hasArefDirection,
	const Handle(StepGeom_Direction)& aRefDirection)
{
	// --- classe own fields ---
	hasAxis = hasAaxis;
	axis = aAxis;
	hasRefDirection = hasArefDirection;
	refDirection = aRefDirection;
	// --- classe inherited fields ---
	StepGeom_Placement::Init(aName, aLocation);
}


void StepGeom_Axis2Placement3d::SetAxis(const Handle(StepGeom_Direction)& aAxis)
{
	axis = aAxis;
	hasAxis = Standard_True;
}

void StepGeom_Axis2Placement3d::UnSetAxis()
{
	hasAxis = Standard_False;
	axis.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement3d::Axis() const
{
	return axis;
}

Standard_Boolean StepGeom_Axis2Placement3d::HasAxis() const
{
	return hasAxis;
}

void StepGeom_Axis2Placement3d::SetRefDirection(const Handle(StepGeom_Direction)& aRefDirection)
{
	refDirection = aRefDirection;
	hasRefDirection = Standard_True;
}

void StepGeom_Axis2Placement3d::UnSetRefDirection()
{
	hasRefDirection = Standard_False;
	refDirection.Nullify();
}

Handle(StepGeom_Direction) StepGeom_Axis2Placement3d::RefDirection() const
{
	return refDirection;
}

Standard_Boolean StepGeom_Axis2Placement3d::HasRefDirection() const
{
	return hasRefDirection;
}
