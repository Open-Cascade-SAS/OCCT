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

#include <Geom_BezierCurve.hxx>
#include <GeomGridEval_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1e-10;

NCollection_Array1<double> CreateUniformParams(double theFirst, double theLast, int theNbPoints)
{
  NCollection_Array1<double> aParams(1, theNbPoints);
  const double         aStep = (theLast - theFirst) / (theNbPoints - 1);
  for (int i = 1; i <= theNbPoints; ++i)
  {
    aParams.SetValue(i, theFirst + (i - 1) * aStep);
  }
  return aParams;
}
} // namespace

TEST(GeomGridEval_BezierCurveTest, BasicEvaluation)
{
  // Create a cubic Bezier curve
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);

  GeomGridEval_BezierCurve anEval(aBezier);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  // Test at 0, 0.5, 1
  NCollection_Array1<double> aParams(1, 3);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, 0.5);
  aParams.SetValue(3, 1.0);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 3);

  // Verify points
  for (int i = 1; i <= 3; ++i)
  {
    gp_Pnt aExpected = aBezier->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeD1)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array1<GeomGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1;
    aBezier->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeD2)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array1<GeomGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2;
    aBezier->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeD3)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array1<GeomGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Pnt aPnt;
    gp_Vec aD1, aD2, aD3;
    aBezier->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, RationalEvaluation)
{
  // Rational Bezier curve (quarter circle)
  NCollection_Array1<gp_Pnt>   aPoles(1, 3);
  NCollection_Array1<double> aWeights(1, 3);

  aPoles.SetValue(1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 1, 0));
  aPoles.SetValue(3, gp_Pnt(0, 1, 0));

  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0 / std::sqrt(2.0));
  aWeights.SetValue(3, 1.0);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles, aWeights);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(aParams);

  for (int i = 1; i <= 11; ++i)
  {
    gp_Pnt aExpected = aBezier->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeDN_Order1)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 1);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Vec aExpected = aBezier->DN(aParams.Value(i), 1);
    EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeDN_Order2)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 2);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Vec aExpected = aBezier->DN(aParams.Value(i), 2);
    EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeDN_Order3)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 3);

  for (int i = 1; i <= 5; ++i)
  {
    gp_Vec aExpected = aBezier->DN(aParams.Value(i), 3);
    EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeDN_BeyondDegree)
{
  // Cubic Bezier (degree 3), test DN for N > 3 should return zero vectors
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles.SetValue(1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 2, 0));
  aPoles.SetValue(3, gp_Pnt(3, 2, 0));
  aPoles.SetValue(4, gp_Pnt(4, 0, 0));

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 5);

  // 4th derivative of cubic Bezier should be zero
  NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, 4);

  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR(aGrid.Value(i).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(GeomGridEval_BezierCurveTest, DerivativeDN_RationalCurve)
{
  // Rational Bezier curve (quarter circle)
  NCollection_Array1<gp_Pnt>   aPoles(1, 3);
  NCollection_Array1<double> aWeights(1, 3);

  aPoles.SetValue(1, gp_Pnt(1, 0, 0));
  aPoles.SetValue(2, gp_Pnt(1, 1, 0));
  aPoles.SetValue(3, gp_Pnt(0, 1, 0));

  aWeights.SetValue(1, 1.0);
  aWeights.SetValue(2, 1.0 / std::sqrt(2.0));
  aWeights.SetValue(3, 1.0);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles, aWeights);
  GeomGridEval_BezierCurve anEval(aBezier);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 1.0, 11);

  // Test DN for orders 1, 2
  for (int aOrder = 1; aOrder <= 2; ++aOrder)
  {
    NCollection_Array1<gp_Vec> aGrid = anEval.EvaluateGridDN(aParams, aOrder);

    for (int i = 1; i <= 11; ++i)
    {
      gp_Vec aExpected = aBezier->DN(aParams.Value(i), aOrder);
      EXPECT_NEAR((aGrid.Value(i) - aExpected).Magnitude(), 0.0, THE_TOLERANCE);
    }
  }
}
