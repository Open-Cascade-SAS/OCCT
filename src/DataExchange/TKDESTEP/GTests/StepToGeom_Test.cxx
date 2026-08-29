// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <NCollection_HArray1.hxx>
#include <Precision.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
static occ::handle<StepGeom_BSplineCurveWithKnots> createLinearCurve(const double theFirstKnot)
{
  const occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("curve");
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aPoles =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 2);
  for (int aPoleIdx = 1; aPoleIdx <= 2; ++aPoleIdx)
  {
    occ::handle<StepGeom_CartesianPoint> aPoint = new StepGeom_CartesianPoint();
    aPoint->Init3D(aName, static_cast<double>(aPoleIdx), 0.0, 0.0);
    aPoles->SetValue(aPoleIdx, aPoint);
  }

  occ::handle<NCollection_HArray1<int>> aMultiplicities = new NCollection_HArray1<int>(1, 2);
  aMultiplicities->SetValue(1, 2);
  aMultiplicities->SetValue(2, 2);
  occ::handle<NCollection_HArray1<double>> aKnots = new NCollection_HArray1<double>(1, 2);
  aKnots->SetValue(1, theFirstKnot);
  aKnots->SetValue(2, 1.0);

  occ::handle<StepGeom_BSplineCurveWithKnots> aCurve = new StepGeom_BSplineCurveWithKnots();
  aCurve->Init(aName,
               1,
               aPoles,
               StepGeom_BSplineCurveForm::StepGeom_bscfUnspecified,
               StepData_Logical::StepData_LFalse,
               StepData_Logical::StepData_LFalse,
               aMultiplicities,
               aKnots,
               StepGeom_KnotType::StepGeom_ktUnspecified);
  return aCurve;
}
} // namespace

TEST(StepToGeomTest, RejectsMismatchedBSplineKnotArrays)
{
  const occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("curve");
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aPoles =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 3);
  for (int aPoleIdx = 1; aPoleIdx <= 3; ++aPoleIdx)
  {
    occ::handle<StepGeom_CartesianPoint> aPoint = new StepGeom_CartesianPoint();
    aPoint->Init3D(aName, static_cast<double>(aPoleIdx), 0.0, 0.0);
    aPoles->SetValue(aPoleIdx, aPoint);
  }

  occ::handle<NCollection_HArray1<int>> aMultiplicities = new NCollection_HArray1<int>(1, 2);
  aMultiplicities->SetValue(1, 2);
  aMultiplicities->SetValue(2, 2);
  occ::handle<NCollection_HArray1<double>> aKnots = new NCollection_HArray1<double>(1, 1);
  aKnots->SetValue(1, 0.0);

  occ::handle<StepGeom_BSplineCurveWithKnots> aCurve = new StepGeom_BSplineCurveWithKnots();
  aCurve->Init(aName,
               1,
               aPoles,
               StepGeom_BSplineCurveForm::StepGeom_bscfUnspecified,
               StepData_Logical::StepData_LFalse,
               StepData_Logical::StepData_LFalse,
               aMultiplicities,
               aKnots,
               StepGeom_KnotType::StepGeom_ktUnspecified);
  EXPECT_TRUE(StepToGeom::MakeBSplineCurve(aCurve).IsNull());
}

TEST(StepToGeomTest, RejectsMissingBSplineData)
{
  const occ::handle<StepGeom_BSplineCurveWithKnots> aCurve = new StepGeom_BSplineCurveWithKnots();
  EXPECT_TRUE(StepToGeom::MakeBSplineCurve(aCurve).IsNull());
}

TEST(StepToGeomTest, RejectsInvalidKnotValues)
{
  EXPECT_TRUE(StepToGeom::MakeBSplineCurve(createLinearCurve(Precision::Infinite())).IsNull());
  EXPECT_TRUE(StepToGeom::MakeBSplineCurve(createLinearCurve(-Precision::Infinite())).IsNull());
  EXPECT_TRUE(StepToGeom::MakeBSplineCurve(createLinearCurve(std::nan(""))).IsNull());
}

TEST(StepToGeomTest, RejectsLargeMergedKnotMultiplicities)
{
  const occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("curve");
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aPoles =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 2);
  for (int aPoleIdx = 1; aPoleIdx <= 2; ++aPoleIdx)
  {
    occ::handle<StepGeom_CartesianPoint> aPoint = new StepGeom_CartesianPoint();
    aPoint->Init3D(aName, static_cast<double>(aPoleIdx), 0.0, 0.0);
    aPoles->SetValue(aPoleIdx, aPoint);
  }

  occ::handle<NCollection_HArray1<int>> aMultiplicities = new NCollection_HArray1<int>(1, 3);
  aMultiplicities->SetValue(1, 2000000000);
  aMultiplicities->SetValue(2, 2000000000);
  aMultiplicities->SetValue(3, 2);
  occ::handle<NCollection_HArray1<double>> aKnots = new NCollection_HArray1<double>(1, 3);
  aKnots->SetValue(1, 0.0);
  aKnots->SetValue(2, 0.0);
  aKnots->SetValue(3, 1.0);

  occ::handle<StepGeom_BSplineCurveWithKnots> aCurve = new StepGeom_BSplineCurveWithKnots();
  aCurve->Init(aName,
               1,
               aPoles,
               StepGeom_BSplineCurveForm::StepGeom_bscfUnspecified,
               StepData_Logical::StepData_LFalse,
               StepData_Logical::StepData_LFalse,
               aMultiplicities,
               aKnots,
               StepGeom_KnotType::StepGeom_ktUnspecified);
  EXPECT_TRUE(StepToGeom::MakeBSplineCurve(aCurve).IsNull());
}

TEST(StepToGeomTest, RejectsInvalidInteriorKnotMultiplicity)
{
  const occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("curve");
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aPoles =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 4);
  for (int aPoleIdx = 1; aPoleIdx <= 4; ++aPoleIdx)
  {
    occ::handle<StepGeom_CartesianPoint> aPoint = new StepGeom_CartesianPoint();
    aPoint->Init3D(aName, static_cast<double>(aPoleIdx), 0.0, 0.0);
    aPoles->SetValue(aPoleIdx, aPoint);
  }

  occ::handle<NCollection_HArray1<int>>    aMultiplicities = new NCollection_HArray1<int>(1, 3);
  occ::handle<NCollection_HArray1<double>> aKnots          = new NCollection_HArray1<double>(1, 3);
  for (int aKnotIdx = 1; aKnotIdx <= 3; ++aKnotIdx)
  {
    aMultiplicities->SetValue(aKnotIdx, 2);
    aKnots->SetValue(aKnotIdx, static_cast<double>(aKnotIdx - 1));
  }

  occ::handle<StepGeom_BSplineCurveWithKnots> aCurve = new StepGeom_BSplineCurveWithKnots();
  aCurve->Init(aName,
               1,
               aPoles,
               StepGeom_BSplineCurveForm::StepGeom_bscfUnspecified,
               StepData_Logical::StepData_LFalse,
               StepData_Logical::StepData_LFalse,
               aMultiplicities,
               aKnots,
               StepGeom_KnotType::StepGeom_ktUnspecified);

  occ::handle<Geom_BSplineCurve> aResult;
  EXPECT_NO_THROW(aResult = StepToGeom::MakeBSplineCurve(aCurve));
  EXPECT_TRUE(aResult.IsNull());
}

TEST(StepToGeomTest, ConvertsValidBSplineCurve)
{
  EXPECT_FALSE(StepToGeom::MakeBSplineCurve(createLinearCurve(0.0)).IsNull());
}
