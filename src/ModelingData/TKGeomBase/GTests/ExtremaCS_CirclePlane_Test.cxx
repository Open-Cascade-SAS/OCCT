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

#include <ExtremaCS_CirclePlane.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_CirclePlane tests.
class ExtremaCS_CirclePlaneTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-9;
};

//==================================================================================================
// Parallel circle tests
//==================================================================================================

TEST_F(ExtremaCS_CirclePlaneTest, Parallel_AbovePlane)
{
  // Circle in plane Z=5, parallel to XY plane
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  gp_Circ aCircle(aCircleAx, 3.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCS::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL); // Distance = 5
}

TEST_F(ExtremaCS_CirclePlaneTest, Parallel_InPlane)
{
  // Circle in XY plane
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ aCircle(aCircleAx, 3.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Tilted circle tests
//==================================================================================================

TEST_F(ExtremaCS_CirclePlaneTest, Tilted_CircleThroughPlane)
{
  // Circle tilted at 45 degrees, center on plane
  gp_Dir  aTiltedNormal(1, 0, 1); // gp_Dir auto-normalizes
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 0), aTiltedNormal);
  gp_Circ aCircle(aCircleAx, 2.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  // Should have 2 extrema (min and max)
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at the intersection (distance 0)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);

  // If we have 2 extrema, check max distance = radius * sin(45) = sqrt(2)
  if (aResult.NbExt() >= 2)
  {
    double aMaxDist = aResult.MaxSquareDistance();
    EXPECT_NEAR(aMaxDist, 2.0, THE_TOL); // (sqrt(2))^2 = 2
  }
}

TEST_F(ExtremaCS_CirclePlaneTest, Tilted_CircleAbovePlane)
{
  // Circle tilted at 45 degrees, center above plane
  gp_Dir  aTiltedNormal(0, 1, 1); // gp_Dir auto-normalizes
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 5), aTiltedNormal);
  gp_Circ aCircle(aCircleAx, 2.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at distance = 5 - sqrt(2) (center height - radius*sin(45))
  double aMinDist     = aResult.MinSquareDistance();
  double aExpectedMin = 5.0 - std::sqrt(2.0);
  EXPECT_NEAR(aMinDist, aExpectedMin * aExpectedMin, 0.1);

  // Maximum should be at distance = 5 + sqrt(2)
  if (aResult.NbExt() >= 2)
  {
    double aMaxDist     = aResult.MaxSquareDistance();
    double aExpectedMax = 5.0 + std::sqrt(2.0);
    EXPECT_NEAR(aMaxDist, aExpectedMax * aExpectedMax, 0.1);
  }
}

TEST_F(ExtremaCS_CirclePlaneTest, Perpendicular_CircleEdgeOnPlane)
{
  // Circle perpendicular to XY plane, edge touches plane
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 3), gp_Dir(1, 0, 0)); // Circle in YZ plane
  gp_Circ aCircle(aCircleAx, 3.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)); // XY plane

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be 0 (circle touches plane)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);

  // Maximum should be 6 (center at Z=3, radius 3, max at Z=6)
  if (aResult.NbExt() >= 2)
  {
    EXPECT_NEAR(aResult.MaxSquareDistance(), 36.0, THE_TOL);
  }
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCS_CirclePlaneTest, SearchMode_MinOnly)
{
  gp_Dir  aTiltedNormal(1, 0, 1); // gp_Dir auto-normalizes
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 5), aTiltedNormal);
  gp_Circ aCircle(aCircleAx, 2.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaCS_CirclePlaneTest, SearchMode_MaxOnly)
{
  gp_Dir  aTiltedNormal(1, 0, 1); // gp_Dir auto-normalizes
  gp_Ax2  aCircleAx(gp_Pnt(0, 0, 5), aTiltedNormal);
  gp_Circ aCircle(aCircleAx, 2.0);
  gp_Pln  aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  ExtremaCS_CirclePlane    anExtrema(aCircle, aPlane);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_FALSE(aResult[0].IsMinimum);
}
