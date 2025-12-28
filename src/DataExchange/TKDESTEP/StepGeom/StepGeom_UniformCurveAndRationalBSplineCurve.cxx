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

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_UniformCurveAndRationalBSplineCurve, StepGeom_BSplineCurve)

StepGeom_UniformCurveAndRationalBSplineCurve::StepGeom_UniformCurveAndRationalBSplineCurve() {}

void StepGeom_UniformCurveAndRationalBSplineCurve::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aDegree,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineCurveForm                 aCurveForm,
  const StepData_Logical                          aClosedCurve,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<StepGeom_UniformCurve>&            aUniformCurve,
  const occ::handle<StepGeom_RationalBSplineCurve>&    aRationalBSplineCurve)
{
  // --- classe own fields ---
  uniformCurve         = aUniformCurve;
  rationalBSplineCurve = aRationalBSplineCurve;
  // --- classe inherited fields ---
  StepGeom_BSplineCurve::Init(aName,
                              aDegree,
                              aControlPointsList,
                              aCurveForm,
                              aClosedCurve,
                              aSelfIntersect);
}

void StepGeom_UniformCurveAndRationalBSplineCurve::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aDegree,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineCurveForm                 aCurveForm,
  const StepData_Logical                          aClosedCurve,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<NCollection_HArray1<double>>&            aWeightsData)
{
  // --- classe inherited fields ---

  StepGeom_BSplineCurve::Init(aName,
                              aDegree,
                              aControlPointsList,
                              aCurveForm,
                              aClosedCurve,
                              aSelfIntersect);

  // --- ANDOR component fields ---

  rationalBSplineCurve = new StepGeom_RationalBSplineCurve();
  rationalBSplineCurve->Init(aName,
                             aDegree,
                             aControlPointsList,
                             aCurveForm,
                             aClosedCurve,
                             aSelfIntersect,
                             aWeightsData);

  // --- ANDOR component fields ---

  uniformCurve = new StepGeom_UniformCurve();
  uniformCurve->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}

void StepGeom_UniformCurveAndRationalBSplineCurve::SetUniformCurve(
  const occ::handle<StepGeom_UniformCurve>& aUniformCurve)
{
  uniformCurve = aUniformCurve;
}

occ::handle<StepGeom_UniformCurve> StepGeom_UniformCurveAndRationalBSplineCurve::UniformCurve() const
{
  return uniformCurve;
}

void StepGeom_UniformCurveAndRationalBSplineCurve::SetRationalBSplineCurve(
  const occ::handle<StepGeom_RationalBSplineCurve>& aRationalBSplineCurve)
{
  rationalBSplineCurve = aRationalBSplineCurve;
}

occ::handle<StepGeom_RationalBSplineCurve> StepGeom_UniformCurveAndRationalBSplineCurve::
  RationalBSplineCurve() const
{
  return rationalBSplineCurve;
}

//--- Specific Methods for AND classe field access ---

void StepGeom_UniformCurveAndRationalBSplineCurve::SetWeightsData(
  const occ::handle<NCollection_HArray1<double>>& aWeightsData)
{
  rationalBSplineCurve->SetWeightsData(aWeightsData);
}

occ::handle<NCollection_HArray1<double>> StepGeom_UniformCurveAndRationalBSplineCurve::WeightsData() const
{
  return rationalBSplineCurve->WeightsData();
}

double StepGeom_UniformCurveAndRationalBSplineCurve::WeightsDataValue(
  const int num) const
{
  return rationalBSplineCurve->WeightsDataValue(num);
}

int StepGeom_UniformCurveAndRationalBSplineCurve::NbWeightsData() const
{
  return rationalBSplineCurve->NbWeightsData();
}

//--- Specific Methods for AND classe field access ---
