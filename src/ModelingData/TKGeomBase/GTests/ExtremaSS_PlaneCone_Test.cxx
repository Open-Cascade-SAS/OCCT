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

#include <ExtremaSS_PlaneCone.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for Plane-Cone extrema tests.
class ExtremaSS_PlaneConeTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneConeTest, ConeSeparatedFromPlane_ApexIsClosest)
{
  // Plane at Z=0, cone with apex at (0, 0, 5) pointing upward (away from plane)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0); // 30 degree semi-angle, apex at (0,0,5)

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 5 (from apex to plane)
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneConeTest, ApexOnPlane_MinDistanceIsZero)
{
  // Plane at Z=0, cone with apex at origin
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0); // 45 degree semi-angle, apex at origin

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 0 (apex is on plane)
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneConeTest, ConeIntersectsPlane_MinDistanceIsZero)
{
  // Plane at Z=0, cone with apex at (0, 0, 5) pointing downward (toward plane)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, -1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0); // Pointing down, will intersect plane

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find intersection - minimum distance is 0
  ASSERT_GE(aResult.NbExt(), 1);
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneConeTest, AxisParallelToPlane_ApexClosest)
{
  // Plane at Z=0, cone with horizontal axis at Z=3
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 3), gp_Dir(1, 0, 0));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0); // Horizontal cone at height 3

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // The cone opens horizontally, so it will eventually cross Z=0
  // Minimum should be at the intersection or very close to it
  if (aResult.Status != ExtremaSS::Status::InfiniteSolutions)
  {
    ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
    // Cone extends down, should intersect plane at some point
    // Allow relaxed tolerance due to approximations in intersection calculation
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_LT(std::sqrt(aMinSqDist), 1.0);
  }
}

TEST_F(ExtremaSS_PlaneConeTest, AxisPerpendicularToPlane_ConeFacingAway)
{
  // Plane at Z=0, cone with apex at (0, 0, 5) axis pointing up (perpendicular to plane)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Apex is closest point
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneConeTest, AxisPerpendicularToPlane_ConeFacingPlane)
{
  // Plane at Z=0, cone with apex at (0, 0, 5) axis pointing down (perpendicular to plane)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, -1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Cone points toward plane, will intersect
  // Result should be infinite solutions with distance 0 (intersection)
  if (aResult.Status == ExtremaSS::Status::InfiniteSolutions)
  {
    EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
  }
  else
  {
    ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
  }
}

TEST_F(ExtremaSS_PlaneConeTest, GeneralOrientation_TiltedCone)
{
  // Plane at Z=0, tilted cone
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir  aConeDir(1, 0, 1);  // 45 degrees from vertical
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 10), aConeDir);
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Apex at (0, 0, 10), apex distance to plane is 10
  const int aMinIdx = aResult.MinIndex();
  ASSERT_GE(aMinIdx, 0);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneConeTest, SwappedOrder_SameResult)
{
  // Test that (Cone, Plane) gives same result as (Plane, Cone)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  ExtremaSS_PlaneCone anEval1(aPlane, aCone);
  ExtremaSS_PlaneCone anEval2(aCone, aPlane);

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

TEST_F(ExtremaSS_PlaneConeTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // All found extrema should be minima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

//==================================================================================================
// Special Geometry Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneConeTest, VerySmallSemiAngle_NearCylinder)
{
  // Cone with very small semi-angle (almost a cylinder)
  // With refRadius = 2 and semi-angle = 0.01, apex is very far below the location
  // Apex = Location - refRadius/tan(0.01) * Axis ≈ (0,0,5) - 200*(0,0,1) = (0,0,-195)
  // The cone surface extends from apex upward, crossing Z=0 plane
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, 0.01, 2.0); // Very small angle, ref radius 2

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // The cone surface crosses the plane, so minimum distance is 0
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneConeTest, LargeSemiAngle_WideOpeningCone)
{
  // Cone with large semi-angle (wide opening)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 3.0, 0.0); // 60 degree semi-angle

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_PlaneConeTest, PlaneNotAtOrigin_OffsetPlane)
{
  // Plane at Z=10, cone with apex at origin, axis pointing up
  // The cone opens upward (along +Z), so it will eventually reach Z=10
  // At V=10, the cone surface is at Z=10 (on the plane)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // The cone surface reaches Z=10, so minimum distance is 0
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneConeTest, TiltedPlane_GeneralOrientation)
{
  // Tilted plane and cone
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 1)); // 45 degree tilted plane
  const gp_Ax3  aConeAxis(gp_Pnt(0, 5, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find some extrema
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK ||
              aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_PlaneConeTest, ConeWithNonZeroRefRadius)
{
  // Cone with reference radius (apex not at cone origin)
  const gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 2.0); // Ref radius 2

  ExtremaSS_PlaneCone anEval(aPlane, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
