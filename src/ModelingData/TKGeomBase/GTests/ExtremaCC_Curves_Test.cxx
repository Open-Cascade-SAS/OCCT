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

#include <ExtremaCC_Curves.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <cmath>

//! Test fixture for ExtremaCC_Curves tests.
class ExtremaCC_CurvesTest : public testing::Test
{
protected:
  static constexpr double THE_TOL    = 1.0e-7;
  static constexpr double THE_PI     = 3.14159265358979323846;
  static constexpr double THE_TWO_PI = 2.0 * THE_PI;
};

//==================================================================================================
// Line-Line tests through aggregator
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, LineLine_Intersecting)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(5, 0, 0), gp_Dir(0, 1, 0));

  GeomAdaptor_Curve aCurve1(aLine1, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aLine2, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCC_CurvesTest, LineLine_Parallel)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(0, 3, 4), gp_Dir(1, 0, 0));

  GeomAdaptor_Curve aCurve1(aLine1, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aLine2, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST_F(ExtremaCC_CurvesTest, LineLine_Skew)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(0, 0, 10), gp_Dir(0, 1, 0));

  GeomAdaptor_Curve aCurve1(aLine1, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aLine2, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL);
}

//==================================================================================================
// Circle-Line tests through aggregator
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, CircleLine_Intersecting)
{
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  Handle(Geom_Line)   aLine   = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  GeomAdaptor_Curve aCurve1(aCircle);
  GeomAdaptor_Curve aCurve2(aLine, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCC_CurvesTest, CircleLine_Parallel)
{
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  Handle(Geom_Line)   aLine   = new Geom_Line(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0));

  GeomAdaptor_Curve aCurve1(aCircle);
  GeomAdaptor_Curve aCurve2(aLine, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.MinSquareDistance(), 25.0, THE_TOL);
}

TEST_F(ExtremaCC_CurvesTest, LineCircle_SwappedOrder)
{
  // Test that Line-Circle gives same result as Circle-Line (swapped internally)
  Handle(Geom_Line)   aLine   = new Geom_Line(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0));
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  GeomAdaptor_Curve aCurve1(aLine, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aCircle);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_TRUE(anExtrema.IsSwapped()); // Should be swapped
  EXPECT_NEAR(aResult.MinSquareDistance(), 25.0, THE_TOL);
}

//==================================================================================================
// Circle-Circle tests through aggregator
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, CircleCircle_Coplanar)
{
  Handle(Geom_Circle) aCircle1 = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  Handle(Geom_Circle) aCircle2 = new Geom_Circle(gp_Ax2(gp_Pnt(30, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  GeomAdaptor_Curve aCurve1(aCircle1);
  GeomAdaptor_Curve aCurve2(aCircle2);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, THE_TOL);  // 10^2
  EXPECT_NEAR(aResult.MaxSquareDistance(), 2500.0, THE_TOL); // 50^2
}

//==================================================================================================
// Ellipse-Line tests through aggregator
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, EllipseLine_Intersecting)
{
  Handle(Geom_Ellipse) anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  GeomAdaptor_Curve aCurve1(anEllipse);
  GeomAdaptor_Curve aCurve2(aLine, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCC_CurvesTest, LineEllipse_SwappedOrder)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Ellipse) anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);

  GeomAdaptor_Curve aCurve1(aLine, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(anEllipse);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_TRUE(anExtrema.IsSwapped());
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Numerical curve tests (BSpline, Bezier)
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, LineBSpline_Simple)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  // Create a simple BSpline curve
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 10, 0);
  aPoles(2) = gp_Pnt(10, 10, 0);
  aPoles(3) = gp_Pnt(20, 10, 0);
  aPoles(4) = gp_Pnt(30, 10, 0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);

  GeomAdaptor_Curve aCurve1(aLine, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aBSpline);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // BSpline is a straight line at Y=10, so minimum distance is 10
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, 1.0); // Allow some tolerance for numerical
}

TEST_F(ExtremaCC_CurvesTest, BezierBezier_Simple)
{
  // Create two Bezier curves
  TColgp_Array1OfPnt aPoles1(1, 3);
  aPoles1(1) = gp_Pnt(0, 0, 0);
  aPoles1(2) = gp_Pnt(5, 10, 0);
  aPoles1(3) = gp_Pnt(10, 0, 0);

  TColgp_Array1OfPnt aPoles2(1, 3);
  aPoles2(1) = gp_Pnt(0, 20, 0);
  aPoles2(2) = gp_Pnt(5, 10, 0);
  aPoles2(3) = gp_Pnt(10, 20, 0);

  Handle(Geom_BezierCurve) aBezier1 = new Geom_BezierCurve(aPoles1);
  Handle(Geom_BezierCurve) aBezier2 = new Geom_BezierCurve(aPoles2);

  GeomAdaptor_Curve aCurve1(aBezier1);
  GeomAdaptor_Curve aCurve2(aBezier2);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Both curves pass through (5, 10, 0), so minimum distance is 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, 1.0);
}

TEST_F(ExtremaCC_CurvesTest, CircleBezier_Mixed)
{
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);

  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(20, 0, 0);
  aPoles(2) = gp_Pnt(25, 5, 0);
  aPoles(3) = gp_Pnt(30, 0, 0);

  Handle(Geom_BezierCurve) aBezier = new Geom_BezierCurve(aPoles);

  GeomAdaptor_Curve aCurve1(aCircle);
  GeomAdaptor_Curve aCurve2(aBezier);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Closest point on circle is (10,0,0), on Bezier is (20,0,0)
  // Distance = 10
  EXPECT_NEAR(aResult.MinSquareDistance(), 100.0, 10.0); // Allow tolerance for numerical curve
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, DomainConstraints_BothCurves)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(50, 0, 0), gp_Dir(0, 1, 0));

  // Line1 domain [0, 10], Line2 domain [-5, 5]
  // Intersection at (50, 0) is outside Line1's domain
  GeomAdaptor_Curve aCurve1(aLine1, 0.0, 10.0);
  GeomAdaptor_Curve aCurve2(aLine2, -5.0, 5.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Closest points: Line1 at (10,0,0), Line2 at (50,0,0)
  // Distance = 40
  EXPECT_NEAR(aResult.MinSquareDistance(), 1600.0, THE_TOL);
}

//==================================================================================================
// Point verification tests
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, VerifyClosestPoints)
{
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  Handle(Geom_Line)   aLine   = new Geom_Line(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0));

  GeomAdaptor_Curve aCurve1(aCircle);
  GeomAdaptor_Curve aCurve2(aLine, -100.0, 100.0);

  ExtremaCC_Curves anExtrema(aCurve1, aCurve2);

  const ExtremaCC::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Verify all extrema have consistent distances
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aComputedDist = aResult[i].Point1.SquareDistance(aResult[i].Point2);
    EXPECT_NEAR(aResult[i].SquareDistance, aComputedDist, 1.0e-6);
  }
}

//==================================================================================================
// Move semantics tests
//==================================================================================================

TEST_F(ExtremaCC_CurvesTest, MoveConstructor)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(0, 5, 0), gp_Dir(1, 0, 0));

  GeomAdaptor_Curve aCurve1(aLine1, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aLine2, -100.0, 100.0);

  ExtremaCC_Curves anExtrema1(aCurve1, aCurve2);
  ExtremaCC_Curves anExtrema2(std::move(anExtrema1));

  const ExtremaCC::Result& aResult = anExtrema2.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
}

TEST_F(ExtremaCC_CurvesTest, MoveAssignment)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(0, 5, 0), gp_Dir(1, 0, 0));
  Handle(Geom_Line) aLine3 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));

  GeomAdaptor_Curve aCurve1(aLine1, -100.0, 100.0);
  GeomAdaptor_Curve aCurve2(aLine2, -100.0, 100.0);
  GeomAdaptor_Curve aCurve3(aLine3, -100.0, 100.0);

  ExtremaCC_Curves anExtrema1(aCurve1, aCurve2);
  ExtremaCC_Curves anExtrema2(aCurve1, aCurve3);

  anExtrema2 = std::move(anExtrema1);

  const ExtremaCC::Result& aResult = anExtrema2.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, ExtremaCC::Status::InfiniteSolutions);
}

