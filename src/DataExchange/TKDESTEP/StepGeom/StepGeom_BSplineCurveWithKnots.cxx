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

#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_BSplineCurveWithKnots, StepGeom_BSplineCurve)

StepGeom_BSplineCurveWithKnots::StepGeom_BSplineCurveWithKnots() {}

void StepGeom_BSplineCurveWithKnots::Init(
  const occ::handle<TCollection_HAsciiString>&         aName,
  const int                          aDegree,
  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList,
  const StepGeom_BSplineCurveForm                 aCurveForm,
  const StepData_Logical                          aClosedCurve,
  const StepData_Logical                          aSelfIntersect,
  const occ::handle<NCollection_HArray1<int>>&         aKnotMultiplicities,
  const occ::handle<NCollection_HArray1<double>>&            aKnots,
  const StepGeom_KnotType                         aKnotSpec)
{
  // --- classe own fields ---
  knotMultiplicities = aKnotMultiplicities;
  knots              = aKnots;
  knotSpec           = aKnotSpec;
  // --- classe inherited fields ---
  StepGeom_BSplineCurve::Init(aName,
                              aDegree,
                              aControlPointsList,
                              aCurveForm,
                              aClosedCurve,
                              aSelfIntersect);
}

void StepGeom_BSplineCurveWithKnots::SetKnotMultiplicities(
  const occ::handle<NCollection_HArray1<int>>& aKnotMultiplicities)
{
  knotMultiplicities = aKnotMultiplicities;
}

occ::handle<NCollection_HArray1<int>> StepGeom_BSplineCurveWithKnots::KnotMultiplicities() const
{
  return knotMultiplicities;
}

int StepGeom_BSplineCurveWithKnots::KnotMultiplicitiesValue(
  const int num) const
{
  return knotMultiplicities->Value(num);
}

int StepGeom_BSplineCurveWithKnots::NbKnotMultiplicities() const
{
  return knotMultiplicities->Length();
}

void StepGeom_BSplineCurveWithKnots::SetKnots(const occ::handle<NCollection_HArray1<double>>& aKnots)
{
  knots = aKnots;
}

occ::handle<NCollection_HArray1<double>> StepGeom_BSplineCurveWithKnots::Knots() const
{
  return knots;
}

double StepGeom_BSplineCurveWithKnots::KnotsValue(const int num) const
{
  return knots->Value(num);
}

int StepGeom_BSplineCurveWithKnots::NbKnots() const
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
