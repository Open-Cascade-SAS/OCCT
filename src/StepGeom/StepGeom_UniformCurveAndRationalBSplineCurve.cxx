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
#include <StepGeom_RationalBSplineCurve.hxx>
#include <StepGeom_UniformCurve.hxx>
#include <StepGeom_UniformCurveAndRationalBSplineCurve.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_UniformCurveAndRationalBSplineCurve,StepGeom_BSplineCurve)

StepGeom_UniformCurveAndRationalBSplineCurve::StepGeom_UniformCurveAndRationalBSplineCurve ()  {}

void StepGeom_UniformCurveAndRationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(StepGeom_UniformCurve)& aUniformCurve,
	const Handle(StepGeom_RationalBSplineCurve)& aRationalBSplineCurve)
{
	// --- classe own fields ---
	uniformCurve = aUniformCurve;
	rationalBSplineCurve = aRationalBSplineCurve;
	// --- classe inherited fields ---
	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void StepGeom_UniformCurveAndRationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	// --- classe inherited fields ---

	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);

	// --- ANDOR componant fields ---

	rationalBSplineCurve = new StepGeom_RationalBSplineCurve();
	rationalBSplineCurve->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect, aWeightsData);

	// --- ANDOR componant fields ---

	uniformCurve = new StepGeom_UniformCurve();
	uniformCurve->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void StepGeom_UniformCurveAndRationalBSplineCurve::SetUniformCurve(const Handle(StepGeom_UniformCurve)& aUniformCurve)
{
	uniformCurve = aUniformCurve;
}

Handle(StepGeom_UniformCurve) StepGeom_UniformCurveAndRationalBSplineCurve::UniformCurve() const
{
	return uniformCurve;
}

void StepGeom_UniformCurveAndRationalBSplineCurve::SetRationalBSplineCurve(const Handle(StepGeom_RationalBSplineCurve)& aRationalBSplineCurve)
{
	rationalBSplineCurve = aRationalBSplineCurve;
}

Handle(StepGeom_RationalBSplineCurve) StepGeom_UniformCurveAndRationalBSplineCurve::RationalBSplineCurve() const
{
	return rationalBSplineCurve;
}

	//--- Specific Methods for AND classe field access ---


void StepGeom_UniformCurveAndRationalBSplineCurve::SetWeightsData(const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	rationalBSplineCurve->SetWeightsData(aWeightsData);
}

Handle(TColStd_HArray1OfReal) StepGeom_UniformCurveAndRationalBSplineCurve::WeightsData() const
{
	return rationalBSplineCurve->WeightsData();
}

Standard_Real StepGeom_UniformCurveAndRationalBSplineCurve::WeightsDataValue(const Standard_Integer num) const
{
	return rationalBSplineCurve->WeightsDataValue(num);
}

Standard_Integer StepGeom_UniformCurveAndRationalBSplineCurve::NbWeightsData () const
{
	return rationalBSplineCurve->NbWeightsData();
}

	//--- Specific Methods for AND classe field access ---

