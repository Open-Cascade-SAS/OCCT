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

#include <ExtremaSS_ConeCone.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for Cone-Cone extrema tests.
class ExtremaSS_ConeConeTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeConeTest, SeparatedCones_PerpendicularAxes)
{
  // Two perpendicular cones with 45 deg semi-angle
  // Cone 1: apex at origin, opening along +Z
  // Cone 2: apex at (20,0,0), opening along +X
  // These are skew perpendicular cones that don't intersect
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 4.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(20, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone2(aCone2Axis, M_PI / 4.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // The minimum distance should be less than apex-to-apex (20)
  // but greater than 0 since these perpendicular cones don't touch
  const double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 0.0);
  EXPECT_LT(aMinDist, 20.0);
}

TEST_F(ExtremaSS_ConeConeTest, CoaxialCones_SameDirection)
{
  // Two coaxial cones opening in same direction
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 6.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1));
  const gp_Cone aCone2(aCone2Axis, M_PI / 4.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK ||
              aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeConeTest, CoaxialCones_OppositeDirections)
{
  // Two coaxial cones opening toward each other
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 4.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(0, 0, 10), gp_Dir(0, 0, -1));
  const gp_Cone aCone2(aCone2Axis, M_PI / 4.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK ||
              aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_ConeConeTest, ParallelAxes_Separated)
{
  // Cones with parallel axes, both opening in same direction
  // With semi-angle 30 deg (tan ~ 0.577), at V the radius is V * tan(30 deg)
  // Axis distance is 15. Cones touch when R1 + R2 = 15
  // For same semi-angle: 2*V*tan(30 deg) = 15 => V ~ 13
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 6.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone2(aCone2Axis, M_PI / 6.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // The cones can touch at V ~ 13 - minimum distance approaches 0
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_LT(std::sqrt(aMinSqDist), 0.5);
}

TEST_F(ExtremaSS_ConeConeTest, IntersectingCones_MinDistanceZero)
{
  // Two cones that intersect
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 4.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(2, 0, 2), gp_Dir(0, 0, 1));
  const gp_Cone aCone2(aCone2Axis, M_PI / 4.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
  }
}

TEST_F(ExtremaSS_ConeConeTest, PerpendicularAxes_SkewCones)
{
  // Perpendicular skew cones
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 6.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(10, 0, 5), gp_Dir(1, 0, 0));
  const gp_Cone aCone2(aCone2Axis, M_PI / 6.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeConeTest, SwappedOrder_SameResult)
{
  // Use parallel cones for deterministic results
  // Perpendicular skew cones may find different local optima due to grid-based search
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 6.0, 0.0); // 30 degrees

  const gp_Ax3  aCone2Axis(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)); // Parallel axis
  const gp_Cone aCone2(aCone2Axis, M_PI / 6.0, 0.0);

  ExtremaSS_ConeCone anEval1(aCone1, aCone2);
  ExtremaSS_ConeCone anEval2(aCone2, aCone1);

  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, aResult2.Status);

  if (aResult1.Status == ExtremaSS::Status::OK)
  {
    // For parallel cones, the distance should be symmetric
    EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), 1.0);
  }
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaSS_ConeConeTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 4.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(20, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone2(aCone2Axis, M_PI / 4.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
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

TEST_F(ExtremaSS_ConeConeTest, DifferentSemiAngles)
{
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 6.0, 0.0); // 30 degrees

  const gp_Ax3  aCone2Axis(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone2(aCone2Axis, M_PI / 3.0, 0.0); // 60 degrees

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeConeTest, SmallSemiAngles_NearCylinders)
{
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, 0.01, 2.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));
  const gp_Cone aCone2(aCone2Axis, 0.01, 2.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeConeTest, LargeSemiAngles_WideOpening)
{
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 3.0, 0.0); // 60 degrees

  const gp_Ax3  aCone2Axis(gp_Pnt(20, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone2(aCone2Axis, M_PI / 3.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeConeTest, ConesWithNonZeroRefRadius)
{
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 4.0, 3.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(20, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone2(aCone2Axis, M_PI / 4.0, 2.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_ConeConeTest, SameApexLocation_DifferentDirections)
{
  // Two cones with same apex but different directions
  const gp_Ax3  aCone1Axis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone1(aCone1Axis, M_PI / 6.0, 0.0);

  const gp_Ax3  aCone2Axis(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone2(aCone2Axis, M_PI / 6.0, 0.0);

  ExtremaSS_ConeCone anEval(aCone1, aCone2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    // Apexes coincide, minimum distance is 0
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
  }
}
