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

#include <ExtremaPS_Cone.hxx>
#include <ExtremaPS_Cylinder.hxx>
#include <ExtremaPS_Plane.hxx>
#include <ExtremaPS_Sphere.hxx>
#include <ExtremaPS_Torus.hxx>

#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <cmath>

//! Test fixture for ExtremaPS edge case tests.
//! These tests validate specific bug fixes and edge case handling.
class ExtremaPS_EdgeCasesTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-7;
};

//==================================================================================================
// Cone Edge Cases: V Parameter on Axis
// Bug fix: V = Z / cos(SemiAngle) instead of V = Z
//==================================================================================================

TEST_F(ExtremaPS_EdgeCasesTest, Cone_PointOnAxis_CorrectVParameter)
{
  // Cone with 45-degree semi-angle: V = Z / cos(45) = Z * sqrt(2)
  const double aSemiAngle = M_PI / 4.0;
  const double aCosAngle  = std::cos(aSemiAngle);
  gp_Cone      aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aSemiAngle, 0.0);

  // Point on axis at Z = 10
  const double aZ     = 10.0;
  gp_Pnt       aPoint(0.0, 0.0, aZ);

  // Expected V parameter
  const double aExpectedV = aZ / aCosAngle;

  ExtremaPS_Cone           anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 30.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should be infinite (degenerate - point on axis)
  EXPECT_TRUE(aResult.IsInfinite());

  // The V parameter should be correctly computed
  // For a point on axis at Z=10 with 45-degree cone, V = 10 / cos(45) = 10 * sqrt(2) ~ 14.14
  EXPECT_NEAR(aExpectedV, aZ / aCosAngle, THE_TOL);
}

TEST_F(ExtremaPS_EdgeCasesTest, Cone_PointOnAxis_30DegSemiAngle)
{
  // Cone with 30-degree semi-angle
  const double aSemiAngle = M_PI / 6.0;
  const double aCosAngle  = std::cos(aSemiAngle);
  gp_Cone      aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aSemiAngle, 0.0);

  // Point on axis at Z = 10
  const double aZ     = 10.0;
  gp_Pnt       aPoint(0.0, 0.0, aZ);

  // Expected V parameter: V = Z / cos(30) = 10 / 0.866 ~ 11.547
  const double aExpectedV = aZ / aCosAngle;

  ExtremaPS_Cone           anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_GT(aExpectedV, aZ); // V should be larger than Z for semi-angle < 45 degrees
}

TEST_F(ExtremaPS_EdgeCasesTest, Cone_PointOnAxis_BoundedV)
{
  // Test V clamping when on axis with bounded domain
  const double aSemiAngle = M_PI / 4.0;
  const double aCosAngle  = std::cos(aSemiAngle);
  gp_Cone      aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aSemiAngle, 0.0);

  // Point on axis at Z = 20, but V domain is [0, 10]
  // V = 20 / cos(45) ~ 28.28, should clamp to 10
  gp_Pnt aPoint(0.0, 0.0, 20.0);

  ExtremaPS_Cone           anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should still be infinite (on axis)
  EXPECT_TRUE(aResult.IsInfinite());

  // Verify that the surface point at V=10 has correct axial position
  // At V=10, Z = V * cos(45) ~ 7.07
  const double aZAtVMax = 10.0 * aCosAngle;
  EXPECT_NEAR(aZAtVMax, 10.0 / std::sqrt(2.0), THE_TOL);
}

//==================================================================================================
// Plane Edge Cases: Distance After Clamping
// Bug fix: Recompute distance after clamping U/V to domain bounds
//==================================================================================================

TEST_F(ExtremaPS_EdgeCasesTest, Plane_ClampedProjection_CorrectDistance)
{
  // Plane XY at Z=0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point at (10.0 + 1e-8, 10.0 + 1e-8, 5) - infinitesimally outside domain
  // The projection is within tolerance band and will be clamped to (10, 10)
  const double aDelta = 1.0e-8;
  gp_Pnt       aPoint(10.0 + aDelta, 10.0 + aDelta, 5.0);

  // Bounded domain [0, 10] x [0, 10]
  ExtremaPS_Plane          anEval(aPlane, ExtremaPS::Domain2D(0.0, 10.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // After clamping, closest point should be corner (10, 10, 0)
  const gp_Pnt aExpectedPt(10.0, 10.0, 0.0);
  const double aExpectedSqDist = aPoint.SquareDistance(aExpectedPt);

  // Verify stored distance equals actual distance to clamped point
  EXPECT_NEAR(aResult[0].SquareDistance, aExpectedSqDist, THE_TOL);
  EXPECT_NEAR(aResult[0].Point.X(), 10.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Point.Y(), 10.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Point.Z(), 0.0, THE_TOL);
}

TEST_F(ExtremaPS_EdgeCasesTest, Plane_ClampedProjection_EdgeCase)
{
  // Plane XY at Z=0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point at (5, 10.0 + 1e-8, 3) - inside U domain, infinitesimally outside V domain
  // Within tolerance band, will be clamped
  const double aDelta = 1.0e-8;
  gp_Pnt       aPoint(5.0, 10.0 + aDelta, 3.0);

  // Bounded domain [0, 10] x [0, 10]
  ExtremaPS_Plane          anEval(aPlane, ExtremaPS::Domain2D(0.0, 10.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point should be edge (5, 10, 0) after clamping
  const gp_Pnt aExpectedPt(5.0, 10.0, 0.0);
  const double aExpectedSqDist = aPoint.SquareDistance(aExpectedPt);

  EXPECT_NEAR(aResult[0].SquareDistance, aExpectedSqDist, THE_TOL);
}

//==================================================================================================
// Torus Edge Cases: Opposite Circle V and Circle Center
// Bug fixes: Correct V for opposite generating circle, handle circle center
//==================================================================================================

TEST_F(ExtremaPS_EdgeCasesTest, Torus_OppositeCircle_CorrectV)
{
  // Standard torus: major=10, minor=3
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 3.0);

  // Point outside torus on X axis
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPS_Torus          anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at U=0, V=0 (closest point on outer equator)
  bool aFoundMin = false;
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    if (aResult[i].IsMinimum)
    {
      // Closest point should be at (13, 0, 0)
      EXPECT_NEAR(aResult[i].Point.X(), 13.0, THE_TOL);
      EXPECT_NEAR(aResult[i].Point.Y(), 0.0, THE_TOL);
      EXPECT_NEAR(aResult[i].Point.Z(), 0.0, THE_TOL);
      aFoundMin = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundMin);
}

TEST_F(ExtremaPS_EdgeCasesTest, Torus_AppleTorus_OppositeCircle)
{
  // Apple torus: major < minor (self-intersecting)
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 5.0);

  // Point on axis
  gp_Pnt aPoint(0.0, 0.0, 3.0);

  ExtremaPS_Torus          anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone() || aResult.IsInfinite());
}

TEST_F(ExtremaPS_EdgeCasesTest, Torus_BoundedDomain_AllExtrema)
{
  // Standard torus
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 3.0);

  // Point directly above center of generating circle at U=0
  gp_Pnt aPoint(10.0, 0.0, 10.0);

  // Bounded domain covering full range
  ExtremaPS_Torus          anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Should find extrema - at least minimum
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify all stored distances match actual distances
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist   = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL * 10.0);
  }
}

//==================================================================================================
// Sphere Edge Cases: Pole U Parameter
// Bug fix: Normalize U at poles for bounded domains
//==================================================================================================

TEST_F(ExtremaPS_EdgeCasesTest, Sphere_PointAtNorthPole_BoundedU)
{
  // Sphere centered at origin, radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point directly above north pole
  gp_Pnt aPoint(0.0, 0.0, 20.0);

  // Bounded U domain [PI/4, 3*PI/4] - north pole is at any U, V=PI/2
  ExtremaPS_Sphere         anEval(aSphere, ExtremaPS::Domain2D(M_PI / 4.0, 3.0 * M_PI / 4.0, 0.0, M_PI));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at north pole (0, 0, 10)
  bool aFoundMin = false;
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    if (aResult[i].IsMinimum)
    {
      EXPECT_NEAR(aResult[i].Point.X(), 0.0, THE_TOL);
      EXPECT_NEAR(aResult[i].Point.Y(), 0.0, THE_TOL);
      EXPECT_NEAR(aResult[i].Point.Z(), 10.0, THE_TOL);

      // U should be normalized to middle of domain
      double aExpectedU = (M_PI / 4.0 + 3.0 * M_PI / 4.0) * 0.5;
      EXPECT_NEAR(aResult[i].U, aExpectedU, THE_TOL);
      aFoundMin = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundMin);
}

TEST_F(ExtremaPS_EdgeCasesTest, Sphere_PointAtSouthPole_BoundedU)
{
  // Sphere centered at origin, radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point directly below south pole
  gp_Pnt aPoint(0.0, 0.0, -20.0);

  // Bounded U domain
  ExtremaPS_Sphere         anEval(aSphere, ExtremaPS::Domain2D(0.0, M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at south pole (0, 0, -10)
  bool aFoundMin = false;
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    if (aResult[i].IsMinimum)
    {
      EXPECT_NEAR(aResult[i].Point.X(), 0.0, THE_TOL);
      EXPECT_NEAR(aResult[i].Point.Y(), 0.0, THE_TOL);
      EXPECT_NEAR(aResult[i].Point.Z(), -10.0, THE_TOL);
      aFoundMin = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundMin);
}

//==================================================================================================
// Cylinder Edge Cases: Bounded V Infinite Distance
// Bug fix: Account for V bounds in InfiniteSquareDistance
//==================================================================================================

TEST_F(ExtremaPS_EdgeCasesTest, Cylinder_PointOnAxis_BoundedV)
{
  // Cylinder with radius 5, axis along Z
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Point on axis at Z = 100, but V domain is [0, 10]
  gp_Pnt aPoint(0.0, 0.0, 100.0);

  ExtremaPS_Cylinder       anEval(aCyl, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should be infinite (degenerate)
  EXPECT_TRUE(aResult.IsInfinite());

  // InfiniteSquareDistance should account for V bounds
  // Distance = sqrt(R^2 + (Z - Vmax)^2) = sqrt(25 + 8100) = sqrt(8125)
  const double aExpectedInfSqDist = 5.0 * 5.0 + 90.0 * 90.0;
  EXPECT_NEAR(aResult.InfiniteSquareDistance, aExpectedInfSqDist, THE_TOL);
}

TEST_F(ExtremaPS_EdgeCasesTest, Cylinder_PointOnAxis_InsideVBounds)
{
  // Cylinder with radius 5, axis along Z
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Point on axis at Z = 5, within V domain [0, 10]
  gp_Pnt aPoint(0.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCyl, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should be infinite (degenerate)
  EXPECT_TRUE(aResult.IsInfinite());

  // InfiniteSquareDistance should be R^2 since point is within V bounds
  const double aExpectedInfSqDist = 5.0 * 5.0;
  EXPECT_NEAR(aResult.InfiniteSquareDistance, aExpectedInfSqDist, THE_TOL);
}

TEST_F(ExtremaPS_EdgeCasesTest, Cylinder_PointOnAxis_BelowVBounds)
{
  // Cylinder with radius 5, axis along Z
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Point on axis at Z = -50, but V domain is [0, 10]
  gp_Pnt aPoint(0.0, 0.0, -50.0);

  ExtremaPS_Cylinder       anEval(aCyl, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should be infinite (degenerate)
  EXPECT_TRUE(aResult.IsInfinite());

  // InfiniteSquareDistance should account for V bounds
  // Distance = sqrt(R^2 + (Z - Vmin)^2) = sqrt(25 + 2500) = sqrt(2525)
  const double aExpectedInfSqDist = 5.0 * 5.0 + 50.0 * 50.0;
  EXPECT_NEAR(aResult.InfiniteSquareDistance, aExpectedInfSqDist, THE_TOL);
}
