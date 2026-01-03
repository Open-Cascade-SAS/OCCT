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

#include <ExtremaCC_CircleCircle.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCC_CircleCircle tests.
class ExtremaCC_CircleCircleTest : public testing::Test
{
protected:
  static constexpr double THE_TOL    = 1.0e-10;
  static constexpr double THE_PI     = 3.14159265358979323846;
  static constexpr double THE_TWO_PI = 2.0 * THE_PI;
};

//==================================================================================================
// Concentric circles tests
//==================================================================================================

TEST_F(ExtremaCC_CircleCircleTest, ConcentricCircles_SamePlane)
{
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  // Concentric circles: infinite solutions with constant distance
  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  // Distance = R1 - R2 = 10 - 5 = 5
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleCircleTest, ConcentricCircles_SameRadius)
{
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  // Same circles: infinite solutions with zero distance
  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Coplanar circles tests
//==================================================================================================

TEST_F(ExtremaCC_CircleCircleTest, CoplanarCircles_External)
{
  // Two circles in XY plane, not overlapping
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(30, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum: 30 - 10 - 10 = 10
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL);
  // Maximum: 30 + 10 + 10 = 50
  EXPECT_NEAR(aResult.MaxSquareDistance(), 2500.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleCircleTest, CoplanarCircles_Intersecting)
{
  // Two circles in XY plane, overlapping
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum distance is 0 (they intersect)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
  // Maximum: 15 + 10 + 10 = 35
  EXPECT_NEAR(aResult.MaxSquareDistance(), 1225.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleCircleTest, CoplanarCircles_Tangent)
{
  // Two circles tangent externally
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum distance is 0 (tangent)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleCircleTest, CoplanarCircles_OneInsideOther)
{
  // Circle2 inside Circle1
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum: closest points on same side
  // Circle1 at (20,0,0), Circle2 at (10,0,0), distance = 10
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL);
}

//==================================================================================================
// Non-coplanar circles tests
//==================================================================================================

TEST_F(ExtremaCC_CircleCircleTest, ParallelPlanes_OffsetZ)
{
  // Circle1 in Z=0, Circle2 in Z=10
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum distance is Z offset = 10
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL);
  // Maximum: sqrt(20^2 + 10^2) = sqrt(500)
  EXPECT_NEAR(aResult.MaxSquareDistance(), 500.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleCircleTest, PerpendicularPlanes)
{
  // Circle1 in XY plane, Circle2 in XZ plane
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Circles intersect at (10,0,0) and (-10,0,0)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCC_CircleCircleTest, PerpendicularPlanes_Offset)
{
  // Circle1 in XY at Z=0, Circle2 in XZ at Y=20
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 20, 0), gp_Dir(0, 1, 0)), 10.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Circle1: (10*cos(t1), 10*sin(t1), 0), Circle2: (10*cos(t2), 20, 10*sin(t2))
  // Minimum at t1=atan(2), t2=0: D^2 = 600 - 200*sqrt(5) ~ 152.786
  double aExpectedMinSqDist = 600.0 - 200.0 * std::sqrt(5.0);
  EXPECT_NEAR(aResult.MinSquareDistance(), aExpectedMinSqDist, 1.0e-6);
}

//==================================================================================================
// Tilted circles tests
//==================================================================================================

TEST_F(ExtremaCC_CircleCircleTest, TiltedCircles_45Degrees)
{
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 1)), 10.0); // gp_Dir auto-normalizes

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Circles share center and have same radius, they intersect
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCC_CircleCircleTest, PartialArcs_BothQuarter)
{
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(30, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // First quadrant arcs only
  ExtremaCC::Domain2D    aDomain{{0.0, THE_PI / 2.0}, {THE_PI / 2.0, THE_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Check that results are within domain
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter1, 0.0 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter1, THE_PI / 2.0 + THE_TOL);
    EXPECT_GE(aResult[i].Parameter2, THE_PI / 2.0 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter2, THE_PI + THE_TOL);
  }
}

//==================================================================================================
// Point verification tests
//==================================================================================================

TEST_F(ExtremaCC_CircleCircleTest, VerifyClosestPoints)
{
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1)), 8.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Verify all extrema have consistent distances
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aComputedDist = aResult[i].Point1.SquareDistance(aResult[i].Point2);
    EXPECT_NEAR(aResult[i].SquareDistance, aComputedDist, THE_TOL);

    // Verify Point1 is on circle1 (distance from center = R1)
    double aRadialDist1 = aResult[i].Point1.Distance(gp_Pnt(0, 0, 0));
    EXPECT_NEAR(aRadialDist1, 10.0, THE_TOL);

    // Verify Point2 is on circle2 (distance from center = R2)
    double aRadialDist2 = aResult[i].Point2.Distance(gp_Pnt(0, 0, 5));
    EXPECT_NEAR(aRadialDist2, 8.0, THE_TOL);
  }
}

TEST_F(ExtremaCC_CircleCircleTest, VerifyDistanceConsistency)
{
  gp_Circ aCircle1(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Circ aCircle2(gp_Ax2(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCC::Domain2D    aDomain{{0.0, THE_TWO_PI}, {0.0, THE_TWO_PI}};
  ExtremaCC_CircleCircle anExtrema(aCircle1, aCircle2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Verify MinSquareDistance() and MaxSquareDistance() match stored values
  int aMinIdx = aResult.MinIndex();
  int aMaxIdx = aResult.MaxIndex();

  EXPECT_NEAR(aResult.MinSquareDistance(), aResult[aMinIdx].SquareDistance, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), aResult[aMaxIdx].SquareDistance, THE_TOL);
}
