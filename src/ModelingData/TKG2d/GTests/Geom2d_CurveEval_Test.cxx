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

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//=================================================================================================
// Circle 2D tests
//=================================================================================================

TEST(Geom2d_CurveEvalTest, Circle_EvalD0D1_ValidResults)
{
  gp_Circ2d                  aCirc(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 4.0);
  occ::handle<Geom2d_Circle> aCurve = new Geom2d_Circle(aCirc);

  const double aU = M_PI / 4.0;

  // EvalD0
  const auto aD0 = aCurve->EvalD0(aU);
  ASSERT_TRUE(aD0.has_value());
  const double aDist = aD0->Distance(gp_Pnt2d(0.0, 0.0));
  EXPECT_NEAR(aDist, 4.0, Precision::Confusion());

  // EvalD1
  const auto aD1 = aCurve->EvalD1(aU);
  ASSERT_TRUE(aD1.has_value());
  EXPECT_NEAR(aD1->Point.Distance(*aD0), 0.0, Precision::Confusion());

  // D1 should be perpendicular to radius
  const gp_Vec2d aRadial(gp_Pnt2d(0.0, 0.0), aD1->Point);
  EXPECT_NEAR(aRadial.Dot(aD1->D1), 0.0, 1.0e-10);
}

//=================================================================================================
// OffsetCurve 2D failure-path test
//=================================================================================================

TEST(Geom2d_CurveEvalTest, OffsetCurve_EvalD0_ValidAtDegenerateCenter)
{
  // Create a circle with offset = -radius to get a degenerate (zero-radius) curve
  gp_Circ2d                  aCirc(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0);
  occ::handle<Geom2d_Circle> aBasis = new Geom2d_Circle(aCirc);

  // Offset by -radius creates a collapsed curve at the center
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aBasis, -3.0);

  // EvalD0 on the offset curve. This may or may not return nullopt depending on
  // implementation - the degenerate curve should still produce a valid point
  // (the center), since offset curve D0 evaluation is typically valid.
  const auto aResult = anOffset->EvalD0(0.0);
  EXPECT_TRUE(aResult.has_value());
}

//=================================================================================================
// BSplineCurve 2D consistency tests
//=================================================================================================

TEST(Geom2d_CurveEvalTest, BSplineCurve_EvalD1_ConsistentWithOldAPI)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 5);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 2.0);
  aPnts(3) = gp_Pnt2d(3.0, 1.0);
  aPnts(4) = gp_Pnt2d(5.0, 3.0);
  aPnts(5) = gp_Pnt2d(7.0, 0.0);

  Geom2dAPI_PointsToBSpline anInterp(aPnts);
  ASSERT_TRUE(anInterp.IsDone());
  occ::handle<Geom2d_BSplineCurve> aCurve = anInterp.Curve();

  const double aFirst = aCurve->FirstParameter();
  const double aLast  = aCurve->LastParameter();
  const double aMid   = (aFirst + aLast) / 2.0;

  // Compare EvalD1 with old D1 wrapper
  const auto aEvalResult = aCurve->EvalD1(aMid);
  ASSERT_TRUE(aEvalResult.has_value());

  gp_Pnt2d aOldP;
  gp_Vec2d aOldV1;
  aCurve->D1(aMid, aOldP, aOldV1);

  EXPECT_NEAR(aEvalResult->Point.Distance(aOldP), 0.0, Precision::Confusion());
  EXPECT_NEAR((aEvalResult->D1 - aOldV1).Magnitude(), 0.0, Precision::Confusion());
}

//=================================================================================================
// Ellipse 2D tests
//=================================================================================================

TEST(Geom2d_CurveEvalTest, Ellipse_EvalD2D3_ValidResults)
{
  gp_Elips2d                  anElips(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
  occ::handle<Geom2d_Ellipse> aCurve = new Geom2d_Ellipse(anElips);

  const double aU = M_PI / 4.0;

  const auto aD2 = aCurve->EvalD2(aU);
  ASSERT_TRUE(aD2.has_value());
  EXPECT_GT(aD2->D2.Magnitude(), 0.0);

  const auto aD3 = aCurve->EvalD3(aU);
  ASSERT_TRUE(aD3.has_value());
  EXPECT_GT(aD3->D3.Magnitude(), 0.0);
}

//=================================================================================================
// Line 2D tests
//=================================================================================================

TEST(Geom2d_CurveEvalTest, Line_EvalDN_ValidResults)
{
  occ::handle<Geom2d_Line> aCurve = new Geom2d_Line(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0));

  // DN(U, 1) should return the direction
  const auto aDN1 = aCurve->EvalDN(5.0, 1);
  ASSERT_TRUE(aDN1.has_value());
  EXPECT_NEAR(aDN1->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDN1->Y(), 0.0, Precision::Confusion());

  // DN(U, 2) should return zero for a line
  const auto aDN2 = aCurve->EvalDN(5.0, 2);
  ASSERT_TRUE(aDN2.has_value());
  EXPECT_NEAR(aDN2->Magnitude(), 0.0, Precision::Confusion());
}

//=================================================================================================
// BSplineCurve 2D EvalDN invalid N test
//=================================================================================================

TEST(Geom2d_CurveEvalTest, BSplineCurve_EvalDN_InvalidN_ReturnsNullopt)
{
  NCollection_Array1<gp_Pnt2d> aPnts(1, 3);
  aPnts(1) = gp_Pnt2d(0.0, 0.0);
  aPnts(2) = gp_Pnt2d(1.0, 1.0);
  aPnts(3) = gp_Pnt2d(2.0, 0.0);

  Geom2dAPI_PointsToBSpline anInterp(aPnts);
  ASSERT_TRUE(anInterp.IsDone());
  occ::handle<Geom2d_BSplineCurve> aCurve = anInterp.Curve();

  const double aMid = (aCurve->FirstParameter() + aCurve->LastParameter()) / 2.0;

  // N=0 should return nullopt
  EXPECT_FALSE(aCurve->EvalDN(aMid, 0).has_value());
  // N=-1 should return nullopt
  EXPECT_FALSE(aCurve->EvalDN(aMid, -1).has_value());
}

//=================================================================================================
// OffsetCurve 2D EvalD1 singular test
//=================================================================================================

TEST(Geom2d_CurveEvalTest, OffsetCurve_EvalD1_DegenerateAtCollapsed)
{
  // Create a circle with offset = -radius to get a degenerate (zero-radius) curve
  gp_Circ2d                  aCirc(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0);
  occ::handle<Geom2d_Circle> aBasis = new Geom2d_Circle(aCirc);

  // Offset by -radius creates a collapsed curve at the center.
  // The basis circle has valid tangents everywhere, so EvalD1 succeeds
  // but produces a degenerate result (all points map to the center).
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aBasis, -3.0);

  const auto aResult = anOffset->EvalD1(0.0);
  ASSERT_TRUE(aResult.has_value());

  // The point should be near the center
  EXPECT_NEAR(aResult->Point.Distance(gp_Pnt2d(0.0, 0.0)), 0.0, Precision::Confusion());
}
