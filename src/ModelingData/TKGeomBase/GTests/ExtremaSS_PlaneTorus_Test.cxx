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

#include <ExtremaSS_PlaneTorus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>

#include <cmath>

//! Test fixture for Plane-Torus extrema tests.
class ExtremaSS_PlaneTorusTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneTorusTest, PlaneParallelToTorusAxis_FindsExtrema)
{
  // Torus at origin with axis along Z
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Horizontal plane at Z=10 (plane perpendicular to torus axis)
  const gp_Pln aPlane(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Plane perpendicular to torus axis: infinite solutions (entire circles at min/max distance)
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Minimum distance: 10 - 1 = 9 (from top of torus to plane)
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 9.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneTorusTest, PlanePerpendicularToTorusAxis_SeparatedFromTorus)
{
  // Torus at origin with axis along Z
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Vertical plane at X=10 (perpendicular to X axis)
  const gp_Pln aPlane(gp_Pnt(10, 0, 0), gp_Dir(1, 0, 0));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance: 10 - 5 - 1 = 4 (from outer torus to plane)
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 4.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneTorusTest, PlaneTouchingTorus_MinDistanceZero)
{
  // Torus at origin
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Plane at Z=1 (touches top of torus) - plane perpendicular to torus axis
  const gp_Pln aPlane(gp_Pnt(0, 0, 1), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Plane touches torus at an entire circle - infinite solutions at distance 0
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneTorusTest, PlaneIntersectingTorus_MinDistanceZero)
{
  // Torus at origin
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Plane at Z=0 (cuts through torus)
  const gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find intersection
  if (aResult.Status == ExtremaSS::Status::OK)
  {
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
  }
  else if (aResult.Status == ExtremaSS::Status::InfiniteSolutions)
  {
    EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
  }
}

TEST_F(ExtremaSS_PlaneTorusTest, PlaneBelowTorus_FindsMinimum)
{
  // Torus at height Z=5
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Plane at Z=0 (perpendicular to torus axis)
  const gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Plane perpendicular to torus axis - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Minimum distance: 5 - 1 = 4
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 4.0, THE_TOL);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneTorusTest, SwappedOrder_SameResult)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Pln   aPlane(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus anEval1(aPlane, aTorus);
  ExtremaSS_PlaneTorus anEval2(aTorus, aPlane);

  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, aResult2.Status);

  if (aResult1.Status == ExtremaSS::Status::OK)
  {
    EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL);
  }
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneTorusTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Pln   aPlane(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      EXPECT_TRUE(aResult[i].IsMinimum);
    }
  }
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST_F(ExtremaSS_PlaneTorusTest, TiltedPlane_GeneralOrientation)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Tilted plane
  const gp_Dir aPlaneDir(1, 1, 1);
  const gp_Pln aPlane(gp_Pnt(10, 10, 10), aPlaneDir);

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find some extrema
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_PlaneTorusTest, TiltedTorus_GeneralOrientation)
{
  // Tilted torus
  const gp_Dir   aTorusDir(1, 1, 1);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), aTorusDir), 5.0, 1.0);

  const gp_Pln aPlane(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_PlaneTorusTest, SmallMinorRadius_ThinTorus)
{
  // Thin torus
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 0.1);

  const gp_Pln aPlane(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Plane perpendicular to torus axis - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Minimum: 5 - 0.1 = 4.9
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 4.9, THE_TOL);
}

TEST_F(ExtremaSS_PlaneTorusTest, LargeTorus_FarFromPlane)
{
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0, 5.0);
  const gp_Pln   aPlane(gp_Pnt(0, 0, 100), gp_Dir(0, 0, 1));

  ExtremaSS_PlaneTorus     anEval(aPlane, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Plane perpendicular to torus axis - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Minimum: 100 - 5 = 95
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 95.0, THE_TOL);
}
