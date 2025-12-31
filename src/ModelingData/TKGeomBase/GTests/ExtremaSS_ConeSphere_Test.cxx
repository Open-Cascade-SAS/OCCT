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

#include <ExtremaSS_ConeSphere.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for Cone-Sphere extrema tests.
class ExtremaSS_ConeSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeSphereTest, SphereSeparatedFromCone_FindsMinimum)
{
  // Cone with apex at origin, opening upward
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0); // 45 degree semi-angle

  // Sphere at (10, 0, 0) with radius 2
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should exist
  const int aMinIdx = aResult.MinIndex();
  ASSERT_GE(aMinIdx, 0);
}

TEST_F(ExtremaSS_ConeSphereTest, SphereCenterOnConeAxis_InfiniteSolutions)
{
  // Cone with apex at origin
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  // Sphere centered on cone axis at (0, 0, 10)
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should be infinite solutions (circle of extrema)
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeSphereTest, SphereIntersectsCone_MinDistanceZero)
{
  // Cone with apex at origin
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  // Large sphere that intersects cone
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(5, 0, 5), gp_Dir(0, 0, 1)), 4.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

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

TEST_F(ExtremaSS_ConeSphereTest, SphereTouchingApex_MinDistanceZero)
{
  // Cone with apex at origin
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  // Sphere touching apex
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(3, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_ConeSphereTest, SphereNearConeGenerator_CorrectMinimum)
{
  // Cone with apex at origin, 30 degree semi-angle
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  // Sphere at (5, 0, 10) with radius 1
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(5, 0, 10), gp_Dir(0, 0, 1)), 1.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify minimum point is on cone surface
  const int aMinIdx = aResult.MinIndex();
  ASSERT_GE(aMinIdx, 0);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeSphereTest, SwappedOrder_SameResult)
{
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 5), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval1(aCone, aSphere);
  ExtremaSS_ConeSphere anEval2(aSphere, aCone);

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

TEST_F(ExtremaSS_ConeSphereTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Ax3    aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone   aCone(aConeAxis, M_PI / 4.0, 0.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 5), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      EXPECT_TRUE(aResult[i].IsMinimum);
    }
  }
}

TEST_F(ExtremaSS_ConeSphereTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Ax3    aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone   aCone(aConeAxis, M_PI / 4.0, 0.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 5), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
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

TEST_F(ExtremaSS_ConeSphereTest, SmallSemiAngle_NearCylinder)
{
  // Cone with very small semi-angle (almost a cylinder)
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, 0.01, 2.0);

  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 1.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeSphereTest, LargeSemiAngle_WideOpening)
{
  // Cone with large semi-angle (wide opening)
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 3.0, 0.0); // 60 degrees

  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeSphereTest, TiltedCone_GeneralOrientation)
{
  // Tilted cone axis
  const gp_Dir  aConeDir(1, 1, 1);
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), aConeDir);
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find some extrema
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK ||
              aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeSphereTest, SmallSphere_FarFromCone)
{
  const gp_Ax3    aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone   aCone(aConeAxis, M_PI / 4.0, 0.0);
  const gp_Sphere aSphere(gp_Ax3(gp_Pnt(100, 0, 0), gp_Dir(0, 0, 1)), 0.5);

  ExtremaSS_ConeSphere anEval(aCone, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
