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

#include <StepShape_RightAngularWedge.ixx>


StepShape_RightAngularWedge::StepShape_RightAngularWedge ()  {}

void StepShape_RightAngularWedge::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_RightAngularWedge::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Axis2Placement3d)& aPosition,
	const Standard_Real aX,
	const Standard_Real aY,
	const Standard_Real aZ,
	const Standard_Real aLtx)
{
	// --- classe own fields ---
	position = aPosition;
	x = aX;
	y = aY;
	z = aZ;
	ltx = aLtx;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_RightAngularWedge::SetPosition(const Handle(StepGeom_Axis2Placement3d)& aPosition)
{
	position = aPosition;
}

Handle(StepGeom_Axis2Placement3d) StepShape_RightAngularWedge::Position() const
{
	return position;
}

void StepShape_RightAngularWedge::SetX(const Standard_Real aX)
{
	x = aX;
}

Standard_Real StepShape_RightAngularWedge::X() const
{
	return x;
}

void StepShape_RightAngularWedge::SetY(const Standard_Real aY)
{
	y = aY;
}

Standard_Real StepShape_RightAngularWedge::Y() const
{
	return y;
}

void StepShape_RightAngularWedge::SetZ(const Standard_Real aZ)
{
	z = aZ;
}

Standard_Real StepShape_RightAngularWedge::Z() const
{
	return z;
}

void StepShape_RightAngularWedge::SetLtx(const Standard_Real aLtx)
{
	ltx = aLtx;
}

Standard_Real StepShape_RightAngularWedge::Ltx() const
{
	return ltx;
}
