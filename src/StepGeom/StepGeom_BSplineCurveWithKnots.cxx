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

#include <StepGeom_BSplineCurveWithKnots.ixx>


StepGeom_BSplineCurveWithKnots::StepGeom_BSplineCurveWithKnots ()  {}

void StepGeom_BSplineCurveWithKnots::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect)
{

	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}

void StepGeom_BSplineCurveWithKnots::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray1OfInteger)& aKnotMultiplicities,
	const Handle(TColStd_HArray1OfReal)& aKnots,
	const StepGeom_KnotType aKnotSpec)
{
	// --- classe own fields ---
	knotMultiplicities = aKnotMultiplicities;
	knots = aKnots;
	knotSpec = aKnotSpec;
	// --- classe inherited fields ---
	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void StepGeom_BSplineCurveWithKnots::SetKnotMultiplicities(const Handle(TColStd_HArray1OfInteger)& aKnotMultiplicities)
{
	knotMultiplicities = aKnotMultiplicities;
}

Handle(TColStd_HArray1OfInteger) StepGeom_BSplineCurveWithKnots::KnotMultiplicities() const
{
	return knotMultiplicities;
}

Standard_Integer StepGeom_BSplineCurveWithKnots::KnotMultiplicitiesValue(const Standard_Integer num) const
{
	return knotMultiplicities->Value(num);
}

Standard_Integer StepGeom_BSplineCurveWithKnots::NbKnotMultiplicities () const
{
	return knotMultiplicities->Length();
}

void StepGeom_BSplineCurveWithKnots::SetKnots(const Handle(TColStd_HArray1OfReal)& aKnots)
{
	knots = aKnots;
}

Handle(TColStd_HArray1OfReal) StepGeom_BSplineCurveWithKnots::Knots() const
{
	return knots;
}

Standard_Real StepGeom_BSplineCurveWithKnots::KnotsValue(const Standard_Integer num) const
{
	return knots->Value(num);
}

Standard_Integer StepGeom_BSplineCurveWithKnots::NbKnots () const
{
	return knots->Length();
}

void StepGeom_BSplineCurveWithKnots::SetKnotSpec(const StepGeom_KnotType aKnotSpec)
{
	knotSpec = aKnotSpec;
}

StepGeom_KnotType StepGeom_BSplineCurveWithKnots::KnotSpec() const
{
	return knotSpec;
}
