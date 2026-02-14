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

#include <ExtremaSS_CylinderCylinder.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for Cylinder-Cylinder extrema tests.
class ExtremaSS_CylinderCylinderTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderCylinderTest, ParallelCylinders_Separated)
{
  // Two parallel cylinders along Z axis
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Parallel cylinders have infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Minimum distance: 10 - 2 - 3 = 5
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderCylinderTest, CoaxialCylinders_InfiniteSolutions)
{
  // Two coaxial cylinders (same axis, different radii)
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Constant distance = |R1 - R2| = 2
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 2.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderCylinderTest, SkewCylinders_FindsMinimum)
{
  // Two skew cylinders (non-parallel, non-intersecting axes)
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0)), 1.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Axis distance is 5, radii are 1 each, so minimum surface distance = 5 - 1 - 1 = 3
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 3.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderCylinderTest, IntersectingAxes_FindsMinimum)
{
  // Two cylinders with intersecting axes
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 2.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Axes intersect at origin, surfaces intersect
  if (aResult.Status == ExtremaSS::Status::OK)
  {
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
  }
}

TEST_F(ExtremaSS_CylinderCylinderTest, TouchingCylinders_MinDistanceZero)
{
  // Two parallel cylinders that touch
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Parallel cylinders, distance = 5 - 2 - 3 = 0
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderCylinderTest, SwappedOrder_SameResult)
{
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0)), 1.0);

  ExtremaSS_CylinderCylinder anEval1(aCyl1, aCyl2);
  ExtremaSS_CylinderCylinder anEval2(aCyl2, aCyl1);

  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, aResult2.Status);

  if (aResult1.Status == ExtremaSS::Status::OK)
  {
    EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL);
  }
  else if (aResult1.Status == ExtremaSS::Status::InfiniteSolutions)
  {
    EXPECT_NEAR(aResult1.InfiniteSquareDistance, aResult2.InfiniteSquareDistance, THE_TOL);
  }
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderCylinderTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0)), 1.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
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

TEST_F(ExtremaSS_CylinderCylinderTest, AlmostParallel_SmallAngle)
{
  // Nearly parallel cylinders
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Dir      aSlightlyTilted(0.001, 0, 1);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(10, 0, 0), aSlightlyTilted), 2.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should still find a solution
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK ||
              aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_CylinderCylinderTest, DifferentRadii_SkewAxes)
{
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 1, 0)), 2.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Axis distance = 20, radii = 5 + 2 = 7, min distance = 20 - 7 = 13
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 13.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderCylinderTest, PerpendicularAxes_AtDistance)
{
  // Perpendicular cylinders with distance between axes
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(5, 0, 5), gp_Dir(1, 0, 0)), 1.0);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_CylinderCylinderTest, SmallRadius_PrecisionTest)
{
  const gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.1);
  const gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0)), 0.1);

  ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Axis distance = 5, radii = 0.2, min distance = 4.8
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 4.8, THE_TOL);
}
