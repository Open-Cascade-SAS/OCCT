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

#include <StepGeom_OffsetCurve3d.ixx>


StepGeom_OffsetCurve3d::StepGeom_OffsetCurve3d ()  {}

void StepGeom_OffsetCurve3d::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_OffsetCurve3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aBasisCurve,
	const Standard_Real aDistance,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_Direction)& aRefDirection)
{
	// --- classe own fields ---
	basisCurve = aBasisCurve;
	distance = aDistance;
	selfIntersect = aSelfIntersect;
	refDirection = aRefDirection;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_OffsetCurve3d::SetBasisCurve(const Handle(StepGeom_Curve)& aBasisCurve)
{
	basisCurve = aBasisCurve;
}

Handle(StepGeom_Curve) StepGeom_OffsetCurve3d::BasisCurve() const
{
	return basisCurve;
}

void StepGeom_OffsetCurve3d::SetDistance(const Standard_Real aDistance)
{
	distance = aDistance;
}

Standard_Real StepGeom_OffsetCurve3d::Distance() const
{
	return distance;
}

void StepGeom_OffsetCurve3d::SetSelfIntersect(const StepData_Logical aSelfIntersect)
{
	selfIntersect = aSelfIntersect;
}

StepData_Logical StepGeom_OffsetCurve3d::SelfIntersect() const
{
	return selfIntersect;
}

void StepGeom_OffsetCurve3d::SetRefDirection(const Handle(StepGeom_Direction)& aRefDirection)
{
	refDirection = aRefDirection;
}

Handle(StepGeom_Direction) StepGeom_OffsetCurve3d::RefDirection() const
{
	return refDirection;
}
