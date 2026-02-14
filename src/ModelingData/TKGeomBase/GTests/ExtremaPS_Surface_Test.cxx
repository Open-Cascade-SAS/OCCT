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

#include <ExtremaPS_Surface.hxx>
#include <ExtremaPS_Plane.hxx>
#include <ExtremaPS_Cylinder.hxx>
#include <ExtremaPS_Cone.hxx>
#include <ExtremaPS_Sphere.hxx>
#include <ExtremaPS_Torus.hxx>
#include <ExtremaPS_BezierSurface.hxx>
#include <ExtremaPS_BSplineSurface.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <cmath>
#include <vector>

//! Test fixture for ExtremaPS tests.
class ExtremaPS_SurfaceTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;
};

//==================================================================================================
// Plane tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Plane_PointAbove)
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

TEST_F(ExtremaPS_SurfaceTest, Plane_PointOnPlane)
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

TEST_F(ExtremaPS_SurfaceTest, Plane_ProjectionOutsideBounds)
{
  // XY plane at Z = 0
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Point that projects outside bounds
  gp_Pnt aPoint(50.0, 50.0, 10.0);

  ExtremaPS_Plane anEval(aPlane, ExtremaPS::Domain2D(0.0, 10.0, 0.0, 10.0));
  // Bounded to [0,10] x [0,10]
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should still find a minimum (at corner or edge)
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Cylinder tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PointOutside)
{
  // Cylinder along Z axis with radius 10
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point outside cylinder
  gp_Pnt aPoint(25.0, 0.0, 5.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PointInside)
{
  // Cylinder along Z axis with radius 10
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point inside cylinder
  gp_Pnt aPoint(3.0, 0.0, 5.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 10 - 3 = 7
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 7.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PointOnAxis_Degenerate)
{
  // Cylinder along Z axis with radius 10
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point on axis - degenerate case
  gp_Pnt aPoint(0.0, 0.0, 5.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should return infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

//==================================================================================================
// Sphere tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_PointOutside)
{
  // Sphere at origin with radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point outside sphere
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2); // At least 2 (min and max), may include boundary extrema

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);

  // Maximum distance should be 25 + 10 = 35
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Sphere_PointAtCenter_Degenerate)
{
  // Sphere at origin with radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point at center - degenerate case
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should return infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

//==================================================================================================
// Cone tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Cone_PointOutside)
{
  // Cone with apex at origin, axis along Z, semi-angle 45 degrees
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);

  // Point outside cone
  gp_Pnt aPoint(20.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify that stored distance matches computed distance
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL);
  }
}

//==================================================================================================
// Torus tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Torus_PointOutside)
{
  // Torus at origin with major radius 20, minor radius 5
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);

  // Point outside torus on X axis
  gp_Pnt aPoint(40.0, 0.0, 0.0);

  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 40 - (20 + 5) = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Torus_PointOnAxis_Degenerate)
{
  // Torus at origin with major radius 20, minor radius 5
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);

  // Point on axis - degenerate case
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should return infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
}

//==================================================================================================
// Aggregator tests with GeomAdaptor_Surface
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Aggregator_Plane)
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

TEST_F(ExtremaPS_SurfaceTest, Aggregator_Cylinder)
{
  occ::handle<Geom_CylindricalSurface> aGeomCylinder = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Surface anAdaptor(aGeomCylinder, 0.0, 2.0 * M_PI, 0.0, 20.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(25.0, 0.0, 5.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Aggregator_Sphere)
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

TEST_F(ExtremaPS_SurfaceTest, Aggregator_Cone)
{
  occ::handle<Geom_ConicalSurface> aGeomCone = new Geom_ConicalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  GeomAdaptor_Surface anAdaptor(aGeomCone, 0.0, 2.0 * M_PI, 0.0, 20.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(20.0, 0.0, 5.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_SurfaceTest, Aggregator_Torus)
{
  occ::handle<Geom_ToroidalSurface> aGeomTorus = new Geom_ToroidalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);
  GeomAdaptor_Surface anAdaptor(aGeomTorus);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(40.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Aggregator_BezierSurface)
{
  // Create a simple 3x3 Bezier surface (degree 2 x 2)
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  aPoles(1, 1) = gp_Pnt(0, 0, 0);
  aPoles(1, 2) = gp_Pnt(0, 5, 0);
  aPoles(1, 3) = gp_Pnt(0, 10, 0);
  aPoles(2, 1) = gp_Pnt(5, 0, 2);
  aPoles(2, 2) = gp_Pnt(5, 5, 4);
  aPoles(2, 3) = gp_Pnt(5, 10, 2);
  aPoles(3, 1) = gp_Pnt(10, 0, 0);
  aPoles(3, 2) = gp_Pnt(10, 5, 0);
  aPoles(3, 3) = gp_Pnt(10, 10, 0);

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomAdaptor_Surface anAdaptor(aBezier);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  // Point above the surface center
  gp_Pnt aPoint(5.0, 5.0, 10.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify that stored distance matches computed distance
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL);
  }
}

TEST_F(ExtremaPS_SurfaceTest, Aggregator_BSplineSurface)
{
  // Create a simple 4x4 BSpline surface (degree 3 x 3)
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double aZ = (i == 2 || i == 3) && (j == 2 || j == 3) ? 3.0 : 0.0;
      aPoles(i, j) = gp_Pnt((i - 1) * 3.0, (j - 1) * 3.0, aZ);
    }
  }

  NCollection_Array1<double> aUKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;

  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  NCollection_Array1<int> aUMults(1, 2);
  aUMults(1) = 4;
  aUMults(2) = 4;

  NCollection_Array1<int> aVMults(1, 2);
  aVMults(1) = 4;
  aVMults(2) = 4;

  occ::handle<Geom_BSplineSurface> aBSpline = new Geom_BSplineSurface(
    aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
  GeomAdaptor_Surface anAdaptor(aBSpline);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  // Point above the surface
  gp_Pnt aPoint(4.5, 4.5, 10.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify that stored distance matches computed distance
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL);
  }
}

//==================================================================================================
// Edge case tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Aggregator_ConstructWithDomain)
{
  occ::handle<Geom_Plane> aGeomPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomAdaptor_Surface anAdaptor(aGeomPlane);

  // Construct with explicit domain
  ExtremaPS_Surface anExtPS(anAdaptor, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(5.0, 3.0, 10.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// SearchMode tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_SearchMode_Min)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1); // Only min
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Sphere_SearchMode_Max)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1); // At least max, may include boundary extrema

  // Maximum distance should be 25 + 10 = 35
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Aggregator_SearchMode_Min)
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
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Aggregator_SearchMode_Max)
{
  occ::handle<Geom_SphericalSurface> aGeomSphere =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Surface anAdaptor(aGeomSphere);
  ExtremaPS_Surface   anExtPS(anAdaptor);

  gp_Pnt                   aPoint(25.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult =
    anExtPS.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1); // At least max, may include boundary extrema

  // Maximum distance should be 25 + 10 = 35
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

//==================================================================================================
// Result utility tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Result_MinMaxSquareDistance)
{
  ExtremaPS::Result aResult;
  aResult.Status = ExtremaPS::Status::OK;

  // Add some extrema
  ExtremaPS::ExtremumResult anExt1;
  anExt1.SquareDistance = 100.0;
  anExt1.IsMinimum = false;
  aResult.Extrema.Append(anExt1);

  ExtremaPS::ExtremumResult anExt2;
  anExt2.SquareDistance = 50.0;
  anExt2.IsMinimum = true;
  aResult.Extrema.Append(anExt2);

  ExtremaPS::ExtremumResult anExt3;
  anExt3.SquareDistance = 75.0;
  anExt3.IsMinimum = false;
  aResult.Extrema.Append(anExt3);

  EXPECT_NEAR(aResult.MinSquareDistance(), 50.0, THE_TOL);
  EXPECT_EQ(aResult.MinIndex(), 1);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 100.0, THE_TOL);
  EXPECT_EQ(aResult.MaxIndex(), 0);
}

TEST_F(ExtremaPS_SurfaceTest, Result_Empty)
{
  ExtremaPS::Result aResult;

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 0);
  EXPECT_EQ(aResult.MinIndex(), -1);
  EXPECT_TRUE(std::isinf(aResult.MinSquareDistance()));
}

//==================================================================================================
// Singularity tests - Sphere poles
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_PointOnNorthPole)
{
  // Sphere at origin with radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point above north pole - unique closest point at pole
  gp_Pnt aPoint(0.0, 0.0, 25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);

  // Maximum distance should be 25 + 10 = 35 (at south pole)
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Sphere_PointOnSouthPole)
{
  // Sphere at origin with radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point below south pole
  gp_Pnt aPoint(0.0, 0.0, -25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Sphere_PointNearPole)
{
  // Sphere at origin with radius 10
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point very close to axis but not exactly on it
  gp_Pnt aPoint(0.001, 0.0, 15.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Should still find a valid extremum near the north pole
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].V, M_PI / 2.0, 0.1); // Near V = π/2 (north pole)
}

//==================================================================================================
// Singularity tests - Cone apex
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Cone_PointAtApex)
{
  // Cone with apex at origin, axis along Z, semi-angle 45 degrees
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);

  // Point exactly at apex - degenerate case
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should return infinite solutions (any point at V=0 is closest)
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Cone_PointNearApex)
{
  // Cone with apex at origin, axis along Z, semi-angle 45 degrees
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);

  // Point very close to apex
  gp_Pnt aPoint(0.0, 0.0, 0.001);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone() || aResult.IsInfinite());
}

TEST_F(ExtremaPS_SurfaceTest, Cone_PointOnAxis)
{
  // Cone with apex at origin, axis along Z, semi-angle 45 degrees
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);

  // Point on axis but away from apex - infinite solutions at same V
  gp_Pnt aPoint(0.0, 0.0, 10.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should return infinite solutions (circle at that height)
  EXPECT_TRUE(aResult.IsInfinite());
}

//==================================================================================================
// Singularity tests - Cylinder seam and axis
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PointNearSeam)
{
  // Cylinder along Z axis with radius 10
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point near seam (U = 0 and U = 2*PI are the same)
  gp_Pnt aPoint(15.0, -0.1, 5.0); // Slightly off the seam

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify found a valid extremum
  int aMinIdx = aResult.MinIndex();
  double aU = aResult[aMinIdx].U;
  // U should be near 0 or near 2*PI (seam)
  bool aNearSeam = (aU < 0.1) || (aU > 2.0 * M_PI - 0.1);
  EXPECT_TRUE(aNearSeam);
}

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PointOnBoundaryV)
{
  // Cylinder along Z axis with radius 10
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point beyond the V bounds
  gp_Pnt aPoint(10.0, 0.0, -5.0); // Below V=0

  // V bounds [0, 10]
  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 10.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point should be at V=0 boundary
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].V, 0.0, THE_TOL);
}

//==================================================================================================
// Singularity tests - Torus
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Torus_PointOnMajorCircle)
{
  // Torus at origin with major radius 20, minor radius 5
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);

  // Point on major circle (Z = 0, distance from axis = 20)
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Point is on major circle, distance to torus is minor radius
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 5.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Torus_PointInsideHole)
{
  // Torus at origin with major radius 20, minor radius 5
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);

  // Point inside the torus hole (distance from axis < major_radius - minor_radius)
  gp_Pnt aPoint(10.0, 0.0, 0.0);

  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point should be on inner surface at U=0
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  // Distance to inner circle is (20 - 5) - 10 = 5
  EXPECT_NEAR(aMinDist, 5.0, THE_TOL);
}

//==================================================================================================
// Edge case tests - Small parametric ranges
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Plane_VerySmallRange)
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

TEST_F(ExtremaPS_SurfaceTest, BSpline_SmallPatch)
{
  // Create a simple 4x4 BSpline surface
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) * 1.0, (j - 1) * 1.0, 0.0);
    }
  }

  NCollection_Array1<double> aUKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;

  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineSurface> aBSpline = new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aMults, aMults, 3, 3);
  GeomAdaptor_Surface anAdaptor(aBSpline);

  ExtremaPS_Surface anExtPS(anAdaptor);
  gp_Pnt aPoint(1.5, 1.5, 1.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance consistency
  int aMinIdx = aResult.MinIndex();
  double aStoredDist = aResult[aMinIdx].SquareDistance;
  double aComputedDist = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aStoredDist, aComputedDist, THE_TOL);
}

//==================================================================================================
// Edge case tests - Point very close to surface
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Plane_PointVeryClose)
{
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

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PointOnSurface)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point exactly on cylinder surface
  gp_Pnt aPoint(10.0, 0.0, 5.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

//==================================================================================================
// Edge case tests - Large tolerance
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_LargeTolerance)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  // Large tolerance
  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, 0.1);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Should still find correct extrema
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, 0.5); // Larger tolerance for result
}

//==================================================================================================
// Robustness tests - Boundary extrema
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_ClosestToBoundaryU)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point closest to the seam at U = 0
  gp_Pnt aPoint(20.0, -0.001, 0.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Should find extremum near U = 0 or U = 2*PI
  int aMinIdx = aResult.MinIndex();
  double aU = aResult[aMinIdx].U;
  EXPECT_TRUE(aU < 0.1 || aU > 2.0 * M_PI - 0.1);
}

TEST_F(ExtremaPS_SurfaceTest, Sphere_ClosestToCorner)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point closest to north pole (V = π/2) and seam (U = 0)
  gp_Pnt aPoint(0.1, -0.001, 20.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Robustness tests - Multiple extrema
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Torus_MultipleExtrema)
{
  // Torus at origin with major radius 20, minor radius 5
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);

  // Point on axis - should have infinite solutions
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Point on axis - infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
}

TEST_F(ExtremaPS_SurfaceTest, Torus_SymmetricPoint)
{
  // Torus at origin with major radius 20, minor radius 5
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);

  // Point on major circle but elevated - 2 closest points symmetric
  gp_Pnt aPoint(30.0, 0.0, 0.0);

  ExtremaPS_Torus anEval(aTorus, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should find at least 2 extrema (min at U=0 and saddle at U=π)
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 30 - 25 = 5
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 5.0, THE_TOL);
}

//==================================================================================================
// Robustness tests - Grid-based surfaces
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, BezierSurface_SaddlePoint)
{
  // Create a saddle-shaped Bezier surface
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  aPoles(1, 1) = gp_Pnt(0, 0, 1);
  aPoles(1, 2) = gp_Pnt(0, 5, 0);
  aPoles(1, 3) = gp_Pnt(0, 10, 1);
  aPoles(2, 1) = gp_Pnt(5, 0, 0);
  aPoles(2, 2) = gp_Pnt(5, 5, -1); // Saddle center
  aPoles(2, 3) = gp_Pnt(5, 10, 0);
  aPoles(3, 1) = gp_Pnt(10, 0, 1);
  aPoles(3, 2) = gp_Pnt(10, 5, 0);
  aPoles(3, 3) = gp_Pnt(10, 10, 1);

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomAdaptor_Surface anAdaptor(aBezier);

  ExtremaPS_Surface anExtPS(anAdaptor);
  // Point above saddle center
  gp_Pnt aPoint(5.0, 5.0, 5.0);
  const ExtremaPS::Result& aResult =anExtPS.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify extremum is found near center
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, 0.5, 0.2);
  EXPECT_NEAR(aResult[aMinIdx].V, 0.5, 0.2);
}

TEST_F(ExtremaPS_SurfaceTest, BSplineSurface_MultiplePeaks)
{
  // Create a wavy BSpline surface with multiple local extrema
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      // Wavy pattern
      double aZ = std::sin((i - 1) * M_PI / 3.0) * std::sin((j - 1) * M_PI / 3.0) * 2.0;
      aPoles(i, j) = gp_Pnt((i - 1) * 3.0, (j - 1) * 3.0, aZ);
    }
  }

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineSurface> aBSpline = new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 3, 3);
  GeomAdaptor_Surface anAdaptor(aBSpline);

  ExtremaPS_Surface anExtPS(anAdaptor);
  gp_Pnt aPoint(4.5, 4.5, 10.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify found extrema are valid
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredDist = aResult[i].SquareDistance;
    double aComputedDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredDist, aComputedDist, THE_TOL);
  }
}

//==================================================================================================
// Partial sphere boundary tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_PartialDomain)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point in the first quadrant
  gp_Pnt aPoint(20.0, 20.0, 0.0);

  // Only first quadrant: U in [0, π/2], V in [0, π/2]
  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, M_PI / 2.0, 0.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at U = π/4 (45°), V = 0
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, M_PI / 4.0, 0.1);
  EXPECT_NEAR(aResult[aMinIdx].V, 0.0, 0.1);
}

TEST_F(ExtremaPS_SurfaceTest, Cylinder_PartialDomain)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point in the first quadrant
  gp_Pnt aPoint(20.0, 20.0, 5.0);

  // Only first quadrant: U in [0, π/2]
  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, M_PI / 2.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at U = π/4 (45°)
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, M_PI / 4.0, THE_TOL);
}

//==================================================================================================
// Edge case tests - Offset surfaces
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, OffsetSurface_FromPlane)
{
  // Create offset surface from a plane
  occ::handle<Geom_Plane> aBasePlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_OffsetSurface> anOffset = new Geom_OffsetSurface(aBasePlane, 5.0);
  GeomAdaptor_Surface anAdaptor(anOffset, -10.0, 10.0, -10.0, 10.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  // Point above the offset plane (which is at Z=5)
  gp_Pnt aPoint(0.0, 0.0, 15.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 15 - 5 = 10
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, THE_TOL);
}

//==================================================================================================
// Stress tests - Multiple query points (cache reuse)
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, BSplineSurface_MultipleQueries)
{
  // Create a BSpline surface
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double aZ = std::sin((i - 1) * M_PI / 4.0) * std::cos((j - 1) * M_PI / 4.0);
      aPoles(i, j) = gp_Pnt((i - 1) * 3.0, (j - 1) * 3.0, aZ);
    }
  }

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineSurface> aBSpline = new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 3, 3);
  GeomAdaptor_Surface anAdaptor(aBSpline);

  ExtremaPS_Surface anExtPS(anAdaptor);

  // Query multiple points
  std::vector<gp_Pnt> aQueryPoints = {
    gp_Pnt(0.0, 0.0, 5.0),
    gp_Pnt(4.5, 4.5, 5.0),
    gp_Pnt(9.0, 9.0, 5.0),
    gp_Pnt(4.5, 4.5, -5.0),
    gp_Pnt(2.0, 7.0, 3.0)
  };

  for (const auto& aPt : aQueryPoints)
  {
    const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPt, THE_TOL);
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_GE(aResult.NbExt(), 1);

    // Verify consistency
    int aMinIdx = aResult.MinIndex();
    double aStoredDist = aResult[aMinIdx].SquareDistance;
    double aComputedDist = aPt.SquareDistance(aResult[aMinIdx].Point);
    EXPECT_NEAR(aStoredDist, aComputedDist, THE_TOL);
  }
}

//==================================================================================================
// Consistency tests - Compare min/max modes with MinMax
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Cylinder_ModesConsistency)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 5.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));

  // Get all extrema and capture values before next call (const ref to internal member)
  const ExtremaPS::Result& aResultAll = anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::MinMax);
  ASSERT_TRUE(aResultAll.IsDone());
  // MinMax should find at least 2 extrema
  EXPECT_GE(aResultAll.NbExt(), 2);
  double aAllMinSqDist = aResultAll.MinSquareDistance();
  double aAllMaxSqDist = aResultAll.MaxSquareDistance();

  // Get only min and verify immediately
  const ExtremaPS::Result& aResultMin = anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);
  ASSERT_TRUE(aResultMin.IsDone());
  // Min mode should find 1 extremum
  EXPECT_EQ(aResultMin.NbExt(), 1);
  EXPECT_TRUE(aResultMin[0].IsMinimum);
  // Min distances should match
  EXPECT_NEAR(aAllMinSqDist, aResultMin.MinSquareDistance(), THE_TOL);

  // Get only max and verify immediately
  const ExtremaPS::Result& aResultMax = anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);
  ASSERT_TRUE(aResultMax.IsDone());
  // Max distances should match (accounting for boundary extrema)
  EXPECT_NEAR(aAllMaxSqDist, aResultMax.MaxSquareDistance(), THE_TOL);
}

//==================================================================================================
// Stress tests - Very high degree BSpline
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, BSplineSurface_HighDegree)
{
  // Create a degree 5 BSpline surface (requires 6x6 poles minimum)
  const int aDegree = 5;
  const int aNbPoles = aDegree + 1;
  NCollection_Array2<gp_Pnt> aPoles(1, aNbPoles, 1, aNbPoles);

  for (int i = 1; i <= aNbPoles; ++i)
  {
    for (int j = 1; j <= aNbPoles; ++j)
    {
      double aZ = std::sin(i * 0.5) * std::cos(j * 0.5);
      aPoles(i, j) = gp_Pnt((i - 1) * 2.0, (j - 1) * 2.0, aZ);
    }
  }

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = aNbPoles;
  aMults(2) = aNbPoles;

  occ::handle<Geom_BSplineSurface> aBSpline = new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, aDegree, aDegree);
  GeomAdaptor_Surface anAdaptor(aBSpline);

  ExtremaPS_Surface anExtPS(anAdaptor);
  gp_Pnt aPoint(5.0, 5.0, 5.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify found extrema
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredDist = aResult[i].SquareDistance;
    double aComputedDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredDist, aComputedDist, THE_TOL);
  }
}

//==================================================================================================
// Newton robustness tests - Surfaces with challenging geometry
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, BezierSurface_AlmostFlatRegion)
{
  // Create a nearly flat Bezier surface with slight bump in corner
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  aPoles(1, 1) = gp_Pnt(0, 0, 0);
  aPoles(1, 2) = gp_Pnt(0, 5, 0);
  aPoles(1, 3) = gp_Pnt(0, 10, 0);
  aPoles(2, 1) = gp_Pnt(5, 0, 0);
  aPoles(2, 2) = gp_Pnt(5, 5, 0.001); // Very slight bump
  aPoles(2, 3) = gp_Pnt(5, 10, 0);
  aPoles(3, 1) = gp_Pnt(10, 0, 0);
  aPoles(3, 2) = gp_Pnt(10, 5, 0);
  aPoles(3, 3) = gp_Pnt(10, 10, 0);

  occ::handle<Geom_BezierSurface> aBezier = new Geom_BezierSurface(aPoles);
  GeomAdaptor_Surface anAdaptor(aBezier);

  ExtremaPS_Surface anExtPS(anAdaptor);
  gp_Pnt aPoint(5.0, 5.0, 1.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be close to 1 (since surface is nearly flat at Z=0)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 1.0, 0.01);
}

TEST_F(ExtremaPS_SurfaceTest, BSplineSurface_NarrowValley)
{
  // Create a BSpline surface with a narrow valley
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      // Create valley at i=2,3 and j=2,3
      double aZ = 0.0;
      if ((i == 2 || i == 3) && (j == 2 || j == 3))
      {
        aZ = -2.0; // Valley
      }
      aPoles(i, j) = gp_Pnt((i - 1) * 3.0, (j - 1) * 3.0, aZ);
    }
  }

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineSurface> aBSpline = new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 3, 3);
  GeomAdaptor_Surface anAdaptor(aBSpline);

  ExtremaPS_Surface anExtPS(anAdaptor);
  // Point inside the valley
  gp_Pnt aPoint(4.5, 4.5, 0.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify extremum is found in the valley region
  int aMinIdx = aResult.MinIndex();
  EXPECT_GE(aResult[aMinIdx].U, 0.3);
  EXPECT_LE(aResult[aMinIdx].U, 0.7);
  EXPECT_GE(aResult[aMinIdx].V, 0.3);
  EXPECT_LE(aResult[aMinIdx].V, 0.7);
}

//==================================================================================================
// Tolerance sensitivity tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Sphere_TightTolerance)
{
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  // Very tight tolerance
  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, 1.0e-12);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Should still find correct extrema with tight tolerance
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, 1.0e-10);
}

//==================================================================================================
// Transformation tests - Rotated/translated surfaces
//==================================================================================================

TEST_F(ExtremaPS_SurfaceTest, Cylinder_Rotated)
{
  // Cylinder along X axis instead of Z
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 10.0);
  gp_Pnt aPoint(5.0, 25.0, 0.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -20.0, 20.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_SurfaceTest, Sphere_Translated)
{
  // Sphere centered at (100, 50, 25) instead of origin
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(100, 50, 25), gp_Dir(0, 0, 1)), 10.0);
  // Point outside sphere
  gp_Pnt aPoint(125.0, 50.0, 25.0);

  ExtremaPS_Sphere anEval(aSphere, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -M_PI / 2.0, M_PI / 2.0));
  const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);

  // Maximum distance should be 25 + 10 = 35
  double aMaxDist = std::sqrt(aResult.MaxSquareDistance());
  EXPECT_NEAR(aMaxDist, 35.0, THE_TOL);
}
