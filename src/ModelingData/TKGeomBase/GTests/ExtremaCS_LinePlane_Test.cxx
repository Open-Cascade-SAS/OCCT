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

#include <ExtremaCS_LinePlane.hxx>

#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_LinePlane tests.
class ExtremaCS_LinePlaneTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;
};

//==================================================================================================
// Intersection tests
//==================================================================================================

TEST_F(ExtremaCS_LinePlaneTest, Intersection_AtOrigin)
{
  gp_Lin aLine(gp_Pnt(0, 0, -5), gp_Dir(0, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Line intersects plane at Z=0, so t=5
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].ParameterOnCurve, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[0].PointOnCurve.X(), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].PointOnCurve.Y(), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].PointOnCurve.Z(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_LinePlaneTest, Intersection_OffOrigin)
{
  gp_Lin aLine(gp_Pnt(5, 3, -10), gp_Dir(0, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].ParameterOnCurve, 10.0, THE_TOL);
  EXPECT_NEAR(aResult[0].PointOnCurve.X(), 5.0, THE_TOL);
  EXPECT_NEAR(aResult[0].PointOnCurve.Y(), 3.0, THE_TOL);
  EXPECT_NEAR(aResult[0].PointOnCurve.Z(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_LinePlaneTest, Intersection_Oblique)
{
  // Line at 45 degrees
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1)); // Plane at Z=5

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  // Line: (t/sqrt(2), 0, t/sqrt(2)), Z=5 when t=5*sqrt(2)
  double aExpectedT = 5.0 * std::sqrt(2.0);
  EXPECT_NEAR(aResult[0].ParameterOnCurve, aExpectedT, THE_TOL);
}

//==================================================================================================
// Parallel line tests
//==================================================================================================

TEST_F(ExtremaCS_LinePlaneTest, Parallel_AbovePlane)
{
  gp_Lin aLine(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0)); // Line at Z=5
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL); // Distance = 5
}

TEST_F(ExtremaCS_LinePlaneTest, Parallel_BelowPlane)
{
  gp_Lin aLine(gp_Pnt(0, 0, -3), gp_Dir(1, 1, 0)); // Line at Z=-3
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 9.0, THE_TOL); // Distance = 3
}

TEST_F(ExtremaCS_LinePlaneTest, Parallel_InPlane)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)); // Line in XY plane
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCS_LinePlaneTest, Domain_IntersectionInside)
{
  gp_Lin aLine(gp_Pnt(0, 0, -5), gp_Dir(0, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS::Domain3D aDomain;
  aDomain.Curve = {0.0, 10.0};
  aDomain.Surface = {-10.0, 10.0, -10.0, 10.0};

  ExtremaCS_LinePlane anExtrema(aLine, aPlane, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaCS_LinePlaneTest, Domain_IntersectionOutside)
{
  gp_Lin aLine(gp_Pnt(0, 0, -5), gp_Dir(0, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  // Domain excludes intersection point (t=5 is outside [0,3])
  ExtremaCS::Domain3D aDomain;
  aDomain.Curve = {0.0, 3.0};
  aDomain.Surface = {-10.0, 10.0, -10.0, 10.0};

  ExtremaCS_LinePlane anExtrema(aLine, aPlane, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  // No solution within domain
  EXPECT_EQ(aResult.Status, ExtremaCS::Status::NoSolution);
}

//==================================================================================================
// Surface parameter verification tests
//==================================================================================================

TEST_F(ExtremaCS_LinePlaneTest, SurfaceParameters_Verify)
{
  gp_Lin aLine(gp_Pnt(3, 4, -5), gp_Dir(0, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Intersection at (3, 4, 0) => U=3, V=4 for standard XY plane
  EXPECT_NEAR(aResult[0].ParameterOnSurfaceU, 3.0, THE_TOL);
  EXPECT_NEAR(aResult[0].ParameterOnSurfaceV, 4.0, THE_TOL);
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCS_LinePlaneTest, SearchMode_MinOnly)
{
  gp_Lin aLine(gp_Pnt(0, 0, -5), gp_Dir(0, 0, 1));
  gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS_LinePlane anExtrema(aLine, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}
