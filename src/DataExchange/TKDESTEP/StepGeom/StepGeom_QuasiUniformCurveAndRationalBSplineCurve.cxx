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
#include <StepGeom_QuasiUniformCurve.hxx>
#include <StepGeom_QuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_RationalBSplineCurve.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_QuasiUniformCurveAndRationalBSplineCurve, StepGeom_BSplineCurve)

StepGeom_QuasiUniformCurveAndRationalBSplineCurve::
  StepGeom_QuasiUniformCurveAndRationalBSplineCurve()
{
}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::Init(
  const occ::handle<TCollection_HAsciiString>&                                  aName,
  const int                                                                     aDegree,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineCurveForm                                               aCurveForm,
  const StepData_Logical                                                        aClosedCurve,
  const StepData_Logical                                                        aSelfIntersect,
  const occ::handle<StepGeom_QuasiUniformCurve>&                                aQuasiUniformCurve,
  const occ::handle<StepGeom_RationalBSplineCurve>& aRationalBSplineCurve)
{
  // --- classe own fields ---
  quasiUniformCurve    = aQuasiUniformCurve;
  rationalBSplineCurve = aRationalBSplineCurve;
  // --- classe inherited fields ---
  StepGeom_BSplineCurve::Init(aName,
                              aDegree,
                              aControlPointsList,
                              aCurveForm,
                              aClosedCurve,
                              aSelfIntersect);
}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::Init(
  const occ::handle<TCollection_HAsciiString>&                                  aName,
  const int                                                                     aDegree,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineCurveForm                                               aCurveForm,
  const StepData_Logical                                                        aClosedCurve,
  const StepData_Logical                                                        aSelfIntersect,
  const occ::handle<NCollection_HArray1<double>>&                               aWeightsData)
{
  // --- classe inherited fields ---

  StepGeom_BSplineCurve::Init(aName,
                              aDegree,
                              aControlPointsList,
                              aCurveForm,
                              aClosedCurve,
                              aSelfIntersect);

  // --- ANDOR component fields ---

  quasiUniformCurve = new StepGeom_QuasiUniformCurve();
  quasiUniformCurve
    ->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);

  // --- ANDOR component fields ---

  rationalBSplineCurve = new StepGeom_RationalBSplineCurve();
  rationalBSplineCurve->Init(aName,
                             aDegree,
                             aControlPointsList,
                             aCurveForm,
                             aClosedCurve,
                             aSelfIntersect,
                             aWeightsData);
}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::SetQuasiUniformCurve(
  const occ::handle<StepGeom_QuasiUniformCurve>& aQuasiUniformCurve)
{
  quasiUniformCurve = aQuasiUniformCurve;
}

occ::handle<StepGeom_QuasiUniformCurve> StepGeom_QuasiUniformCurveAndRationalBSplineCurve::
  QuasiUniformCurve() const
{
  return quasiUniformCurve;
}

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::SetRationalBSplineCurve(
  const occ::handle<StepGeom_RationalBSplineCurve>& aRationalBSplineCurve)
{
  rationalBSplineCurve = aRationalBSplineCurve;
}

occ::handle<StepGeom_RationalBSplineCurve> StepGeom_QuasiUniformCurveAndRationalBSplineCurve::
  RationalBSplineCurve() const
{
  return rationalBSplineCurve;
}

//--- Specific Methods for AND classe field access ---

//--- Specific Methods for AND classe field access ---

void StepGeom_QuasiUniformCurveAndRationalBSplineCurve::SetWeightsData(
  const occ::handle<NCollection_HArray1<double>>& aWeightsData)
{
  rationalBSplineCurve->SetWeightsData(aWeightsData);
}

occ::handle<NCollection_HArray1<double>> StepGeom_QuasiUniformCurveAndRationalBSplineCurve::
  WeightsData() const
{
  return rationalBSplineCurve->WeightsData();
}

double StepGeom_QuasiUniformCurveAndRationalBSplineCurve::WeightsDataValue(const int num) const
{
  return rationalBSplineCurve->WeightsDataValue(num);
}

int StepGeom_QuasiUniformCurveAndRationalBSplineCurve::NbWeightsData() const
{
  return rationalBSplineCurve->NbWeightsData();
}
