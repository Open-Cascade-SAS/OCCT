// Copyright (c) 2026 OPEN CASCADE SAS
// This file is part of Open CASCADE Technology software library.

#include <ExtremaPC2d_Line.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOL = 1.0e-10;
}

TEST(ExtremaPC2d_LineTest, UnboundedProjectionIsExact)
{
  ExtremaPC2d_Line anEvaluator(gp_Lin2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(4.0, 6.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 3.0, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 16.0, THE_TOL);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST(ExtremaPC2d_LineTest, ReversedAndDiagonalDirectionsPreserveParameter)
{
  ExtremaPC2d_Line aReversed(gp_Lin2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(-1.0, 0.0)));
  EXPECT_NEAR(aReversed.Perform(gp_Pnt2d(-2.0, 5.0), THE_TOL)[0].Parameter, 3.0, THE_TOL);

  ExtremaPC2d_Line aDiagonal(gp_Lin2d(gp_Pnt2d(), gp_Dir2d(1.0, 1.0)));
  const ExtremaPC2d::Result& aResult = aDiagonal.Perform(gp_Pnt2d(5.0, 5.0), THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter, 5.0 * std::sqrt(2.0), THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
}

TEST(ExtremaPC2d_LineTest, SearchModesFilterInteriorProjection)
{
  ExtremaPC2d_Line anEvaluator(gp_Lin2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)));
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(4.0, 3.0), THE_TOL,
                                ExtremaPC2d::SearchMode::Min).NbExt(), 1);
  const ExtremaPC2d::Result& aMax = anEvaluator.Perform(gp_Pnt2d(4.0, 3.0), THE_TOL,
                                                        ExtremaPC2d::SearchMode::Max);
  EXPECT_EQ(aMax.Status, ExtremaPC2d::Status::NoSolution);
  EXPECT_EQ(aMax.NbExt(), 0);
}

TEST(ExtremaPC2d_LineTest, BoundedProjectionAndEndpointsAreClassified)
{
  ExtremaPC2d_Line anEvaluator(gp_Lin2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), {0.0, 10.0});
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(4.0, 3.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 4.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].SquareDistance, 9.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, 10.0, THE_TOL);
}

TEST(ExtremaPC2d_LineTest, ProjectionOutsideDomainUsesEndpointsOnlyWhenRequested)
{
  ExtremaPC2d_Line anEvaluator(gp_Lin2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), {0.0, 10.0});
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(-2.0, 3.0), THE_TOL).NbExt(), 0);
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(-2.0, 3.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].SquareDistance, 13.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, 10.0, THE_TOL);
}

TEST(ExtremaPC2d_LineTest, BoundaryProjectionIsNotDuplicated)
{
  ExtremaPC2d_Line anEvaluator(gp_Lin2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), {0.0, 10.0});
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(0.0, 3.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, 10.0, THE_TOL);
}

TEST(ExtremaPC2d_LineTest, SingletonDomainHonorsModes)
{
  ExtremaPC2d_Line anEvaluator(gp_Lin2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)), {2.0, 2.0});
  const ExtremaPC2d::Result& aBoth =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(5.0, 3.0), THE_TOL);
  ASSERT_EQ(aBoth.NbExt(), 1);
  EXPECT_TRUE(aBoth[0].IsMinimum);
  EXPECT_TRUE(aBoth[0].IsMaximum);
  const ExtremaPC2d::Result& aMax = anEvaluator.PerformWithEndpoints(
    gp_Pnt2d(5.0, 3.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_FALSE(aMax[0].IsMinimum);
  EXPECT_TRUE(aMax[0].IsMaximum);
}

TEST(ExtremaPC2d_LineTest, InvalidInputAndAccessors)
{
  const gp_Lin2d aLine(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0));
  ExtremaPC2d_Line anEvaluator(aLine, {3.0, 1.0});
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(), THE_TOL).Status,
            ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(), 0.0).Status,
            ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(anEvaluator.Line().Location().Distance(aLine.Location()), 0.0);
  EXPECT_EQ(anEvaluator.Value(2.0).Distance(gp_Pnt2d(1.0, 4.0)), 0.0);
}
