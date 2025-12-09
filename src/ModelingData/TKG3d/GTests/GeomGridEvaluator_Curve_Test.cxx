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

#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomGridEvaluator_BSplineCurve.hxx>
#include <GeomGridEvaluator_Circle.hxx>
#include <GeomGridEvaluator_Curve.hxx>
#include <GeomGridEvaluator_Line.hxx>
#include <GeomGridEvaluator_OtherCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

//! Helper function to create uniform parameters
TColStd_Array1OfReal CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  TColStd_Array1OfReal aParams(1, theNbPoints);
  const double         aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}

//! Helper function to create a simple B-spline curve
Handle(Geom_BSplineCurve) CreateSimpleBSpline()
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  TColStd_Array1OfReal    aKnots(1, 2);
  TColStd_Array1OfInteger aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
}
} // namespace

//==================================================================================================
// Tests for GeomGridEvaluator_Line
//==================================================================================================

TEST(GeomGridEvaluator_LineTest, BasicEvaluation)
{
  // Create a Geom_Line along X axis starting at origin
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  GeomGridEvaluator_Line anEval(aGeomLine);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 10.0, 11);
  anEval.SetParams(aParams);
  EXPECT_EQ(anEval.NbParams(), 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
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

TEST(GeomGridEvaluator_LineTest, NonOriginLine)
{
  // Create a line at (1, 2, 3) along direction (1, 1, 1)/sqrt(3)
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(1, 2, 3), gp_Dir(1, 1, 1));

  GeomGridEvaluator_Line anEval(aGeomLine);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify first and last point
  EXPECT_NEAR(aGrid.Value(1).Distance(gp_Pnt(1, 2, 3)), 0.0, THE_TOLERANCE);

  // Line direction is normalized, so t=5 moves 5 units along normalized direction
  const double d     = 5.0 / std::sqrt(3.0);
  gp_Pnt       aLast = gp_Pnt(1 + d, 2 + d, 3 + d);
  EXPECT_NEAR(aGrid.Value(6).Distance(aLast), 0.0, THE_TOLERANCE);
}

//==================================================================================================
// Tests for GeomGridEvaluator_Circle
//==================================================================================================

TEST(GeomGridEvaluator_CircleTest, BasicEvaluation)
{
  // Circle in XY plane, radius 2, center at origin
  Handle(Geom_Circle) aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  GeomGridEvaluator_Circle anEval(aGeomCircle);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  // Test at 0, PI/2, PI, 3PI/2, 2PI
  TColStd_Array1OfReal aParams(1, 5);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);
  aParams.SetValue(5, 2 * M_PI);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

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

TEST(GeomGridEvaluator_CircleTest, NonStandardCircle)
{
  // Circle in YZ plane, radius 3, center at (1, 0, 0)
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(gp_Ax2(gp_Pnt(1, 0, 0), gp_Dir(1, 0, 0)), 3.0);

  GeomGridEvaluator_Circle anEval(aGeomCircle);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // All points should be at distance 3 from center (1, 0, 0)
  const gp_Pnt aCenter(1, 0, 0);
  for (int i = 1; i <= aGrid.Size(); ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).Distance(aCenter), 3.0, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).X(), 1.0, THE_TOLERANCE); // All points have X=1
  }
}

//==================================================================================================
// Tests for GeomGridEvaluator_BSplineCurve
//==================================================================================================

TEST(GeomGridEvaluator_BSplineCurveTest, BasicEvaluation)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();

  GeomGridEvaluator_BSplineCurve anEval(aCurve);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_EQ(aGrid.Size(), 11);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_BSplineCurveTest, EndpointsMatch)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();

  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams(1, 2);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, 1.0);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // First point should match first pole
  EXPECT_NEAR(aGrid.Value(1).Distance(gp_Pnt(0, 0, 0)), 0.0, THE_TOLERANCE);
  // Last point should match last pole
  EXPECT_NEAR(aGrid.Value(2).Distance(gp_Pnt(4, 0, 0)), 0.0, THE_TOLERANCE);
}

//==================================================================================================
// Tests for GeomGridEvaluator_OtherCurve (fallback)
//==================================================================================================

TEST(GeomGridEvaluator_OtherCurveTest, EllipseFallback)
{
  // Ellipse is not directly supported, so it should use OtherCurve fallback
  Handle(Geom_Ellipse) anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  GeomAdaptor_Curve anAdaptor(anEllipse);

  GeomGridEvaluator_OtherCurve anEval(anAdaptor.ShallowCopy());
  EXPECT_FALSE(anEval.Curve().IsNull());

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt aExpected = anEllipse->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for GeomGridEvaluator_Curve (unified dispatcher)
//==================================================================================================

TEST(GeomGridEvaluator_CurveTest, LineDispatch)
{
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve anAdaptor(aGeomLine);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 10.0, 11);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_EQ(aGrid.Size(), 11);

  // Verify against direct evaluation
  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aGeomLine->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_CurveTest, CircleDispatch)
{
  Handle(Geom_Circle) aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 2 * M_PI, 17);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt aExpected = aGeomCircle->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_CurveTest, BSplineDispatch)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();
  GeomAdaptor_Curve         anAdaptor(aCurve);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 21);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_CurveTest, EllipseFallbackDispatch)
{
  Handle(Geom_Ellipse) anEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0, 2.0);
  GeomAdaptor_Curve anAdaptor(anEllipse);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Ellipse);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 2 * M_PI, 13);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt aExpected = anEllipse->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_CurveTest, UninitializedState)
{
  GeomGridEvaluator_Curve anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbParams(), 0);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_TRUE(aGrid.IsEmpty());
}

TEST(GeomGridEvaluator_CurveTest, EmptyParams)
{
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve anAdaptor(aGeomLine);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbParams(), 0);

  // EvaluateGrid without setting params should return empty
  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_TRUE(aGrid.IsEmpty());
}

//==================================================================================================
// Additional tests for B-spline curves (from BSplCLib_GridEvaluator_Test)
//==================================================================================================

TEST(GeomGridEvaluator_BSplineCurveTest, RationalBSpline)
{
  // Create a rational B-spline (NURBS circle approximation)
  TColgp_Array1OfPnt  aPoles(1, 4);
  TColStd_Array1OfReal aWeights(1, 4);

  aPoles.SetValue(1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 1, 0));
  aPoles.SetValue(3, gp_Pnt(0, 1, 0));
  aPoles.SetValue(4, gp_Pnt(-1, 1, 0));

  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0 / std::sqrt(2.0));
  aWeights.SetValue(3, 1.0);
  aWeights.SetValue(4, 1.0 / std::sqrt(2.0));

  TColStd_Array1OfReal    aKnots(1, 2);
  TColStd_Array1OfInteger aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  Handle(Geom_BSplineCurve) aCurve = new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 3);

  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 21);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
  EXPECT_EQ(aGrid.Size(), 21);

  // Verify against direct evaluation
  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_BSplineCurveTest, MultiSpanBSpline)
{
  // Create a B-spline with multiple spans (degree 3, 6 control points, 1 internal knot)
  // Formula: n_poles = sum(multiplicities) - degree - 1 = (4 + 2 + 4) - 3 - 1 = 6
  TColgp_Array1OfPnt aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(2, 2, 0));
  aPoles.SetValue(4, gp_Pnt(3, 0, 0));
  aPoles.SetValue(5, gp_Pnt(4, -1, 0));
  aPoles.SetValue(6, gp_Pnt(5, 0, 0));

  TColStd_Array1OfReal    aKnots(1, 3);
  TColStd_Array1OfInteger aMults(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 2);
  aMults.SetValue(3, 4);

  Handle(Geom_BSplineCurve) aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);

  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 31);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation - should match exactly
  for (int i = 1; i <= 31; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_BSplineCurveTest, HighDegree)
{
  // Create a higher degree B-spline (degree 5)
  TColgp_Array1OfPnt aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 3, 0));
  aPoles.SetValue(3, gp_Pnt(2, 1, 0));
  aPoles.SetValue(4, gp_Pnt(3, 4, 0));
  aPoles.SetValue(5, gp_Pnt(4, 2, 0));
  aPoles.SetValue(6, gp_Pnt(5, 0, 0));

  TColStd_Array1OfReal    aKnots(1, 2);
  TColStd_Array1OfInteger aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 6);
  aMults.SetValue(2, 6);

  Handle(Geom_BSplineCurve) aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMults, 5);

  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 51);
  anEval.SetParams(aParams);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();

  // Verify against direct evaluation
  for (int i = 1; i <= 51; ++i)
  {
    gp_Pnt aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for Curve Derivative Evaluation (D1, D2, D3)
//==================================================================================================

TEST(GeomGridEvaluator_LineTest, DerivativeD1)
{
  // Line along (1, 2, 3) normalized direction
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 2, 3));
  GeomGridEvaluator_Line anEval(aGeomLine);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1();
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

TEST(GeomGridEvaluator_LineTest, DerivativeD2D3)
{
  Handle(Geom_Line) aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomGridEvaluator_Line anEval(aGeomLine);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 5.0, 6);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD2> aGridD2 = anEval.EvaluateGridD2();
  NCollection_Array1<GeomGridEval::CurveD3> aGridD3 = anEval.EvaluateGridD3();

  // For a line, D2 and D3 are zero
  for (int i = 1; i <= 6; ++i)
  {
    EXPECT_NEAR(aGridD2.Value(i).D2.Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGridD3.Value(i).D2.Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGridD3.Value(i).D3.Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEvaluator_CircleTest, DerivativeD1)
{
  Handle(Geom_Circle) aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEvaluator_Circle anEval(aGeomCircle);

  TColStd_Array1OfReal aParams(1, 5);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);
  aParams.SetValue(5, 2 * M_PI);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1();

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

TEST(GeomGridEvaluator_CircleTest, DerivativeD2)
{
  Handle(Geom_Circle) aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomGridEvaluator_Circle anEval(aGeomCircle);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2();

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

TEST(GeomGridEvaluator_BSplineCurveTest, DerivativeD1)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();
  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1();

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

TEST(GeomGridEvaluator_BSplineCurveTest, DerivativeD2)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();
  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2();

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

TEST(GeomGridEvaluator_BSplineCurveTest, DerivativeD3)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();
  GeomGridEvaluator_BSplineCurve anEval(aCurve);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3();

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

TEST(GeomGridEvaluator_CurveTest, UnifiedDerivativeD1)
{
  Handle(Geom_Circle) aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 2 * M_PI, 9);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1();

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

TEST(GeomGridEvaluator_CurveTest, UnifiedDerivativeD2)
{
  Handle(Geom_BSplineCurve) aCurve = CreateSimpleBSpline();
  GeomAdaptor_Curve anAdaptor(aCurve);

  GeomGridEvaluator_Curve anEval;
  anEval.Initialize(anAdaptor);

  TColStd_Array1OfReal aParams = CreateUniformParams(0.0, 1.0, 11);
  anEval.SetParams(aParams);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2();

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
