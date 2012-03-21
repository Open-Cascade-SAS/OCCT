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

#include <StepGeom_CartesianTransformationOperator.ixx>


StepGeom_CartesianTransformationOperator::StepGeom_CartesianTransformationOperator ()  {}

void StepGeom_CartesianTransformationOperator::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

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
