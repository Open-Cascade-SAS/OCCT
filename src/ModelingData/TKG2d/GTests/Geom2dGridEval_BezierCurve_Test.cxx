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
#include <Geom2dGridEval_BezierCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

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
} // namespace

TEST(Geom2dGridEval_BezierCurveTest, BasicD0)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dGridEval_BezierCurve anEval(aBezier);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 11);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aExpected = aBezier->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BezierCurveTest, D1)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    aBezier->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BezierCurveTest, D2)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2;
    aBezier->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BezierCurveTest, D3)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<Geom2dGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2, aD3;
    aBezier->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BezierCurveTest, DN_BeyondDegree)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt2d(0, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 2));
  aPoles.SetValue(3, gp_Pnt2d(3, 2));
  aPoles.SetValue(4, gp_Pnt2d(4, 0));
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  // Degree 3 Bezier, 4th derivative should be zero
  NCollection_Array1<gp_Vec2d> aGrid = anEval.EvaluateGridDN(aParams, 4);

  for (int i = 1; i <= 11; ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_BezierCurveTest, RationalBezier)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  NCollection_Array1<double>   aWeights(1, 3);

  aPoles.SetValue(1, gp_Pnt2d(1, 0));
  aPoles.SetValue(2, gp_Pnt2d(1, 1));
  aPoles.SetValue(3, gp_Pnt2d(0, 1));

  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0 / std::sqrt(2.0));
  aWeights.SetValue(3, 1.0);

  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles, aWeights);

  Geom2dGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 21);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 21; ++i)
  {
    gp_Pnt2d aExpected = aBezier->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}
