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

#include <ExtremaPC_Line.hxx>

#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_Line tests.
//! With endpoint handling enabled, line extrema include:
//! - 1 minimum (the projection or clamped point)
//! - 0-2 maxima at endpoints (when they are local maxima)
class ExtremaPC_LineTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;
};

//==================================================================================================
// Basic projection tests
// When projection is inside bounds: 3 extrema (1 min projection + 2 max endpoints)
//==================================================================================================

TEST_F(ExtremaPC_LineTest, PointOnLine_AtOrigin)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
  EXPECT_TRUE(aResult[aMinIdx].IsMinimum);
}

TEST_F(ExtremaPC_LineTest, PointOnLine_Positive)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(5.0, 0.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_LineTest, PointOnLine_Negative)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(-7.5, 0.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, -7.5, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_LineTest, PointOffLine_YOffset)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(5.0, 3.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 9.0, THE_TOL); // 3^2 = 9
}

TEST_F(ExtremaPC_LineTest, PointOffLine_ZOffset)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(5.0, 0.0, 4.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 16.0, THE_TOL); // 4^2 = 16
}

TEST_F(ExtremaPC_LineTest, PointOffLine_YZOffset)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(5.0, 3.0, 4.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, THE_TOL); // 3^2 + 4^2 = 25
}

//==================================================================================================
// Bound constraint tests
// When projection is outside bounds: 2 extrema (1 min at closer endpoint + 1 max at farther)
//==================================================================================================

TEST_F(ExtremaPC_LineTest, ProjectionOutsideBounds_Left)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(-50.0, 3.0, 4.0);

  // Domain [0, 100] - projection at -50 is outside bounds (left of 0)
  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{0.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2); // min at 0, max at 100

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 0.0, THE_TOL); // Clamped to lower bound
}

TEST_F(ExtremaPC_LineTest, ProjectionOutsideBounds_Right)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(150.0, 3.0, 4.0);

  // Domain [0, 100] - projection at 150 is outside bounds (right of 100)
  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{0.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2); // min at 100, max at 0

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 100.0, THE_TOL); // Clamped to upper bound
}

TEST_F(ExtremaPC_LineTest, ProjectionAtLowerBound)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(10.0, 5.0, 0.0);

  // Domain [10, 100] - projection at 10 is exactly at lower bound
  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{10.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2); // min at 10, max at 100

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 10.0, THE_TOL);
}

TEST_F(ExtremaPC_LineTest, ProjectionAtUpperBound)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(100.0, 5.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2); // min at 100, max at 0

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 100.0, THE_TOL);
}

TEST_F(ExtremaPC_LineTest, NarrowBounds)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(50.0, 1.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // min at 50, max at endpoints

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 50.0, THE_TOL);
}

//==================================================================================================
// Different line orientations
//==================================================================================================

TEST_F(ExtremaPC_LineTest, LineAlongY)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));
  gp_Pnt aPoint(3.0, 5.0, 4.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, THE_TOL); // 3^2 + 4^2 = 25
}

TEST_F(ExtremaPC_LineTest, LineAlongZ)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pnt aPoint(3.0, 4.0, 10.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 10.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, THE_TOL); // 3^2 + 4^2 = 25
}

TEST_F(ExtremaPC_LineTest, LineDiagonal_XY)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 0));
  gp_Pnt aPoint(5.0, 5.0, 3.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  // Parameter along diagonal: (5,5,0) projected = 5*sqrt(2)
  double aExpectedParam = 5.0 * std::sqrt(2.0);
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, aExpectedParam, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 9.0, THE_TOL); // Z offset = 3
}

TEST_F(ExtremaPC_LineTest, LineDiagonal_XYZ)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  gp_Pnt aPoint(3.0, 3.0, 3.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max
  // Point is on the line
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Offset line origin tests
//==================================================================================================

TEST_F(ExtremaPC_LineTest, LineWithOffset_XAxis)
{
  gp_Lin aLine(gp_Pnt(10, 20, 30), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(15.0, 20.0, 30.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL); // 15 - 10 = 5
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_LineTest, LineWithOffset_PointOff)
{
  gp_Lin aLine(gp_Pnt(10, 20, 30), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(15.0, 23.0, 34.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, THE_TOL); // 3^2 + 4^2 = 25
}

//==================================================================================================
// Negative parameter tests
//==================================================================================================

TEST_F(ExtremaPC_LineTest, NegativeParameters)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(-25.0, 0.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, -25.0, THE_TOL);
}

TEST_F(ExtremaPC_LineTest, NegativeBoundsClamp)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(-5.0, 1.0, 0.0);

  // Domain [-50, -10] - projection at -5 is outside (right of -10), clamps to -10
  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-50.0, -10.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2); // min at -10, max at -50

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, -10.0, THE_TOL); // Clamped to upper bound
}

//==================================================================================================
// Large coordinate tests
//==================================================================================================

TEST_F(ExtremaPC_LineTest, LargeCoordinates)
{
  gp_Lin aLine(gp_Pnt(1e6, 1e6, 1e6), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(1e6 + 100.0, 1e6 + 3.0, 1e6 + 4.0);

  // Use domain [-100, 200] so projection at 100 is inside bounds
  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 200.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 100.0, 1e-6);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, 1e-6);
}

TEST_F(ExtremaPC_LineTest, SmallCoordinates)
{
  gp_Lin aLine(gp_Pnt(1e-6, 1e-6, 1e-6), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(1e-6 + 1e-3, 1e-6 + 3e-4, 1e-6 + 4e-4);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 1e-3, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25e-8, THE_TOL); // (3e-4)^2 + (4e-4)^2
}

//==================================================================================================
// Point verification tests
//==================================================================================================

TEST_F(ExtremaPC_LineTest, VerifyProjectedPoint)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(7.0, 5.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  // Verify the minimum point (projected point)
  int aMinIdx = aResult.MinIndex();
  gp_Pnt aExpectedPt(7.0, 0.0, 0.0);
  EXPECT_NEAR(aResult[aMinIdx].Point.X(), aExpectedPt.X(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Y(), aExpectedPt.Y(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Z(), aExpectedPt.Z(), THE_TOL);
}

TEST_F(ExtremaPC_LineTest, VerifyDistanceConsistency)
{
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(10.0, 6.0, 8.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{-100.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max

  // Verify distance matches point distance for minimum
  int aMinIdx = aResult.MinIndex();
  double aComputedDist = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aComputedDist, THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL); // 6^2 + 8^2 = 100
}

//==================================================================================================
// Endpoint handling verification
//==================================================================================================

TEST_F(ExtremaPC_LineTest, EndpointsAsMaxima)
{
  // For a line segment with projection inside, endpoints should be local maxima
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  gp_Pnt aPoint(5.0, 1.0, 0.0); // Point above line at X=5

  // Domain [0, 10] for specific endpoint checks
  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{0.0, 10.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should have 3 extrema: 1 minimum (projection) + 2 maxima (endpoints)
  ASSERT_EQ(aResult.NbExt(), 3);

  // Verify minimum is at the projection
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 1.0, THE_TOL); // 1^2

  // Verify maximum is at one of the endpoints
  int aMaxIdx = aResult.MaxIndex();
  double aMaxParam = aResult[aMaxIdx].Parameter;
  EXPECT_TRUE(std::abs(aMaxParam - 0.0) < THE_TOL || std::abs(aMaxParam - 10.0) < THE_TOL);
}

