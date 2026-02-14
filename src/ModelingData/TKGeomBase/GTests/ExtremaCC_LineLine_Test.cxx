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

#include <ExtremaCC_LineLine.hxx>

#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCC_LineLine tests.
class ExtremaCC_LineLineTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;
};

//==================================================================================================
// Parallel lines tests
//==================================================================================================

TEST_F(ExtremaCC_LineLineTest, ParallelLines_SameDirection)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(0, 3, 4), gp_Dir(1, 0, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  // Parallel lines have infinite solutions
  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  // Distance should be constant: sqrt(3^2 + 4^2) = 5
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, ParallelLines_OppositeDirection)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(0, 5, 0), gp_Dir(-1, 0, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, ParallelLines_ZeroDistance)
{
  // Same line
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(5, 0, 0), gp_Dir(1, 0, 0)); // Same line, different origin

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

//==================================================================================================
// Intersecting lines tests
//==================================================================================================

TEST_F(ExtremaCC_LineLineTest, IntersectingLines_AtOrigin)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter1, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter2, 0.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, IntersectingLines_OffOrigin)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter1, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter2, 0.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, IntersectingLines_Diagonal)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 0));
  gp_Lin aLine2(gp_Pnt(0, 0, 0), gp_Dir(1, -1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter1, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter2, 0.0, THE_TOL);
}

//==================================================================================================
// Skew lines tests
//==================================================================================================

TEST_F(ExtremaCC_LineLineTest, SkewLines_Simple)
{
  // Line 1 along X, Line 2 along Y offset in Z
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(0, 0, 5), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Minimum distance is Z offset = 5
  EXPECT_NEAR(aResult[0].SquareDistance, 25.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter1, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter2, 0.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, SkewLines_OffsetOrigins)
{
  // Line 1 at origin along X, Line 2 offset in Y and Z
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(10, 3, 4), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Minimum distance is Z offset = 4
  // Line2: (10, 3+t2, 4), perpendicular to Line1 dir requires 3+t2=0, so t2=-3
  EXPECT_NEAR(aResult[0].SquareDistance, 16.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter1, 10.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter2, -3.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, SkewLines_3D)
{
  // Two skew lines in 3D space
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)); // Along Z, offset by 10

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Lines are perpendicular and closest at their origins
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter1, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].Parameter2, -10.0, THE_TOL);
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCC_LineLineTest, DomainConstraints_IntersectionInside)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0));

  // Domain contains the intersection
  ExtremaCC::Domain2D aDomain{{0.0, 10.0}, {-5.0, 5.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, DomainConstraints_IntersectionOutside)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(50, 0, 0), gp_Dir(0, 1, 0)); // Intersection at u1=50

  // Domain excludes the intersection point on line1
  ExtremaCC::Domain2D aDomain{{0.0, 10.0}, {-5.0, 5.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // With bounded domain, closest point is at boundary
  ASSERT_GE(aResult.NbExt(), 1);
  // Minimum distance should be at u1=10 (upper bound)
  EXPECT_NEAR(aResult.MinSquareDistance(), 1600.0, THE_TOL); // (50-10)^2
}

//==================================================================================================
// Point verification tests
//==================================================================================================

TEST_F(ExtremaCC_LineLineTest, VerifyClosestPoints_Skew)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(5, 3, 4), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Verify points
  // Line1: (t1, 0, 0), Line2: (5, 3+t2, 4)
  // Perpendicular conditions: t1=5, t2=-3
  // So P1=(5,0,0), P2=(5, 3+(-3), 4)=(5,0,4)
  gp_Pnt aExpectedP1(5, 0, 0);
  gp_Pnt aExpectedP2(5, 0, 4);

  EXPECT_NEAR(aResult[0].Point1.X(), aExpectedP1.X(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point1.Y(), aExpectedP1.Y(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point1.Z(), aExpectedP1.Z(), THE_TOL);

  EXPECT_NEAR(aResult[0].Point2.X(), aExpectedP2.X(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point2.Y(), aExpectedP2.Y(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point2.Z(), aExpectedP2.Z(), THE_TOL);

  // Verify distance consistency: only Z difference of 4
  double aComputedDist = aResult[0].Point1.SquareDistance(aResult[0].Point2);
  EXPECT_NEAR(aResult[0].SquareDistance, aComputedDist, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 16.0, THE_TOL);
}

TEST_F(ExtremaCC_LineLineTest, VerifyClosestPoints_Intersecting)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(10, 0, 0), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);

  // Both points should be at the intersection
  gp_Pnt aExpected(10, 0, 0);

  EXPECT_NEAR(aResult[0].Point1.X(), aExpected.X(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point1.Y(), aExpected.Y(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point1.Z(), aExpected.Z(), THE_TOL);

  EXPECT_NEAR(aResult[0].Point2.X(), aExpected.X(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point2.Y(), aExpected.Y(), THE_TOL);
  EXPECT_NEAR(aResult[0].Point2.Z(), aExpected.Z(), THE_TOL);
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCC_LineLineTest, SearchMode_MinOnly)
{
  gp_Lin aLine1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Lin aLine2(gp_Pnt(0, 0, 5), gp_Dir(0, 1, 0));

  ExtremaCC::Domain2D aDomain{{-100.0, 100.0}, {-100.0, 100.0}};
  ExtremaCC_LineLine  anExtrema(aLine1, aLine2, aDomain);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCC::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // All results should be minima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

