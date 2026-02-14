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

#include <ExtremaCC_CircleLine.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCC_CircleLine tests.
class ExtremaCC_CircleLineTest : public testing::Test
{
protected:
  static constexpr double THE_TOL    = 1.0e-10;
  static constexpr double THE_PI     = 3.14159265358979323846;
  static constexpr double THE_TWO_PI = 2.0 * THE_PI;
};

//==================================================================================================
// Line through circle center tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, LineThroughCenter_Perpendicular)
{
  // Circle in XY plane, line along Z through center
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  // All points on circle are equidistant from line (R=10)
  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 100.0, THE_TOL); // R^2
}

TEST_F(ExtremaCC_CircleLineTest, LineThroughCenter_InPlane)
{
  // Circle in XY plane, line along X through center (in circle plane)
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should have 2 minima (intersections at R and -R) and 2 maxima (at +Y and -Y)
  ASSERT_GE(aResult.NbExt(), 2);

  // Check minimum distance is 0 (line intersects circle)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Line parallel to circle plane tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, LineParallelToPlane_OffsetZ)
{
  // Circle in XY plane at Z=0, line parallel at Z=5
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0));

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum: distance = sqrt(5^2) = 5 at closest point on circle
  // Point on circle at (10, 0, 0), line passes through (10, 0, 5)
  double aExpectedMinSqDist = 25.0; // 5^2
  EXPECT_NEAR(aResult.MinSquareDistance(), aExpectedMinSqDist, THE_TOL);
}

TEST_F(ExtremaCC_CircleLineTest, LineParallelToPlane_OffsetXY)
{
  // Circle in XY plane, line parallel but offset in Y
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 20, 5), gp_Dir(1, 0, 0));

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum: closest point on circle is at (0, 10, 0)
  // Line passes through (0, 20, 5), so distance = sqrt(10^2 + 5^2) = sqrt(125)
  double aExpectedMinSqDist = 125.0;
  EXPECT_NEAR(aResult.MinSquareDistance(), aExpectedMinSqDist, THE_TOL);
}

//==================================================================================================
// Line intersecting circle tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, LineIntersectsCircle_TwoPoints)
{
  // Circle in XY plane, line along X in same plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 5, 0), gp_Dir(1, 0, 0)); // Line at Y=5

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Line intersects circle at two points
  // At Y=5, X = +/-sqrt(100-25) = +/-sqrt(75)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleLineTest, LineTangentToCircle)
{
  // Circle in XY plane, line tangent at (10, 0, 0)
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0)); // Tangent line at (10,0,0)

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Tangent point is a minimum with distance 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Line outside circle tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, LineOutsideCircle_SamePlane)
{
  // Circle in XY plane R=10, line at Y=20 (outside circle)
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 20, 0), gp_Dir(1, 0, 0));

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum distance: from (0,10,0) on circle to line at Y=20
  // Distance = 20 - 10 = 10
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL);

  // Maximum distance: from (0,-10,0) to line
  // Distance = 20 + 10 = 30
  EXPECT_NEAR(aResult.MaxSquareDistance(), 900.0, THE_TOL);
}

//==================================================================================================
// 3D configuration tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, Circle3D_LineSkew)
{
  // Circle tilted, line skew
  gp_Ax2  anAx2(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  gp_Circ aCircle(anAx2, 5.0);
  gp_Lin  aLine(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance consistency
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aComputedDist = aResult[i].Point1.SquareDistance(aResult[i].Point2);
    EXPECT_NEAR(aResult[i].SquareDistance, aComputedDist, THE_TOL);
  }
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, PartialCircle_Arc)
{
  // Only a quarter of the circle
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  // Arc from 0 to pi/2 (first quadrant)
  ExtremaCC::Domain2D  aDomain{{0.0, THE_PI / 2.0}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum is at theta=0 where circle point is (10,0,0) which is on the line
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Point verification tests
//==================================================================================================

TEST_F(ExtremaCC_CircleLineTest, VerifyClosestPoints)
{
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Lin  aLine(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0)); // Line at Z=5

  ExtremaCC::Domain2D  aDomain{{0.0, THE_TWO_PI}, {-100.0, 100.0}};
  ExtremaCC_CircleLine anExtrema(aCircle, aLine, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Verify all extrema have consistent distances
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aComputedDist = aResult[i].Point1.SquareDistance(aResult[i].Point2);
    EXPECT_NEAR(aResult[i].SquareDistance, aComputedDist, THE_TOL);

    // Verify Point1 is on circle (distance from origin = R)
    double aRadialDist = aResult[i].Point1.Distance(gp_Pnt(0, 0, 0));
    EXPECT_NEAR(aRadialDist, 10.0, THE_TOL);

    // Verify Point2 is on line (Z = 5)
    EXPECT_NEAR(aResult[i].Point2.Z(), 5.0, THE_TOL);
  }
}
