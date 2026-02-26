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

// Cross-validation tests comparing Geom2dProp_Curve against Geom2dLProp_CLProps2d
// for local differential properties (tangent, curvature, normal, centre of curvature).

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <Geom2dLProp_CLProps2d.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

  namespace
{
  constexpr double THE_PARAM_TOL = Precision::Confusion();
  constexpr double THE_VALUE_TOL = 1.0e-10;
  constexpr double THE_DIR_TOL   = 1.0e-10;
  constexpr double THE_POINT_TOL = 1.0e-8;

  //! Compare tangent from new Geom2dProp vs old CLProps2d at given parameter.
  void compareTangent(Geom2dProp_Curve & theProp,
                      Geom2dLProp_CLProps2d & theOld,
                      const double theParam,
                      const ::testing::TestInfo* = nullptr)
  {
    theOld.SetParameter(theParam);

    const Geom2dProp::TangentResult aNewTan     = theProp.Tangent(theParam, THE_PARAM_TOL);
    const bool                      aOldDefined = theOld.IsTangentDefined();

    EXPECT_EQ(aNewTan.IsDefined, aOldDefined) << "Tangent defined mismatch at U=" << theParam;

    if (aNewTan.IsDefined && aOldDefined)
    {
      gp_Dir2d aOldDir;
      theOld.Tangent(aOldDir);
      // Tangent directions may differ by sign; compare absolute dot product
      const double aDot = aNewTan.Direction.X() * aOldDir.X() + aNewTan.Direction.Y() * aOldDir.Y();
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Tangent direction mismatch at U=" << theParam;
    }
  }

  //! Compare curvature from new Geom2dProp vs old CLProps2d at given parameter.
  void compareCurvature(Geom2dProp_Curve & theProp,
                        Geom2dLProp_CLProps2d & theOld,
                        const double theParam)
  {
    theOld.SetParameter(theParam);

    const Geom2dProp::CurvatureResult aNewCurv = theProp.Curvature(theParam, THE_PARAM_TOL);
    const double                      aOldCurv = theOld.Curvature();

    if (aNewCurv.IsDefined && !aNewCurv.IsInfinite)
    {
      EXPECT_NEAR(aNewCurv.Value, aOldCurv, THE_VALUE_TOL)
        << "Curvature mismatch at U=" << theParam;
    }
  }

  //! Compare normal from new Geom2dProp vs old CLProps2d at given parameter.
  void compareNormal(Geom2dProp_Curve & theProp,
                     Geom2dLProp_CLProps2d & theOld,
                     const double theParam)
  {
    theOld.SetParameter(theParam);

    const Geom2dProp::NormalResult aNewNorm = theProp.Normal(theParam, THE_PARAM_TOL);

    // Old API: Normal() throws if curvature is nearly zero; curvature check needed
    const double aOldCurv = theOld.Curvature();
    if (std::abs(aOldCurv) < THE_PARAM_TOL)
    {
      // Old API would throw - new API returns IsDefined=false
      EXPECT_FALSE(aNewNorm.IsDefined) << "Normal should be undefined at U=" << theParam;
      return;
    }

    if (aNewNorm.IsDefined)
    {
      gp_Dir2d aOldDir;
      theOld.Normal(aOldDir);
      const double aDot =
        aNewNorm.Direction.X() * aOldDir.X() + aNewNorm.Direction.Y() * aOldDir.Y();
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
        << "Normal direction mismatch at U=" << theParam;
    }
  }

  //! Compare centre of curvature from new Geom2dProp vs old CLProps2d at given parameter.
  void compareCentre(Geom2dProp_Curve & theProp,
                     Geom2dLProp_CLProps2d & theOld,
                     const double theParam)
  {
    theOld.SetParameter(theParam);

    const Geom2dProp::CentreResult aNewCentre = theProp.CentreOfCurvature(theParam, THE_PARAM_TOL);

    // Old API: CentreOfCurvature() throws if curvature is nearly zero
    const double aOldCurv = theOld.Curvature();
    if (std::abs(aOldCurv) < THE_PARAM_TOL)
    {
      EXPECT_FALSE(aNewCentre.IsDefined) << "Centre should be undefined at U=" << theParam;
      return;
    }

    if (aNewCentre.IsDefined)
    {
      gp_Pnt2d aOldCentre;
      theOld.CentreOfCurvature(aOldCentre);
      EXPECT_NEAR(aNewCentre.Centre.X(), aOldCentre.X(), THE_POINT_TOL)
        << "Centre X mismatch at U=" << theParam;
      EXPECT_NEAR(aNewCentre.Centre.Y(), aOldCentre.Y(), THE_POINT_TOL)
        << "Centre Y mismatch at U=" << theParam;
    }
  }

  //! Run all four property comparisons at given parameter.
  void compareAllProperties(Geom2dProp_Curve & theProp,
                            Geom2dLProp_CLProps2d & theOld,
                            const double theParam)
  {
    compareTangent(theProp, theOld, theParam);
    compareCurvature(theProp, theOld, theParam);
    compareNormal(theProp, theOld, theParam);
    compareCentre(theProp, theOld, theParam);
  }

} // namespace

// ============================================================================
// Line
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, Line_Tangent)
{
  gp_Lin2d                 aLin(gp_Pnt2d(1.0, 2.0), gp_Dir2d(3.0, 4.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve      aProp(aLine);
  Geom2dLProp_CLProps2d aOld(aLine, 2, THE_PARAM_TOL);

  for (double u = -10.0; u <= 10.0; u += 2.5)
  {
    compareTangent(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Line_Curvature)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve      aProp(aLine);
  Geom2dLProp_CLProps2d aOld(aLine, 2, THE_PARAM_TOL);

  for (double u = -5.0; u <= 5.0; u += 1.0)
  {
    compareCurvature(aProp, aOld, u);
  }
}

// ============================================================================
// Circle
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, Circle_AllProperties)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(3.0, 4.0), gp_Dir2d(1.0, 0.0)), 7.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Circle_SmallRadius)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.01);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Circle_LargeRadius)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1000.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// Ellipse
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, Ellipse_AllProperties)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 12.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Ellipse_HighEccentricity)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 100.0, 1.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 8.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Ellipse_OffCenter)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(100.0, -50.0), gp_Dir2d(1.0, 0.0)), 8.0, 3.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 8.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// Hyperbola
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, Hyperbola_AllProperties)
{
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_PARAM_TOL);

  for (double u = -2.0; u <= 2.0; u += 0.5)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Hyperbola_NearVertex)
{
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 4.0, 2.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_PARAM_TOL);

  // Fine-grained near vertex
  for (double u = -0.5; u <= 0.5; u += 0.1)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// Parabola
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, Parabola_AllProperties)
{
  gp_Parab2d                   aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(aParab);

  Geom2dProp_Curve      aProp(aParabola);
  Geom2dLProp_CLProps2d aOld(aParabola, 2, THE_PARAM_TOL);

  for (double u = -5.0; u <= 5.0; u += 1.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Parabola_SmallFocal)
{
  gp_Parab2d                   aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.1);
  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(aParab);

  Geom2dProp_Curve      aProp(aParabola);
  Geom2dLProp_CLProps2d aOld(aParabola, 2, THE_PARAM_TOL);

  for (double u = -3.0; u <= 3.0; u += 0.5)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Parabola_LargeFocal)
{
  gp_Parab2d                   aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 50.0);
  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(aParab);

  Geom2dProp_Curve      aProp(aParabola);
  Geom2dLProp_CLProps2d aOld(aParabola, 2, THE_PARAM_TOL);

  for (double u = -10.0; u <= 10.0; u += 2.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// Bezier curve
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, Bezier_CubicSShape)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 2.0);
  aPoles(3)                               = gp_Pnt2d(3.0, -2.0);
  aPoles(4)                               = gp_Pnt2d(4.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_PARAM_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Bezier_Quadratic)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(2.0, 4.0);
  aPoles(3)                               = gp_Pnt2d(4.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_PARAM_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, Bezier_HighDegree)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 3.0);
  aPoles(3)                               = gp_Pnt2d(2.0, -1.0);
  aPoles(4)                               = gp_Pnt2d(3.0, 2.0);
  aPoles(5)                               = gp_Pnt2d(4.0, -2.0);
  aPoles(6)                               = gp_Pnt2d(5.0, 1.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_PARAM_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.05)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// BSpline curve
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, BSpline_Quadratic)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 2.0);
  aPoles(3) = gp_Pnt2d(3.0, 2.0);
  aPoles(4) = gp_Pnt2d(4.0, 0.0);

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;

  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 3;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);

  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_PARAM_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, BSpline_Cubic)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, 1.0);
  aPoles(4) = gp_Pnt2d(3.0, 4.0);
  aPoles(5) = gp_Pnt2d(4.0, 2.0);
  aPoles(6) = gp_Pnt2d(5.0, 0.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.66;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 4;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);

  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_PARAM_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.05)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, BSpline_Degree4)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, -1.0);
  aPoles(4) = gp_Pnt2d(3.0, 2.0);
  aPoles(5) = gp_Pnt2d(4.0, 0.0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 5;
  aMults(2) = 5;

  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 4);

  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_PARAM_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.05)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// Offset curve
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, OffsetCircle_AllProperties)
{
  gp_Circ2d                       aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle>      aCircle  = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 2.0);

  Geom2dProp_Curve      aProp(anOffset);
  Geom2dLProp_CLProps2d aOld(anOffset, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, OffsetEllipse_AllProperties)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse    = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(anEllipse, 1.0);

  Geom2dProp_Curve      aProp(anOffset);
  Geom2dLProp_CLProps2d aOld(anOffset, 2, THE_PARAM_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 8.0)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

// ============================================================================
// Trimmed curve
// ============================================================================

TEST(Geom2dProp_VsCLProps2dTest, TrimmedEllipse_AllProperties)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 8.0, 4.0);
  occ::handle<Geom2d_Ellipse> anEllipse     = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(anEllipse, 0.5, 2.5);

  Geom2dProp_Curve      aProp(aTrimmed);
  Geom2dLProp_CLProps2d aOld(aTrimmed, 2, THE_PARAM_TOL);

  for (double u = 0.5; u <= 2.5; u += 0.2)
  {
    compareAllProperties(aProp, aOld, u);
  }
}

TEST(Geom2dProp_VsCLProps2dTest, TrimmedBezier_AllProperties)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                                 = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                                 = gp_Pnt2d(1.0, 3.0);
  aPoles(3)                                 = gp_Pnt2d(3.0, -1.0);
  aPoles(4)                                 = gp_Pnt2d(4.0, 1.0);
  occ::handle<Geom2d_BezierCurve>  aBezier  = new Geom2d_BezierCurve(aPoles);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aBezier, 0.2, 0.8);

  Geom2dProp_Curve      aProp(aTrimmed);
  Geom2dLProp_CLProps2d aOld(aTrimmed, 2, THE_PARAM_TOL);

  for (double u = 0.2; u <= 0.8; u += 0.1)
  {
    compareAllProperties(aProp, aOld, u);
  }
}
