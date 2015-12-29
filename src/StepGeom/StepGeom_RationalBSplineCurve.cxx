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
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_RationalBSplineCurve,StepGeom_BSplineCurve)

StepGeom_RationalBSplineCurve::StepGeom_RationalBSplineCurve ()  {}

void StepGeom_RationalBSplineCurve::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Standard_Integer aDegree,
	const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList,
	const StepGeom_BSplineCurveForm aCurveForm,
	const StepData_Logical aClosedCurve,
	const StepData_Logical aSelfIntersect,
	const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	// --- classe own fields ---
	weightsData = aWeightsData;
	// --- classe inherited fields ---
	StepGeom_BSplineCurve::Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void StepGeom_RationalBSplineCurve::SetWeightsData(const Handle(TColStd_HArray1OfReal)& aWeightsData)
{
	weightsData = aWeightsData;
}

Handle(TColStd_HArray1OfReal) StepGeom_RationalBSplineCurve::WeightsData() const
{
	return weightsData;
}

Standard_Real StepGeom_RationalBSplineCurve::WeightsDataValue(const Standard_Integer num) const
{
	return weightsData->Value(num);
}

Standard_Integer StepGeom_RationalBSplineCurve::NbWeightsData () const
{
	return weightsData->Length();
}
