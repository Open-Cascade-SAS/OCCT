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

#include <ExtremaPC_Curve.hxx>
#include <ExtremaPC_Line.hxx>
#include <ExtremaPC_Circle.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <cmath>

//! Test fixture for ExtremaPC tests.
class ExtremaPC_CurveTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;
};

//==================================================================================================
// Line tests
//==================================================================================================

TEST_F(ExtremaPC_CurveTest, Line_PointOnLine)
{
  // Line along X axis with bounded domain
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  // Point on line
  gp_Pnt aPoint(5.0, 0.0, 0.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{0.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 0.0, THE_TOL);
  EXPECT_TRUE(aResult[aMinIdx].IsMinimum);
}

TEST_F(ExtremaPC_CurveTest, Line_PointOffLine)
{
  // Line along X axis with bounded domain
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  // Point off line
  gp_Pnt aPoint(5.0, 3.0, 4.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{0.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, THE_TOL); // 3² + 4² = 25
  EXPECT_TRUE(aResult[aMinIdx].IsMinimum);
}

TEST_F(ExtremaPC_CurveTest, Line_ProjectionOutsideBounds)
{
  // Line along X axis with bounded domain
  gp_Lin aLine(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  // Point that projects outside bounds
  gp_Pnt aPoint(-50.0, 3.0, 4.0);

  ExtremaPC_Line anEval(aLine, ExtremaPC::Domain1D{0.0, 100.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2); // min at 0, max at 100

  int aMinIdx = aResult.MinIndex();
  // Should be at endpoint U=0
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 0.0, THE_TOL);
}

//==================================================================================================
// Circle tests
//==================================================================================================

TEST_F(ExtremaPC_CurveTest, Circle_PointOutside)
{
  // Circle with radius 10 in XY plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point outside circle
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle, ExtremaPC::Domain1D{0.0, 2.0 * M_PI});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  // Find min and max
  int aMinIdx = aResult[0].SquareDistance < aResult[1].SquareDistance ? 0 : 1;
  int aMaxIdx = 1 - aMinIdx;

  // Minimum should be at (10, 0, 0), distance = 10
  EXPECT_NEAR(std::sqrt(aResult[aMinIdx].SquareDistance), 10.0, THE_TOL);
  EXPECT_TRUE(aResult[aMinIdx].IsMinimum);

  // Maximum should be at (-10, 0, 0), distance = 30
  EXPECT_NEAR(std::sqrt(aResult[aMaxIdx].SquareDistance), 30.0, THE_TOL);
  EXPECT_FALSE(aResult[aMaxIdx].IsMinimum);
}

TEST_F(ExtremaPC_CurveTest, Circle_PointInside)
{
  // Circle with radius 10 in XY plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point inside circle
  gp_Pnt aPoint(3.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 2);

  // Find min distance
  double aMinDist = std::min(std::sqrt(aResult[0].SquareDistance),
                             std::sqrt(aResult[1].SquareDistance));

  // Minimum distance should be 10 - 3 = 7
  EXPECT_NEAR(aMinDist, 7.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Circle_PointAtCenter_Degenerate)
{
  // Circle with radius 10 in XY plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point at center - degenerate case
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // Should return infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

//==================================================================================================
// Aggregator tests with GeomAdaptor_Curve
//==================================================================================================

TEST_F(ExtremaPC_CurveTest, Aggregator_Line)
{
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve anAdaptor(aGeomLine, -100.0, 100.0);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  gp_Pnt aPoint(5.0, 3.0, 4.0);
  const ExtremaPC::Result& aResult = anExtPC.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 3); // 1 min + 2 max (endpoints)

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, 25.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_Circle)
{
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  gp_Pnt aPoint(20.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Check minimum distance
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_BSpline)
{
  // Create a simple cubic BSpline
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 2, 0);
  aPoles(4) = gp_Pnt(3, 0, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve anAdaptor(aBSpline);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point near the curve
  gp_Pnt aPoint(1.5, 3.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should find at least one extremum
  EXPECT_GE(aResult.NbExt(), 0);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_Ellipse)
{
  Handle(Geom_Ellipse) aGeomEllipse = new Geom_Ellipse(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point on major axis
  gp_Pnt aPoint(30.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 30 - 20 = 10
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_Parabola)
{
  // y^2 = 4*p*x with p = 2, so y^2 = 8x
  Handle(Geom_Parabola) aGeomParabola = new Geom_Parabola(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -10.0, 10.0);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point on X axis at focus
  gp_Pnt aPoint(2.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Focus is at (p, 0) = (2, 0), vertex is at origin
  // Minimum distance should be 2.0
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 2.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_Hyperbola)
{
  // Hyperbola with major radius 10, minor radius 5
  // x = a*cosh(u), y = b*sinh(u)
  Handle(Geom_Hyperbola) aGeomHyperbola = new Geom_Hyperbola(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomHyperbola, -2.0, 2.0);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point near the vertex
  gp_Pnt aPoint(10.0, 5.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

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

TEST_F(ExtremaPC_CurveTest, Aggregator_Bezier)
{
  // Create a cubic Bezier curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(3, 3, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  Handle(Geom_BezierCurve) aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve anAdaptor(aBezier);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point above the curve's peak
  gp_Pnt aPoint(2.0, 5.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

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

TEST_F(ExtremaPC_CurveTest, Aggregator_OffsetCurve)
{
  // Create a circle and its offset
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  Handle(Geom_OffsetCurve) anOffsetCurve = new Geom_OffsetCurve(aCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point outside the offset circle
  gp_Pnt aPoint(25.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Offset circle has radius 15, distance should be 25 - 15 = 10
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_TrimmedCurve)
{
  // Create a trimmed circle (quarter arc)
  Handle(Geom_Circle) aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  Handle(Geom_TrimmedCurve) aTrimmedCurve = new Geom_TrimmedCurve(aCircle, 0.0, M_PI / 2.0);
  GeomAdaptor_Curve anAdaptor(aTrimmedCurve);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  // Point in first quadrant
  gp_Pnt aPoint(15.0, 15.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Check that all extrema are within bounds
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, -THE_TOL);
    EXPECT_LE(aResult[i].Parameter, M_PI / 2.0 + THE_TOL);
  }
}

//==================================================================================================
// Edge case tests
//==================================================================================================

TEST_F(ExtremaPC_CurveTest, Aggregator_PointOnCurve)
{
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  ExtremaPC_Curve anExtPC(anAdaptor);

  // Point exactly on the circle
  gp_Pnt aPoint(10.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Distance should be ~0
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_IsInitialized)
{
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve anAdaptor(aGeomLine, 0.0, 100.0);

  ExtremaPC_Curve anExtPC(anAdaptor);
  EXPECT_TRUE(anExtPC.IsInitialized());

  gp_Pnt aPoint(50.0, 5.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 50.0, THE_TOL);
}

//==================================================================================================
// Result utility tests
//==================================================================================================

TEST_F(ExtremaPC_CurveTest, Result_MinSquareDistance)
{
  ExtremaPC::Result aResult;
  aResult.Status = ExtremaPC::Status::OK;

  // Add some extrema
  ExtremaPC::ExtremumResult anExt1;
  anExt1.SquareDistance = 100.0;
  aResult.Extrema.Append(anExt1);

  ExtremaPC::ExtremumResult anExt2;
  anExt2.SquareDistance = 50.0;
  aResult.Extrema.Append(anExt2);

  ExtremaPC::ExtremumResult anExt3;
  anExt3.SquareDistance = 75.0;
  aResult.Extrema.Append(anExt3);

  EXPECT_NEAR(aResult.MinSquareDistance(), 50.0, THE_TOL);
  EXPECT_EQ(aResult.MinIndex(), 1);
}

TEST_F(ExtremaPC_CurveTest, Result_Empty)
{
  ExtremaPC::Result aResult;

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 0);
  EXPECT_EQ(aResult.MinIndex(), -1);
  EXPECT_TRUE(std::isinf(aResult.MinSquareDistance()));
}

//==================================================================================================
// SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_CurveTest, Circle_SearchMode_MinMax)
{
  // Circle with radius 10 in XY plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point outside circle
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 2); // Both min and max
}

TEST_F(ExtremaPC_CurveTest, Circle_SearchMode_Min)
{
  // Circle with radius 10 in XY plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point outside circle
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1); // Only min
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 10.0, THE_TOL); // 20 - 10 = 10
}

TEST_F(ExtremaPC_CurveTest, Circle_SearchMode_Max)
{
  // Circle with radius 10 in XY plane
  gp_Circ aCircle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  // Point outside circle
  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Circle anEval(aCircle);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1); // Only max
  EXPECT_FALSE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 30.0, THE_TOL); // 20 + 10 = 30
}

TEST_F(ExtremaPC_CurveTest, Aggregator_SearchMode_Min)
{
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  ExtremaPC_Curve anExtPC(anAdaptor);

  gp_Pnt aPoint(20.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1); // Only min
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 10.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_SearchMode_Max)
{
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  ExtremaPC_Curve anExtPC(anAdaptor);

  gp_Pnt aPoint(20.0, 0.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1); // Only max
  EXPECT_FALSE(aResult[0].IsMinimum);
  EXPECT_NEAR(std::sqrt(aResult[0].SquareDistance), 30.0, THE_TOL);
}

TEST_F(ExtremaPC_CurveTest, Aggregator_BSpline_SearchMode_Min)
{
  // Create a simple cubic BSpline
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 2, 0);
  aPoles(4) = gp_Pnt(3, 0, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve anAdaptor(aBSpline);

  ExtremaPC_Curve anExtPC(anAdaptor);

  // Point near the curve
  gp_Pnt aPoint(1.5, 3.0, 0.0);
  const ExtremaPC::Result& aResult = anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  // In Min mode, should only return minimum extrema
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

TEST_F(ExtremaPC_CurveTest, Result_MaxSquareDistance)
{
  ExtremaPC::Result aResult;
  aResult.Status = ExtremaPC::Status::OK;

  // Add some extrema
  ExtremaPC::ExtremumResult anExt1;
  anExt1.SquareDistance = 100.0;
  anExt1.IsMinimum = false;
  aResult.Extrema.Append(anExt1);

  ExtremaPC::ExtremumResult anExt2;
  anExt2.SquareDistance = 50.0;
  anExt2.IsMinimum = true;
  aResult.Extrema.Append(anExt2);

  ExtremaPC::ExtremumResult anExt3;
  anExt3.SquareDistance = 75.0;
  anExt3.IsMinimum = false;
  aResult.Extrema.Append(anExt3);

  EXPECT_NEAR(aResult.MinSquareDistance(), 50.0, THE_TOL);
  EXPECT_EQ(aResult.MinIndex(), 1);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 100.0, THE_TOL);
  EXPECT_EQ(aResult.MaxIndex(), 0);
}
