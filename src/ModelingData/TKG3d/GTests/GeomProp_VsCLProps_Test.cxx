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

// Cross-validation tests comparing GeomProp_Curve against GeomLProp_CLProps
// for local curve differential properties (tangent, curvature, normal, centre).

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_CLProps.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_LIN_TOL   = Precision::PConfusion();
constexpr double THE_CURV_TOL  = 1.0e-8;
constexpr double THE_DIR_TOL   = 1.0e-6;
constexpr double THE_POINT_TOL = 1.0e-6;

//! Compare tangent from new GeomProp_Curve vs old GeomLProp_CLProps.
void compareTangent(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve                aProp(theCurve);
  const GeomProp::TangentResult aNew = aProp.Tangent(theParam, THE_LIN_TOL);

  GeomLProp_CLProps anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New tangent undefined but old is defined at param=" << theParam;
    gp_Dir anOldDir;
    anOld.Tangent(anOldDir);
    // Tangent directions may differ by sign
    const double aDot = aNew.Direction.Dot(anOldDir);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Tangent direction mismatch at param=" << theParam;
  }
}

//! Compare curvature from new GeomProp_Curve vs old GeomLProp_CLProps.
void compareCurvature(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve                  aProp(theCurve);
  const GeomProp::CurvatureResult aNew = aProp.Curvature(theParam, THE_LIN_TOL);

  GeomLProp_CLProps anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined) << "New curvature undefined at param=" << theParam;
    EXPECT_NEAR(aNew.Value, anOld.Curvature(), THE_CURV_TOL)
      << "Curvature mismatch at param=" << theParam;
  }
}

//! Compare normal from new GeomProp_Curve vs old GeomLProp_CLProps.
void compareNormal(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve               aProp(theCurve);
  const GeomProp::NormalResult aNew = aProp.Normal(theParam, THE_LIN_TOL);

  GeomLProp_CLProps anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined) << "New normal undefined at param=" << theParam;
    gp_Dir anOldNorm;
    anOld.Normal(anOldNorm);
    const double aDot = aNew.Direction.Dot(anOldNorm);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Normal direction mismatch at param=" << theParam;
  }
}

//! Compare centre of curvature from new vs old.
void compareCentre(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve               aProp(theCurve);
  const GeomProp::CentreResult aNew = aProp.CentreOfCurvature(theParam, THE_LIN_TOL);

  GeomLProp_CLProps anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined) << "New centre undefined at param=" << theParam;
    gp_Pnt anOldCentre;
    anOld.CentreOfCurvature(anOldCentre);
    EXPECT_NEAR(aNew.Centre.Distance(anOldCentre), 0.0, THE_POINT_TOL)
      << "Centre mismatch at param=" << theParam;
  }
}

//! Run all comparisons at several parameter values.
void compareAll(const occ::handle<Geom_Curve>& theCurve,
                const double                   theFirst,
                const double                   theLast,
                const int                      theNbSamples = 10)
{
  const double aStep = (theLast - theFirst) / theNbSamples;
  for (int i = 0; i <= theNbSamples; ++i)
  {
    const double aParam = theFirst + i * aStep;
    compareTangent(theCurve, aParam);
    compareCurvature(theCurve, aParam);
    compareNormal(theCurve, aParam);
    compareCentre(theCurve, aParam);
  }
}
} // namespace

// ============================================================================
// Line
// ============================================================================

TEST(GeomProp_VsCLPropsTest, Line)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 0));
  compareAll(aLine, -5.0, 5.0);
}

// ============================================================================
// Circle
// ============================================================================

TEST(GeomProp_VsCLPropsTest, Circle)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  compareAll(aCircle, 0.0, 2.0 * M_PI);
}

// ============================================================================
// Ellipse
// ============================================================================

TEST(GeomProp_VsCLPropsTest, Ellipse)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  compareAll(anEllipse, 0.0, 2.0 * M_PI);
}

// ============================================================================
// Hyperbola
// ============================================================================

TEST(GeomProp_VsCLPropsTest, Hyperbola)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -2.0, 2.0);
}

// ============================================================================
// Parabola
// ============================================================================

TEST(GeomProp_VsCLPropsTest, Parabola)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -5.0, 5.0);
}

// ============================================================================
// Bezier
// ============================================================================

TEST(GeomProp_VsCLPropsTest, BezierCubic)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1)                             = gp_Pnt(0, 0, 0);
  aPoles(2)                             = gp_Pnt(1, 2, 1);
  aPoles(3)                             = gp_Pnt(3, -1, 0);
  aPoles(4)                             = gp_Pnt(4, 1, 1);
  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  compareAll(aBezier, 0.0, 1.0);
}

// ============================================================================
// BSpline
// ============================================================================

TEST(GeomProp_VsCLPropsTest, BSplineCubic)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(2, 1, 1);
  aPoles(4) = gp_Pnt(3, 4, 0);
  aPoles(5) = gp_Pnt(4, 2, 1);
  aPoles(6) = gp_Pnt(5, 0, 0);

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

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  compareAll(aBSpline, 0.0, 1.0);
}

// ============================================================================
// Offset curve
// ============================================================================

TEST(GeomProp_VsCLPropsTest, OffsetCircle)
{
  gp_Circ                       aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle>      aCircle  = new Geom_Circle(aCirc);
  occ::handle<Geom_OffsetCurve> anOffset = new Geom_OffsetCurve(aCircle, 2.0, gp_Dir(0, 0, 1));
  compareAll(anOffset, 0.0, 2.0 * M_PI);
}
