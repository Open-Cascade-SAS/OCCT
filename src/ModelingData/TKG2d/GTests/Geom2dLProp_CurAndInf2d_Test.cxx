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

#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <Geom2dLProp_CurAndInf2d.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <LProp_CIType.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
#include <Precision.hxx>

#include <gtest/gtest.h>

class Geom2dLProp_CurAndInf2dTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Circle centered at origin, radius 5
    gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
    myCircle = new Geom2d_Circle(aCirc);

    // Ellipse centered at origin, major radius 10, minor radius 3
    gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 3.0);
    myEllipse = new Geom2d_Ellipse(anElips);
  }

  occ::handle<Geom2d_Circle>  myCircle;
  occ::handle<Geom2d_Ellipse> myEllipse;
};

TEST_F(Geom2dLProp_CurAndInf2dTest, Circle_Perform_NoInflections)
{
  // A circle has constant curvature: no inflections or extrema
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.Perform(myCircle);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Circle_PerformInf_NoInflections)
{
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformInf(myCircle);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Circle_PerformCurExt_NoExtrema)
{
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myCircle);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Ellipse_PerformCurExt_HasExtrema)
{
  // An ellipse has 4 curvature extrema:
  // 2 maxima (at ends of minor axis) and 2 minima (at ends of major axis)
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 4);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Ellipse_PerformCurExt_Types)
{
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 4);

  // Count min and max curvature points
  int aNbMin = 0;
  int aNbMax = 0;
  for (int i = 1; i <= aAnalyzer.NbPoints(); ++i)
  {
    const LProp_CIType aType = aAnalyzer.Type(i);
    if (aType == LProp_MinCur)
      ++aNbMin;
    else if (aType == LProp_MaxCur)
      ++aNbMax;
  }
  EXPECT_EQ(aNbMin, 2);
  EXPECT_EQ(aNbMax, 2);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Ellipse_PerformCurExt_Parameters)
{
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 4);

  // Parameters should be sorted in increasing order
  for (int i = 1; i < aAnalyzer.NbPoints(); ++i)
  {
    EXPECT_LT(aAnalyzer.Parameter(i), aAnalyzer.Parameter(i + 1));
  }
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Ellipse_PerformInf_NoInflections)
{
  // An ellipse is a convex curve, so it has no inflection points
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformInf(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Ellipse_Perform_CombinedResult)
{
  // Perform computes both inflections and curvature extrema
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.Perform(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  // Ellipse: 4 extrema, 0 inflections
  EXPECT_EQ(aAnalyzer.NbPoints(), 4);
}

TEST_F(Geom2dLProp_CurAndInf2dTest, Ellipse_CurvatureExtremaAtExpectedParameters)
{
  // For an ellipse with major axis along X:
  // Curvature extrema at U = 0, PI/2, PI, 3*PI/2
  Geom2dLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 4);

  const double anExpectedParams[] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_NEAR(aAnalyzer.Parameter(i), anExpectedParams[i - 1], 1e-6);
  }
}
