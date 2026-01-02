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

#include <ExtremaSS_PlaneSphere.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for Plane-Sphere extrema tests.
class ExtremaSS_PlaneSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneSphereTest, SphereSeparatedFromPlane_FindsMinAndMax)
{
  // Plane at Z=0, sphere at Z=10 with radius 3
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Minimum distance: 10 - 3 = 7
  const double aExpectedMinDist = 7.0;
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedMinDist, THE_TOL);

  // Maximum distance: 10 + 3 = 13
  const double aExpectedMaxDist = 13.0;
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), aExpectedMaxDist, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SphereTouchingPlane_MinDistanceZero)
{
  // Plane at Z=0, sphere touching plane at Z=3 with radius 3
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 3), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Distance: 3 - 3 = 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);

  // Maximum distance: 3 + 3 = 6
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), 6.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SphereIntersectingPlane_MinDistanceZero)
{
  // Plane at Z=0, sphere with center at Z=2 and radius 5 (intersecting)
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 2), gp_Dir(0, 0, 1)), 5.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Sphere intersects plane, minimum distance = 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);

  // Maximum distance: 2 + 5 = 7
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), 7.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SphereBelowPlane_FindsMinAndMax)
{
  // Plane at Z=0, sphere below plane at Z=-10
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, -10), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Minimum distance: |-10| - 3 = 7
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 7.0, THE_TOL);

  // Maximum distance: 10 + 3 = 13
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), 13.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SwappedOrder_SameResult)
{
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_PlaneSphere anEval1(aPlane, aSphere);
  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);

  ExtremaSS_PlaneSphere anEval2(aSphere, aPlane);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult2.Status, ExtremaSS::Status::OK);

  EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL * THE_TOL);
  EXPECT_NEAR(aResult1.MaxSquareDistance(), aResult2.MaxSquareDistance(), THE_TOL * THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 7.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_FALSE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 13.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, TiltedPlane_SphereAbove)
{
  // Tilted plane and sphere above it
  const gp_Dir    aTiltedNormal(0, 1, 1);
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), aTiltedNormal);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 10, 10), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance from center (0,10,10) to plane with normal (0,1,1)/sqrt(2):
  // d = (10*1 + 10*1) / sqrt(2) = 20/sqrt(2) = 10*sqrt(2)
  const double aCenterDist = 10.0 * std::sqrt(2.0);
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aCenterDist - 2.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SmallSphere_FarFromPlane)
{
  // Small sphere far from plane
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 100), gp_Dir(0, 0, 1)), 0.5);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Minimum distance: 100 - 0.5 = 99.5
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 99.5, THE_TOL);

  // Maximum distance: 100 + 0.5 = 100.5
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), 100.5, THE_TOL);
}

TEST_F(ExtremaSS_PlaneSphereTest, SphereOnDiagonal_CorrectExtrema)
{
  // Sphere at diagonal position from plane origin
  const gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 10, 5), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Distance to plane is just Z-coordinate = 5
  // Minimum: 5 - 2 = 3
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 3.0, THE_TOL);

  // Maximum: 5 + 2 = 7
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), 7.0, THE_TOL);
}
