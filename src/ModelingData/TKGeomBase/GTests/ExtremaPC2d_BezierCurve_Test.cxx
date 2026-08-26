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

#include <ExtremaPC2d_BezierCurve.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

occ::handle<Geom2d_BezierCurve> makeLinear()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(10.0, 0.0);
  return new Geom2d_BezierCurve(aPoles);
}

occ::handle<Geom2d_BezierCurve> makeQuadratic()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(-2.0, 0.0);
  aPoles(2) = gp_Pnt2d(0.0, 2.0);
  aPoles(3) = gp_Pnt2d(2.0, 0.0);
  return new Geom2d_BezierCurve(aPoles);
}
} // namespace

TEST(ExtremaPC2d_BezierCurveTest, LinearProjectionIsExact)
{
  ExtremaPC2d_BezierCurve    anEvaluator(makeLinear());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(4.0, 3.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.4, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 9.0, THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, SearchModesAndEndpoints)
{
  ExtremaPC2d_BezierCurve anEvaluator(makeLinear());
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(4.0, 3.0), THE_TOL, ExtremaPC2d::SearchMode::Max).NbExt(),
            0);
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(-2.0, 0.0), THE_TOL).NbExt(), 0);
  const ExtremaPC2d::Result& aMin =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(-2.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aMin.NbExt(), 1);
  EXPECT_NEAR(aMin[0].Parameter, 0.0, THE_TOL);
  const ExtremaPC2d::Result& aMax =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(-2.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_NEAR(aMax[0].Parameter, 1.0, THE_TOL);
  EXPECT_NEAR(aMax[0].SquareDistance, 144.0, THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, SymmetricQuadraticHasCentralMinimum)
{
  ExtremaPC2d_BezierCurve    anEvaluator(makeQuadratic());
  const ExtremaPC2d::Result& aResult =
    anEvaluator.Perform(gp_Pnt2d(), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.5, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 1.0, THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, RationalArcHasConstantDistance)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 1.0);
  aPoles(3) = gp_Pnt2d(0.0, 1.0);
  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = M_SQRT1_2;
  aWeights(3) = 1.0;
  ExtremaPC2d_BezierCurve    anEvaluator(new Geom2d_BezierCurve(aPoles, aWeights));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 1.0, THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, DegenerateCurveHasInfiniteSolutions)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.Init(gp_Pnt2d(2.0, -1.0));
  ExtremaPC2d_BezierCurve    anEvaluator(new Geom2d_BezierCurve(aPoles));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(5.0, 3.0), THE_TOL);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, MaxDegreeRationalWorkspaceHandlesConstantCurve)
{
  const int                    aNbPoles = Geom2d_BezierCurve::MaxDegree() + 1;
  NCollection_Array1<gp_Pnt2d> aPoles(1, aNbPoles);
  NCollection_Array1<double>   aWeights(1, aNbPoles);
  for (size_t aPoleIndex = 0; aPoleIndex < aPoles.Size(); ++aPoleIndex)
  {
    aPoles.ChangeAt(aPoleIndex)   = gp_Pnt2d(2.0, -1.0);
    aWeights.ChangeAt(aPoleIndex) = 1.0 + 0.01 * static_cast<double>(aPoleIndex + 1);
  }

  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  ExtremaPC2d_BezierCurve         anExtrema(aCurve);
  const ExtremaPC2d::Result&      aResult = anExtrema.Perform(gp_Pnt2d(5.0, 3.0), THE_TOL);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, MutationIsObserved)
{
  occ::handle<Geom2d_BezierCurve> aCurve = makeQuadratic();
  ExtremaPC2d_BezierCurve         anEvaluator(aCurve);
  const double aBefore = anEvaluator.Perform(gp_Pnt2d(0.0, 1.5), THE_TOL).MinSquareDistance();
  aCurve->SetPole(2, gp_Pnt2d(3.0, -4.0));
  const ExtremaPC2d::Result& anAfter = anEvaluator.Perform(gp_Pnt2d(0.0, 1.5), THE_TOL);
  ASSERT_TRUE(anAfter.IsDone());
  EXPECT_GT(std::abs(anAfter.MinSquareDistance() - aBefore), 1.0e-4);
  EXPECT_NEAR(anAfter[anAfter.MinIndex()].Point.Distance(
                aCurve->Value(anAfter[anAfter.MinIndex()].Parameter)),
              0.0,
              THE_TOL);
}

TEST(ExtremaPC2d_BezierCurveTest, SingletonInvalidNullAndOwnership)
{
  ExtremaPC2d_BezierCurve aSingleton(makeLinear(), {0.4, 0.4});
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(), THE_TOL).NbExt(), 0);
  EXPECT_EQ(aSingleton.PerformWithEndpoints(gp_Pnt2d(), THE_TOL).NbExt(), 1);
  ExtremaPC2d_BezierCurve anInvalid(makeLinear(), {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  occ::handle<Geom2d_BezierCurve> aNull;
  ExtremaPC2d_BezierCurve         aNullEvaluator(aNull);
  EXPECT_EQ(aNullEvaluator.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::NotDone);

  occ::handle<Geom2d_BezierCurve> aCurve = makeLinear();
  ExtremaPC2d_BezierCurve         anOwned(aCurve);
  aCurve.Nullify();
  EXPECT_FALSE(anOwned.Curve().IsNull());
  EXPECT_TRUE(anOwned.Perform(gp_Pnt2d(4.0, 3.0), THE_TOL).IsDone());
}
