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

#include <ExtremaCS_CircleSphere.hxx>

#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_CircleSphere tests.
class ExtremaCS_CircleSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-9;
};

//==================================================================================================
// Concentric tests (circle center on sphere axis)
//==================================================================================================

TEST_F(ExtremaCS_CircleSphereTest, Concentric_CircleInsideSphere)
{
  // Circle in XY plane at Z=0, radius 3, sphere at origin radius 5
  gp_Ax2    aCircleAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 3.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  // Circle center on sphere axis - infinite solutions
  ASSERT_TRUE(aResult.IsInfinite());
  // Distance from any circle point to sphere surface = 5 - 3 = 2
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 4.0, THE_TOL);
}

TEST_F(ExtremaCS_CircleSphereTest, Concentric_CircleOnSphere)
{
  // Circle in XY plane at Z=0, radius 5, sphere at origin radius 5
  gp_Ax2    aCircleAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 5.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  // Circle lies exactly on sphere equator
  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaCS_CircleSphereTest, Concentric_CircleOutsideSphere)
{
  // Circle in XY plane at Z=0, radius 8, sphere at origin radius 5
  gp_Ax2    aCircleAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 8.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  // Circle center on sphere axis - infinite solutions
  ASSERT_TRUE(aResult.IsInfinite());
  // Distance = 8 - 5 = 3
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 9.0, THE_TOL);
}

//==================================================================================================
// Circle offset from sphere center tests
//==================================================================================================

TEST_F(ExtremaCS_CircleSphereTest, Offset_CircleInXYPlane)
{
  // Circle in XY plane, center offset from sphere
  gp_Ax2    aCircleAx(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point on circle: (10-2, 0, 0) = (8, 0, 0)
  // Distance to sphere surface = 8 - 5 = 3
  EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);

  // Farthest point on circle: (10+2, 0, 0) = (12, 0, 0)
  // Distance to sphere surface = 12 - 5 = 7
  if (aResult.NbExt() >= 2)
  {
    EXPECT_NEAR(aResult.MaxSquareDistance(), 49.0, THE_TOL);
  }
}

TEST_F(ExtremaCS_CircleSphereTest, Offset_CircleIntersectsSphere)
{
  // Circle overlaps with sphere
  gp_Ax2    aCircleAx(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 3.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point: (5-3, 0, 0) = (2, 0, 0), distance to sphere = |2 - 5| = 3
  // Farthest point: (5+3, 0, 0) = (8, 0, 0), distance to sphere = 8 - 5 = 3
  // Both at same distance!
  EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);
}

TEST_F(ExtremaCS_CircleSphereTest, Offset_CircleInsideSphere)
{
  // Small circle entirely inside sphere
  gp_Ax2    aCircleAx(gp_Pnt(2, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 1.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point: (2+1, 0, 0) = (3, 0, 0), distance to sphere = 5 - 3 = 2
  EXPECT_NEAR(aResult.MinSquareDistance(), 4.0, THE_TOL);

  // Farthest point: (2-1, 0, 0) = (1, 0, 0), distance to sphere = 5 - 1 = 4
  if (aResult.NbExt() >= 2)
  {
    EXPECT_NEAR(aResult.MaxSquareDistance(), 16.0, THE_TOL);
  }
}

//==================================================================================================
// Tilted circle tests
//==================================================================================================

TEST_F(ExtremaCS_CircleSphereTest, Tilted_CircleAboveSphere)
{
  // Circle tilted and above sphere
  gp_Dir    aTiltedNormal(0, 1, 1); // 45 degree tilt
  gp_Ax2    aCircleAx(gp_Pnt(0, 0, 10), aTiltedNormal);
  gp_Circ   aCircle(aCircleAx, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Circle above sphere - minimum distance should be positive
  // (circle doesn't intersect sphere)
  EXPECT_GT(aResult.MinSquareDistance(), 0.0);
  // The minimum should be less than the distance from circle center (10) minus radius (5)
  EXPECT_LT(aResult.MinSquareDistance(), 25.0); // < (10-5)^2
}

//==================================================================================================
// Circle touching sphere tests
//==================================================================================================

TEST_F(ExtremaCS_CircleSphereTest, Touching_CircleTangentToSphere)
{
  // Circle positioned so it just touches the sphere
  gp_Ax2    aCircleAx(gp_Pnt(7, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point: (7-2, 0, 0) = (5, 0, 0), on sphere surface
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCS_CircleSphereTest, SearchMode_MinOnly)
{
  gp_Ax2    aCircleAx(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaCS_CircleSphereTest, SearchMode_MaxOnly)
{
  gp_Ax2    aCircleAx(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_FALSE(aResult[0].IsMinimum);
}

//==================================================================================================
// Parameter verification tests
//==================================================================================================

TEST_F(ExtremaCS_CircleSphereTest, Parameters_Verify)
{
  gp_Ax2    aCircleAx(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ   aCircle(aCircleAx, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_CircleSphere   anExtrema(aCircle, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify that points match parameters
  const ExtremaCS::ExtremumResult& anExt = aResult[0];

  // Point on circle should be at parameter t
  gp_Pnt aExpectedCircPt = ElCLib::Value(anExt.ParameterOnCurve, aCircle);
  EXPECT_NEAR(anExt.PointOnCurve.Distance(aExpectedCircPt), 0.0, THE_TOL);

  // Point on sphere should be at parameters (u, v)
  gp_Pnt aExpectedSphPt =
    ElSLib::Value(anExt.ParameterOnSurfaceU, anExt.ParameterOnSurfaceV, aSphere);
  EXPECT_NEAR(anExt.PointOnSurface.Distance(aExpectedSphPt), 0.0, THE_TOL);
}
