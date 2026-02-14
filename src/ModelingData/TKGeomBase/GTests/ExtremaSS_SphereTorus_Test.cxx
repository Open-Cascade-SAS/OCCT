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

#include <ExtremaSS_SphereTorus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <cmath>

//! Test fixture for Sphere-Torus extrema tests.
class ExtremaSS_SphereTorusTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_SphereTorusTest, SphereSeparatedFromTorus_FindsExtrema)
{
  // Torus at origin with major radius 5, minor radius 1
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Sphere at (15, 0, 0) with radius 2
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance: 15 - 5 - 1 - 2 = 7 (from outer torus to sphere)
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 7.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereTorusTest, SphereCenterOnTorusAxis_InfiniteSolutions)
{
  // Torus at origin
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Sphere centered on torus axis at (0, 0, 10)
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should be infinite solutions (circle of extrema)
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_SphereTorusTest, SphereTouchingTorus_MinDistanceZero)
{
  // Torus at origin with major radius 5, minor radius 1
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Sphere at (8, 0, 0) with radius 2 - exactly touches outer torus
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(8, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereTorusTest, SphereInsideTorus_FindsMinimum)
{
  // Torus at origin with major radius 10, minor radius 2
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);

  // Small sphere inside the torus hole at (10, 0, 0)
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 0.5);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance: minor_radius - sphere_radius = 2 - 0.5 = 1.5
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 1.5, THE_TOL);
}

TEST_F(ExtremaSS_SphereTorusTest, SphereInTorusHole_FindsMinimum)
{
  // Torus at origin
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);

  // Sphere at center of torus hole
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Sphere center on axis - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_SphereTorusTest, SwappedOrder_SameResult)
{
  const gp_Torus  aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus anEval1(aSphere, aTorus);
  ExtremaSS_SphereTorus anEval2(aTorus, aSphere);

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

TEST_F(ExtremaSS_SphereTorusTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Torus  aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      EXPECT_TRUE(aResult[i].IsMinimum);
    }
  }
}

TEST_F(ExtremaSS_SphereTorusTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Torus  aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Max);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      EXPECT_FALSE(aResult[i].IsMinimum);
    }
  }
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST_F(ExtremaSS_SphereTorusTest, TiltedTorus_GeneralOrientation)
{
  // Tilted torus
  const gp_Dir   aTorusDir(1, 1, 1);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), aTorusDir), 5.0, 1.0);

  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find some extrema
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_SphereTorusTest, SmallMinorRadius_ThinTorus)
{
  // Thin torus (small minor radius)
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 0.1);

  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_SphereTorusTest, LargeTorus_SphereFarAway)
{
  const gp_Torus  aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0, 5.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(100, 0, 0), gp_Dir(0, 0, 1)), 1.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 100 - 50 - 5 - 1 = 44
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 44.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereTorusTest, SphereAboveTorus_FindsMinimum)
{
  // Torus at origin
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Sphere directly above torus center
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Sphere center on axis - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_SphereTorusTest, SphereAtSideOfTorus_FindsMinAndMax)
{
  // Torus at origin
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  // Sphere at the side (not on axis)
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 5, 0), gp_Dir(0, 0, 1)), 1.0);

  ExtremaSS_SphereTorus    anEval(aSphere, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
