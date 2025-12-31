// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <ExtremaPC_Circle.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_Circle tests.
class ExtremaPC_CircleTest : public testing::Test
{
protected:
  static constexpr double THE_TOL  = 1.0e-10;
  static constexpr double THE_2PI  = 2.0 * M_PI;
  static constexpr double THE_PI   = M_PI;
  static constexpr double THE_PI_2 = M_PI / 2.0;
};

//==================================================================================================
// Basic tests - point outside circle
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, PointOutside_OnXAxis)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle, ExtremaPC::Domain1D{0.0, THE_2PI});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  // Find min and max
  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;
  int aMaxIdx = 1 - aMinIdx;

  // Minimum at (10, 0, 0), distance = 10
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 10.0, THE_TOL);
  EXPECT_TRUE(aResult[aMinIdx].IsMinimum);
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 0.0, THE_TOL);

  // Maximum at (-10, 0, 0), distance = 30
  EXPECT_NEAR(std::sqrt(aResult[aMaxIdx].SquareDistance), 30.0, THE_TOL);
  EXPECT_FALSE(aResult[aMaxIdx].IsMinimum);
  EXPECT_NEAR(aResult[aMaxIdx].Parameter, THE_PI, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointOutside_OnYAxis)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(0.0, 25.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;
  int aMaxIdx = 1 - aMinIdx;

  // Minimum at (0, 10, 0), distance = 15
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 15.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Parameter, THE_PI_2, THE_TOL);

  // Maximum at (0, -10, 0), distance = 35
  EXPECT_NEAR(std::sqrt(aResult[aMaxIdx].SquareDistance), 35.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointOutside_Diagonal)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(20.0, 20.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Distance from (20,20) to circle center = sqrt(800) = 28.28...
  // Minimum distance = 28.28 - 10 = 18.28...
  double aDistToCenter = std::sqrt(800.0);
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), aDistToCenter - 10.0, THE_TOL);
}

//==================================================================================================
// Point inside circle tests
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, PointInside_OnXAxis)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(3.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;
  int aMaxIdx = 1 - aMinIdx;

  // Minimum distance = 10 - 3 = 7
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 7.0, THE_TOL);

  // Maximum distance = 10 + 3 = 13
  EXPECT_NEAR(std::sqrt(aResult[aMaxIdx].SquareDistance), 13.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointInside_OnYAxis)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(0.0, 5.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;
  int aMaxIdx = 1 - aMinIdx;

  // Minimum distance = 10 - 5 = 5
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 5.0, THE_TOL);

  // Maximum distance = 10 + 5 = 15
  EXPECT_NEAR(std::sqrt(aResult[aMaxIdx].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointInside_NearCenter)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(0.1, 0.1, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  double aDistToCenter = std::sqrt(0.02);
  int    aMinIdx       = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum distance ~= 10 - 0.14
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 10.0 - aDistToCenter, THE_TOL);
}

//==================================================================================================
// Degenerate case - point at center
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, PointAtCenter_Degenerate)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(0.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // Should return infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointNearCenter_NotDegenerate)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(1e-8, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, 1e-12);

  // Should still find 2 extrema (not degenerate with smaller tolerance)
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 2);

  // Point is essentially at center, so min and max distances are nearly equal to radius
  double aOffset      = 1e-8;
  int    aMinIdx      = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;
  int    aMaxIdx      = 1 - aMinIdx;
  double aMinExpected = 10.0 - aOffset;
  double aMaxExpected = 10.0 + aOffset;

  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), aMinExpected, 1e-6);
  EXPECT_NEAR(std::sqrt(aResult[aMaxIdx].SquareDistance), aMaxExpected, 1e-6);

  // Verify min is near 0 and max is near PI
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMaxIdx].Parameter, THE_PI, THE_TOL);
}

//==================================================================================================
// Point out of plane tests
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, PointAbovePlane)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(20.0, 0.0, 5.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum: closest point on circle is (10,0,0)
  // Distance = sqrt(10^2 + 5^2) = sqrt(125)
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 125.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointBelowPlane)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(0.0, 15.0, -7.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum: closest point on circle is (0,10,0)
  // Distance = sqrt(5^2 + 7^2) = sqrt(74)
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 74.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointOnAxis_Degenerate)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(0.0, 0.0, 15.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // Point on axis = infinite solutions (all points equidistant)
  EXPECT_TRUE(aResult.IsInfinite());
  // Distance = sqrt(R^2 + h^2) = sqrt(100 + 225) = sqrt(325)
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 325.0, THE_TOL);
}

//==================================================================================================
// Partial arc tests
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, PartialArc_FirstQuadrant)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum should be at u=0 (closest point on arc)
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PartialArc_SecondQuadrant)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(-20.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum should be at u=PI (closest point on arc)
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PartialArc_ExtremaOutsideRange)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle);
  // Arc from PI/2 to 3*PI/2 (left half of circle)
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // The natural extrema (at 0 and PI) - 0 is outside, PI is inside
  // Should find extremum at PI and possibly endpoints
  bool aFoundPi = false;
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    if (std::abs(aResult[i].Parameter - THE_PI) < 0.1)
    {
      aFoundPi = true;
      // Maximum distance at PI
      EXPECT_NEAR(std::sqrt(aResult[i].SquareDistance), 30.0, THE_TOL);
    }
  }
  EXPECT_TRUE(aFoundPi);
}

//==================================================================================================
// Different circle configurations
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, CircleWithOffset)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1)), 15.0);
  gp_Pnt  aPoint(130.0, 200.0, 50.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Distance from point to center = 30
  // Minimum distance = 30 - 15 = 15
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, CircleInYZPlane)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 10.0);
  gp_Pnt  aPoint(5.0, 20.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Closest point on circle is (0, 10, 0)
  // Distance = sqrt(5^2 + 10^2) = sqrt(125)
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 125.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, CircleInXZPlane)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 10.0);
  gp_Pnt  aPoint(20.0, 3.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Closest point on circle is (10, 0, 0)
  // Distance = sqrt(10^2 + 3^2) = sqrt(109)
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 109.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, SmallRadius)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.001);
  gp_Pnt  aPoint(0.01, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum distance = 0.01 - 0.001 = 0.009
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 0.009, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, LargeRadius)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1000.0);
  gp_Pnt  aPoint(1500.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum distance = 1500 - 1000 = 500
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 500.0, THE_TOL);
}

//==================================================================================================
// Point on circle tests
//==================================================================================================

TEST_F(ExtremaPC_CircleTest, PointOnCircle)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(10.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum distance = 0 (point is on circle)
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_CircleTest, PointOnCircle_AtPi)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt  aPoint(-10.0, 0.0, 0.0);

  ExtremaPC_Circle         anEval(aCircle);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;

  // Minimum distance = 0 (point is on circle)
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Parameter, THE_PI, THE_TOL);
}
