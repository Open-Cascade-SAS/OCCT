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

#include <ExtremaPC_OffsetCurve.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_OffsetCurve tests.
class ExtremaPC_OffsetCurveTest : public testing::Test
{
protected:
  static constexpr double THE_TOL  = 1.0e-6;
  static constexpr double THE_2PI  = 2.0 * M_PI;
  static constexpr double THE_PI   = M_PI;
};

//==================================================================================================
// Offset of Circle tests
//==================================================================================================

TEST_F(ExtremaPC_OffsetCurveTest, OffsetCircle_PointOutside)
{
  // Create a circle of radius 10 in XY plane
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5 (outer circle of radius 15)
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point outside the offset circle
  gp_Pnt aPoint(30.0, 0.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 30 - 15 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPC_OffsetCurveTest, OffsetCircle_PointInside)
{
  // Create a circle of radius 10 in XY plane
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5 (outer circle of radius 15)
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point inside the offset circle but outside original
  gp_Pnt aPoint(12.0, 0.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 15 - 12 = 3
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 3.0, THE_TOL);
}

TEST_F(ExtremaPC_OffsetCurveTest, OffsetCircle_NegativeOffset)
{
  // Create a circle of radius 10 in XY plane
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with negative offset (inner circle of radius 5)
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, -5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point outside the inner circle
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 20 - 5 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPC_OffsetCurveTest, OffsetCircle_PointOnOffsetCurve)
{
  // Create a circle of radius 10 in XY plane
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5 (outer circle of radius 15)
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point exactly on the offset curve
  gp_Pnt aPoint(15.0, 0.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum distance should be ~0
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

//==================================================================================================
// Offset of Ellipse tests
//==================================================================================================

TEST_F(ExtremaPC_OffsetCurveTest, OffsetEllipse_PointOutside)
{
  // Create an ellipse with major radius 20, minor radius 10
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);

  // Create offset curve with offset 5
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(anEllipse, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point on major axis outside
  gp_Pnt aPoint(40.0, 0.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Offset ellipse at u=0 is at (25, 0, 0)
  // Minimum distance should be 40 - 25 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPC_OffsetCurveTest, OffsetEllipse_PointOnMinorAxis)
{
  // Create an ellipse with major radius 20, minor radius 10
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);

  // Create offset curve with offset 5
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(anEllipse, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point on minor axis
  gp_Pnt aPoint(0.0, 25.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Offset ellipse at u=PI/2 is at (0, 15, 0)
  // Minimum distance should be 25 - 15 = 10
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, THE_TOL);
}

//==================================================================================================
// Offset of Line tests
//==================================================================================================

TEST_F(ExtremaPC_OffsetCurveTest, OffsetLine_PointNear)
{
  // Create a line along X axis
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  // Create offset curve with offset 5 in Y direction
  // For a line in XY plane with reference direction Z, offset is in +Y
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aLine, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve, 0.0, 100.0);

  // Query where the offset curve actually is
  gp_Pnt aOffsetPt = anAdaptor.Value(50.0);

  // Point near the offset line
  gp_Pnt aPoint(50.0, aOffsetPt.Y() + 5.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Distance should be approximately 5
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 5.0, THE_TOL);
}

//==================================================================================================
// Partial range tests
//==================================================================================================

TEST_F(ExtremaPC_OffsetCurveTest, PartialRange_FirstQuadrant)
{
  // Create a circle of radius 10 in XY plane
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point in first quadrant
  gp_Pnt aPoint(20.0, 20.0, 0.0);

  // Create evaluator with domain limited to first quadrant [0, PI/2]
  ExtremaPC_OffsetCurve anEval(anAdaptor, ExtremaPC::Domain1D{0.0, THE_PI / 2.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Check that extremum is in the first quadrant
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, -THE_TOL);
    EXPECT_LE(aResult[i].Parameter, THE_PI / 2.0 + THE_TOL);
  }
}

//==================================================================================================
// 3D offset curve tests (out of plane)
//==================================================================================================

TEST_F(ExtremaPC_OffsetCurveTest, OffsetCircle_PointOutOfPlane)
{
  // Create a circle of radius 10 in XY plane
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point above the plane
  gp_Pnt aPoint(15.0, 0.0, 10.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Closest point on offset curve is (15, 0, 0)
  // Distance = sqrt(0^2 + 0^2 + 10^2) = 10
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, THE_TOL);
}

//==================================================================================================
// Verify extremum condition tests
//==================================================================================================

TEST_F(ExtremaPC_OffsetCurveTest, VerifyExtremumCondition)
{
  // Create a circle of radius 10
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  // Point on the X axis - simple case with known extremum
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify that stored distance matches computed distance
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL);
  }
}

TEST_F(ExtremaPC_OffsetCurveTest, VerifyDistanceConsistency)
{
  // Create a circle of radius 10
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Create offset curve with offset 5
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  gp_Pnt aPoint(18.0, 12.0, 3.0);

  ExtremaPC_OffsetCurve anEval(anAdaptor);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify that stored distance matches computed distance
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);

    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL)
      << "Distance mismatch at extremum " << i;
  }
}

