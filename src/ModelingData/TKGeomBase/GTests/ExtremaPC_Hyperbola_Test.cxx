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

#include <ExtremaPC_Hyperbola.hxx>

#include <gp_Ax2.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <ElCLib.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_Hyperbola tests.
class ExtremaPC_HyperbolaTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-8;
};

//==================================================================================================
// Basic tests - point on major axis
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, PointOnMajorAxis_Outside)
{
  // Hyperbola with a=20, b=10
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(50.0, 0.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // For point (50, 0) on major axis, the closest point is NOT the vertex!
  // The extremum condition gives cosh(u) = 2 (u ≈ 1.317), yielding
  // closest point at (40, ~17.3, 0) with distance ≈ 20.
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 400.0, 1.0);
}

TEST_F(ExtremaPC_HyperbolaTest, PointOnMajorAxis_AtVertex)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, PointOnMajorAxis_BetweenBranches)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(0.0, 0.0, 0.0); // Center

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Closest point is at vertex (20, 0, 0), distance = 20
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 20.0, THE_TOL);
}

//==================================================================================================
// Point off axis tests
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, PointOffAxis_FirstQuadrant)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, 15.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the point is on the hyperbola
  int    aMinIdx     = aResult.MinIndex();
  gp_Pnt aPtOnHyper  = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  double aExpectedSq = aPoint.SquareDistance(aPtOnHyper);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aExpectedSq, THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, PointOffAxis_FourthQuadrant)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, -15.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the closest point is on the hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnHyper), THE_TOL);

  // For negative Y, parameter should be negative
  EXPECT_LT(aResult[aMinIdx].Parameter, 0.0);
}

TEST_F(ExtremaPC_HyperbolaTest, PointInsideAsymptotes)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(25.0, 5.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the closest point is on the hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);

  // Distance should be less than distance to center (sqrt(25^2+5^2) = 25.5)
  EXPECT_LT(std::sqrt(aResult.MinSquareDistance()), 26.0);
}

//==================================================================================================
// Point out of plane
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, PointAbovePlane)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, 0.0, 5.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Distance should include Z component
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GE(aMinSqDist, 25.0); // At least 5^2
}

TEST_F(ExtremaPC_HyperbolaTest, PointBelowPlane)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(20.0, 0.0, -8.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Closest point is at vertex (20, 0, 0), distance = 8
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 64.0, THE_TOL);
}

//==================================================================================================
// Bound constraint tests
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, BoundsPositive)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, -20.0, 0.0);

  // Create evaluator with positive parameter domain [0, 10]
  ExtremaPC_Hyperbola      anEval(aHyperbola, ExtremaPC::Domain1D{0.0, 10.0});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All parameters should be positive
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, 0.0 - THE_TOL);
  }
}

TEST_F(ExtremaPC_HyperbolaTest, BoundsNegative)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, 20.0, 0.0);

  // Create evaluator with negative parameter domain [-10, 0]
  ExtremaPC_Hyperbola      anEval(aHyperbola, ExtremaPC::Domain1D{-10.0, 0.0});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All parameters should be negative or zero
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_LE(aResult[i].Parameter, 0.0 + THE_TOL);
  }
}

TEST_F(ExtremaPC_HyperbolaTest, NarrowBounds)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, 10.0, 0.0);

  // Create evaluator with narrow domain [0.5, 1.5]
  ExtremaPC_Hyperbola      anEval(aHyperbola, ExtremaPC::Domain1D{0.5, 1.5});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Parameter should be within bounds
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, 0.5 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter, 1.5 + THE_TOL);
  }
}

//==================================================================================================
// Different semi-axes
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, SmallSemiAxes)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0, 1.0);
  gp_Pnt  aPoint(5.0, 2.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnHyper), THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, LargeSemiAxes)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 200.0, 100.0);
  gp_Pnt  aPoint(300.0, 100.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, NearlyRectangular)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 10.0);
  gp_Pnt  aPoint(20.0, 15.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on hyperbola and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnHyper), THE_TOL);
}

//==================================================================================================
// Point on hyperbola
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, PointOnHyperbola_AtVertex)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint = ElCLib::Value(0.0, aHyperbola);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, PointOnHyperbola_Positive)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint = ElCLib::Value(1.5, aHyperbola);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, PointOnHyperbola_Negative)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint = ElCLib::Value(-2.0, aHyperbola);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

//==================================================================================================
// Hyperbola with offset
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, HyperbolaWithOffset)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(120.0, 200.0, 50.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Point is at vertex
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, HyperbolaWithOffset_PointOff)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(150.0, 210.0, 50.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on hyperbola and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnHyper), THE_TOL);
}

//==================================================================================================
// Different orientations
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, HyperbolaInYZPlane)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 20.0, 10.0);
  gp_Pnt  aPoint(5.0, 30.0, 10.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);

  // Distance should include X component (out of plane)
  EXPECT_GE(aResult.MinSquareDistance(), 25.0); // At least 5^2
}

TEST_F(ExtremaPC_HyperbolaTest, HyperbolaInXZPlane)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 20.0, 10.0);
  gp_Pnt  aPoint(30.0, 5.0, 10.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify point is on hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnHyper = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnHyper), 0.0, THE_TOL);

  // Distance should include Y component (out of plane)
  EXPECT_GE(aResult.MinSquareDistance(), 25.0); // At least 5^2
}

//==================================================================================================
// Verification tests
//==================================================================================================

TEST_F(ExtremaPC_HyperbolaTest, VerifyProjectedPoint)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(35.0, 20.0, 0.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify the projected point is on the hyperbola
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = ElCLib::Value(aResult[aMinIdx].Parameter, aHyperbola);
  EXPECT_NEAR(aResult[aMinIdx].Point.X(), aPtOnCurve.X(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Y(), aPtOnCurve.Y(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Z(), aPtOnCurve.Z(), THE_TOL);
}

TEST_F(ExtremaPC_HyperbolaTest, VerifyDistanceConsistency)
{
  gp_Hypr aHyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  gp_Pnt  aPoint(40.0, 25.0, 3.0);

  ExtremaPC_Hyperbola      anEval(aHyperbola);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance matches point distance
  int    aMinIdx     = aResult.MinIndex();
  double aComputedSq = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aComputedSq, THE_TOL);
}
