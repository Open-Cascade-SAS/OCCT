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

#include <ExtremaPS_Sphere.hxx>
#include <ExtremaPS_Surface.hxx>

#include <Geom_SphericalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for ExtremaPS_Sphere tests.
class ExtremaPS_SphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;
};

//==================================================================================================
// Basic projection tests
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, PointOutside_OnXAxis)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);

  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointOutside_OnYAxis)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.0, 25.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointOutside_OnZAxis)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.0, 0.0, 25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointOutside_Diagonal)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point at equal distance from all axes
  double aCoord = 25.0 / std::sqrt(3.0);
  gp_Pnt aPoint(aCoord, aCoord, aCoord);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointInside)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(3.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Min distance = 10 - 3 = 7
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 7.0, THE_TOL);

  // Max distance = 10 + 3 = 13
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 13.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointOnSurface)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(10.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

//==================================================================================================
// Degenerate case tests (poles)
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, PointAtCenter_Degenerate)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointOnNorthPole)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.0, 0.0, 25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);

  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointOnSouthPole)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.0, 0.0, -25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, PointNearPole)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.001, 0.0, 15.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Should find extremum near north pole
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].V, M_PI / 2.0, 0.1);
}

//==================================================================================================
// Boundary tests
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, ClosestToBoundaryU)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(20.0, -0.001, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  double aU = aResult[aMinIdx].U;
  // U should be near 0 or 2*PI
  EXPECT_TRUE(aU < 0.1 || aU > 2.0 * M_PI - 0.1);
}

TEST_F(ExtremaPS_SphereTest, ClosestToCorner)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(0.1, -0.001, 20.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_SphereTest, PartialDomain)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(20.0, 20.0, 0.0);

  // Only first quadrant
  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, M_PI / 2.0, 0.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at U = π/4, V = 0
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, M_PI / 4.0, 0.1);
  EXPECT_NEAR(aResult[aMinIdx].V, 0.0, 0.1);
}

//==================================================================================================
// Different radii tests
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, SmallRadius)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.1);
  gp_Pnt aPoint(1.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.9, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, LargeRadius)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1000.0);
  gp_Pnt aPoint(2000.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 1000.0, THE_TOL);
}

//==================================================================================================
// Translated sphere tests
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, TranslatedSphere)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(100, 50, 25), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(125.0, 50.0, 25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);

  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

//==================================================================================================
// SearchMode tests
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, SearchMode_Min)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, SearchMode_Max)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, TightTolerance)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  // Very tight tolerance
  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, 1.0e-12);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, 1.0e-10);
}

TEST_F(ExtremaPS_SphereTest, LargeTolerance)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  // Large tolerance
  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, 0.1);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, 0.5);
}

//==================================================================================================
// Aggregator tests
//==================================================================================================

TEST_F(ExtremaPS_SphereTest, Aggregator_Basic)
{
  occ::handle<Geom_SphericalSurface> aGeomSphere = new Geom_SphericalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Surface anAdaptor(aGeomSphere);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(25.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult =anExtPS.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SphereTest, Aggregator_SearchMode_Min)
{
  occ::handle<Geom_SphericalSurface> aGeomSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Surface anAdaptor(aGeomSphere);
  ExtremaPS_Surface   anExtPS(anAdaptor);

  gp_Pnt                   aPoint(25.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult =
    anExtPS.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaPS_SphereTest, Aggregator_SearchMode_Max)
{
  occ::handle<Geom_SphericalSurface> aGeomSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Surface anAdaptor(aGeomSphere);
  ExtremaPS_Surface   anExtPS(anAdaptor);

  gp_Pnt                   aPoint(25.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}
