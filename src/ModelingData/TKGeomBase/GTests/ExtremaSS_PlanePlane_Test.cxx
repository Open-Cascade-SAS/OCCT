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

#include <ExtremaSS_PlanePlane.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for Plane-Plane extrema tests.
class ExtremaSS_PlanePlaneTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_PlanePlaneTest, ParallelPlanes_InfiniteSolutions)
{
  // Two parallel planes at Z=0 and Z=5
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Pln aPlane2(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Constant distance = 5
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_PlanePlaneTest, ParallelPlanes_OppositeNormals_InfiniteSolutions)
{
  // Two parallel planes with opposite normals
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Pln aPlane2(gp_Pnt(0, 0, 10), gp_Dir(0, 0, -1));

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Constant distance = 10
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

TEST_F(ExtremaSS_PlanePlaneTest, CoincidentPlanes_InfiniteSolutions)
{
  // Two coincident planes
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Pln aPlane2(gp_Pnt(5, 5, 0), gp_Dir(0, 0, 1));

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Constant distance = 0
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL * THE_TOL);
}

TEST_F(ExtremaSS_PlanePlaneTest, PerpendicularPlanes_NoSolution)
{
  // Two perpendicular planes that intersect
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane
  const gp_Pln aPlane2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)); // YZ plane

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Intersecting planes - no meaningful extremum
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::NoSolution);
}

TEST_F(ExtremaSS_PlanePlaneTest, AngledPlanes_NoSolution)
{
  // Two planes at 45 degrees
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Pln aPlane2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 1));

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Non-parallel planes intersect
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::NoSolution);
}

TEST_F(ExtremaSS_PlanePlaneTest, TiltedParallelPlanes_InfiniteSolutions)
{
  // Two parallel tilted planes
  const gp_Dir aTiltedNormal(1, 1, 1);
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), aTiltedNormal);
  const gp_Pln aPlane2(gp_Pnt(10, 10, 10), aTiltedNormal);
  // Distance = (10,10,10) . (1,1,1)/|1,1,1| = 30/sqrt(3) = 10*sqrt(3)

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);

  // Distance = 10 * sqrt(3) ~ 17.32
  const double aExpectedDist = 10.0 * std::sqrt(3.0);
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), aExpectedDist, THE_TOL);
}

TEST_F(ExtremaSS_PlanePlaneTest, SearchModeMin_ParallelPlanes)
{
  const gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const gp_Pln aPlane2(gp_Pnt(0, 0, 7), gp_Dir(0, 0, 1));

  ExtremaSS_PlanePlane anEval(aPlane1, aPlane2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  // Even with SearchMode::Min, parallel planes give infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaSS::Status::InfiniteSolutions);
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 7.0, THE_TOL);
}
