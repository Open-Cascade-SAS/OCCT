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
#include <StepGeom_Direction.hxx>
#include <StepGeom_OffsetCurve3d.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_OffsetCurve3d,StepGeom_Curve)

StepGeom_OffsetCurve3d::StepGeom_OffsetCurve3d ()  {}

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
