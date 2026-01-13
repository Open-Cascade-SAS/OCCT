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

#include <ExtremaSS_CylinderTorus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>

#include <cmath>

//! Test fixture for Cylinder-Torus extrema tests.
class ExtremaSS_CylinderTorusTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderTorusTest, SeparatedCylinderAndTorus_FindsMinimum)
{
  // Cylinder along Z axis
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  // Torus at (20, 0, 0)
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 20 - 2 - 5 - 1 = 12
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 12.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderTorusTest, CoaxialCylinderAndTorus_InfiniteSolutions)
{
  // Cylinder and torus with same axis
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 1.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Coaxial - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Distance: 10 - 1 - 2 = 7 (inner torus to cylinder)
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 7.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderTorusTest, PerpendicularAxes_FindsMinimum)
{
  // Cylinder along Z
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);

  // Torus with axis along X
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(1, 0, 0)), 3.0, 0.5);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_CylinderTorusTest, CylinderThroughTorusHole_FindsMinimum)
{
  // Thin cylinder that could go through torus hole
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.5);

  // Torus with large major radius
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 5.0, 1.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_CylinderTorusTest, TouchingCylinderAndTorus_MinDistanceZero)
{
  // Cylinder and torus that touch
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 4.0, 2.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Coaxial, inner torus surface at 4-2=2 from axis, cylinder at 2
  // They should touch
  if (aResult.Status == ExtremaSS::Status::InfiniteSolutions)
  {
    EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
  }
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderTorusTest, SwappedOrder_SameResult)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_CylinderTorus anEval1(aCyl, aTorus);
  ExtremaSS_CylinderTorus anEval2(aTorus, aCyl);

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

TEST_F(ExtremaSS_CylinderTorusTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
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

TEST_F(ExtremaSS_CylinderTorusTest, TiltedTorus_GeneralOrientation)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  // Tilted torus
  const gp_Dir   aTorusDir(1, 1, 1);
  const gp_Torus aTorus(gp_Ax3(gp_Pnt(15, 0, 0), aTorusDir), 5.0, 1.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_CylinderTorusTest, SmallMinorRadius_ThinTorus)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 5.0, 0.1);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 15 - 2 - 5 - 0.1 = 7.9
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 7.9, THE_TOL);
}

TEST_F(ExtremaSS_CylinderTorusTest, LargeCylinder_FarFromTorus)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(50, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 50 - 10 - 5 - 1 = 34
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 34.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderTorusTest, ParallelButNotCoaxial_Offset)
{
  // Cylinder and torus with parallel but offset axes
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  const gp_Torus    aTorus(gp_Ax3(gp_Pnt(10, 5, 0), gp_Dir(0, 0, 1)), 3.0, 0.5);

  ExtremaSS_CylinderTorus  anEval(aCyl, aTorus);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
