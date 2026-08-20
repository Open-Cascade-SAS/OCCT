// Copyright (c) 2026 OPEN CASCADE SAS
// This file is part of Open CASCADE Technology software library.

#include <ExtremaPC2d_Circle.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-9;
gp_Circ2d makeCircle(double theRadius = 5.0)
{
  return gp_Circ2d(gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true), theRadius);
}
} // namespace

TEST(ExtremaPC2d_CircleTest, OutsideAndInsidePointsHaveOppositeExtrema)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle());
  const ExtremaPC2d::Result& anOutside = anEvaluator.Perform(gp_Pnt2d(13.0, 0.0), THE_TOL);
  ASSERT_EQ(anOutside.NbExt(), 2);
  EXPECT_NEAR(anOutside[anOutside.MinIndex()].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(anOutside.MinSquareDistance(), 64.0, THE_TOL);
  EXPECT_NEAR(anOutside.MaxSquareDistance(), 324.0, THE_TOL);

  const ExtremaPC2d::Result& anInside = anEvaluator.Perform(gp_Pnt2d(2.0, 0.0), THE_TOL);
  EXPECT_NEAR(anInside.MinSquareDistance(), 9.0, THE_TOL);
  EXPECT_NEAR(anInside.MaxSquareDistance(), 49.0, THE_TOL);
}

TEST(ExtremaPC2d_CircleTest, SearchModesReturnExactSubsets)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle());
  const ExtremaPC2d::Result& aMin = anEvaluator.Perform(gp_Pnt2d(13.0, 0.0), THE_TOL,
                                                        ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aMin.NbExt(), 1);
  EXPECT_TRUE(aMin[0].IsMinimum);
  const ExtremaPC2d::Result& aMax = anEvaluator.Perform(gp_Pnt2d(13.0, 0.0), THE_TOL,
                                                        ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_TRUE(aMax[0].IsMaximum);
  EXPECT_NEAR(aMax[0].Parameter, M_PI, THE_TOL);
}

TEST(ExtremaPC2d_CircleTest, IndirectOrientationPreservesSense)
{
  const gp_Ax22d aPosition(gp_Pnt2d(2.0, -1.0), gp_Dir2d(0.0, 1.0), gp_Dir2d(1.0, 0.0));
  const gp_Circ2d aCircle(aPosition, 4.0);
  const double anAngle = 0.7;
  const gp_Pnt2d aQuery = aCircle.Location().Translated(
    7.0 * (std::cos(anAngle) * gp_Vec2d(aPosition.XDirection())
           + std::sin(anAngle) * gp_Vec2d(aPosition.YDirection())));
  ExtremaPC2d_Circle anEvaluator(aCircle);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(aQuery, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, anAngle, THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);
}

TEST(ExtremaPC2d_CircleTest, CenterAndZeroRadiusHaveInfiniteSolutions)
{
  ExtremaPC2d_Circle aCircle(makeCircle());
  const ExtremaPC2d::Result& aCenter = aCircle.Perform(gp_Pnt2d(), THE_TOL);
  EXPECT_TRUE(aCenter.IsInfinite());
  EXPECT_NEAR(aCenter.InfiniteSquareDistance, 25.0, THE_TOL);

  ExtremaPC2d_Circle aPointCircle(makeCircle(0.0));
  const ExtremaPC2d::Result& aPointResult = aPointCircle.Perform(gp_Pnt2d(3.0, 4.0), THE_TOL);
  EXPECT_TRUE(aPointResult.IsInfinite());
  EXPECT_NEAR(aPointResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST(ExtremaPC2d_CircleTest, ShiftedFullPeriodHasUniqueRepresentatives)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle(), {7.0, 7.0 + 2.0 * M_PI});
  const ExtremaPC2d::Result& aResult = anEvaluator.PerformWithEndpoints(gp_Pnt2d(13.0, 0.0),
                                                                        THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 4.0 * M_PI, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, 3.0 * M_PI, THE_TOL);
}

TEST(ExtremaPC2d_CircleTest, MultiplePeriodsReturnAllParameterExtrema)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle(), {0.0, 4.0 * M_PI});
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(13.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 4);
  EXPECT_NEAR(aResult[0].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[1].Parameter, 2.0 * M_PI, THE_TOL);
  EXPECT_NEAR(aResult[2].Parameter, M_PI, THE_TOL);
  EXPECT_NEAR(aResult[3].Parameter, 3.0 * M_PI, THE_TOL);
}

TEST(ExtremaPC2d_CircleTest, HalfCircleClassifiesInteriorAndEndpoints)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle(), {M_PI_2, 3.0 * M_PI_2});
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(10.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, M_PI, THE_TOL);
  int aMinimumCount = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    aMinimumCount += aResult[anIndex].IsMinimum ? 1 : 0;
  }
  EXPECT_EQ(aMinimumCount, 2);
}

TEST(ExtremaPC2d_CircleTest, SingletonAndInvalidInput)
{
  ExtremaPC2d_Circle aSingleton(makeCircle(), {0.4, 0.4});
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(8.0, 0.0), THE_TOL).NbExt(), 0);
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(), THE_TOL).Status,
            ExtremaPC2d::Status::NoSolution);
  EXPECT_EQ(aSingleton.PerformWithEndpoints(gp_Pnt2d(8.0, 0.0), THE_TOL).NbExt(), 1);
  ExtremaPC2d_Circle anInvalid(makeCircle(), {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status,
            ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), 0.0).Status,
            ExtremaPC2d::Status::InvalidInput);
}

TEST(ExtremaPC2d_CircleTest, LargeToleranceDoesNotExpandDomain)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle(1.0), {1.0, 2.0});
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(2.0, 0.0), 2.0);
  EXPECT_EQ(aResult.Status, ExtremaPC2d::Status::NoSolution);
  EXPECT_EQ(aResult.NbExt(), 0);
}

TEST(ExtremaPC2d_CircleTest, NonFiniteQueryIsRejected)
{
  ExtremaPC2d_Circle anEvaluator(makeCircle());
  const gp_Pnt2d aQuery(Precision::Infinite(), 0.0);
  EXPECT_EQ(anEvaluator.Perform(aQuery, THE_TOL).Status,
            ExtremaPC2d::Status::InvalidInput);
}

TEST(ExtremaPC2d_CircleTest, UnrepresentablePeriodicStepReportsNumericalError)
{
  const double aFirst = 1.0e17;
  ExtremaPC2d_Circle anEvaluator(makeCircle(), {aFirst, aFirst + 100.0});
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(12.0, 0.0), THE_TOL).Status,
            ExtremaPC2d::Status::NumericalError);
}

TEST(ExtremaPC2d_CircleTest, AccessorsAndValueMatchGeometry)
{
  const gp_Circ2d aCircle(gp_Ax22d(gp_Pnt2d(2.0, 3.0), gp_Dir2d(0.0, 1.0), false), 4.0);
  ExtremaPC2d_Circle anEvaluator(aCircle, {-1.0, 2.0});
  EXPECT_TRUE(anEvaluator.IsBounded());
  EXPECT_NEAR(anEvaluator.Domain().Min, -1.0, THE_TOL);
  EXPECT_EQ(anEvaluator.Value(0.3).Distance(ElCLib::Value(0.3, aCircle)), 0.0);
}
