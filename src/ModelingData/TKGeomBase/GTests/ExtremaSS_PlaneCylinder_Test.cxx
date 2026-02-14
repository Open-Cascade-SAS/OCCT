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

#include <ExtremaSS_PlaneCylinder.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for Plane-Cylinder extrema tests.
class ExtremaSS_PlaneCylinderTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Infinite Solutions Tests
//==================================================================================================

TEST_F(ExtremaSS_PlaneCylinderTest, AxisParallelToPlane_InfiniteSolutions)
{
  // Plane at Z=0, cylinder with axis parallel to plane at height Z=10
  // When axis is parallel to plane, there are infinite extrema along the closest generator
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(1, 0, 0)), 3.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Minimum distance: 10 - 3 = 7
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 7.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneCylinderTest, AxisPerpendicularToPlane_InfiniteSolutions)
{
  // Plane at Z=0, cylinder with axis perpendicular to plane (along Z)
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 5.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // When axis is perpendicular to plane, all points on the circle at any V
  // are equidistant from plane - infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_PlaneCylinderTest, AxisParallelToPlane_Touching_InfiniteSolutions)
{
  // Plane at Z=0, cylinder with axis at Z=3 and radius 3 (touching plane)
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 3), gp_Dir(1, 0, 0)), 3.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Axis parallel to plane but touching -> InfiniteSolutions with distance 0
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaSS_PlaneCylinderTest, AxisParallelToPlane_Intersecting_InfiniteSolutions)
{
  // Plane at Z=0, cylinder with axis at Z=2 and radius 5 (intersecting)
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 2), gp_Dir(1, 0, 0)), 5.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Axis parallel to plane and intersecting -> InfiniteSolutions with distance 0
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Tilted Cylinder Tests (General Case with Finite Solutions)
//==================================================================================================

TEST_F(ExtremaSS_PlaneCylinderTest, TiltedCylinder_FindsMinAndMax)
{
  // Plane at Z=0, cylinder tilted at 45 degrees
  // Tilted axis has non-zero dot with plane normal, so we get finite solutions
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir      aTiltedAxis(1, 0, 1); // 45 degrees to plane
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 10), aTiltedAxis), 2.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be positive (cylinder above plane)
  EXPECT_GT(std::sqrt(aResult.MinSquareDistance()), 0.0);
}

TEST_F(ExtremaSS_PlaneCylinderTest, TiltedCylinder_PredictableMinDistance)
{
  // Plane at Z=0, tilted cylinder at 45 degrees
  // For an infinite tilted cylinder, the minimum distance to a horizontal plane
  // is always R * sin(tilt_angle), regardless of axis position
  // This is because the axis always intersects the plane somewhere
  const gp_Pln aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir aTiltedAxis(1, 0, 1); // 45 degrees to plane (sin(45) = 1/sqrt(2))

  const double      aRadius = 2.0;
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 100), aTiltedAxis), aRadius);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Minimum distance = R * sin(45 deg) = R / sqrt(2) ~ 1.414
  const double aExpectedMinDist = aRadius / std::sqrt(2.0);
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedMinDist, THE_TOL);
}

TEST_F(ExtremaSS_PlaneCylinderTest, SwappedOrder_SameResult)
{
  // Use tilted cylinder for finite solutions
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir      aTiltedAxis(1, 0, 1);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 10), aTiltedAxis), 3.0);

  ExtremaSS_PlaneCylinder  anEval1(aPlane, aCylinder);
  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);

  ExtremaSS_PlaneCylinder  anEval2(aCylinder, aPlane);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult2.Status, ExtremaSS::Status::OK);

  EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL * THE_TOL);
}

TEST_F(ExtremaSS_PlaneCylinderTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir      aTiltedAxis(1, 0, 1);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 10), aTiltedAxis), 3.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // All found extrema should be minima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

TEST_F(ExtremaSS_PlaneCylinderTest, SearchModeMax_OnlyFindsMaximum)
{
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir      aTiltedAxis(1, 0, 1);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 10), aTiltedAxis), 3.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // All found extrema should be maxima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_FALSE(aResult[i].IsMinimum);
  }
}

TEST_F(ExtremaSS_PlaneCylinderTest, TiltedPlane_TiltedCylinder)
{
  // Tilted plane and cylinder with different tilt
  const gp_Dir aTiltedNormal(0, 1, 1);
  const gp_Pln aPlane(gp_Pnt(0, 0, 0), aTiltedNormal);

  // Cylinder axis not parallel to tilted plane
  const gp_Dir      aCylAxis(1, 1, 0);
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 10, 10), aCylAxis), 2.0);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_PlaneCylinderTest, SmallTiltedCylinder_MinDistanceFormula)
{
  // Small tilted cylinder - verify minimum distance formula
  // For 45-degree tilt, min distance = R / sqrt(2)
  const gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Dir      aTiltedAxis(1, 0, 1);
  const double      aRadius = 0.5;
  const gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 100), aTiltedAxis), aRadius);

  ExtremaSS_PlaneCylinder  anEval(aPlane, aCylinder);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);

  // Minimum distance = R * sin(45 deg) = R / sqrt(2) ~ 0.354
  const double aExpectedMinDist = aRadius / std::sqrt(2.0);
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedMinDist, THE_TOL);
}
