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

#include <ExtremaSS_TorusTorus.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>

#include <cmath>

//! Test fixture for Torus-Torus extrema tests.
class ExtremaSS_TorusTorusTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_TorusTorusTest, SeparatedTori_FindsMinimum)
{
  // Two tori separated along X axis
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 20 - (5+1) - (5+1) = 20 - 12 = 8
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 8.0, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, ConcentricTori_InfiniteSolutions)
{
  // Two concentric tori (same center and axis)
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Concentric tori have infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_TorusTorusTest, CoaxialTori_DifferentCenters)
{
  // Two coaxial tori at different heights
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Coaxial tori have infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Distance between tori: 10 - 1 - 1 = 8
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 8.0, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, PerpendicularAxes_FindsMinimum)
{
  // Two tori with perpendicular axes
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0)), 3.0, 0.5);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_TorusTorusTest, TouchingTori_MinDistanceZero)
{
  // Two tori that touch
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(12, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Distance: 12 - 6 - 6 = 0
  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, IntersectingTori_MinDistanceZero)
{
  // Two tori that intersect
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 2.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(8, 0, 0), gp_Dir(0, 0, 1)), 5.0, 2.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Distance: 8 - 7 - 7 = -6 (they intersect)
  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, NestedTori_OneInsideOther)
{
  // One torus completely inside another
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 5.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Concentric - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_TorusTorusTest, SwappedOrder_SameResult)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus anEval1(aTorus1, aTorus2);
  ExtremaSS_TorusTorus anEval2(aTorus2, aTorus1);

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

TEST_F(ExtremaSS_TorusTorusTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      EXPECT_TRUE(aResult[i].IsMinimum);
    }
  }
}

TEST_F(ExtremaSS_TorusTorusTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
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

TEST_F(ExtremaSS_TorusTorusTest, ParallelAxes_NotCoaxial)
{
  // Tori with parallel but offset axes
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(15, 5, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_TorusTorusTest, TiltedAxes_GeneralOrientation)
{
  const gp_Dir   aDir1(1, 1, 1);
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), aDir1), 5.0, 1.0);

  const gp_Dir   aDir2(1, 0, 1);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(20, 0, 0), aDir2), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_TorusTorusTest, SmallMinorRadii_ThinTori)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 0.1);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 10.0, 0.1);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 25 - 10.1 - 10.1 = 4.8
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 4.8, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, DifferentSizes_LargeAndSmall)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0, 10.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(100, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 100 - 60 - 6 = 34
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 34.0, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, CloseButNotTouching)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(12.5, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: 12.5 - 6 - 6 = 0.5
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 0.5, THE_TOL);
}

TEST_F(ExtremaSS_TorusTorusTest, AxesAtAngle_45Degrees)
{
  const gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 1.0);

  const gp_Dir   aDir2(1, 0, 1); // 45 degrees from Z
  const gp_Torus aTorus2(gp_Ax3(gp_Pnt(15, 0, 0), aDir2), 5.0, 1.0);

  ExtremaSS_TorusTorus     anEval(aTorus1, aTorus2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
