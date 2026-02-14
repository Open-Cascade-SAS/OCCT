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

#include <ExtremaPS.hxx>
#include <ExtremaPS_Surface.hxx>
#include <ExtremaPS_Torus.hxx>

#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1.0e-7;
const double THE_PI        = M_PI;
} // namespace

//==================================================================================================
// Test fixture for ExtremaPS_Torus tests
//==================================================================================================
class ExtremaPS_TorusTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Default torus: major radius 5, minor radius 1, at origin with Z axis
    myTorus = gp_Torus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  }

  gp_Torus myTorus;
};

//==================================================================================================
// Basic Projection Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, BasicProjection_PointOutsideTorusXAxis)
{
  // Point outside torus on X axis - projects to outer edge
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum should be at U=0, V=0 (outer edge at +X)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE); // distance is 10 - 6 = 4
}

TEST_F(ExtremaPS_TorusTest, BasicProjection_PointInsideHole)
{
  // Point inside the torus hole (on axis)
  gp_Pnt         aP(0.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  // Point on axis -> infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaPS::Status::InfiniteSolutions);
}

TEST_F(ExtremaPS_TorusTest, BasicProjection_PointAboveTorus)
{
  // Point above the torus
  gp_Pnt         aP(5.0, 0.0, 5.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum should be at U=0, V=PI/2 (top of generating circle)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE); // distance is 5 - 1 = 4
}

TEST_F(ExtremaPS_TorusTest, BasicProjection_PointBelowTorus)
{
  // Point below the torus
  gp_Pnt         aP(5.0, 0.0, -3.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum should be at U=0, V=3*PI/2 (bottom of generating circle)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 2.0, THE_TOLERANCE); // distance is 3 - 1 = 2
}

TEST_F(ExtremaPS_TorusTest, BasicProjection_PointOnTorusSurface)
{
  // Point exactly on torus surface
  gp_Pnt         aP(6.0, 0.0, 0.0); // outer edge at U=0, V=0
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, BasicProjection_PointInsideTorus)
{
  // Point inside torus tube
  gp_Pnt         aP(5.5, 0.0, 0.0); // between inner (4) and outer (6) radius
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 0.5, THE_TOLERANCE);
}

//==================================================================================================
// Degenerate Case Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, Degenerate_PointOnAxis)
{
  // Point exactly on torus axis
  gp_Pnt         aP(0.0, 0.0, 3.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  EXPECT_EQ(aResult.Status, ExtremaPS::Status::InfiniteSolutions);
}

TEST_F(ExtremaPS_TorusTest, Degenerate_PointNearAxis)
{
  // Point very close to axis but not exactly on it
  gp_Pnt         aP(1.0e-8, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  EXPECT_EQ(aResult.Status, ExtremaPS::Status::InfiniteSolutions);
}

TEST_F(ExtremaPS_TorusTest, Degenerate_PointFarOnAxis)
{
  // Point far on axis
  gp_Pnt         aP(0.0, 0.0, 100.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  EXPECT_EQ(aResult.Status, ExtremaPS::Status::InfiniteSolutions);
  // Infinite square distance should be set
  double aExpectedDist = std::sqrt(100.0 * 100.0 + 5.0 * 5.0) - 1.0;
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), aExpectedDist, 0.1);
}

//==================================================================================================
// Different Torus Configurations
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, Config_LargeMajorRadius)
{
  // Large major radius
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 100.0, 1.0);
  gp_Pnt         aP(105.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE); // 105 - 101 = 4
}

TEST_F(ExtremaPS_TorusTest, Config_LargeMinorRadius)
{
  // Large minor radius (thick tube)
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 3.0);
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 2.0, THE_TOLERANCE); // 10 - 8 = 2
}

TEST_F(ExtremaPS_TorusTest, Config_SmallMinorRadius)
{
  // Small minor radius (thin tube)
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 0.1);
  gp_Pnt         aP(6.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 0.9, THE_TOLERANCE); // 6 - 5.1 = 0.9
}

TEST_F(ExtremaPS_TorusTest, Config_AlmostRing)
{
  // Minor radius almost equals major radius
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 4.9);
  gp_Pnt         aP(15.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 5.1, THE_TOLERANCE); // 15 - 9.9 = 5.1
}

//==================================================================================================
// Translated/Rotated Torus Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, Transform_TranslatedTorus)
{
  // Torus centered at (10, 10, 10)
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(10, 10, 10), gp_Dir(0, 0, 1)), 5.0, 1.0);
  gp_Pnt         aP(20.0, 10.0, 10.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE); // 20-10 - 6 = 4
}

TEST_F(ExtremaPS_TorusTest, Transform_RotatedTorusXAxis)
{
  // Torus with axis along X
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 5.0, 1.0);
  gp_Pnt         aP(0.0, 10.0, 0.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, Transform_RotatedTorusYAxis)
{
  // Torus with axis along Y
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 5.0, 1.0);
  gp_Pnt         aP(0.0, 0.0, 10.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, Transform_TiltedTorus)
{
  // Torus with tilted axis (45 degrees from Z)
  gp_Dir         aTiltedDir(1.0 / std::sqrt(2.0), 0.0, 1.0 / std::sqrt(2.0));
  gp_Torus       aTorus(gp_Ax3(gp_Pnt(0, 0, 0), aTiltedDir), 5.0, 1.0);
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, SearchMode_MinOnly)
{
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // All results should be minima
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_TRUE(aResult.Extrema.Value(i).IsMinimum);
  }
}

TEST_F(ExtremaPS_TorusTest, SearchMode_MaxOnly)
{
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // All results should be maxima
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_FALSE(aResult.Extrema.Value(i).IsMinimum);
  }
}

TEST_F(ExtremaPS_TorusTest, SearchMode_MinMax)
{
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  // Torus should have multiple extrema (4 for general point)
  EXPECT_GE(aResult.Extrema.Length(), 2);
}

//==================================================================================================
// Boundary/Partial Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, Boundary_PartialURange)
{
  // Only first quadrant in U
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, THE_PI / 2.0, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // All U should be in [0, PI/2]
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_GE(aResult.Extrema.Value(i).U, -THE_TOLERANCE);
    EXPECT_LE(aResult.Extrema.Value(i).U, THE_PI / 2.0 + THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_TorusTest, Boundary_PartialVRange)
{
  // Only upper half of generating circle (V in [0, PI])
  gp_Pnt         aP(5.0, 0.0, 3.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_TorusTest, Boundary_SmallUVPatch)
{
  // Small patch around U=0, V=0
  gp_Pnt         aP(6.5, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(-0.1, 0.1, -0.1, 0.1));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_TorusTest, Boundary_ExtremumOutsideRange)
{
  // Point projects to U=PI, but range is [0, PI/2]
  gp_Pnt         aP(-10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, THE_PI / 2.0, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  // Should still find boundary extrema
  if (aResult.Status == ExtremaPS::Status::OK)
  {
    for (int i = 0; i < aResult.Extrema.Length(); ++i)
    {
      EXPECT_GE(aResult.Extrema.Value(i).U, -THE_TOLERANCE);
      EXPECT_LE(aResult.Extrema.Value(i).U, THE_PI / 2.0 + THE_TOLERANCE);
    }
  }
}

//==================================================================================================
// Aggregator Integration Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, Aggregator_ToroidalSurface)
{
  occ::handle<Geom_ToroidalSurface> aGeomTorus = new Geom_ToroidalSurface(myTorus);
  GeomAdaptor_Surface          anAdaptor(aGeomTorus);
  ExtremaPS_Surface            anExtPS(anAdaptor);

  gp_Pnt            aP(10.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, Aggregator_WithSearchMode)
{
  occ::handle<Geom_ToroidalSurface> aGeomTorus = new Geom_ToroidalSurface(myTorus);
  GeomAdaptor_Surface          anAdaptor(aGeomTorus);
  ExtremaPS_Surface            anExtPS(anAdaptor);

  gp_Pnt                   aP(10.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult =
    anExtPS.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_TRUE(aResult.Extrema.Value(i).IsMinimum);
  }
}

//==================================================================================================
// Multiple Extrema Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, MultipleExtrema_FourExtrema)
{
  // General point should give 4 extrema on full torus
  gp_Pnt         aP(8.0, 0.0, 2.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  // Should have at least 2 extrema (possibly 4)
  EXPECT_GE(aResult.Extrema.Length(), 2);
}

TEST_F(ExtremaPS_TorusTest, MultipleExtrema_MinMaxDifferent)
{
  gp_Pnt         aP(10.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 2);

  double aMinDist = aResult.MinSquareDistance();
  double aMaxDist = aResult.MaxSquareDistance();

  EXPECT_LT(aMinDist, aMaxDist);
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE);  // outer edge: 10 - 6 = 4
  EXPECT_NEAR(std::sqrt(aMaxDist), 16.0, THE_TOLERANCE); // farthest point: 10 + 6 = 16
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, EdgeCase_PointOnMajorCircle)
{
  // Point on the major circle plane, at major radius distance
  gp_Pnt         aP(5.0, 0.0, 0.0); // center of generating circle
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 1.0, THE_TOLERANCE); // minor radius
}

TEST_F(ExtremaPS_TorusTest, EdgeCase_PointAtInnerEdge)
{
  // Point at inner edge of torus
  gp_Pnt         aP(4.0, 0.0, 0.0); // exactly at inner radius
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, EdgeCase_PointAtOuterEdge)
{
  // Point at outer edge of torus
  gp_Pnt         aP(6.0, 0.0, 0.0); // exactly at outer radius
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, EdgeCase_PointAtTopOfTorus)
{
  // Point at top of torus (Z = minor radius)
  gp_Pnt         aP(5.0, 0.0, 1.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_TorusTest, EdgeCase_VeryFarPoint)
{
  // Point very far from torus
  gp_Pnt         aP(1000.0, 0.0, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 994.0, THE_TOLERANCE); // 1000 - 6 = 994
}

TEST_F(ExtremaPS_TorusTest, EdgeCase_DiagonalPoint)
{
  // Point at 45 degrees in XY plane
  double         aCoord = 10.0 / std::sqrt(2.0);
  gp_Pnt         aP(aCoord, aCoord, 0.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 4.0, THE_TOLERANCE); // 10 - 6 = 4
}

//==================================================================================================
// Result Verification Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusTest, Verify_PointOnSurface)
{
  gp_Pnt         aP(8.0, 3.0, 1.5);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Verify each extremum point is on the surface
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt = aResult.Extrema.Value(i);
    gp_Pnt                           aSurfPt = ElSLib::Value(anExt.U, anExt.V, myTorus);
    EXPECT_NEAR(anExt.Point.Distance(aSurfPt), 0.0, THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_TorusTest, Verify_SquareDistanceConsistent)
{
  gp_Pnt         aP(7.0, 2.0, 1.0);
  ExtremaPS_Torus anEval(myTorus, ExtremaPS::Domain2D(0.0, 2.0 * THE_PI, 0.0, 2.0 * THE_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Verify square distance matches actual distance
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt      = aResult.Extrema.Value(i);
    double                           aActualSqDist = aP.SquareDistance(anExt.Point);
    EXPECT_NEAR(anExt.SquareDistance, aActualSqDist, THE_TOLERANCE);
  }
}
