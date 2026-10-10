// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <Convert_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <gp_Pnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

TEST(Convert_CompBezierCurvesToBSplineCurveTest, SingleLinearBezier)
{
  Convert_CompBezierCurvesToBSplineCurve aConv;

  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);

  aConv.AddCurve(aPoles);
  aConv.Perform();

  EXPECT_GE(aConv.Degree(), 1);
  EXPECT_EQ(aConv.NbPoles(), 2);
  EXPECT_EQ(aConv.NbKnots(), 2);

  NCollection_Array1<gp_Pnt> aResPoles(1, aConv.NbPoles());
  aConv.Poles(aResPoles);
  EXPECT_NEAR(aResPoles(1).X(), 0.0, 1.0e-15);
  EXPECT_NEAR(aResPoles(2).X(), 1.0, 1.0e-15);
}

TEST(Convert_CompBezierCurvesToBSplineCurveTest, SingleCubicBezier)
{
  Convert_CompBezierCurvesToBSplineCurve aConv;

  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 0.0);
  aPoles(3) = gp_Pnt(3.0, 2.0, 0.0);
  aPoles(4) = gp_Pnt(4.0, 0.0, 0.0);

  aConv.AddCurve(aPoles);
  aConv.Perform();

  EXPECT_EQ(aConv.Degree(), 3);
  EXPECT_EQ(aConv.NbPoles(), 4);
  EXPECT_EQ(aConv.NbKnots(), 2);
}

TEST(Convert_CompBezierCurvesToBSplineCurveTest, TwoAdjacentBeziers_C0)
{
  Convert_CompBezierCurvesToBSplineCurve aConv;

  // First linear segment
  NCollection_Array1<gp_Pnt> aPoles1(1, 2);
  aPoles1(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles1(2) = gp_Pnt(1.0, 1.0, 0.0);

  // Second linear segment, adjacent but not tangent
  NCollection_Array1<gp_Pnt> aPoles2(1, 2);
  aPoles2(1) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles2(2) = gp_Pnt(2.0, 0.0, 0.0);

  aConv.AddCurve(aPoles1);
  aConv.AddCurve(aPoles2);
  aConv.Perform();

  EXPECT_GE(aConv.Degree(), 1);
  EXPECT_EQ(aConv.NbKnots(), 3);

  NCollection_Array1<gp_Pnt> aResPoles(1, aConv.NbPoles());
  NCollection_Array1<double> aKnots(1, aConv.NbKnots());
  NCollection_Array1<int>    aMults(1, aConv.NbKnots());
  aConv.Poles(aResPoles);
  aConv.KnotsAndMults(aKnots, aMults);

  // First and last knot multiplicities should be degree+1
  EXPECT_EQ(aMults(1), aConv.Degree() + 1);
  EXPECT_EQ(aMults(aConv.NbKnots()), aConv.Degree() + 1);
}

TEST(Convert_CompBezierCurvesToBSplineCurveTest, TwoAdjacentBeziers_C1)
{
  Convert_CompBezierCurvesToBSplineCurve aConv;

  // Two cubic Beziers with parallel tangent at junction
  NCollection_Array1<gp_Pnt> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles1(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles1(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles1(4) = gp_Pnt(3.0, 0.0, 0.0);

  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(3.0, 0.0, 0.0);
  aPoles2(2) = gp_Pnt(4.0, -1.0, 0.0);
  aPoles2(3) = gp_Pnt(5.0, -1.0, 0.0);
  aPoles2(4) = gp_Pnt(6.0, 0.0, 0.0);

  aConv.AddCurve(aPoles1);
  aConv.AddCurve(aPoles2);
  aConv.Perform();

  EXPECT_EQ(aConv.Degree(), 3);
  EXPECT_EQ(aConv.NbKnots(), 3);

  // Check that junction has multiplicity Degree-1 (C1)
  NCollection_Array1<int>    aMults(1, aConv.NbKnots());
  NCollection_Array1<double> aKnots(1, aConv.NbKnots());
  aConv.KnotsAndMults(aKnots, aMults);
  EXPECT_EQ(aMults(2), aConv.Degree() - 1);
}

TEST(Convert_CompBezierCurvesToBSplineCurveTest, MixedDegreeBeziers)
{
  Convert_CompBezierCurvesToBSplineCurve aConv;

  // Linear segment
  NCollection_Array1<gp_Pnt> aPoles1(1, 2);
  aPoles1(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles1(2) = gp_Pnt(1.0, 0.0, 0.0);

  // Cubic segment
  NCollection_Array1<gp_Pnt> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles2(2) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles2(3) = gp_Pnt(3.0, 1.0, 0.0);
  aPoles2(4) = gp_Pnt(4.0, 0.0, 0.0);

  aConv.AddCurve(aPoles1);
  aConv.AddCurve(aPoles2);
  aConv.Perform();

  // Degree should be raised to the maximum
  EXPECT_EQ(aConv.Degree(), 3);
}

// 2D variant tests
TEST(Convert_CompBezierCurves2dToBSplineCurve2dTest, SingleLinear2d)
{
  Convert_CompBezierCurves2dToBSplineCurve2d aConv;

  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 1.0);

  aConv.AddCurve(aPoles);
  aConv.Perform();

  EXPECT_GE(aConv.Degree(), 1);
  EXPECT_EQ(aConv.NbPoles(), 2);
  EXPECT_EQ(aConv.NbKnots(), 2);

  NCollection_Array1<gp_Pnt2d> aResPoles(1, aConv.NbPoles());
  aConv.Poles(aResPoles);
  EXPECT_NEAR(aResPoles(1).X(), 0.0, 1.0e-15);
  EXPECT_NEAR(aResPoles(2).X(), 1.0, 1.0e-15);
}

TEST(Convert_CompBezierCurves2dToBSplineCurve2dTest, TwoAdjacent2d_C1)
{
  Convert_CompBezierCurves2dToBSplineCurve2d aConv;

  NCollection_Array1<gp_Pnt2d> aPoles1(1, 4);
  aPoles1(1) = gp_Pnt2d(0.0, 0.0);
  aPoles1(2) = gp_Pnt2d(1.0, 1.0);
  aPoles1(3) = gp_Pnt2d(2.0, 1.0);
  aPoles1(4) = gp_Pnt2d(3.0, 0.0);

  NCollection_Array1<gp_Pnt2d> aPoles2(1, 4);
  aPoles2(1) = gp_Pnt2d(3.0, 0.0);
  aPoles2(2) = gp_Pnt2d(4.0, -1.0);
  aPoles2(3) = gp_Pnt2d(5.0, -1.0);
  aPoles2(4) = gp_Pnt2d(6.0, 0.0);

  aConv.AddCurve(aPoles1);
  aConv.AddCurve(aPoles2);
  aConv.Perform();

  EXPECT_EQ(aConv.Degree(), 3);
  EXPECT_EQ(aConv.NbKnots(), 3);

  NCollection_Array1<int>    aMults(1, aConv.NbKnots());
  NCollection_Array1<double> aKnots(1, aConv.NbKnots());
  aConv.KnotsAndMults(aKnots, aMults);
  EXPECT_EQ(aMults(2), aConv.Degree() - 1);
}

//==================================================================================================

TEST(Convert_CompBezierCurvesToBSplineCurveTest, OppositeTangentsKeepCusp3d)
{
  NCollection_Array1<gp_Pnt> aFirst(1, 3);
  NCollection_Array1<gp_Pnt> aSecond(1, 3);
  aFirst(1)  = gp_Pnt(0, 0, 0);
  aFirst(2)  = gp_Pnt(0.5, 0, 0);
  aFirst(3)  = gp_Pnt(1, 0, 0);
  aSecond(1) = aFirst(3);
  aSecond(2) = aFirst(2);
  aSecond(3) = aFirst(1);
  Convert_CompBezierCurvesToBSplineCurve aConverter;
  aConverter.AddCurve(aFirst);
  aConverter.AddCurve(aSecond);
  aConverter.Perform();
  ASSERT_EQ(aConverter.NbKnots(), 3);
  NCollection_Array1<double> aKnots(1, 3);
  NCollection_Array1<int>    aMults(1, 3);
  aConverter.KnotsAndMults(aKnots, aMults);
  EXPECT_EQ(aMults(2), 2);
  NCollection_Array1<gp_Pnt> aPoles(1, aConverter.NbPoles());
  aConverter.Poles(aPoles);
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aKnots, aMults, aConverter.Degree());
  EXPECT_DOUBLE_EQ(aCurve->EvalD0(aKnots(2)).X(), 1.0);
}

//==================================================================================================

TEST(Convert_CompBezierCurvesToBSplineCurveTest, OppositeTangentsKeepCusp2d)
{
  NCollection_Array1<gp_Pnt2d> aFirst(1, 3);
  NCollection_Array1<gp_Pnt2d> aSecond(1, 3);
  aFirst(1)  = gp_Pnt2d(0, 0);
  aFirst(2)  = gp_Pnt2d(0.5, 0);
  aFirst(3)  = gp_Pnt2d(1, 0);
  aSecond(1) = aFirst(3);
  aSecond(2) = aFirst(2);
  aSecond(3) = aFirst(1);
  Convert_CompBezierCurves2dToBSplineCurve2d aConverter;
  aConverter.AddCurve(aFirst);
  aConverter.AddCurve(aSecond);
  aConverter.Perform();
  ASSERT_EQ(aConverter.NbKnots(), 3);
  NCollection_Array1<double> aKnots(1, 3);
  NCollection_Array1<int>    aMults(1, 3);
  aConverter.KnotsAndMults(aKnots, aMults);
  EXPECT_EQ(aMults(2), 2);
  NCollection_Array1<gp_Pnt2d> aPoles(1, aConverter.NbPoles());
  aConverter.Poles(aPoles);
  const occ::handle<Geom2d_BSplineCurve> aCurve =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, aConverter.Degree());
  EXPECT_DOUBLE_EQ(aCurve->EvalD0(aKnots(2)).X(), 1.0);
}
