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
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_CartesianTransformationOperator.hxx>
#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_CartesianTransformationOperator,StepGeom_GeometricRepresentationItem)

StepGeom_CartesianTransformationOperator::StepGeom_CartesianTransformationOperator ()  {}

void StepGeom_CartesianTransformationOperator::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAaxis1,
	const Handle(StepGeom_Direction)& aAxis1,
	const Standard_Boolean hasAaxis2,
	const Handle(StepGeom_Direction)& aAxis2,
	const Handle(StepGeom_CartesianPoint)& aLocalOrigin,
	const Standard_Boolean hasAscale,
	const Standard_Real aScale)
{
	// --- classe own fields ---
	hasAxis1 = hasAaxis1;
	axis1 = aAxis1;
	hasAxis2 = hasAaxis2;
	axis2 = aAxis2;
	localOrigin = aLocalOrigin;
	hasScale = hasAscale;
	scale = aScale;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_CartesianTransformationOperator::SetAxis1(const Handle(StepGeom_Direction)& aAxis1)
{
	axis1 = aAxis1;
	hasAxis1 = Standard_True;
}

void StepGeom_CartesianTransformationOperator::UnSetAxis1()
{
	hasAxis1 = Standard_False;
	axis1.Nullify();
}

Handle(StepGeom_Direction) StepGeom_CartesianTransformationOperator::Axis1() const
{
	return axis1;
}

Standard_Boolean StepGeom_CartesianTransformationOperator::HasAxis1() const
{
	return hasAxis1;
}

void StepGeom_CartesianTransformationOperator::SetAxis2(const Handle(StepGeom_Direction)& aAxis2)
{
	axis2 = aAxis2;
	hasAxis2 = Standard_True;
}

void StepGeom_CartesianTransformationOperator::UnSetAxis2()
{
	hasAxis2 = Standard_False;
	axis2.Nullify();
}

Handle(StepGeom_Direction) StepGeom_CartesianTransformationOperator::Axis2() const
{
	return axis2;
}

Standard_Boolean StepGeom_CartesianTransformationOperator::HasAxis2() const
{
	return hasAxis2;
}

void StepGeom_CartesianTransformationOperator::SetLocalOrigin(const Handle(StepGeom_CartesianPoint)& aLocalOrigin)
{
	localOrigin = aLocalOrigin;
}

Handle(StepGeom_CartesianPoint) StepGeom_CartesianTransformationOperator::LocalOrigin() const
{
	return localOrigin;
}

void StepGeom_CartesianTransformationOperator::SetScale(const Standard_Real aScale)
{
	scale = aScale;
	hasScale = Standard_True;
}

void StepGeom_CartesianTransformationOperator::UnSetScale()
{
	hasScale = Standard_False;
}

Standard_Real StepGeom_CartesianTransformationOperator::Scale() const
{
	return scale;
}

Standard_Boolean StepGeom_CartesianTransformationOperator::HasScale() const
{
	return hasScale;
}
