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

#include <ExtremaPC_Parabola.hxx>

#include <gp_Ax2.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <ElCLib.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_Parabola tests.
class ExtremaPC_ParabolaTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-8;
};

//==================================================================================================
// Basic tests - point on axis
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, PointOnAxis_AtVertex)
{
  // Parabola with focal length 5, vertex at origin
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(0.0, 0.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, PointOnAxis_Positive)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(10.0, 0.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // For parabola y² = 20x with point at (10,0,0) on axis,
  // the closest point is the vertex at (0,0,0) with distance = 10
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 100.0, THE_TOL); // Distance² to vertex
}

TEST_F(ExtremaPC_ParabolaTest, PointOnAxis_Negative)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(-10.0, 0.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Closest point is at vertex (0,0,0)
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 100.0, THE_TOL); // Distance to vertex
}

//==================================================================================================
// Point off axis
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, PointOffAxis_Positive)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(5.0, 10.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the point is on the parabola
  int    aMinIdx     = aResult.MinIndex();
  gp_Pnt aPtOnParab  = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  double aExpectedSq = aPoint.SquareDistance(aPtOnParab);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aExpectedSq, THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, PointOffAxis_Negative)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(5.0, -10.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the closest point is on the parabola and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnParab), THE_TOL);

  // For negative Y, parameter should be negative
  EXPECT_LT(aResult[aMinIdx].Parameter, 0.0);
}

TEST_F(ExtremaPC_ParabolaTest, PointOffAxis_LargeY)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(0.0, 50.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the closest point is on the parabola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);

  // Distance should be less than distance to vertex (50)
  EXPECT_LT(std::sqrt(aResult.MinSquareDistance()), 50.0);

  // Parameter should be positive (point at positive Y)
  EXPECT_GT(aResult[aMinIdx].Parameter, 0.0);
}

//==================================================================================================
// Point out of plane
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, PointAbovePlane)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(5.0, 10.0, 7.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Distance should include Z component
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GE(aMinSqDist, 49.0); // At least 7^2
}

TEST_F(ExtremaPC_ParabolaTest, PointBelowPlane)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(0.0, 0.0, -5.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Closest point is at vertex, distance = 5
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 25.0, THE_TOL);
}

//==================================================================================================
// Bound constraint tests
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, BoundsPositive)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(0.0, -10.0, 0.0);

  // Create evaluator with positive domain [0, 10]
  ExtremaPC_Parabola anEval(aParabola, ExtremaPC::Domain1D{0.0, 10.0});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Since bounds are positive and point is at negative Y,
  // closest point should be at u=0 (vertex)
  int aMinIdx = aResult.MinIndex();
  EXPECT_GE(aResult[aMinIdx].Parameter, 0.0);
}

TEST_F(ExtremaPC_ParabolaTest, BoundsNegative)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(0.0, 10.0, 0.0);

  // Create evaluator with negative domain [-10, 0]
  ExtremaPC_Parabola anEval(aParabola, ExtremaPC::Domain1D{-10.0, 0.0});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Since bounds are negative and point is at positive Y,
  // closest point should be at u=0 (vertex)
  int aMinIdx = aResult.MinIndex();
  EXPECT_LE(aResult[aMinIdx].Parameter, 0.0);
}

TEST_F(ExtremaPC_ParabolaTest, NarrowBounds)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(5.0, 5.0, 0.0);

  // Create evaluator with narrow domain [4, 6]
  ExtremaPC_Parabola anEval(aParabola, ExtremaPC::Domain1D{4.0, 6.0});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Parameter should be within bounds
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, 4.0 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter, 6.0 + THE_TOL);
  }
}

//==================================================================================================
// Different focal lengths
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, SmallFocalLength)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 0.5);
  gp_Pnt   aPoint(1.0, 2.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on parabola and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnParab), THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, LargeFocalLength)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 100.0);
  gp_Pnt   aPoint(50.0, 100.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on parabola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);
}

//==================================================================================================
// Point on parabola
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, PointOnParabola_AtVertex)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint = ElCLib::Value(0.0, aParabola);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, PointOnParabola_Positive)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint = ElCLib::Value(10.0, aParabola);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, PointOnParabola_Negative)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint = ElCLib::Value(-15.0, aParabola);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

//==================================================================================================
// Parabola with offset
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, ParabolaWithOffset)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(100.0, 200.0, 50.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Point is at vertex
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, ParabolaWithOffset_PointOff)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(110.0, 210.0, 50.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on parabola and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnParab), THE_TOL);
}

//==================================================================================================
// Different orientations
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, ParabolaInYZPlane)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0)), 5.0);
  gp_Pnt   aPoint(3.0, 10.0, 5.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify extremum point is on the parabola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);

  // Distance should include X component (out of plane)
  EXPECT_GE(aResult.MinSquareDistance(), 9.0); // At least 3^2
}

TEST_F(ExtremaPC_ParabolaTest, ParabolaInXZPlane)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(10.0, 3.0, 5.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify extremum point is on the parabola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnParab = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnParab), 0.0, THE_TOL);

  // Distance should include Y component (out of plane)
  EXPECT_GE(aResult.MinSquareDistance(), 9.0); // At least 3^2
}

//==================================================================================================
// Verification tests
//==================================================================================================

TEST_F(ExtremaPC_ParabolaTest, VerifyProjectedPoint)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(10.0, 15.0, 0.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify the projected point is on the parabola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = ElCLib::Value(aResult[aMinIdx].Parameter, aParabola);
  EXPECT_NEAR(aResult[aMinIdx].Point.X(), aPtOnCurve.X(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Y(), aPtOnCurve.Y(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Z(), aPtOnCurve.Z(), THE_TOL);
}

TEST_F(ExtremaPC_ParabolaTest, VerifyDistanceConsistency)
{
  gp_Parab aParabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), 5.0);
  gp_Pnt   aPoint(8.0, 12.0, 3.0);

  ExtremaPC_Parabola anEval(aParabola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance matches point distance
  int    aMinIdx      = aResult.MinIndex();
  double aComputedSq  = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aComputedSq, THE_TOL);
}

