// Copyright (c) 2026 OPEN CASCADE SAS
// This file is part of Open CASCADE Technology software library.

#include <ExtremaPC2d_Hyperbola.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

gp_Hypr2d makeHyperbola(double theMajor = 2.0, double theMinor = 1.0)
{
  return gp_Hypr2d(gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true), theMajor, theMinor);
}
} // namespace

TEST(ExtremaPC2d_HyperbolaTest, CenterHasVertexMinimum)
{
  ExtremaPC2d_Hyperbola      anEvaluator(makeHyperbola());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 4.0, THE_TOL);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST(ExtremaPC2d_HyperbolaTest, MajorAxisPointHasThreeExtrema)
{
  const double               aRoot = std::acosh(2.0);
  ExtremaPC2d_Hyperbola      anEvaluator(makeHyperbola());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(5.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  int  aMinima     = 0;
  int  aMaxima     = 0;
  bool hasNegative = false;
  bool hasPositive = false;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    aMinima += aResult[anIndex].IsMinimum ? 1 : 0;
    aMaxima += aResult[anIndex].IsMaximum ? 1 : 0;
    hasNegative = hasNegative || std::abs(aResult[anIndex].Parameter + aRoot) <= THE_TOL;
    hasPositive = hasPositive || std::abs(aResult[anIndex].Parameter - aRoot) <= THE_TOL;
  }
  EXPECT_EQ(aMinima, 2);
  EXPECT_EQ(aMaxima, 1);
  EXPECT_TRUE(hasNegative);
  EXPECT_TRUE(hasPositive);
  EXPECT_NEAR(aResult.MinSquareDistance(), 4.0, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 9.0, THE_TOL);
}

TEST(ExtremaPC2d_HyperbolaTest, RootWithinParameterToleranceIsClampedToBoundary)
{
  const double               aRoot   = std::acosh(2.0);
  const double               anUpper = aRoot - 0.5 * ExtremaPC2d::THE_PARAM_TOLERANCE;
  ExtremaPC2d_Hyperbola      anEvaluator(makeHyperbola(), {0.5, anUpper});
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(5.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_EQ(aResult[0].Parameter, anUpper);
}

TEST(ExtremaPC2d_HyperbolaTest, SearchModesReturnExactClasses)
{
  ExtremaPC2d_Hyperbola      anEvaluator(makeHyperbola());
  const ExtremaPC2d::Result& aMin =
    anEvaluator.Perform(gp_Pnt2d(5.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aMin.NbExt(), 2);
  EXPECT_TRUE(aMin[0].IsMinimum && aMin[1].IsMinimum);
  const ExtremaPC2d::Result& aMax =
    anEvaluator.Perform(gp_Pnt2d(5.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_NEAR(aMax[0].Parameter, 0.0, THE_TOL);
  EXPECT_TRUE(aMax[0].IsMaximum);
}

TEST(ExtremaPC2d_HyperbolaTest, PointOnCurveHasZeroDistanceMinimum)
{
  const gp_Hypr2d            aCurve = makeHyperbola();
  constexpr double           aU     = 0.7;
  ExtremaPC2d_Hyperbola      anEvaluator(aCurve);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(ElCLib::Value(aU, aCurve), THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, aU, THE_TOL);
}

TEST(ExtremaPC2d_HyperbolaTest, IndirectOrientationPreservesSignedParameters)
{
  const gp_Ax22d             aPosition(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0), gp_Dir2d(1.0, 0.0));
  const gp_Hypr2d            aCurve(aPosition, 2.0, 1.0);
  ExtremaPC2d_Hyperbola      anEvaluator(aCurve);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(1.0, 7.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult.MinSquareDistance(), 4.0, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 9.0, THE_TOL);
}

TEST(ExtremaPC2d_HyperbolaTest, BoundedArcAddsEndpointMinima)
{
  ExtremaPC2d_Hyperbola      anEvaluator(makeHyperbola(), {-0.5, 0.5});
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(5.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, 0.0, THE_TOL);
  int aMinima = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    aMinima += aResult[anIndex].IsMinimum ? 1 : 0;
  }
  EXPECT_EQ(aMinima, 2);
}

TEST(ExtremaPC2d_HyperbolaTest, ZeroAxesDegeneracies)
{
  ExtremaPC2d_Hyperbola      aSinhLine(makeHyperbola(0.0, 1.0));
  const ExtremaPC2d::Result& aLineResult = aSinhLine.Perform(gp_Pnt2d(3.0, 2.0), THE_TOL);
  ASSERT_GE(aLineResult.NbExt(), 1);
  EXPECT_NEAR(aLineResult.MinSquareDistance(), 9.0, THE_TOL);

  ExtremaPC2d_Hyperbola      aPoint(makeHyperbola(0.0, 0.0));
  const ExtremaPC2d::Result& aPointResult = aPoint.Perform(gp_Pnt2d(3.0, 4.0), THE_TOL);
  EXPECT_TRUE(aPointResult.IsInfinite());
  EXPECT_NEAR(aPointResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST(ExtremaPC2d_HyperbolaTest, SingletonInvalidAndValueAccessor)
{
  ExtremaPC2d_Hyperbola aSingleton(makeHyperbola(), {0.4, 0.4});
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(), THE_TOL).NbExt(), 0);
  EXPECT_EQ(aSingleton.PerformWithEndpoints(gp_Pnt2d(), THE_TOL).NbExt(), 1);
  ExtremaPC2d_Hyperbola anInvalid(makeHyperbola(), {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(aSingleton.Value(-0.7).Distance(ElCLib::Value(-0.7, makeHyperbola())), 0.0);
}
