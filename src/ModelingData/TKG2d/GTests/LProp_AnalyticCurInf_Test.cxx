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

#include <GeomAbs_CurveType.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <LProp_AnalyticCurInf.hxx>
#include <LProp_CIType.hxx>
#include <LProp_CurAndInf.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS

#include <gtest/gtest.h>

#include <cmath>

TEST(LProp_AnalyticCurInfTest, Line_NoExtrema)
{
  // A line has constant zero curvature - no extrema
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Line, 0.0, 10.0, aResult);
  EXPECT_EQ(aResult.NbPoints(), 0);
}

TEST(LProp_AnalyticCurInfTest, Circle_NoExtrema)
{
  // A circle has constant curvature - no extrema
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Circle, 0.0, 2.0 * M_PI, aResult);
  EXPECT_EQ(aResult.NbPoints(), 0);
}

TEST(LProp_AnalyticCurInfTest, Ellipse_FourExtrema)
{
  // An ellipse has 4 curvature extrema in a full period
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Ellipse, 0.0, 2.0 * M_PI, aResult);
  EXPECT_EQ(aResult.NbPoints(), 4);
}

TEST(LProp_AnalyticCurInfTest, Ellipse_ExtremaTypes)
{
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Ellipse, 0.0, 2.0 * M_PI, aResult);
  ASSERT_EQ(aResult.NbPoints(), 4);

  int aNbMin = 0;
  int aNbMax = 0;
  for (int i = 1; i <= aResult.NbPoints(); ++i)
  {
    if (aResult.Type(i) == LProp_MinCur)
      ++aNbMin;
    else if (aResult.Type(i) == LProp_MaxCur)
      ++aNbMax;
  }
  EXPECT_EQ(aNbMin, 2);
  EXPECT_EQ(aNbMax, 2);
}

TEST(LProp_AnalyticCurInfTest, Ellipse_ExtremaAtExpectedParameters)
{
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Ellipse, 0.0, 2.0 * M_PI, aResult);
  ASSERT_EQ(aResult.NbPoints(), 4);

  // Extrema at 0, PI/2, PI, 3*PI/2
  const double anExpected[] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_NEAR(aResult.Parameter(i), anExpected[i - 1], 1e-10);
  }
}

TEST(LProp_AnalyticCurInfTest, Ellipse_PartialRange)
{
  // Only first quadrant: should have 2 extrema (at 0 and PI/2)
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Ellipse, 0.0, M_PI / 2.0, aResult);
  EXPECT_EQ(aResult.NbPoints(), 2);
}

TEST(LProp_AnalyticCurInfTest, Parabola_MinRadiusAtVertex)
{
  // Parabola has minimum radius of curvature (= maximum curvature) at vertex.
  // LProp_AnalyticCurInf reports this as LProp_MinCur (minimum radius of curvature).
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Parabola, -10.0, 10.0, aResult);
  ASSERT_EQ(aResult.NbPoints(), 1);
  EXPECT_NEAR(aResult.Parameter(1), 0.0, 1e-10);
  EXPECT_EQ(aResult.Type(1), LProp_MinCur);
}

TEST(LProp_AnalyticCurInfTest, Hyperbola_MinRadiusAtVertex)
{
  // Hyperbola has minimum radius of curvature at vertex (parameter 0).
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Hyperbola, -5.0, 5.0, aResult);
  ASSERT_EQ(aResult.NbPoints(), 1);
  EXPECT_NEAR(aResult.Parameter(1), 0.0, 1e-10);
  EXPECT_EQ(aResult.Type(1), LProp_MinCur);
}

TEST(LProp_AnalyticCurInfTest, Ellipse_ParametersSorted)
{
  LProp_AnalyticCurInf anAnalyzer;
  LProp_CurAndInf      aResult;

  anAnalyzer.Perform(GeomAbs_Ellipse, 0.0, 2.0 * M_PI, aResult);

  for (int i = 1; i < aResult.NbPoints(); ++i)
  {
    EXPECT_LE(aResult.Parameter(i), aResult.Parameter(i + 1));
  }
}
