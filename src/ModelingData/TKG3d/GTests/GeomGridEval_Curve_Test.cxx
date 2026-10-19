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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <GeomEval_RepCurveDesc.hxx>
#include <Geom_BezierSurface.hxx>
#include <NCollection_Array2.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomGridEval_BSplineCurve.hxx>
#include <GeomGridEval_Circle.hxx>
#include <GeomGridEval_Curve.hxx>
#include <GeomGridEval_Line.hxx>
#include <GeomGridEval_OtherCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <array>

namespace
{
const double THE_TOLERANCE = 1e-10;

//! Helper function to create uniform parameters
NCollection_Array1<double> CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  NCollection_Array1<double> aParams(1, theNbPoints);
  const double               aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}

//! Helper function to create a simple B-spline curve
occ::handle<Geom_BSplineCurve> CreateSimpleBSpline()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
}
} // namespace

//=================================================================================================
// Tests for GeomGridEval_Line
//=================================================================================================

TEST(GeomGridEval_LineTest, BasicEvaluation)
{
  // Create a Geom_Line along X axis starting at origin
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  GeomGridEval_Line anEval(aGeomLine);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  // Verify points
  for (int i = 1; i <= 11; ++i)
  {
    const double t         = aParams.Value(i);
    const gp_Pnt aExpected = gp_Pnt(t, 0, 0);
    EXPECT_NEAR(aGrid.Value(i).X(), aExpected.X(), THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).Y(), aExpected.Y(), THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).Z(), aExpected.Z(), THE_TOLERANCE);
  }
}

TEST(GeomGridEval_LineTest, NonOriginLine)
{
  // Create a line at (1, 2, 3) along direction (1, 1, 1)/sqrt(3)
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(1, 2, 3), gp_Dir(1, 1, 1));

  GeomGridEval_Line anEval(aGeomLine);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify first and last point
  EXPECT_NEAR(aGrid.Value(1).Distance(gp_Pnt(1, 2, 3)), 0.0, THE_TOLERANCE);

  // Line direction is normalized, so t=5 moves 5 units along normalized direction
  const double d     = 5.0 / std::sqrt(3.0);
  gp_Pnt       aLast = gp_Pnt(1 + d, 2 + d, 3 + d);
  EXPECT_NEAR(aGrid.Value(6).Distance(aLast), 0.0, THE_TOLERANCE);
}

//=================================================================================================
// Tests for GeomGridEval_Circle
//=================================================================================================

TEST(GeomGridEval_CircleTest, BasicEvaluation)
{
  // Circle in XY plane, radius 2, center at origin
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  GeomGridEval_Circle anEval(aGeomCircle);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  // Test at 0, PI/2, PI, 3PI/2, 2PI
  NCollection_Array1<double> aParams(1, 5);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);
  aParams.SetValue(5, 2 * M_PI);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // u=0: (2, 0, 0)
  EXPECT_NEAR(aGrid.Value(1).X(), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(1).Y(), 0.0, THE_TOLERANCE);

  // u=PI/2: (0, 2, 0)
  EXPECT_NEAR(aGrid.Value(2).X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2).Y(), 2.0, THE_TOLERANCE);

  // u=PI: (-2, 0, 0)
  EXPECT_NEAR(aGrid.Value(3).X(), -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(3).Y(), 0.0, THE_TOLERANCE);

  // u=3PI/2: (0, -2, 0)
  EXPECT_NEAR(aGrid.Value(4).X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(4).Y(), -2.0, THE_TOLERANCE);

  // u=2PI: back to (2, 0, 0)
  EXPECT_NEAR(aGrid.Value(5).X(), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(5).Y(), 0.0, THE_TOLERANCE);
}

TEST(GeomGridEval_CircleTest, NonStandardCircle)
{
  // Circle in YZ plane, radius 3, center at (1, 0, 0)
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(1, 0, 0), gp_Dir(1, 0, 0)), 3.0);

  GeomGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // All points should be at distance 3 from center (1, 0, 0)
  const gp_Pnt aCenter(1, 0, 0);
  for (int i = 1; i <= aGrid.Length(); ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).Distance(aCenter), 3.0, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).X(), 1.0, THE_TOLERANCE); // All points have X=1
  }
}

//=================================================================================================
// Tests for GeomGridEval_BSplineCurve
//=================================================================================================

TEST(GeomGridEval_BSplineCurveTest, BasicEvaluation)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();

  GeomGridEval_BSplineCurve anEval(aCurve);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, EndpointsMatch)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();

  GeomGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams(1, 2);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, 1.0);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // First point should match first pole
  EXPECT_NEAR(aGrid.Value(1).Distance(gp_Pnt(0, 0, 0)), 0.0, THE_TOLERANCE);
  // Last point should match last pole
  EXPECT_NEAR(aGrid.Value(2).Distance(gp_Pnt(4, 0, 0)), 0.0, THE_TOLERANCE);
}

//=================================================================================================
// Tests for GeomGridEval_OtherCurve (fallback)
//=================================================================================================

TEST(GeomGridEval_OtherCurveTest, EllipseFallback)
{
  // Ellipse is not directly supported, so it should use OtherCurve fallback
  occ::handle<Geom_Ellipse> anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  GeomAdaptor_Curve anAdaptor(anEllipse);

  GeomGridEval_OtherCurve anEval(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aExpected = anEllipse->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//=================================================================================================
// Tests for GeomGridEval_Curve (unified dispatcher)
//=================================================================================================

TEST(GeomGridEval_CurveTest, LineDispatch)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aGeomLine);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aGeomLine->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, CircleDispatch)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aExpected = aGeomCircle->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, BSplineDispatch)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomAdaptor_Curve              anAdaptor(aCurve);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, EllipseDispatch)
{
  occ::handle<Geom_Ellipse> anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  GeomAdaptor_Curve anAdaptor(anEllipse);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Ellipse);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 13);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt aExpected = anEllipse->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, HyperbolaDispatch)
{
  occ::handle<Geom_Hyperbola> aHypr =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  GeomAdaptor_Curve anAdaptor(aHypr);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Hyperbola);

  NCollection_Array1<double> aParams = CreateUniformParams(-2.0, 2.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aHypr->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, ParabolaDispatch)
{
  occ::handle<Geom_Parabola> aParab =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  GeomAdaptor_Curve anAdaptor(aParab);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Parabola);

  NCollection_Array1<double> aParams = CreateUniformParams(-2.0, 2.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aParab->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, BezierCurveDispatch)
{
  // Bezier curve is not optimized, should use fallback
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));
  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_BezierCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aBezier->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, OffsetCurveFallbackDispatch)
{
  // Offset curve is not optimized, should use fallback
  occ::handle<Geom_Line>        aLine    = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_OffsetCurve> anOffset = new Geom_OffsetCurve(aLine, 1.0, gp::DZ());
  GeomAdaptor_Curve             anAdaptor(anOffset);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_OffsetCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 6; ++i)
  {
    gp_Pnt aExpected = anOffset->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, DirectHandleInit)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  GeomGridEval_Curve anEval(aGeomLine);
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);
  EXPECT_NEAR(aGrid.Value(1).X(), 0.0, THE_TOLERANCE);
}

TEST(GeomGridEval_CurveTest, EmptyParams)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aGeomLine);

  GeomGridEval_Curve anEval(anAdaptor);

  // EvaluateGrid with empty params should return empty
  NCollection_Array1<double> aEmptyParams;
  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aEmptyParams);
  EXPECT_TRUE(aGrid.IsEmpty());
}

//=================================================================================================
// Additional tests for B-spline curves (from BSplCLib_GridEvaluator_Test)
//=================================================================================================

TEST(GeomGridEval_BSplineCurveTest, RationalBSpline)
{
  // Create a rational B-spline (NURBS circle approximation)
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  NCollection_Array1<double> aWeights(1, 4);

  aPoles.SetValue(1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 1, 0));
  aPoles.SetValue(3, gp_Pnt(0, 1, 0));
  aPoles.SetValue(4, gp_Pnt(-1, 1, 0));

  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0 / std::sqrt(2.0));
  aWeights.SetValue(3, 1.0);
  aWeights.SetValue(4, 1.0 / std::sqrt(2.0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 3);

  GeomGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 21);

  // Verify against direct evaluation
  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, MultiSpanBSpline)
{
  // Create a B-spline with multiple spans (degree 3, 6 control points, 1 internal knot)
  // Formula: n_poles = sum(multiplicities) - degree - 1 = (4 + 2 + 4) - 3 - 1 = 6
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(2, 2, 0));
  aPoles.SetValue(4, gp_Pnt(3, 0, 0));
  aPoles.SetValue(5, gp_Pnt(4, -1, 0));
  aPoles.SetValue(6, gp_Pnt(5, 0, 0));

  NCollection_Array1<double> aKnots(1, 3);
  NCollection_Array1<int>    aMults(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 2);
  aMults.SetValue(3, 4);

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);

  GeomGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 31);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation - should match exactly
  for (int i = 1; i <= 31; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, NearRepeatedKnotUsesActualSpan)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 7);
  for (int anIndex = aPoles.Lower(); anIndex <= aPoles.Upper(); ++anIndex)
  {
    const double aValue = static_cast<double>(anIndex);
    aPoles(anIndex)     = gp_Pnt(aValue, std::sin(aValue), aValue * aValue);
  }

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;
  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 4;
  aMults(2) = 3;
  aMults(3) = 4;

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams(1, 8);
  aParams(1) = 0.45;
  aParams(2) = 0.48;
  aParams(3) = 0.49;
  aParams(4) = std::nextafter(0.5, 0.0);
  aParams(5) = 0.5;
  aParams(6) = 0.51;
  aParams(7) = 0.52;
  aParams(8) = 0.55;

  const NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);
  for (int anIndex = aParams.Lower(); anIndex <= aParams.Upper(); ++anIndex)
  {
    gp_Pnt aPoint;
    gp_Vec aD1, aD2, aD3;
    aCurve->D3(aParams(anIndex), aPoint, aD1, aD2, aD3);

    const GeomGridEval::CurveD3& aGridValue = aGrid.Value(anIndex);
    EXPECT_TRUE(aGridValue.Point.IsEqual(aPoint, THE_TOLERANCE));
    EXPECT_TRUE(aGridValue.D1.IsEqual(aD1, THE_TOLERANCE, THE_TOLERANCE));
    EXPECT_TRUE(aGridValue.D2.IsEqual(aD2, THE_TOLERANCE, THE_TOLERANCE));
    EXPECT_TRUE(aGridValue.D3.IsEqual(aD3, THE_TOLERANCE, THE_TOLERANCE));
  }
}

TEST(GeomGridEval_BSplineCurveTest, HighDegree)
{
  // Create a higher degree B-spline (degree 5)
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 3, 0));
  aPoles.SetValue(3, gp_Pnt(2, 1, 0));
  aPoles.SetValue(4, gp_Pnt(3, 4, 0));
  aPoles.SetValue(5, gp_Pnt(4, 2, 0));
  aPoles.SetValue(6, gp_Pnt(5, 0, 0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 6);
  aMults.SetValue(2, 6);

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 5);

  GeomGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 51);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 51; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//=================================================================================================
// Tests for Curve Derivative Evaluation (D1, D2, D3)
//=================================================================================================

TEST(GeomGridEval_LineTest, DerivativeD1)
{
  // Line along (1, 2, 3) normalized direction
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 2, 3));
  GeomGridEval_Line      anEval(aGeomLine);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);
  EXPECT_EQ(aGrid.Size(), 6);

  // For a line, D1 is constant (the direction vector)
  gp_Dir aDir(1, 2, 3);
  for (int i = 1; i <= 6; ++i)
  {
    // Verify D1 is the direction
    EXPECT_NEAR(aGrid.Value(i).D1.X(), aDir.X(), THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).D1.Y(), aDir.Y(), THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).D1.Z(), aDir.Z(), THE_TOLERANCE);
  }
}

TEST(GeomGridEval_LineTest, DerivativeD2D3)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomGridEval_Line      anEval(aGeomLine);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<GeomGridEval::CurveD2> aGridD2 = anEval.EvaluateGridD2(aParams);
  NCollection_Array1<GeomGridEval::CurveD3> aGridD3 = anEval.EvaluateGridD3(aParams);

  // For a line, D2 and D3 are zero
  for (int i = 1; i <= 6; ++i)
  {
    EXPECT_NEAR(aGridD2.Value(i).D2.Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGridD3.Value(i).D2.Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGridD3.Value(i).D3.Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CircleTest, DerivativeD1)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams(1, 5);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);
  aParams.SetValue(5, 2 * M_PI);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  // Verify D1 against direct evaluation
  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aGeomCircle->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CircleTest, DerivativeD2)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  // Verify D2 against direct evaluation
  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aGeomCircle->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeD1)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aCurve->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeD2)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aCurve->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeD3)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aCurve->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, UnifiedDerivativeD1)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  GeomGridEval_Curve anEval(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aGeomCircle->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, UnifiedDerivativeD2)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomAdaptor_Curve              anAdaptor(aCurve);

  GeomGridEval_Curve anEval(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aCurve->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//=================================================================================================
// Tests for Curve Third Derivative Evaluation (D3)
//=================================================================================================

TEST(GeomGridEval_CircleTest, DerivativeD3)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  // Verify D3 against direct evaluation
  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aGeomCircle->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_OffsetCurveTest, DerivativeD3)
{
  // Offset curve from a circle
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_OffsetCurve> anOffset = new Geom_OffsetCurve(aCircle, 0.5, gp::DZ());
  GeomAdaptor_Curve             anAdaptor(anOffset);

  GeomGridEval_OtherCurve anEval(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  // Verify D3 against adaptor evaluation
  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    anAdaptor.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, OffsetCurveDerivativeD3)
{
  // Offset curve via unified dispatcher
  occ::handle<Geom_Line>        aLine    = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_OffsetCurve> anOffset = new Geom_OffsetCurve(aLine, 1.0, gp::DZ());
  GeomAdaptor_Curve             anAdaptor(anOffset);

  GeomGridEval_Curve anEval(anAdaptor);
  EXPECT_EQ(anEval.GetType(), GeomAbs_OffsetCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  // Verify D3 against adaptor evaluation
  for (int i = 1; i <= 6; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    anAdaptor.D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_CurveTest, UnifiedDerivativeD3)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomAdaptor_Curve              anAdaptor(aCurve);

  GeomGridEval_Curve anEval(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aCurve->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//=================================================================================================
// Tests for BSpline Curve DN (Arbitrary Order Derivative)
//=================================================================================================

TEST(GeomGridEval_BSplineCurveTest, DerivativeDN_Order1)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 1);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Vec aExpected = aCurve->DN(aParams.Value(i), 1);
    EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeDN_Order2)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 2);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Vec aExpected = aCurve->DN(aParams.Value(i), 2);
    EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeDN_Order3)
{
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 3);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Vec aExpected = aCurve->DN(aParams.Value(i), 3);
    EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeDN_BeyondDegree)
{
  // Cubic B-spline (degree 3), 4th derivative should be zero
  occ::handle<Geom_BSplineCurve> aCurve = CreateSimpleBSpline();
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 4);

  for (int i = 1; i <= 11; ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeDN_RationalCurve)
{
  // Create a rational B-spline (NURBS)
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  NCollection_Array1<double> aWeights(1, 4);

  aPoles.SetValue(1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 1, 0));
  aPoles.SetValue(3, gp_Pnt(0, 1, 0));
  aPoles.SetValue(4, gp_Pnt(-1, 1, 0));

  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0 / std::sqrt(2.0));
  aWeights.SetValue(3, 1.0);
  aWeights.SetValue(4, 1.0 / std::sqrt(2.0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 3);
  GeomGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  // Test DN for orders 1, 2, 3
  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);

    for (int i = 1; i <= 21; ++i)
    {
      gp_Vec aExpected = aCurve->DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

TEST(GeomGridEval_BSplineCurveTest, DerivativeDN_MultiSpan)
{
  // Create a multi-span B-spline (degree 3, with internal knot)
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(2, 2, 0));
  aPoles.SetValue(4, gp_Pnt(3, 0, 0));
  aPoles.SetValue(5, gp_Pnt(4, -1, 0));
  aPoles.SetValue(6, gp_Pnt(5, 0, 0));

  NCollection_Array1<double> aKnots(1, 3);
  NCollection_Array1<int>    aMults(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 2);
  aMults.SetValue(3, 4);

  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 31);

  // Test DN for orders 1, 2, 3
  for (int aOrder = 1; aOrder <= 3; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);

    for (int i = 1; i <= 31; ++i)
    {
      gp_Vec aExpected = aCurve->DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}

//==================================================================================================

TEST(GeomGridEval_CurveTest, NativeBSplineTrimsPreserveDerivativeSidesAndSampleOrder)
{
  NCollection_Array1<gp_Pnt> aPoles(5);
  aPoles.ChangeAt(0) = gp_Pnt(0, 0, 0);
  aPoles.ChangeAt(1) = gp_Pnt(0.25, 0, 0);
  aPoles.ChangeAt(2) = gp_Pnt(0.5, 0, 0);
  aPoles.ChangeAt(3) = gp_Pnt(0.75, 1, 0);
  aPoles.ChangeAt(4) = gp_Pnt(1, 3, 0);
  NCollection_Array1<double> aKnots(3);
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 0.5;
  aKnots.ChangeAt(2) = 1;
  NCollection_Array1<int> aMultiplicities(3);
  aMultiplicities.ChangeAt(0) = 3;
  aMultiplicities.ChangeAt(1) = 2;
  aMultiplicities.ChangeAt(2) = 3;
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
  for (const auto& aBounds :
       {std::array<double, 2>{0, 0.5}, std::array<double, 2>{0.5, 1}, std::array<double, 2>{0, 1}})
  {
    const GeomAdaptor_Curve  anAdaptor(aCurve, aBounds[0], aBounds[1]);
    const GeomGridEval_Curve aUnified(anAdaptor);

    const GeomGridEval_Curve aWrapped(
      occ::handle<Geom_Curve>(new Geom_TrimmedCurve(aCurve, aBounds[0], aBounds[1])));
    NCollection_Array1<double> aParameters(-2, 2);
    aParameters.ChangeAt(0) = aBounds[1];
    aParameters.ChangeAt(1) = aBounds[0] + (aBounds[1] - aBounds[0]) * 0.25;
    aParameters.ChangeAt(2) = aBounds[0];
    aParameters.ChangeAt(3) = aBounds[0] + (aBounds[1] - aBounds[0]) * 0.75;
    aParameters.ChangeAt(4) = aBounds[1];

    const NCollection_Array1<GeomGridEval::CurveD3> aUnifiedValues =
      aUnified.EvaluateGridD3(aParameters);
    const NCollection_Array1<GeomGridEval::CurveD1> aWrappedValues =
      aWrapped.EvaluateGridD1(aParameters);
    gp_Trsf aPlacement;
    aPlacement.SetScale(gp_Pnt(2.0, 1.0, 3.0), -2.0);
    const GeomAdaptor_TransformedCurve aLocated(aCurve, aBounds[0], aBounds[1], aPlacement);
    const GeomGridEval_Curve           aLocatedEvaluator(aLocated);
    const NCollection_Array1<GeomGridEval::CurveD3> aLocatedValues =
      aLocatedEvaluator.EvaluateGridD3(aParameters);
    ASSERT_EQ(aUnifiedValues.Size(), aParameters.Size());
    ASSERT_EQ(aWrappedValues.Size(), aParameters.Size());
    ASSERT_EQ(aLocatedValues.Size(), aParameters.Size());
    for (size_t anIndex = 0; anIndex < aParameters.Size(); ++anIndex)
    {
      const Geom_Curve::ResD3 anExpected       = anAdaptor.EvalD3(aParameters.At(anIndex));
      const Geom_Curve::ResD3 aLocatedExpected = aLocated.EvalD3(aParameters.At(anIndex));
      const auto&             aLocatedValue    = aLocatedValues.At(anIndex);
      EXPECT_NEAR(aLocatedValue.Point.Distance(aLocatedExpected.Point), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aLocatedValue.D1 - aLocatedExpected.D1).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aLocatedValue.D2 - aLocatedExpected.D2).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aLocatedValue.D3 - aLocatedExpected.D3).Magnitude(), 0.0, THE_TOLERANCE);
      const auto& aValue = aUnifiedValues.At(anIndex);
      EXPECT_NEAR(aValue.Point.Distance(anExpected.Point), 0, 1.e-10);
      EXPECT_NEAR((aValue.D1 - anExpected.D1).Magnitude(), 0, 1.e-10);
      EXPECT_NEAR((aValue.D2 - anExpected.D2).Magnitude(), 0, 1.e-10);
      EXPECT_NEAR((aValue.D3 - anExpected.D3).Magnitude(), 0, 1.e-10);
      EXPECT_NEAR((aWrappedValues.At(anIndex).D1 - anExpected.D1).Magnitude(), 0, 1.e-10);
    }
  }
}

//==================================================================================================

TEST(GeomGridEval_CurveTest, RationalDerivativeAbovePolynomialDegree)
{
  NCollection_Array1<gp_Pnt> aPoles(2);
  aPoles.ChangeAt(0) = gp_Pnt(0, 0, 0);
  aPoles.ChangeAt(1) = gp_Pnt(1, 0, 0);
  NCollection_Array1<double> aWeights(2);
  aWeights.ChangeAt(0) = 2;
  aWeights.ChangeAt(1) = 1;
  NCollection_Array1<double> aKnots(2);
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 1;
  NCollection_Array1<int> aMultiplicities(2);
  aMultiplicities.Init(2);
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 1);
  const GeomGridEval_BSplineCurve anEvaluator(aCurve);
  NCollection_Array1<double>      aParameters(3);
  aParameters.ChangeAt(0)                  = 1;
  aParameters.ChangeAt(1)                  = 0.3;
  aParameters.ChangeAt(2)                  = 0;
  const NCollection_Array1<gp_Vec> aValues = anEvaluator.EvaluateGridDN(aParameters, 3);
  for (size_t anIndex = 0; anIndex < aParameters.Size(); ++anIndex)
  {
    EXPECT_NEAR(aValues.At(anIndex).X(), 12 / std::pow(2 - aParameters.At(anIndex), 4), 1.e-10);
  }
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, ReloadRebuildsEndpointSurfaces)
{
  const occ::handle<Adaptor2d_Curve2d> aCurve =
    new Geom2dAdaptor_Curve(new Geom2d_Line(gp_Pnt2d(0.0, 0.25), gp_Dir2d(1.0, 0.0)), 0.1, 0.9);
  const occ::handle<Geom_Surface> anExtrusion =
    new Geom_SurfaceOfLinearExtrusion(CreateSimpleBSpline(), gp_Dir(0.0, 0.0, 1.0));
  const occ::handle<Adaptor3d_Surface> anOldSurface =
    new GeomAdaptor_Surface(anExtrusion, 0.0, 1.0, 0.0, 1.0);
  const occ::handle<Adaptor3d_Surface> aNewSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(0.0, 0.0, 1.0)));
  for (const bool aLoadBoth : {false, true})
  {
    Adaptor3d_CurveOnSurface           anAdaptor(aCurve, anOldSurface);
    const occ::handle<Adaptor3d_Curve> anOldCopy = anAdaptor.ShallowCopy();
    if (aLoadBoth)
    {
      anAdaptor.Load(aCurve, aNewSurface);
    }
    else
    {
      anAdaptor.Load(aNewSurface);
    }
    const Adaptor3d_CurveOnSurface aFresh(aCurve, aNewSurface);
    for (const double aParameter : {0.1, 0.5, 0.9})
    {
      const Geom_Curve::ResD3 aResult    = anAdaptor.EvalD3(aParameter);
      const Geom_Curve::ResD3 anExpected = aFresh.EvalD3(aParameter);
      EXPECT_NEAR(aResult.Point.Distance(anExpected.Point), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D1 - anExpected.D1).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D2 - anExpected.D2).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR((aResult.D3 - anExpected.D3).Magnitude(), 0.0, THE_TOLERANCE);
      EXPECT_NEAR(anOldCopy->EvalD1(aParameter).Point.Z(), 0.25, THE_TOLERANCE);
    }
  }
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, ReloadInvalidatesContinuityIntervals)
{
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
  const occ::handle<Adaptor2d_Curve2d> aLine =
    new Geom2dAdaptor_Curve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.1, 0.9);
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aPoles.ChangeAt(1) = gp_Pnt2d(0.5, 0.0);
  aPoles.ChangeAt(2) = gp_Pnt2d(1.0, 1.0);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 0.5;
  aKnots.ChangeAt(2) = 1.0;
  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities.ChangeAt(0) = 2;
  aMultiplicities.ChangeAt(1) = 1;
  aMultiplicities.ChangeAt(2) = 2;
  const occ::handle<Adaptor2d_Curve2d> aSpline =
    new Geom2dAdaptor_Curve(new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 1));
  Adaptor3d_CurveOnSurface anAdaptor(aLine, aSurface);
  ASSERT_EQ(anAdaptor.NbIntervals(GeomAbs_C1), 1);
  const occ::handle<Adaptor3d_Curve> anOldCopy = anAdaptor.ShallowCopy();
  anAdaptor.Load(aSpline);
  ASSERT_EQ(anAdaptor.NbIntervals(GeomAbs_C1), 2);
  NCollection_Array1<double> anIntervals(1, 3);
  anAdaptor.Intervals(anIntervals, GeomAbs_C1);
  EXPECT_DOUBLE_EQ(anIntervals.At(0), 0.0);
  EXPECT_DOUBLE_EQ(anIntervals.At(1), 0.5);
  EXPECT_DOUBLE_EQ(anIntervals.At(2), 1.0);
  ASSERT_EQ(anOldCopy->NbIntervals(GeomAbs_C1), 1);
  anAdaptor.Load(aLine, aSurface);
  EXPECT_EQ(anAdaptor.NbIntervals(GeomAbs_C1), 1);
}

//==================================================================================================

TEST(GeomGridEval_BSplineCurveTest, DecreasingSamplesCrossSpanBoundary)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.ChangeAt(0) = gp_Pnt(0, 0, 0);
  aPoles.ChangeAt(1) = gp_Pnt(0.5, 0, 0);
  aPoles.ChangeAt(2) = gp_Pnt(2, 0, 0);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 0.5;
  aKnots.ChangeAt(2) = 1;
  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities.ChangeAt(0) = 2;
  aMultiplicities.ChangeAt(1) = 1;
  aMultiplicities.ChangeAt(2) = 2;
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
  const GeomGridEval_BSplineCurve anEvaluator(aCurve);
  NCollection_Array1<double>      aParameters(1, 4);
  aParameters.ChangeAt(0)                  = 0.75;
  aParameters.ChangeAt(1)                  = 0.8;
  aParameters.ChangeAt(2)                  = 0.85;
  aParameters.ChangeAt(3)                  = 0.25;
  const NCollection_Array1<gp_Pnt> aPoints = anEvaluator.EvaluateGrid(aParameters);
  ASSERT_EQ(aPoints.Size(), aParameters.Size());
  for (size_t anIndex = 0; anIndex < aParameters.Size(); ++anIndex)
  {
    EXPECT_LT(aPoints.At(anIndex).Distance(aCurve->EvalD0(aParameters.At(anIndex))), 1.e-13);
  }
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, CubicOnPlaneThirdDerivative)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.Init(gp_Pnt2d(0, 0));
  aPoles.ChangeAt(3)                                   = gp_Pnt2d(1, 0);
  const occ::handle<Geom2d_Curve>      aCurve          = new Geom2d_BezierCurve(aPoles);
  const occ::handle<Adaptor2d_Curve2d> aCurveAdaptor   = new Geom2dAdaptor_Curve(aCurve);
  const occ::handle<Geom_Surface>      aPlane          = new Geom_Plane(gp_Ax3());
  const occ::handle<Adaptor3d_Surface> aSurfaceAdaptor = new GeomAdaptor_Surface(aPlane);
  const Adaptor3d_CurveOnSurface       anAdaptor(aCurveAdaptor, aSurfaceAdaptor);
  const Geom_Curve::ResD3              aResult = anAdaptor.EvalD3(0.5);
  EXPECT_DOUBLE_EQ(aResult.D3.X(), 6);
  EXPECT_DOUBLE_EQ(aResult.D3.Y(), 0);
  EXPECT_DOUBLE_EQ(aResult.D3.Z(), 0);
}

//==================================================================================================

TEST(GeomGridEval_CurveTest, AlternateSplineEvaluationMatchesScalarEvaluation)
{
  NCollection_Array1<gp_Pnt> aPoles(0, 1);
  aPoles.ChangeAt(0) = gp_Pnt(0, 0, 0);
  aPoles.ChangeAt(1) = gp_Pnt(1, 0, 0);
  NCollection_Array1<double> aKnots(0, 1);
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 1;
  NCollection_Array1<int> aMultiplicities(0, 1);
  aMultiplicities.Init(2);
  const occ::handle<GeomEval_RepCurveDesc::Full> aDescriptor = new GeomEval_RepCurveDesc::Full();
  aDescriptor->Representation                  = new Geom_Line(gp_Pnt(100, 20, 5), gp_Dir(1, 0, 0));
  const occ::handle<Geom_BezierCurve>  aBezier = new Geom_BezierCurve(aPoles);
  const occ::handle<Geom_BSplineCurve> aSpline =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
  aBezier->SetEvalRepresentation(aDescriptor);
  aSpline->SetEvalRepresentation(aDescriptor);
  const std::array<occ::handle<Geom_Curve>, 2> aCurves{aBezier, aSpline};
  NCollection_Array1<double>                   aParameters(-2, -1);
  aParameters.ChangeAt(0) = 0.25;
  aParameters.ChangeAt(1) = 0.75;
  for (const auto& aCurve : aCurves)
  {
    const GeomAdaptor_Curve  anAdaptor(aCurve);
    const GeomGridEval_Curve aGeometryEvaluator(aCurve), anAdaptorEvaluator(anAdaptor);
    for (const GeomGridEval_Curve* anEvaluator : {&aGeometryEvaluator, &anAdaptorEvaluator})
    {
      const NCollection_Array1<GeomGridEval::CurveD3> aValues =
        anEvaluator->EvaluateGridD3(aParameters);
      for (size_t anIndex = 0; anIndex < aParameters.Size(); ++anIndex)
      {
        const Geom_Curve::ResD3 anExpected = aCurve->EvalD3(aParameters.At(anIndex));
        EXPECT_LE(aValues.At(anIndex).Point.Distance(anExpected.Point), 1.e-12);
        EXPECT_LE((aValues.At(anIndex).D1 - anExpected.D1).Magnitude(), 1.e-12);
        EXPECT_LE((aValues.At(anIndex).D2 - anExpected.D2).Magnitude(), 1.e-12);
        EXPECT_LE((aValues.At(anIndex).D3 - anExpected.D3).Magnitude(), 1.e-12);
      }
    }
  }
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, MixedSurfaceThirdDerivative)
{
  // S(u,v) = (u,v,u*v*v), UV(t) = (t*t,t), so S(UV(t)).Z = t*t*t*t.
  NCollection_Array2<gp_Pnt> aSurfacePoles(1, 2, 1, 3);
  for (size_t aUIndex = 0; aUIndex < 2; ++aUIndex)
  {
    const double aU = static_cast<double>(aUIndex);
    for (size_t aVIndex = 0; aVIndex < 3; ++aVIndex)
    {
      const double aV                          = 0.5 * static_cast<double>(aVIndex);
      aSurfacePoles.ChangeAt(aUIndex, aVIndex) = gp_Pnt(aU, aV, aVIndex == 2 ? aU : 0.0);
    }
  }
  const occ::handle<Geom_Surface> aSurface = new Geom_BezierSurface(aSurfacePoles);
  NCollection_Array1<gp_Pnt2d>    aCurvePoles(1, 3);
  aCurvePoles.ChangeAt(0)                = gp_Pnt2d(0, 0);
  aCurvePoles.ChangeAt(1)                = gp_Pnt2d(0, 0.5);
  aCurvePoles.ChangeAt(2)                = gp_Pnt2d(1, 1);
  const occ::handle<Geom2d_Curve> aCurve = new Geom2d_BezierCurve(aCurvePoles);
  const Adaptor3d_CurveOnSurface  anAdaptor(new Geom2dAdaptor_Curve(aCurve),
                                           new GeomAdaptor_Surface(aSurface));
  for (const double aParameter : {0.0, 0.5, 1.0})
  {
    const Geom_Curve::ResD3 aResult = anAdaptor.EvalD3(aParameter);
    EXPECT_NEAR(aResult.D3.X(), 0, 1.e-12);
    EXPECT_NEAR(aResult.D3.Y(), 0, 1.e-12);
    EXPECT_NEAR(aResult.D2.Z(), 12 * aParameter * aParameter, 1.e-12);
    EXPECT_NEAR(aResult.D3.Z(), 24 * aParameter, 1.e-12);
  }
}
