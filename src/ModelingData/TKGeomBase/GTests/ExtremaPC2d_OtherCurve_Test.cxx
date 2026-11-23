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

#include <ExtremaPC2d_OtherCurve.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2dEval_SineWaveCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;
}

TEST(ExtremaPC2d_OtherCurveTest, SineWaveFindsOriginMinimum)
{
  occ::handle<Geom2dEval_SineWaveCurve> aCurve =
    new Geom2dEval_SineWaveCurve(gp_Ax2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), 1.0, 1.0);
  Geom2dAdaptor_Curve        anAdaptor(aCurve, -M_PI, M_PI);
  ExtremaPC2d_OtherCurve     anEvaluator(anAdaptor, {-M_PI, M_PI});
  const ExtremaPC2d::Result& aResult =
    anEvaluator.Perform(gp_Pnt2d(-1.0, 1.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_GE(aResult.NbExt(), 1);
  bool hasOrigin = false;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    hasOrigin = hasOrigin || std::abs(aResult[anIndex].Parameter) <= THE_TOL;
    EXPECT_NEAR(aResult[anIndex].Point.Distance(aCurve->Value(aResult[anIndex].Parameter)),
                0.0,
                THE_TOL);
  }
  EXPECT_TRUE(hasOrigin);
}

TEST(ExtremaPC2d_OtherCurveTest, MultipleWavesReturnMultipleExtrema)
{
  occ::handle<Geom2dEval_SineWaveCurve> aCurve =
    new Geom2dEval_SineWaveCurve(gp_Ax2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), 1.0, 3.0);
  Geom2dAdaptor_Curve        anAdaptor(aCurve, -2.0, 2.0);
  ExtremaPC2d_OtherCurve     anEvaluator(anAdaptor, {-2.0, 2.0});
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 5);
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    gp_Pnt2d aPoint;
    gp_Vec2d aDerivative;
    aCurve->D1(aResult[anIndex].Parameter, aPoint, aDerivative);
    EXPECT_NEAR(gp_Vec2d(gp_Pnt2d(), aPoint).Dot(aDerivative), 0.0, 1.0e-6);
  }
}

TEST(ExtremaPC2d_OtherCurveTest, EndpointsAndModesAreForwarded)
{
  occ::handle<Geom2dEval_SineWaveCurve> aCurve =
    new Geom2dEval_SineWaveCurve(gp_Ax2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), 1.0, 1.0);
  Geom2dAdaptor_Curve        anAdaptor(aCurve, -1.0, 1.0);
  ExtremaPC2d_OtherCurve     anEvaluator(anAdaptor, {-1.0, 1.0});
  const ExtremaPC2d::Result& aMin =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(3.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  for (size_t anIndex = 0; anIndex < aMin.NbExt(); ++anIndex)
  {
    EXPECT_TRUE(aMin[anIndex].IsMinimum);
  }
  const ExtremaPC2d::Result& aMax =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(3.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  for (size_t anIndex = 0; anIndex < aMax.NbExt(); ++anIndex)
  {
    EXPECT_TRUE(aMax[anIndex].IsMaximum);
  }
}

TEST(ExtremaPC2d_OtherCurveTest, NaturalAndInvalidDomains)
{
  occ::handle<Geom2dEval_SineWaveCurve> aCurve =
    new Geom2dEval_SineWaveCurve(gp_Ax2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), 1.0, 1.0);
  Geom2dAdaptor_Curve    anAdaptor(aCurve);
  ExtremaPC2d_OtherCurve aNatural(anAdaptor);
  EXPECT_EQ(aNatural.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  ExtremaPC2d_OtherCurve anInvalid(anAdaptor, {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
}

TEST(ExtremaPC2d_OtherCurveTest, EndpointPathRefreshesPartitionAfterMutation)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 26);
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    const double aParameter  = static_cast<double>(anIndex) / (aPoles.Size() - 1);
    aPoles.ChangeAt(anIndex) = gp_Pnt2d(aParameter, 0.0);
  }
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_OtherCurve          aReusedEvaluator(anAdaptor, {0.0, 1.0});

  for (size_t anIndex = 1; anIndex + 1 < aPoles.Size(); ++anIndex)
  {
    const int    aPoleNumber = static_cast<int>(anIndex + 1);
    const double aParameter  = static_cast<double>(anIndex) / (aPoles.Size() - 1);
    aCurve->SetPole(aPoleNumber, gp_Pnt2d(aParameter, aPoleNumber % 2 == 0 ? 4.0 : -4.0));
  }

  const gp_Pnt2d             aQuery(0.5, 0.25);
  const ExtremaPC2d::Result& aReusedResult = aReusedEvaluator.PerformWithEndpoints(aQuery, THE_TOL);
  ExtremaPC2d_OtherCurve     aFreshEvaluator(anAdaptor, {0.0, 1.0});
  const ExtremaPC2d::Result& aFreshResult = aFreshEvaluator.PerformWithEndpoints(aQuery, THE_TOL);

  ASSERT_EQ(aReusedResult.Status, aFreshResult.Status);
  ASSERT_EQ(aReusedResult.NbExt(), aFreshResult.NbExt());
  for (size_t anIndex = 0; anIndex < aFreshResult.NbExt(); ++anIndex)
  {
    EXPECT_NEAR(aReusedResult[anIndex].Parameter, aFreshResult[anIndex].Parameter, 1.0e-7);
    EXPECT_NEAR(aReusedResult[anIndex].SquareDistance,
                aFreshResult[anIndex].SquareDistance,
                1.0e-7);
  }
}
