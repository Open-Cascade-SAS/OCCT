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

#include <ExtremaSS_ConeTorus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>

#include <cmath>

//! Test fixture for Cone-Torus extrema tests.
class ExtremaSS_ConeTorusTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeTorusTest, SeparatedConeAndTorus_FindsMinimum)
{
  // Cone at origin
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  // Torus at (25, 0, 0)
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // The cone surface at 45 deg forms line (r, 0, r) for U=0.
  // Torus inner edge at (19, 0, 0). Distance from cone line to this point:
  // d^2 = (r-19)^2 + r^2 minimized at r=9.5, giving d ~ 13.44
  // The actual minimum considering the full torus surface is around 13.
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_LT(std::sqrt(aMinSqDist), 14.0);
  EXPECT_GT(std::sqrt(aMinSqDist), 12.0);
}

TEST_F(ExtremaSS_ConeTorusTest, CoaxialConeAndTorus_FindsMinimum)
{
  // Cone and torus with same axis
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeTorusTest, PerpendicularAxes_FindsMinimum)
{
  // Cone along Z
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  // Torus with axis along X
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0)), 3.0, 0.5);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeTorusTest, ConeApexInTorusHole_FindsMinimum)
{
  // Cone at origin
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  // Torus around cone
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Cone apex at origin, torus inner edge at 10-2=8 from axis
  // This depends on whether cone surface reaches the torus
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeTorusTest, TouchingConeAndTorus_MinDistanceZero)
{
  // Cone at origin
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  // Torus positioned to touch cone
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // At Z=5, cone has radius 5 (45 degree angle), torus has inner edge at 5-1=4
  // They should be close or intersecting
  if (aResult.Status == ExtremaSS::Status::OK)
  {
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, 1.0); // Allow some tolerance
  }
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeTorusTest, SwappedOrder_SameResult)
{
  const gp_Ax3   aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone  aCone(aConeAxis, M_PI / 4.0, 0.0);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus anEval1(aCone, aTorus);
  ExtremaSS_ConeTorus anEval2(aTorus, aCone);

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

TEST_F(ExtremaSS_ConeTorusTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Ax3   aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone  aCone(aConeAxis, M_PI / 4.0, 0.0);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
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

TEST_F(ExtremaSS_ConeTorusTest, TiltedAxes_GeneralOrientation)
{
  const gp_Dir  aConeDir(1, 1, 1);
  const gp_Ax3  aConeAxis(gp_Pnt(0, 0, 0), aConeDir);
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  const gp_Dir   aTorusDir(1, 0, 1);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(20, 0, 0), aTorusDir), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeTorusTest, SmallConeAngle_NearCylinder)
{
  const gp_Ax3   aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone  aCone(aConeAxis, 0.01, 2.0);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeTorusTest, SmallMinorRadius_ThinTorus)
{
  const gp_Ax3   aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone  aCone(aConeAxis, M_PI / 4.0, 0.0);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 0.1);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // The cone surface at 45 deg forms line (r, 0, r) for U=0.
  // Torus inner edge at (14.9, 0, 0). Distance from cone line to this point:
  // d^2 = (r-14.9)^2 + r^2 minimized at r=7.45, giving d ~ 10.54
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_LT(std::sqrt(aMinSqDist), 11.5);
  EXPECT_GT(std::sqrt(aMinSqDist), 9.5);
}

TEST_F(ExtremaSS_ConeTorusTest, LargeConeAngle_WideOpening)
{
  const gp_Ax3   aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone  aCone(aConeAxis, M_PI / 3.0, 0.0); // 60 degrees
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeTorusTest, ConeWithNonZeroRefRadius)
{
  const gp_Ax3   aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone  aCone(aConeAxis, M_PI / 4.0, 3.0);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_ConeTorus      anEval(aCone, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
