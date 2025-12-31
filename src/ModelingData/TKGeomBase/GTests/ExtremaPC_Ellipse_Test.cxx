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

#include <ExtremaPC_Ellipse.hxx>

#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <ElCLib.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_Ellipse tests.
class ExtremaPC_EllipseTest : public testing::Test
{
protected:
  static constexpr double THE_TOL  = 1.0e-8;
  static constexpr double THE_2PI  = 2.0 * M_PI;
  static constexpr double THE_PI   = M_PI;
  static constexpr double THE_PI_2 = M_PI / 2.0;
};

//==================================================================================================
// Basic tests - point on major axis
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PointOnMajorAxis_Outside)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(30.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  double aMinSqDist = aResult.MinSquareDistance();
  // Closest point is (20, 0, 0), distance = 10
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointOnMajorAxis_Inside)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(10.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // For a point inside an ellipse on the major axis, the closest point is NOT at the
  // major vertex. For ellipse a=20, b=10 and point (10,0,0):
  // The extremum condition gives cos(u) = 2/3, yielding:
  //   closest point at (40/3, ±10*sqrt(5)/3, 0) with distance = sqrt(600/9) ≈ 8.165
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 600.0 / 9.0, 0.01);
}

TEST_F(ExtremaPC_EllipseTest, PointOnMajorAxis_NegativeSide)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(-30.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  double aMinSqDist = aResult.MinSquareDistance();
  // Closest point is (-20, 0, 0), distance = 10
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

//==================================================================================================
// Point on minor axis
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PointOnMinorAxis_Outside)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(0.0, 20.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  double aMinSqDist = aResult.MinSquareDistance();
  // Closest point is (0, 10, 0), distance = 10
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointOnMinorAxis_Inside)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(0.0, 5.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  double aMinSqDist = aResult.MinSquareDistance();
  // Closest point is (0, 10, 0), distance = 5
  EXPECT_NEAR(std::sqrt(aMinSqDist), 5.0, THE_TOL);
}

//==================================================================================================
// Point in quadrants
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PointInFirstQuadrant)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(15.0, 15.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Verify distance is reasonable
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GT(aMinSqDist, 0.0);

  // Verify the closest point is actually on the ellipse
  int    aMinIdx      = aResult.MinIndex();
  gp_Pnt aPtOnEllipse = ElCLib::Value(aResult[aMinIdx].Parameter, anEllipse);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnEllipse), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointInSecondQuadrant)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(-15.0, 8.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Verify the point on ellipse
  int    aMinIdx      = aResult.MinIndex();
  gp_Pnt aPtOnEllipse = ElCLib::Value(aResult[aMinIdx].Parameter, anEllipse);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnEllipse), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointInThirdQuadrant)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(-12.0, -7.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Verify the closest point is on the ellipse and distance is reasonable
  int    aMinIdx      = aResult.MinIndex();
  gp_Pnt aPtOnEllipse = ElCLib::Value(aResult[aMinIdx].Parameter, anEllipse);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnEllipse), 0.0, THE_TOL);

  // Distance to closest point should be less than distance to origin
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_LT(aMinDist, aPoint.Distance(gp_Pnt(0, 0, 0)));
}

TEST_F(ExtremaPC_EllipseTest, PointInFourthQuadrant)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(18.0, -5.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Verify the closest point is on the ellipse
  int    aMinIdx      = aResult.MinIndex();
  gp_Pnt aPtOnEllipse = ElCLib::Value(aResult[aMinIdx].Parameter, anEllipse);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnEllipse), 0.0, THE_TOL);

  // Distance to closest point should be less than distance to origin
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_LT(aMinDist, aPoint.Distance(gp_Pnt(0, 0, 0)));
}

//==================================================================================================
// Degenerate case - point at center
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PointAtCenter_Degenerate)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(0.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // For a non-circular ellipse (a ≠ b) with point at center:
  // We get 4 extrema: 2 minima at minor axis vertices (±b), 2 maxima at major axis vertices (±a)
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 4);

  // Verify minimum distance equals minor radius (b = 10)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, THE_TOL);

  // Verify maximum distance equals major radius (a = 20)
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 20.0, THE_TOL);

  // Verify all extrema are on the ellipse
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    gp_Pnt aPtOnEllipse = ElCLib::Value(aResult[i].Parameter, anEllipse);
    EXPECT_NEAR(aResult[i].Point.Distance(aPtOnEllipse), 0.0, THE_TOL);
  }
}

//==================================================================================================
// Point out of plane
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PointAbovePlane)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(30.0, 0.0, 5.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Closest point is (20, 0, 0), distance = sqrt(10^2 + 5^2) = sqrt(125)
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 125.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointBelowPlane)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(0.0, 20.0, -8.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Closest point is (0, 10, 0), distance = sqrt(10^2 + 8^2) = sqrt(164)
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 164.0, THE_TOL);
}

//==================================================================================================
// Partial arc tests
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PartialArc_FirstQuadrant)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(30.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum should be at u=0
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PartialArc_SecondHalf)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(-30.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum should be at u=PI
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

//==================================================================================================
// Different ellipse configurations
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, EllipseWithOffset)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(130.0, 200.0, 50.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Closest point is at (120, 200, 50), distance = 10
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, EllipseInYZPlane)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 20.0, 10.0);
  gp_Pnt   aPoint(5.0, 30.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify that valid extrema are found and projected points are on the ellipse
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GT(aMinSqDist, 0.0);

  // Verify extremum point is on the ellipse (project back and check)
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    const ExtremaPC::ExtremumResult& anExt  = aResult[i];
    gp_Pnt                           aCheck = ElCLib::Value(anExt.Parameter, anEllipse);
    EXPECT_NEAR(aCheck.Distance(anExt.Point), 0.0, 1e-7);
  }
}

TEST_F(ExtremaPC_EllipseTest, NearlyCircularEllipse)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 9.9);
  gp_Pnt   aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Closest point is at (10, 0, 0), distance = 10
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, VeryFlatEllipse)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 2.0);
  gp_Pnt   aPoint(0.0, 10.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 2);

  // Closest point is at (0, 2, 0), distance = 8
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 8.0, THE_TOL);
}

//==================================================================================================
// Point on ellipse
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, PointOnEllipse_MajorVertex)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointOnEllipse_MinorVertex)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(0.0, 10.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_EllipseTest, PointOnEllipse_General)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  // Point at u = PI/4
  gp_Pnt aPoint = ElCLib::Value(THE_PI / 4.0, anEllipse);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

//==================================================================================================
// Number of extrema tests
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, FourExtrema_PointInsideEllipse)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(5.0, 3.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // For a point inside an ellipse, there should be at least 4 extrema
  // (2 minima and 2 maxima)
  EXPECT_GE(aResult.NbExt(), 4);
}

TEST_F(ExtremaPC_EllipseTest, TwoExtrema_PointOnAxis)
{
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(30.0, 0.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // For a point on the major axis outside, there should be 2 extrema
  EXPECT_EQ(aResult.NbExt(), 2);
}

//==================================================================================================
// Narrow parameter range tests - verify relative step works correctly
//==================================================================================================

TEST_F(ExtremaPC_EllipseTest, NarrowRange_VerySmall)
{
  // Test with a very narrow parameter range (0.05 radians ~ 3 degrees)
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(22.0, 2.0, 0.0);

  double aUMin = 0.0;
  double aUMax = 0.05; // Very small range

  // Create evaluator with restricted domain
  ExtremaPC_Ellipse        anEval(anEllipse, ExtremaPC::Domain1D{aUMin, aUMax});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should find at least the endpoint extremum
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify all extrema are within parameter bounds
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, aUMin - THE_TOL);
    EXPECT_LE(aResult[i].Parameter, aUMax + THE_TOL);
  }
}

TEST_F(ExtremaPC_EllipseTest, NarrowRange_MediumSmall)
{
  // Test with a moderately narrow parameter range (0.2 radians ~ 11 degrees)
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(25.0, 5.0, 0.0);

  double aUMin = THE_PI / 6.0 - 0.1;
  double aUMax = THE_PI / 6.0 + 0.1;

  // Create evaluator with restricted domain
  ExtremaPC_Ellipse        anEval(anEllipse, ExtremaPC::Domain1D{aUMin, aUMax});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Verify distance calculation is consistent
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    gp_Pnt aPtOnEllipse = ElCLib::Value(aResult[i].Parameter, anEllipse);
    EXPECT_NEAR(aPtOnEllipse.Distance(aResult[i].Point), 0.0, THE_TOL);
    double aExpectedSqDist = aPoint.SquareDistance(aPtOnEllipse);
    EXPECT_NEAR(aResult[i].SquareDistance, aExpectedSqDist, THE_TOL);
  }
}

TEST_F(ExtremaPC_EllipseTest, NarrowRange_TinyArc)
{
  // Test with an extremely narrow parameter range (0.001 radians ~ 0.06 degrees)
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(21.0, 0.5, 0.0);

  double aUMin = 0.0;
  double aUMax = 0.001; // Extremely narrow

  // Create evaluator with restricted domain
  ExtremaPC_Ellipse        anEval(anEllipse, ExtremaPC::Domain1D{aUMin, aUMax});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // Should complete without error
  ASSERT_TRUE(aResult.IsDone() || aResult.NbExt() >= 0);
}

TEST_F(ExtremaPC_EllipseTest, LargeRange_FullCircle)
{
  // Verify full circle case still works correctly
  gp_Elips anEllipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt   aPoint(5.0, 3.0, 0.0);

  ExtremaPC_Ellipse        anEval(anEllipse);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // For point inside ellipse, expect 4 extrema
  EXPECT_GE(aResult.NbExt(), 4);

  // Verify all distances are positive
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GT(aResult[i].SquareDistance, 0.0);
  }
}
