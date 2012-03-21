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

#include <StepGeom_CartesianTransformationOperator3d.ixx>


StepGeom_CartesianTransformationOperator3d::StepGeom_CartesianTransformationOperator3d ()  {}

void StepGeom_CartesianTransformationOperator3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAaxis1,
	const Handle(StepGeom_Direction)& aAxis1,
	const Standard_Boolean hasAaxis2,
	const Handle(StepGeom_Direction)& aAxis2,
	const Handle(StepGeom_CartesianPoint)& aLocalOrigin,
	const Standard_Boolean hasAscale,
	const Standard_Real aScale)
{

	StepGeom_CartesianTransformationOperator::Init(aName, hasAaxis1, aAxis1, hasAaxis2, aAxis2, aLocalOrigin, hasAscale, aScale);
}

void StepGeom_CartesianTransformationOperator3d::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Boolean hasAaxis1,
	const Handle(StepGeom_Direction)& aAxis1,
	const Standard_Boolean hasAaxis2,
	const Handle(StepGeom_Direction)& aAxis2,
	const Handle(StepGeom_CartesianPoint)& aLocalOrigin,
	const Standard_Boolean hasAscale,
	const Standard_Real aScale,
	const Standard_Boolean hasAaxis3,
	const Handle(StepGeom_Direction)& aAxis3)
{
	// --- classe own fields ---
	hasAxis3 = hasAaxis3;
	axis3 = aAxis3;
	// --- classe inherited fields ---
	StepGeom_CartesianTransformationOperator::Init(aName, hasAaxis1, aAxis1, hasAaxis2, aAxis2, aLocalOrigin, hasAscale, aScale);
}


void StepGeom_CartesianTransformationOperator3d::SetAxis3(const Handle(StepGeom_Direction)& aAxis3)
{
	axis3 = aAxis3;
	hasAxis3 = Standard_True;
}

void StepGeom_CartesianTransformationOperator3d::UnSetAxis3()
{
	hasAxis3 = Standard_False;
	axis3.Nullify();
}

Handle(StepGeom_Direction) StepGeom_CartesianTransformationOperator3d::Axis3() const
{
	return axis3;
}

Standard_Boolean StepGeom_CartesianTransformationOperator3d::HasAxis3() const
{
	return hasAxis3;
}
