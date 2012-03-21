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

#include <StepGeom_CurveReplica.ixx>


StepGeom_CurveReplica::StepGeom_CurveReplica ()  {}

void StepGeom_CurveReplica::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepGeom_CurveReplica::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Curve)& aParentCurve,
	const Handle(StepGeom_CartesianTransformationOperator)& aTransformation)
{
	// --- classe own fields ---
	parentCurve = aParentCurve;
	transformation = aTransformation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_CurveReplica::SetParentCurve(const Handle(StepGeom_Curve)& aParentCurve)
{
	parentCurve = aParentCurve;
}

Handle(StepGeom_Curve) StepGeom_CurveReplica::ParentCurve() const
{
	return parentCurve;
}

void StepGeom_CurveReplica::SetTransformation(const Handle(StepGeom_CartesianTransformationOperator)& aTransformation)
{
	transformation = aTransformation;
}

Handle(StepGeom_CartesianTransformationOperator) StepGeom_CurveReplica::Transformation() const
{
	return transformation;
}
