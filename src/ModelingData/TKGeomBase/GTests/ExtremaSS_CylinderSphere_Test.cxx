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

#include <ExtremaSS_CylinderSphere.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for Cylinder-Sphere extrema tests.
class ExtremaSS_CylinderSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderSphereTest, SeparatedCylinderAndSphere_FindsMinAndMax)
{
  // Cylinder along Z axis at origin with radius 2
  // Sphere at (10, 0, 0) with radius 3
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Distance from sphere center to cylinder axis = 10
  // Minimum distance: 10 - 2 - 3 = 5
  const double aExpectedMinDist = 5.0;
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedMinDist, THE_TOL);

  // Maximum distance: 10 + 2 + 3 = 15
  const double aExpectedMaxDist = 15.0;
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), aExpectedMaxDist, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, SphereCenterOnCylinderAxis_InfiniteSolutions)
{
  // Cylinder along Z axis at origin
  // Sphere centered on cylinder axis
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Constant distance = |Rcyl - Rsph| = |5 - 3| = 2
  const double aExpectedDist = 2.0;
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), aExpectedDist, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, TouchingCylinderAndSphere_MinDistanceZero)
{
  // Cylinder along Z axis with radius 2
  // Sphere touching cylinder at (5, 0, 0) with radius 3
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Distance: 5 - 2 - 3 = 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, IntersectingCylinderAndSphere_MinDistanceZero)
{
  // Overlapping cylinder and sphere
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(4, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Distance from center to axis (4) < Rcyl + Rsph (6), so they intersect
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, SwappedOrder_SameResult)
{
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval1(aCylinder, aSphere);
  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);

  ExtremaSS_CylinderSphere anEval2(aSphere, aCylinder);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult2.Status, ExtremaSS::Status::OK);

  // Distances should be identical
  EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL * THE_TOL);
  EXPECT_NEAR(aResult1.MaxSquareDistance(), aResult2.MaxSquareDistance(), THE_TOL * THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_FALSE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 15.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, TiltedCylinder_GeneralOrientation)
{
  // Cylinder tilted at 45 degrees
  const gp_Dir      aTiltedAxis(1, 0, 1);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), aTiltedAxis), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(10, 10, 0), gp_Dir(0, 0, 1)), 3.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify minimum distance is positive (surfaces are separated)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 0.0);
}

TEST_F(ExtremaSS_CylinderSphereTest, SmallCylinder_FarFromSphere)
{
  // Small cylinder far from large sphere
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.5);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(100, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Minimum distance: 100 - 0.5 - 10 = 89.5
  const double aExpectedMinDist = 89.5;
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedMinDist, THE_TOL);

  // Maximum distance: 100 + 0.5 + 10 = 110.5
  const double aExpectedMaxDist = 110.5;
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), aExpectedMaxDist, THE_TOL);
}

TEST_F(ExtremaSS_CylinderSphereTest, SphereDiagonalFromCylinder_CorrectExtrema)
{
  // Sphere at diagonal position from cylinder
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Sphere   aSphere(gp_Ax3(gp_Pnt(3, 4, 5), gp_Dir(0, 0, 1)), 1.0);
  // Distance from sphere center to cylinder axis = sqrt(9 + 16) = 5

  ExtremaSS_CylinderSphere anEval(aCylinder, aSphere);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult.NbExt(), 2);

  // Minimum distance: 5 - 2 - 1 = 2
  const double aExpectedMinDist = 2.0;
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedMinDist, THE_TOL);

  // Maximum distance: 5 + 2 + 1 = 8
  const double aExpectedMaxDist = 8.0;
  EXPECT_NEAR(std::sqrt(aResult.MaxSquareDistance()), aExpectedMaxDist, THE_TOL);
}
