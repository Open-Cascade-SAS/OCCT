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

#include <Geom2d_Ellipse.hxx>
#include <Geom2dGridEval_Ellipse.hxx>
#include <gp_Ax22d.hxx>
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

TEST(Geom2dGridEval_EllipseTest, BasicD0)
{
  occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 3.0, 2.0);

  Geom2dGridEval_Ellipse anEval(anEllipse);
  EXPECT_FALSE(anEval.Geometry().IsNull());

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 13);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);
  EXPECT_EQ(aGrid.Size(), 13);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt2d aExpected = anEllipse->Value(aParams.Value(i));
    EXPECT_NEAR(aGrid.Value(i).Distance(aExpected), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_EllipseTest, CardinalPoints)
{
  occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 3.0, 2.0);

  Geom2dGridEval_Ellipse anEval(anEllipse);

  NCollection_Array1<double> aParams(1, 4);
  aParams.SetValue(1, 0.0);
  aParams.SetValue(2, M_PI / 2);
  aParams.SetValue(3, M_PI);
  aParams.SetValue(4, 3 * M_PI / 2);

  NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(aParams);

  // u=0: (3, 0)
  EXPECT_NEAR(aGrid.Value(1).X(), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(1).Y(), 0.0, THE_TOLERANCE);

  // u=PI/2: (0, 2)
  EXPECT_NEAR(aGrid.Value(2).X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(2).Y(), 2.0, THE_TOLERANCE);

  // u=PI: (-3, 0)
  EXPECT_NEAR(aGrid.Value(3).X(), -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(3).Y(), 0.0, THE_TOLERANCE);

  // u=3PI/2: (0, -2)
  EXPECT_NEAR(aGrid.Value(4).X(), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrid.Value(4).Y(), -2.0, THE_TOLERANCE);
}

TEST(Geom2dGridEval_EllipseTest, D1)
{
  occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(1, 2), gp_Dir2d(1, 0)), 5.0, 3.0);

  Geom2dGridEval_Ellipse anEval(anEllipse);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 13);

  NCollection_Array1<Geom2dGridEval::CurveD1> aGrid = anEval.EvaluateGridD1(aParams);

  for (int i = 1; i <= 13; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    anEllipse->D1(aParams.Value(i), aPnt, aD1);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_EllipseTest, D2)
{
  occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 3.0, 2.0);

  Geom2dGridEval_Ellipse anEval(anEllipse);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD2> aGrid = anEval.EvaluateGridD2(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2;
    anEllipse->D2(aParams.Value(i), aPnt, aD1, aD2);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
  }
}

TEST(Geom2dGridEval_EllipseTest, D3)
{
  occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)), 3.0, 2.0);

  Geom2dGridEval_Ellipse anEval(anEllipse);

  NCollection_Array1<double> aParams = CreateUniformParams(0.0, 2 * M_PI, 9);

  NCollection_Array1<Geom2dGridEval::CurveD3> aGrid = anEval.EvaluateGridD3(aParams);

  for (int i = 1; i <= 9; ++i)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1, aD2, aD3;
    anEllipse->D3(aParams.Value(i), aPnt, aD1, aD2, aD3);
    EXPECT_NEAR(aGrid.Value(i).Point.Distance(aPnt), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D1 - aD1).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D2 - aD2).Magnitude(), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((aGrid.Value(i).D3 - aD3).Magnitude(), 0.0, THE_TOLERANCE);
  }
}
