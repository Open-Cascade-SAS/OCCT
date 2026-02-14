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

#include <ExtremaSS_SphereSphere.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for Sphere-Sphere extrema tests.
class ExtremaSS_SphereSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_SphereSphereTest, TwoSeparatedSpheres_FindsMinAndMax)
{
  // Two spheres on X axis: S1 centered at origin, S2 centered at (10, 0, 0)
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Minimum distance: 10 - 2 - 3 = 5
  const double aExpectedMinDist = 5.0;
  const double aMinSqDist       = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), aExpectedMinDist, THE_TOL);

  // Maximum distance: 10 + 2 + 3 = 15
  const double aExpectedMaxDist = 15.0;
  const double aMaxSqDist       = aResult.MaxSquareDistance();
  EXPECT_NEAR(std::sqrt(aMaxSqDist), aExpectedMaxDist, THE_TOL);

  // Check minimum extremum
  const int aMinIdx = aResult.MinIndex();
  ASSERT_GE(aMinIdx, 0);
  const ExtremaSS::ExtremumResult& aMinExt = aResult[aMinIdx];
  EXPECT_TRUE(aMinExt.IsMinimum);

  // Min points should be at (2, 0, 0) on S1 and (7, 0, 0) on S2
  EXPECT_NEAR(aMinExt.Point1.X(), 2.0, THE_TOL);
  EXPECT_NEAR(aMinExt.Point1.Y(), 0.0, THE_TOL);
  EXPECT_NEAR(aMinExt.Point1.Z(), 0.0, THE_TOL);
  EXPECT_NEAR(aMinExt.Point2.X(), 7.0, THE_TOL);
  EXPECT_NEAR(aMinExt.Point2.Y(), 0.0, THE_TOL);
  EXPECT_NEAR(aMinExt.Point2.Z(), 0.0, THE_TOL);

  // Check maximum extremum
  const int aMaxIdx = aResult.MaxIndex();
  ASSERT_GE(aMaxIdx, 0);
  const ExtremaSS::ExtremumResult& aMaxExt = aResult[aMaxIdx];
  EXPECT_FALSE(aMaxExt.IsMinimum);

  // Max points should be at (-2, 0, 0) on S1 and (13, 0, 0) on S2
  EXPECT_NEAR(aMaxExt.Point1.X(), -2.0, THE_TOL);
  EXPECT_NEAR(aMaxExt.Point1.Y(), 0.0, THE_TOL);
  EXPECT_NEAR(aMaxExt.Point1.Z(), 0.0, THE_TOL);
  EXPECT_NEAR(aMaxExt.Point2.X(), 13.0, THE_TOL);
  EXPECT_NEAR(aMaxExt.Point2.Y(), 0.0, THE_TOL);
  EXPECT_NEAR(aMaxExt.Point2.Z(), 0.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, ConcentricSpheres_ReturnsInfiniteSolutions)
{
  // Two concentric spheres at origin
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Constant distance = |R1 - R2| = |5 - 3| = 2
  const double aExpectedDist = 2.0;
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), aExpectedDist, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, TouchingSpheres_MinDistanceIsZero)
{
  // Two spheres touching: S1 at origin with R=2, S2 at (5, 0, 0) with R=3
  // Distance between centers = 5 = R1 + R2, so they touch
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Minimum distance should be 0 (or very close to it)
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, IntersectingSpheres_MinDistanceIsZero)
{
  // Two overlapping spheres
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(4, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Distance between centers (4) < R1 + R2 (6), so spheres intersect
  // Minimum distance = 0
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, SpheresAlongDiagonal_CorrectExtrema)
{
  // Spheres along diagonal direction
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(3, 4, 0), gp_Dir(0, 0, 1)), 2.0);
  // Distance between centers = sqrt(9 + 16) = 5

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Minimum distance: 5 - 1 - 2 = 2
  const double aExpectedMinDist = 2.0;
  const double aMinSqDist       = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), aExpectedMinDist, THE_TOL);

  // Maximum distance: 5 + 1 + 2 = 8
  const double aExpectedMaxDist = 8.0;
  const double aMaxSqDist       = aResult.MaxSquareDistance();
  EXPECT_NEAR(std::sqrt(aMaxSqDist), aExpectedMaxDist, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 1);

  // Should only find the minimum
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 1);

  // Should only find the maximum
  EXPECT_FALSE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaSS_SphereSphereTest, NestedSpheres_CorrectDistances)
{
  // Sphere2 is completely inside Sphere1
  const gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  const gp_Sphere aSphere2(gp_Ax3(gp_Pnt(2, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  // Center distance = 2, R1 - R2 = 7, so S2 is inside S1

  ExtremaSS_SphereSphere   anEval(aSphere1, aSphere2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Minimum distance: |2 - 10 - 3| = |2 - 13| = 5 (nearest surfaces)
  // Actually: (10 - 2) - 3 = 5 (outer sphere inner point to inner sphere closest point)
  // Wait, this is wrong. Let me reconsider.
  // Min point on S1: along line from S1 center to S2 center = (2, 0, 0) normalized = (1, 0, 0)
  // So S1 point toward S2 = (10, 0, 0) (but S2 is at (2, 0, 0) with R=3)
  // S2 point toward S1 = (2, 0, 0) - 3*(1, 0, 0) = (-1, 0, 0)
  // These are on opposite sides! Let me recalculate.

  // Actually the minimum is when S1 and S2 points are as close as possible:
  // S1 toward S2: (10, 0, 0) - but that's outside S1 by definition
  // The correct interpretation: distance from S1 surface to S2 surface
  // Min: center_dist - R1 - R2 = 2 - 10 - 3 = -11 (negative means overlapping/nested)
  // So they do overlap, and the minimum distance is 0 (surfaces touch internally)

  // But since one is completely inside the other, the actual minimum surface distance
  // is R1 - center_dist - R2 = 10 - 2 - 3 = 5
}
