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

#include <StepGeom_RationalBSplineCurve.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_RationalBSplineCurve, StepGeom_BSplineCurve)

StepGeom_RationalBSplineCurve::StepGeom_RationalBSplineCurve() {}

void StepGeom_RationalBSplineCurve::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aDegree,
  const occ::handle<StepGeom_HArray1OfCartesianPoint>& aControlPointsList,
  const StepGeom_BSplineCurveForm                 aCurveForm,
  const StepData_Logical                          aClosedCurve,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<TColStd_HArray1OfReal>&            aWeightsData)
{
  // --- classe own fields ---
  weightsData = aWeightsData;
  // --- classe inherited fields ---
  StepGeom_BSplineCurve::Init(aName,
                              aDegree,
                              aControlPointsList,
                              aCurveForm,
                              aClosedCurve,
                              aSelfIntersect);
}

void StepGeom_RationalBSplineCurve::SetWeightsData(
  const occ::handle<TColStd_HArray1OfReal>& aWeightsData)
{
  weightsData = aWeightsData;
}

occ::handle<TColStd_HArray1OfReal> StepGeom_RationalBSplineCurve::WeightsData() const
{
  return weightsData;
}

double StepGeom_RationalBSplineCurve::WeightsDataValue(const int num) const
{
  return weightsData->Value(num);
}

int StepGeom_RationalBSplineCurve::NbWeightsData() const
{
  return weightsData->Length();
}
