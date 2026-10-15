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
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <GeomLProp_CurAndInf2d.hxx>
#include <Geom2d_Parabola.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <LProp_CIType.hxx>
#include <Precision.hxx>

#include <cmath>

#include <gtest/gtest.h>

TEST(GeomLProp_CurAndInf2dKnotTest, CurvatureMaximumAtC2Knot)
{
  for (double aSign : {-1.0, 1.0})
  {
    for (double anOrigin : {0.0, 0.37, 100.0})
    {
      for (double aRightCubic : {1.0, 4.0, 10.0})
      {
        SCOPED_TRACE(::testing::Message() << aSign << " " << anOrigin << " " << aRightCubic);
        constexpr double             aSpan = 0.1;
        const double                 aY[]  = {aSpan * aSpan - aSpan * aSpan * aSpan,
                                              aSpan * aSpan / 3.0,
                                              0.0,
                                              0.0,
                                              0.0,
                                              aSpan * aSpan / 3.0,
                                              aSpan * aSpan - aRightCubic * aSpan * aSpan * aSpan};
        NCollection_Array1<gp_Pnt2d> aPoles(1, 7);
        for (size_t i = 0; i < aPoles.Size(); ++i)
        {
          aPoles.ChangeAt(i) =
            gp_Pnt2d((static_cast<double>(i) - 3.0) * aSpan / 3.0, aSign * aY[i]);
        }
        NCollection_Array1<double> aKnots(1, 3);
        aKnots.ChangeAt(0) = anOrigin - aSpan;
        aKnots.ChangeAt(1) = anOrigin;
        aKnots.ChangeAt(2) = anOrigin + aSpan;
        NCollection_Array1<int> aMultiplicities(1, 3);
        aMultiplicities.ChangeAt(0) = 4;
        aMultiplicities.ChangeAt(1) = 3;
        aMultiplicities.ChangeAt(2) = 4;
        occ::handle<Geom2d_BSplineCurve> aCurve =
          new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 3);
        ASSERT_TRUE(aCurve->RemoveKnot(2, 1, 1.e-12));

        GeomLProp_CurAndInf2d anAnalyzer;
        anAnalyzer.PerformCurExt(aCurve);
        ASSERT_TRUE(anAnalyzer.IsDone());
        int aNbKnotExtrema = 0;
        for (int i = 1; i <= anAnalyzer.NbPoints(); ++i)
        {
          if (std::abs(anAnalyzer.Parameter(i) - anOrigin) <= Precision::PConfusion())
          {
            ++aNbKnotExtrema;
            EXPECT_DOUBLE_EQ(anAnalyzer.Parameter(i), anOrigin);
            EXPECT_EQ(anAnalyzer.Type(i), LProp_MinCur);
          }
        }
        EXPECT_EQ(aNbKnotExtrema, 1);
      }
    }
  }
}

class GeomLProp_CurAndInf2dTest : public ::testing::Test
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

    gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
    myHyperbola = new Geom2d_Hyperbola(aHypr);

    gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
    myParabola = new Geom2d_Parabola(aParab);
  }

  occ::handle<Geom2d_Circle>    myCircle;
  occ::handle<Geom2d_Ellipse>   myEllipse;
  occ::handle<Geom2d_Hyperbola> myHyperbola;
  occ::handle<Geom2d_Parabola>  myParabola;
};

TEST_F(GeomLProp_CurAndInf2dTest, Circle_Perform_NoInflections)
{
  // A circle has constant curvature: no inflections or extrema
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.Perform(myCircle);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(GeomLProp_CurAndInf2dTest, Circle_PerformInf_NoInflections)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformInf(myCircle);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(GeomLProp_CurAndInf2dTest, Circle_PerformCurExt_NoExtrema)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myCircle);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(GeomLProp_CurAndInf2dTest, Ellipse_PerformCurExt_HasExtrema)
{
  // An ellipse has 4 curvature extrema:
  // 2 maxima (at ends of minor axis) and 2 minima (at ends of major axis)
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 4);
}

TEST_F(GeomLProp_CurAndInf2dTest, Ellipse_PerformCurExt_Types)
{
  GeomLProp_CurAndInf2d aAnalyzer;
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
    {
      ++aNbMin;
    }
    else if (aType == LProp_MaxCur)
    {
      ++aNbMax;
    }
  }
  EXPECT_EQ(aNbMin, 2);
  EXPECT_EQ(aNbMax, 2);
}

TEST_F(GeomLProp_CurAndInf2dTest, Ellipse_PerformCurExt_Parameters)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 4);

  // Parameters should be sorted in increasing order
  for (int i = 1; i < aAnalyzer.NbPoints(); ++i)
  {
    EXPECT_LT(aAnalyzer.Parameter(i), aAnalyzer.Parameter(i + 1));
  }
}

TEST_F(GeomLProp_CurAndInf2dTest, Ellipse_PerformInf_NoInflections)
{
  // An ellipse is a convex curve, so it has no inflection points
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformInf(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(GeomLProp_CurAndInf2dTest, Ellipse_Perform_CombinedResult)
{
  // Perform computes both inflections and curvature extrema
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.Perform(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  // Ellipse: 4 extrema, 0 inflections
  EXPECT_EQ(aAnalyzer.NbPoints(), 4);
}

TEST_F(GeomLProp_CurAndInf2dTest, Ellipse_CurvatureExtremaAtExpectedParameters)
{
  // For an ellipse with major axis along X:
  // Curvature extrema at U = 0, PI/2, PI, 3*PI/2
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 4);

  const double anExpectedParams[] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_NEAR(aAnalyzer.Parameter(i), anExpectedParams[i - 1], 1e-6);
  }
}

TEST_F(GeomLProp_CurAndInf2dTest, Hyperbola_PerformCurExt_VertexOnly)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myHyperbola);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 1);
  EXPECT_NEAR(aAnalyzer.Parameter(1), 0.0, Precision::PConfusion());
  EXPECT_EQ(aAnalyzer.Type(1), LProp_MinCur);
}

TEST_F(GeomLProp_CurAndInf2dTest, Hyperbola_PerformInf_NoInflections)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformInf(myHyperbola);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}

TEST_F(GeomLProp_CurAndInf2dTest, Parabola_PerformCurExt_VertexOnly)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myParabola);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 1);
  EXPECT_NEAR(aAnalyzer.Parameter(1), 0.0, Precision::PConfusion());
  EXPECT_EQ(aAnalyzer.Type(1), LProp_MinCur);
}

TEST_F(GeomLProp_CurAndInf2dTest, PerformInf_ClearsPreviousExtrema)
{
  GeomLProp_CurAndInf2d aAnalyzer;
  aAnalyzer.PerformCurExt(myEllipse);

  ASSERT_TRUE(aAnalyzer.IsDone());
  ASSERT_EQ(aAnalyzer.NbPoints(), 4);

  aAnalyzer.PerformInf(myEllipse);

  EXPECT_TRUE(aAnalyzer.IsDone());
  EXPECT_EQ(aAnalyzer.NbPoints(), 0);
}
