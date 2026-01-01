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

#include <ExtremaCS_EllipsePlane.hxx>

#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_EllipsePlane tests.
class ExtremaCS_EllipsePlaneTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-9;
};

//==================================================================================================
// Parallel ellipse tests
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, Parallel_AbovePlane)
{
  // Ellipse in plane Z=5, parallel to XY plane
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL); // Distance = 5
}

TEST_F(ExtremaCS_EllipsePlaneTest, Parallel_InPlane)
{
  // Ellipse in XY plane
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Tilted ellipse tests (ellipse intersects plane)
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, Tilted_CenterOnPlane)
{
  // Ellipse tilted at 45 degrees, center on plane
  gp_Dir aTiltedNormal(1, 0, 1); // gp_Dir auto-normalizes
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 0), aTiltedNormal);
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at the intersection (distance 0)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_EllipsePlaneTest, Tilted_CenterAbovePlane)
{
  // Ellipse tilted, center above plane
  gp_Dir aTiltedNormal(0, 1, 1); // gp_Dir auto-normalizes (45 degree tilt)
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 5), aTiltedNormal);
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // For tilted ellipse above plane:
  // - Minimum distance should be positive (ellipse above plane)
  // - Minimum should be less than center height (5)
  EXPECT_GT(aResult.MinSquareDistance(), 0.0);
  EXPECT_LT(aResult.MinSquareDistance(), 25.0);  // < 5^2

  if (aResult.NbExt() >= 2)
  {
    // Maximum should be greater than center height
    EXPECT_GT(aResult.MaxSquareDistance(), 25.0);  // > 5^2
  }
}

TEST_F(ExtremaCS_EllipsePlaneTest, Tilted_MajorAxisIntersects)
{
  // Ellipse tilted so major axis intersects plane
  gp_Dir aTiltedNormal(1, 0, 1); // 45 degree tilt around Y axis
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 2), aTiltedNormal);
  gp_Elips anEllipse(anEllipseAx, 4.0, 1.0); // major = 4, minor = 1
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Major axis tilted at 45 degrees, amplitude = 4*sin(45) = 4/sqrt(2) = 2*sqrt(2) ~ 2.83
  // Center at Z=2, so ellipse intersects plane (amplitude > 2)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Perpendicular ellipse tests
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, Perpendicular_EllipseEdgeOnPlane)
{
  // Ellipse perpendicular to XY plane, edge touches plane
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 4), gp_Dir(1, 0, 0)); // Ellipse in YZ plane
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be 0 (ellipse touches plane at one point)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);

  // Maximum at top of ellipse: Z = 4 + 4 = 8
  if (aResult.NbExt() >= 2)
  {
    EXPECT_NEAR(aResult.MaxSquareDistance(), 64.0, THE_TOL);
  }
}

TEST_F(ExtremaCS_EllipsePlaneTest, Perpendicular_EllipseAbovePlane)
{
  // Ellipse perpendicular to XY plane, entirely above
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 10), gp_Dir(1, 0, 0)); // Ellipse in YZ plane
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum at bottom of ellipse: Z = 10 - 4 = 6
  EXPECT_NEAR(aResult.MinSquareDistance(), 36.0, THE_TOL);

  // Maximum at top of ellipse: Z = 10 + 4 = 14
  if (aResult.NbExt() >= 2)
  {
    EXPECT_NEAR(aResult.MaxSquareDistance(), 196.0, THE_TOL);
  }
}

//==================================================================================================
// Different ellipse orientations
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, Orientation_MinorAxisTowardPlane)
{
  // Ellipse tilted so minor axis points toward plane
  gp_Dir aTiltedNormal(0, 1, 1); // 45 degree tilt
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 10), aTiltedNormal);
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0); // major = 4, minor = 2
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Tilted ellipse above plane - minimum should be positive and less than center height
  EXPECT_GT(aResult.MinSquareDistance(), 0.0);
  EXPECT_LT(aResult.MinSquareDistance(), 100.0);  // < 10^2
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, SearchMode_MinOnly)
{
  gp_Dir aTiltedNormal(1, 0, 1);
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 5), aTiltedNormal);
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaCS_EllipsePlaneTest, SearchMode_MaxOnly)
{
  gp_Dir aTiltedNormal(1, 0, 1);
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 5), aTiltedNormal);
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_FALSE(aResult[0].IsMinimum);
}

//==================================================================================================
// Parameter verification tests
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, Parameters_Verify)
{
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 10), gp_Dir(1, 0, 0));
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  const ExtremaCS::ExtremumResult& anExt = aResult[0];

  // Verify point on ellipse matches parameter
  gp_Pnt aExpectedEllPt = ElCLib::Value(anExt.ParameterOnCurve, anEllipse);
  EXPECT_NEAR(anExt.PointOnCurve.Distance(aExpectedEllPt), 0.0, THE_TOL);

  // Verify point on plane matches parameters
  gp_Pnt aExpectedPlanePt = ElSLib::Value(anExt.ParameterOnSurfaceU,
                                           anExt.ParameterOnSurfaceV, aPlane);
  EXPECT_NEAR(anExt.PointOnSurface.Distance(aExpectedPlanePt), 0.0, THE_TOL);
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCS_EllipsePlaneTest, Domain_CurveRestricted)
{
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 10), gp_Dir(1, 0, 0));
  gp_Elips anEllipse(anEllipseAx, 4.0, 2.0);
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Restrict curve domain to upper half of ellipse
  ExtremaCS::Domain3D aDomain;
  aDomain.Curve = {0.0, M_PI};
  aDomain.Surface = {-100.0, 100.0, -100.0, 100.0};

  ExtremaCS_EllipsePlane anExtrema(anEllipse, aPlane, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());

  // All results should have parameters within domain
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].ParameterOnCurve, 0.0 - THE_TOL);
    EXPECT_LE(aResult[i].ParameterOnCurve, M_PI + THE_TOL);
  }
}
