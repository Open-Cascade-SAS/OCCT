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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGridEval_BSplineCurve.hxx>
#include <Geom2dGridEval_Circle.hxx>
#include <Geom2dGridEval_Curve.hxx>
#include <Geom2dGridEval_Line.hxx>
#include <Geom2dGridEval_OtherCurve.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

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

//! Helper function to create a simple 2D B-spline curve
occ::handle<Geom2d_BSplineCurve> CreateSimpleBSpline2d()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 4);

  return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
}
} // namespace

//==================================================================================================
// Tests for Geom2dGridEval_Line
//==================================================================================================

TEST(Geom2dGridEval_LineTest, BasicEvaluation)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));

  Geom2dGridEval_Line anEval(aGeomLine);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  for (int i = 1; i <= 11; ++i)
  {
    const double t = aParams.Value(i);
    EXPECT_NEAR(aGrid.Value(i).X(), t, THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).Y(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_LineTest, NonOriginLine)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(1, 2), gp_Dir2d(1, 1));

  Geom2dGridEval_Line anEval(aGeomLine);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  EXPECT_NEAR(aGrid.Value(1).Distance(gp_Pnt2d(1, 2)), 0.0, THE_TOLERANCE);

  const double d = 5.0 / std::sqrt(2.0);
  EXPECT_NEAR(aGrid.Value(6).Distance(gp_Pnt2d(1 + d, 2 + d)), 0.0, THE_TOLERANCE);
}

TEST(Geom2dGridEval_LineTest, DerivativeD1)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 2));
  Geom2dGridEval_Line      anEval(aGeomLine);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<Geom2dGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);
  EXPECT_EQ(aGrid.Size(), 6);

  gp_Dir2d aDir(1, 2);
  for (int i = 1; i <= 6; ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).D1.X(), aDir.X(), THE_TOLERANCE);
    EXPECT_NEAR(aGrid.Value(i).D1.Y(), aDir.Y(), THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_LineTest, DerivativeD2D3)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  Geom2dGridEval_Line      anEval(aGeomLine);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<Geom2dGridEval::CurveD2> aGridD2 = anEval.EvaluateGridD2(aParams);
  NCollection_Array1<Geom2dGridEval::CurveD3> aGridD3 = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 6; ++i)
  {
    EXPECT_NEAR(aGridD2.Value(i).D2.Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGridD3.Value(i).D2.Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR(aGridD3.Value(i).D3.Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for Geom2dGridEval_Circle
//==================================================================================================

TEST(Geom2dGridEval_CircleTest, BasicEvaluation)
{
  occ::handle<Geom2d_Circle> aGeomCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);

  Geom2dGridEval_Circle anEval(aGeomCircle);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams(1, 5);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);
  aParams.SetValue(5, 2 * M_PI);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  EXPECT_NEAR(aGrid.Value(1).X(), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(1).Y(), 0.0, THE_TOLERANCE);

  EXPECT_NEAR(aGrid.Value(2).X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2).Y(), 2.0, THE_TOLERANCE);

  EXPECT_NEAR(aGrid.Value(3).X(), -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(3).Y(), 0.0, THE_TOLERANCE);

  EXPECT_NEAR(aGrid.Value(4).X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(4).Y(), -2.0, THE_TOLERANCE);

  EXPECT_NEAR(aGrid.Value(5).X(), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(5).Y(), 0.0, THE_TOLERANCE);
}

TEST(Geom2dGridEval_CircleTest, DerivativeD1)
{
  occ::handle<Geom2d_Circle> aGeomCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);
  Geom2dGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    aGeomCircle->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CircleTest, DerivativeD2)
{
  occ::handle<Geom2d_Circle> aGeomCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);
  Geom2dGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2;
    aGeomCircle->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CircleTest, DerivativeD3)
{
  occ::handle<Geom2d_Circle> aGeomCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);
  Geom2dGridEval_Circle anEval(aGeomCircle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2, aD3;
    aGeomCircle->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for Geom2dGridEval_BSplineCurve
//==================================================================================================

TEST(Geom2dGridEval_BSplineCurveTest, BasicEvaluation)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();

  Geom2dGridEval_BSplineCurve anEval(aCurve);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BSplineCurveTest, EndpointsMatch)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();

  Geom2dGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams(1, 2);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, 1.0);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  EXPECT_NEAR(aGrid.Value(1).Distance(gp_Pnt2d(0, 0)), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2).Distance(gp_Pnt2d(4, 0)), 0.0, THE_TOLERANCE);
}

TEST(Geom2dGridEval_BSplineCurveTest, DerivativeD1)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    aCurve->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BSplineCurveTest, DerivativeD2)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2;
    aCurve->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BSplineCurveTest, DerivativeD3)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2, aD3;
    aCurve->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BSplineCurveTest, DerivativeDN_BeyondDegree)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dGridEval_BSplineCurve      anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Vec2d> aGrid = anEval.EvaluateGridDN(aParams, 4);

  for (int i = 1; i <= 11; ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BSplineCurveTest, MultiSpanBSpline)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(2, 2));
  aPoles.SetValue(4, gp_Pnt2d(3, 0));
  aPoles.SetValue(5, gp_Pnt2d(4, -1));
  aPoles.SetValue(6, gp_Pnt2d(5, 0));

  NCollection_Array1<double> aKnots(1, 3);
  NCollection_Array1<int>    aMults(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);
  aMults.SetValue(1, 4);
  aMults.SetValue(2, 2);
  aMults.SetValue(3, 4);

  occ::handle<Geom2d_BSplineCurve> aCurve = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  Geom2dGridEval_BSplineCurve anEval(aCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 31);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 31; ++i)
  {
    gp_Pnt2d aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Tests for Geom2dGridEval_Curve (unified dispatcher)
//==================================================================================================

TEST(Geom2dGridEval_CurveTest, LineDispatch)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  Geom2dAdaptor_Curve      anAdaptor(aGeomLine);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aExpected = aGeomLine->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, CircleDispatch)
{
  occ::handle<Geom2d_Circle> aGeomCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);
  Geom2dAdaptor_Curve anAdaptor(aGeomCircle);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Circle);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 17);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 17; ++i)
  {
    gp_Pnt2d aExpected = aGeomCircle->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, EllipseDispatch)
{
  occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 3.0, 2.0);
  Geom2dAdaptor_Curve anAdaptor(anEllipse);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Ellipse);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 13);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt2d aExpected = anEllipse->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, HyperbolaDispatch)
{
  occ::handle<Geom2d_Hyperbola> aHypr =
    new Geom2d_Hyperbola(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 3.0, 2.0);
  Geom2dAdaptor_Curve anAdaptor(aHypr);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Hyperbola);

  NCollection_Array1<double> aParams = CreateUniformParams(-2.0, 2.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aExpected = aHypr->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, ParabolaDispatch)
{
  occ::handle<Geom2d_Parabola> aParab =
    new Geom2d_Parabola(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 1.0);
  Geom2dAdaptor_Curve anAdaptor(aParab);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Parabola);

  NCollection_Array1<double> aParams = CreateUniformParams(-2.0, 2.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aExpected = aParab->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, BSplineDispatch)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dAdaptor_Curve              anAdaptor(aCurve);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BSplineCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt2d aExpected = aCurve->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, BezierCurveDispatch)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve              anAdaptor(aBezier);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_BezierCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aExpected = aBezier->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, OffsetCurveDispatch)
{
  occ::handle<Geom2d_Line>        aLine    = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aLine, 1.0);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anOffset);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_OffsetCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 5.0, 6);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 6; ++i)
  {
    gp_Pnt2d aExpected = anOffset->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, DirectHandleInit)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(aGeomLine);
  EXPECT_TRUE(anEval.IsInitialized());
  EXPECT_EQ(anEval.GetType(), GeomAbs_Line);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 10.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);
  EXPECT_NEAR(aGrid.Value(1).X(), 0.0, THE_TOLERANCE);
}

TEST(Geom2dGridEval_CurveTest, UninitializedState)
{
  Geom2dGridEval_Curve anEval;
  EXPECT_FALSE(anEval.IsInitialized());

  NCollection_Array1<double>   aEmptyParams;
  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aEmptyParams);
  EXPECT_TRUE(aGrid.IsEmpty());
}

TEST(Geom2dGridEval_CurveTest, EmptyParams)
{
  occ::handle<Geom2d_Line> aGeomLine = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  Geom2dAdaptor_Curve      anAdaptor(aGeomLine);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);
  EXPECT_TRUE(anEval.IsInitialized());

  NCollection_Array1<double>   aEmptyParams;
  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aEmptyParams);
  EXPECT_TRUE(aGrid.IsEmpty());
}

TEST(Geom2dGridEval_CurveTest, UnifiedDerivativeD1)
{
  occ::handle<Geom2d_Circle> aGeomCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);
  Geom2dAdaptor_Curve anAdaptor(aGeomCircle);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    aGeomCircle->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, UnifiedDerivativeD2)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dAdaptor_Curve              anAdaptor(aCurve);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2;
    aCurve->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, UnifiedDerivativeD3)
{
  occ::handle<Geom2d_BSplineCurve> aCurve = CreateSimpleBSpline2d();
  Geom2dAdaptor_Curve              anAdaptor(aCurve);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anAdaptor);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2, aD3;
    aCurve->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_CurveTest, OffsetCurveDerivativeD3)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 2.0);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 0.5);

  Geom2dGridEval_Curve anEval;
  anEval.Initialize(anOffset);
  EXPECT_EQ(anEval.GetType(), GeomAbs_OffsetCurve);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2, aD3;
    anOffset->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}
