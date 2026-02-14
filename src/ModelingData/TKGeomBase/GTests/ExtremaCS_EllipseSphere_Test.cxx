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

#include <ExtremaCS_EllipseSphere.hxx>

#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_EllipseSphere tests.
class ExtremaCS_EllipseSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-9;
};

//==================================================================================================
// Coaxial ellipse tests (ellipse axis passes through sphere center)
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, Coaxial_EllipseInsideSphere)
{
  // Ellipse in XY plane with major=3, minor=2, sphere radius=10
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Min distance: sphere radius - major axis = 10 - 3 = 7
  // Max distance: sphere radius - minor axis = 10 - 2 = 8
  EXPECT_NEAR(aResult.MinSquareDistance(), 49.0, 0.1); // 7^2
  EXPECT_NEAR(aResult.MaxSquareDistance(), 64.0, 0.1); // 8^2
}

TEST_F(ExtremaCS_EllipseSphereTest, Coaxial_EllipseOutsideSphere)
{
  // Ellipse with major=10, minor=8, sphere radius=3
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 10.0, 8.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 2);

  // Min distance: minor axis - sphere radius = 8 - 3 = 5
  // Max distance: major axis - sphere radius = 10 - 3 = 7
  EXPECT_NEAR(aResult.MinSquareDistance(), 25.0, 0.1); // 5^2
  EXPECT_NEAR(aResult.MaxSquareDistance(), 49.0, 0.1); // 7^2
}

TEST_F(ExtremaCS_EllipseSphereTest, Coaxial_EllipseTouchesSphere)
{
  // Ellipse major axis equals sphere radius (touches on major axis)
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 5.0, 3.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Min distance = 0 (touching at major axis vertices)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Offset ellipse tests (ellipse center offset from sphere center)
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, Offset_ParallelToEquator)
{
  // Ellipse parallel to sphere equator, offset in X
  gp_Ax2    anEllipseAx(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 2.0, 1.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Min distance is the closest approach - check it's reasonable
  EXPECT_GE(aResult.MinSquareDistance(), 0.0);
}

TEST_F(ExtremaCS_EllipseSphereTest, Offset_AboveSphere)
{
  // Ellipse above sphere, not intersecting
  // Ellipse in XY plane at Z=10, center at origin, major=3, minor=2
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Ellipse points: (a*cos(t), b*sin(t), 10) = (3*cos(t), 2*sin(t), 10)
  // Distance to sphere center: sqrt(9*cos^2(t) + 4*sin^2(t) + 100)
  // Minimum at t=PI/2: sqrt(4 + 100) = sqrt(104) ~ 10.2
  // Distance to sphere surface = sqrt(104) - 5 ~ 5.2
  // Squared distance ~ 27
  double aExpectedMinDist = std::sqrt(104.0) - 5.0;
  EXPECT_NEAR(aResult.MinSquareDistance(), aExpectedMinDist * aExpectedMinDist, 0.5);
}

//==================================================================================================
// Tilted ellipse tests
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, Tilted_45Degrees)
{
  // Ellipse tilted at 45 degrees, center at sphere center
  gp_Dir    aTiltedNormal(1, 0, 1); // 45 degree tilt
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), aTiltedNormal);
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Ellipse is inside sphere, so minimum distance is positive
  EXPECT_GT(aResult.MinSquareDistance(), 0.0);
  EXPECT_LT(aResult.MinSquareDistance(), 25.0); // Less than 5^2
}

TEST_F(ExtremaCS_EllipseSphereTest, Tilted_Perpendicular)
{
  // Ellipse perpendicular to XY plane
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Elips  anEllipse(anEllipseAx, 5.0, 3.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Major axis touches sphere (equals radius)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Intersection tests
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, Tangent_EllipseJustTouchesSphere)
{
  // Ellipse that is tangent to sphere - the extremum is also an intersection
  // Ellipse in XY plane, center on X axis, sized to just touch sphere at one point
  // Ellipse center at (7, 0, 0), major=2, minor=1.5 - so closest point is at (5, 0, 0)
  // which is exactly on the sphere surface
  gp_Ax2    anEllipseAx(gp_Pnt(7, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 2.0, 1.5);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Ellipse point at t=pi: (7-2, 0, 0) = (5, 0, 0)
  // Distance to sphere center = 5, which equals sphere radius
  // So the ellipse is tangent to the sphere at (5, 0, 0)
  // Minimum distance should be close to 0 (tangent case)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, 0.01); // Allow some tolerance for numerics
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, SearchMode_MinOnly)
{
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // All results should be minima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

TEST_F(ExtremaCS_EllipseSphereTest, SearchMode_MaxOnly)
{
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // All results should be maxima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_FALSE(aResult[i].IsMinimum);
  }
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, Domain_CurveRestricted)
{
  gp_Ax2    anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Restrict to first quadrant of ellipse
  ExtremaCS::Domain3D aDomain;
  aDomain.Curve   = {0.0, M_PI / 2.0};
  aDomain.Surface = {0.0, ExtremaCS::THE_TWO_PI, -M_PI / 2.0, M_PI / 2.0};

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());

  // All results should have parameters within domain
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].ParameterOnCurve, 0.0 - THE_TOL);
    EXPECT_LE(aResult[i].ParameterOnCurve, M_PI / 2.0 + THE_TOL);
  }
}

//==================================================================================================
// Parameter verification tests
//==================================================================================================

TEST_F(ExtremaCS_EllipseSphereTest, Parameters_Verify)
{
  gp_Ax2    anEllipseAx(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips  anEllipse(anEllipseAx, 3.0, 2.0);
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_EllipseSphere  anExtrema(anEllipse, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  const ExtremaCS::ExtremumResult& anExt = aResult[0];

  // Verify point on ellipse matches parameter
  gp_Pnt aExpectedEllPt = ElCLib::Value(anExt.ParameterOnCurve, anEllipse);
  EXPECT_NEAR(anExt.PointOnCurve.Distance(aExpectedEllPt), 0.0, THE_TOL);

  // Verify point on sphere matches parameters
  gp_Pnt aExpectedSphPt =
    ElSLib::Value(anExt.ParameterOnSurfaceU, anExt.ParameterOnSurfaceV, aSphere);
  EXPECT_NEAR(anExt.PointOnSurface.Distance(aExpectedSphPt), 0.0, THE_TOL);

  // Verify distance consistency
  double aActualDist = anExt.PointOnCurve.Distance(anExt.PointOnSurface);
  EXPECT_NEAR(aActualDist * aActualDist, anExt.SquareDistance, THE_TOL);
}
