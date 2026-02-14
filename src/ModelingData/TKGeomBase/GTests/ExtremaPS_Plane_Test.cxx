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

#include <ExtremaPS_Plane.hxx>
#include <ExtremaPS_Surface.hxx>

#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaPS_Plane tests.
class ExtremaPS_PlaneTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;
};

//==================================================================================================
// Basic projection tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, PointAbove_XYPlane)
{
  // XY plane at Z = 0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point above plane
  gp_Pnt aPoint(5.0, 3.0, 10.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, 3.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 100.0, THE_TOL); // 10^2
  EXPECT_TRUE(aResult[aMinIdx].IsMinimum);
}

TEST_F(ExtremaPS_PlaneTest, PointBelow_XYPlane)
{
  // XY plane at Z = 0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point below plane
  gp_Pnt aPoint(5.0, 3.0, -10.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, 3.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 100.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, PointOnPlane)
{
  // XY plane at Z = 0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point on plane
  gp_Pnt aPoint(5.0, 3.0, 0.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, PointVeryClose)
{
  // XY plane at Z = 0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point very close to plane
  gp_Pnt aPoint(5.0, 3.0, 1.0e-10);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 1.0e-20, 1.0e-15);
}

//==================================================================================================
// Tilted plane tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, TiltedPlane_45Deg_X)
{
  // Plane tilted 45 degrees about X axis
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 1));

  // Point above tilted plane
  gp_Pnt aPoint(0.0, 10.0, 10.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance is correct (perpendicular distance to plane)
  double aExpectedDist = std::abs((10.0 + 10.0) / std::sqrt(2.0));
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedDist, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, TiltedPlane_45Deg_Y)
{
  // Plane tilted 45 degrees about Y axis
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 1));

  // Point offset
  gp_Pnt aPoint(5.0, 0.0, 5.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance is perpendicular component
  double aExpectedDist = std::abs((5.0 + 5.0) / std::sqrt(2.0));
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedDist, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, TiltedPlane_Arbitrary)
{
  // Arbitrary tilted plane
  gp_Pln aPlane(gp_Pnt(10, 20, 30), gp_Dir(1, 2, 3));

  gp_Pnt aPoint(15.0, 25.0, 35.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify stored distance matches computed distance
  int aMinIdx = aResult.MinIndex();
  double aComputedDist = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aComputedDist, THE_TOL);
}

//==================================================================================================
// Translated plane tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, TranslatedPlane_Positive)
{
  // XY plane at Z = 10
  gp_Pln aPlane(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));

  gp_Pnt aPoint(5.0, 3.0, 25.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 25 - 10 = 15
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 15.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, TranslatedPlane_Negative)
{
  // XY plane at Z = -10
  gp_Pln aPlane(gp_Pnt(0, 0, -10), gp_Dir(0, 0, 1));

  gp_Pnt aPoint(0.0, 0.0, 5.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 5 - (-10) = 15
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 15.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, TranslatedPlane_LargeOffset)
{
  // Plane with large offset
  gp_Pln aPlane(gp_Pnt(1000, 2000, 3000), gp_Dir(0, 0, 1));

  gp_Pnt aPoint(1005.0, 2003.0, 3010.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-1000.0, 1000.0, -1000.0, 1000.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, 3.0, THE_TOL);
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 10.0, THE_TOL);
}

//==================================================================================================
// Boundary tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, ProjectionOutsideBounds)
{
  // XY plane at Z = 0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point that projects outside bounds
  gp_Pnt aPoint(50.0, 50.0, 10.0);

  // Bounded to [0,10] x [0,10]
  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(0.0, 10.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should find at least one extremum (boundary corner)
  ASSERT_GE(aResult.NbExt(), 1);

  // Check that parameters are within bounds
  int aMinIdx = aResult.MinIndex();
  EXPECT_GE(aResult[aMinIdx].U, 0.0 - THE_TOL);
  EXPECT_LE(aResult[aMinIdx].U, 10.0 + THE_TOL);
  EXPECT_GE(aResult[aMinIdx].V, 0.0 - THE_TOL);
  EXPECT_LE(aResult[aMinIdx].V, 10.0 + THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, VerySmallRange)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(0.5, 0.5, 10.0);

  // Very small parametric range
  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Projection should be at (0.5, 0.5)
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 0.5, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, 0.5, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, NegativeBounds)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(-5.0, -3.0, 10.0);

  // Negative parametric range
  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-10.0, 0.0, -10.0, 0.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, -5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, -3.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, MixedSignBounds)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(3.0, -4.0, 5.0);

  // Mixed sign bounds
  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-10.0, 10.0, -10.0, 10.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 3.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, -4.0, THE_TOL);
}

//==================================================================================================
// SearchMode tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, SearchMode_Min)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(5.0, 3.0, 10.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaPS_PlaneTest, SearchMode_Max)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(5.0, 5.0, 10.0);

  // Bounded region with corners
  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(0.0, 10.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_PlaneTest, SearchMode_MinMax)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(5.0, 5.0, 10.0);

  // Bounded region
  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(0.0, 10.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  // Should find both min and max (corner extrema)
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Aggregator tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, Aggregator_Basic)
{
  occ::handle<Geom_Plane> aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomAdaptor_Surface anAdaptor(aGeomPlane, -100.0, 100.0, -100.0, 100.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(5.0, 3.0, 10.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 100.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, Aggregator_TiltedPlane)
{
  occ::handle<Geom_Plane> aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  GeomAdaptor_Surface anAdaptor(aGeomPlane, -100.0, 100.0, -100.0, 100.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(10.0, 10.0, 10.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Point is on plane normal axis, distance is 10*sqrt(3)/sqrt(3) = 10
  double aExpectedDist = 30.0 / std::sqrt(3.0);
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedDist, THE_TOL);
}

//==================================================================================================
// Special coordinate tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, Origin_XYPlane)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, LargeCoordinates)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(1.0e6, 1.0e6, 100.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-1.0e7, 1.0e7, -1.0e7, 1.0e7));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 1.0e6, THE_TOL * 1.0e6);
  EXPECT_NEAR(aResult[aMinIdx].V, 1.0e6, THE_TOL * 1.0e6);
}

TEST_F(ExtremaPS_PlaneTest, SmallCoordinates)
{
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(1.0e-6, 1.0e-6, 1.0e-3);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-1.0, 1.0, -1.0, 1.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 1.0e-6, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].V, 1.0e-6, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 1.0e-6, THE_TOL);
}

//==================================================================================================
// Non-standard orientations
//==================================================================================================

TEST_F(ExtremaPS_PlaneTest, YZPlane)
{
  // YZ plane (normal along X)
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  gp_Pnt aPoint(10.0, 5.0, 3.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 10 (X component)
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 10.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, XZPlane)
{
  // XZ plane (normal along Y)
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));

  gp_Pnt aPoint(5.0, 10.0, 3.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 10 (Y component)
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 10.0, THE_TOL);
}

TEST_F(ExtremaPS_PlaneTest, NegativeNormal)
{
  // XY plane with negative Z normal
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, -1));

  gp_Pnt aPoint(5.0, 3.0, 10.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 10.0, THE_TOL);
}
