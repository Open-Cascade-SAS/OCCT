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

#include <ExtremaPC2d_BSplineCurve.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

occ::handle<Geom2d_BSplineCurve> makeLinear()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(10.0, 0.0);
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 2;
  aMults(2) = 2;
  return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 1);
}

occ::handle<Geom2d_BSplineCurve> makeC1Curve()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 1.0);
  aPoles(3) = gp_Pnt2d(2.0, 1.0);
  aPoles(4) = gp_Pnt2d(3.0, 0.0);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;
  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 3;
  return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
}
} // namespace

TEST(ExtremaPC2d_BSplineCurveTest, LinearProjectionIsExact)
{
  ExtremaPC2d_BSplineCurve   anEvaluator(makeLinear());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(6.0, 4.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.6, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 16.0, THE_TOL);
}

TEST(ExtremaPC2d_BSplineCurveTest, PartialDomainUsesEndpointWhenRequested)
{
  ExtremaPC2d_BSplineCurve anEvaluator(makeLinear(), {0.0, 0.5});
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(8.0, 3.0), THE_TOL).NbExt(), 0);
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(8.0, 3.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.5, THE_TOL);
}

TEST(ExtremaPC2d_BSplineCurveTest, C1JunctionPointIsFound)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = makeC1Curve();
  ExtremaPC2d_BSplineCurve         anEvaluator(aCurve);
  const ExtremaPC2d::Result&       aResult =
    anEvaluator.Perform(aCurve->Value(0.5), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 0.5, THE_TOL);
}

TEST(ExtremaPC2d_BSplineCurveTest, DegenerateCurveHasInfiniteSolutions)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = makeLinear();
  aCurve->SetPole(1, gp_Pnt2d(2.0, -1.0));
  aCurve->SetPole(2, gp_Pnt2d(2.0, -1.0));
  ExtremaPC2d_BSplineCurve   anEvaluator(aCurve);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(5.0, 3.0), THE_TOL);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST(ExtremaPC2d_BSplineCurveTest, PoleMutationIsObserved)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = makeC1Curve();
  ExtremaPC2d_BSplineCurve         anEvaluator(aCurve);
  const double aBefore = anEvaluator.Perform(gp_Pnt2d(1.5, 2.0), THE_TOL).MinSquareDistance();
  aCurve->SetPole(2, gp_Pnt2d(1.0, -3.0));
  const ExtremaPC2d::Result& anAfter = anEvaluator.Perform(gp_Pnt2d(1.5, 2.0), THE_TOL);
  ASSERT_TRUE(anAfter.IsDone());
  EXPECT_GT(std::abs(anAfter.MinSquareDistance() - aBefore), 1.0e-4);
  EXPECT_NEAR(anAfter[anAfter.MinIndex()].Point.Distance(
                aCurve->Value(anAfter[anAfter.MinIndex()].Parameter)),
              0.0,
              THE_TOL);
}

TEST(ExtremaPC2d_BSplineCurveTest, SearchModesFilterExtrema)
{
  ExtremaPC2d_BSplineCurve   anEvaluator(makeC1Curve());
  const ExtremaPC2d::Result& aMin =
    anEvaluator.Perform(gp_Pnt2d(1.5, 2.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  for (size_t anIndex = 0; anIndex < aMin.NbExt(); ++anIndex)
  {
    EXPECT_TRUE(aMin[anIndex].IsMinimum);
  }
  const ExtremaPC2d::Result& aMax =
    anEvaluator.Perform(gp_Pnt2d(1.5, 2.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  for (size_t anIndex = 0; anIndex < aMax.NbExt(); ++anIndex)
  {
    EXPECT_TRUE(aMax[anIndex].IsMaximum);
  }
}

TEST(ExtremaPC2d_BSplineCurveTest, SingletonInvalidNullAndOwnership)
{
  ExtremaPC2d_BSplineCurve aSingleton(makeLinear(), {0.4, 0.4});
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(), THE_TOL).NbExt(), 0);
  EXPECT_EQ(aSingleton.PerformWithEndpoints(gp_Pnt2d(), THE_TOL).NbExt(), 1);
  ExtremaPC2d_BSplineCurve anInvalid(makeLinear(), {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  occ::handle<Geom2d_BSplineCurve> aNull;
  ExtremaPC2d_BSplineCurve         aNullEvaluator(aNull);
  EXPECT_EQ(aNullEvaluator.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::NotDone);
  occ::handle<Geom2d_BSplineCurve> aCurve = makeLinear();
  ExtremaPC2d_BSplineCurve         anOwned(aCurve);
  aCurve.Nullify();
  EXPECT_FALSE(anOwned.Curve().IsNull());
}
