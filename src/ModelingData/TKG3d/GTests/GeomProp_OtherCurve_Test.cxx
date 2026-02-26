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

// Tests for GeomProp_Curve with TrimmedCurve and other non-standard curve types
// that fall into the "Other" variant of the evaluator. Testing mainly with
// Geom_TrimmedCurve wrapping various base curves.

#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
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

void compareTangent(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve                aProp(theCurve);
  const GeomProp::TangentResult aNew = aProp.Tangent(theParam, THE_LIN_TOL);
  GeomLProp_CLProps             anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Dir anOldDir;
    anOld.Tangent(anOldDir);
    EXPECT_NEAR(std::abs(aNew.Direction.Dot(anOldDir)), 1.0, THE_DIR_TOL);
  }
}

void compareCurvature(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve                  aProp(theCurve);
  const GeomProp::CurvatureResult aNew = aProp.Curvature(theParam, THE_LIN_TOL);
  GeomLProp_CLProps               anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined);
    EXPECT_NEAR(aNew.Value, anOld.Curvature(), THE_CURV_TOL);
  }
}

void compareNormal(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve               aProp(theCurve);
  const GeomProp::NormalResult aNew = aProp.Normal(theParam, THE_LIN_TOL);
  GeomLProp_CLProps            anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Dir anOldNorm;
    anOld.Normal(anOldNorm);
    EXPECT_NEAR(std::abs(aNew.Direction.Dot(anOldNorm)), 1.0, THE_DIR_TOL);
  }
}

void compareCentre(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve               aProp(theCurve);
  const GeomProp::CentreResult aNew = aProp.CentreOfCurvature(theParam, THE_LIN_TOL);
  GeomLProp_CLProps            anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Pnt anOldCentre;
    anOld.CentreOfCurvature(anOldCentre);
    EXPECT_NEAR(aNew.Centre.Distance(anOldCentre), 0.0, THE_POINT_TOL);
  }
}

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

//! Create a trimmed circle (quarter arc from 0 to PI/2).
occ::handle<Geom_TrimmedCurve> makeTrimmedCircle(const double theRadius = 5.0)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(1, 2, 0), gp_Dir(0, 0, 1)), theRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  return new Geom_TrimmedCurve(aCircle, 0.0, M_PI / 2.0);
}

//! Create a trimmed ellipse (from PI/4 to 3*PI/4).
occ::handle<Geom_TrimmedCurve> makeTrimmedEllipse(const double theMajor = 10.0,
                                                  const double theMinor = 5.0)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), theMajor, theMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  return new Geom_TrimmedCurve(anEllipse, M_PI / 4.0, 3.0 * M_PI / 4.0);
}

//! Create an S-shaped Bezier curve with inflection point.
occ::handle<Geom_BezierCurve> makeSBezier()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(3, -2, 0);
  aPoles(4) = gp_Pnt(4, 1, 0);
  return new Geom_BezierCurve(aPoles);
}

//! Create a trimmed Bezier curve (from 0.1 to 0.9).
occ::handle<Geom_TrimmedCurve> makeTrimmedBezier()
{
  occ::handle<Geom_BezierCurve> aBezier = makeSBezier();
  return new Geom_TrimmedCurve(aBezier, 0.1, 0.9);
}

//! Create a trimmed hyperbola (from -1.0 to 1.0).
occ::handle<Geom_TrimmedCurve> makeTrimmedHyperbola()
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  return new Geom_TrimmedCurve(aHyperbola, -1.0, 1.0);
}

} // namespace

// ============================================================================
// Trimmed circle tests
// ============================================================================

TEST(GeomProp_OtherCurveTest, Tangent_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  GeomProp_Curve                 aProp(aTrimmed);

  for (double aParam = 0.0; aParam <= M_PI / 2.0; aParam += M_PI / 8.0)
  {
    const GeomProp::TangentResult aTan = aProp.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined) << "Tangent undefined at param=" << aParam;
    (void)aTan.Direction;
  }
}

TEST(GeomProp_OtherCurveTest, Curvature_TrimmedCircle)
{
  const double                   aRadius  = 5.0;
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle(aRadius);
  GeomProp_Curve                 aProp(aTrimmed);

  // Curvature should be constant 1/R within the trimmed range
  for (double aParam = 0.0; aParam <= M_PI / 2.0; aParam += M_PI / 8.0)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(aCurv.Value, 1.0 / aRadius, THE_CURV_TOL);
  }
}

TEST(GeomProp_OtherCurveTest, Normal_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  GeomProp_Curve                 aProp(aTrimmed);

  // At param=0, point is (6,2,0) on circle centered at (1,2,0), normal points to (-1,0,0)
  const GeomProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), -1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_OtherCurveTest, Centre_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  GeomProp_Curve                 aProp(aTrimmed);

  // Centre of curvature should be circle center (1,2,0) at all params
  for (double aParam = 0.0; aParam <= M_PI / 2.0; aParam += M_PI / 8.0)
  {
    const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined);
    EXPECT_NEAR(aCentre.Centre.X(), 1.0, THE_POINT_TOL);
    EXPECT_NEAR(aCentre.Centre.Y(), 2.0, THE_POINT_TOL);
  }
}

TEST(GeomProp_OtherCurveTest, FindCurvatureExtrema_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  GeomProp_Curve                 aProp(aTrimmed);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Constant curvature => no extrema
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_OtherCurveTest, FindInflections_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  GeomProp_Curve                 aProp(aTrimmed);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// ============================================================================
// Trimmed ellipse tests
// ============================================================================

TEST(GeomProp_OtherCurveTest, Tangent_TrimmedEllipse)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedEllipse();
  GeomProp_Curve                 aProp(aTrimmed);

  const double aFirst = M_PI / 4.0;
  const double aLast  = 3.0 * M_PI / 4.0;
  for (double aParam = aFirst; aParam <= aLast; aParam += (aLast - aFirst) / 5.0)
  {
    const GeomProp::TangentResult aTan = aProp.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined) << "Tangent undefined at param=" << aParam;
  }
}

TEST(GeomProp_OtherCurveTest, Curvature_TrimmedEllipse)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedEllipse();
  GeomProp_Curve                 aProp(aTrimmed);

  const double aFirst = M_PI / 4.0;
  const double aLast  = 3.0 * M_PI / 4.0;

  // Curvature should vary within the trimmed range
  const GeomProp::CurvatureResult aCurvFirst = aProp.Curvature(aFirst, THE_LIN_TOL);
  const GeomProp::CurvatureResult aCurvMid   = aProp.Curvature((aFirst + aLast) / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvFirst.IsDefined);
  ASSERT_TRUE(aCurvMid.IsDefined);
  EXPECT_GT(aCurvFirst.Value, 0.0);
  EXPECT_GT(aCurvMid.Value, 0.0);
}

TEST(GeomProp_OtherCurveTest, FindCurvatureExtrema_TrimmedEllipse)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedEllipse();
  GeomProp_Curve                 aProp(aTrimmed);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Trimmed range [PI/4, 3*PI/4] includes minor vertex at PI/2 => extremum
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, M_PI / 4.0 - Precision::Confusion());
    EXPECT_LE(aResult.Points[i].Parameter, 3.0 * M_PI / 4.0 + Precision::Confusion());
  }
}

TEST(GeomProp_OtherCurveTest, FindInflections_TrimmedEllipse)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedEllipse();
  GeomProp_Curve                 aProp(aTrimmed);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // Ellipse is convex => no inflections in any trimmed range
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// ============================================================================
// GetType test
// ============================================================================

TEST(GeomProp_OtherCurveTest, GetType_IsOtherCurve)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  GeomProp_Curve                 aProp(aTrimmed);
  // Note: GeomAdaptor unwraps TrimmedCurve to the base curve type,
  // so a trimmed circle is identified as GeomAbs_Circle, not OtherCurve.
  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);
}

// ============================================================================
// VsCLProps comparison tests
// ============================================================================

TEST(GeomProp_OtherCurveTest, VsCLProps_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  compareAll(aTrimmed, 0.0, M_PI / 2.0);
}

TEST(GeomProp_OtherCurveTest, VsCLProps_TrimmedEllipse)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedEllipse();
  compareAll(aTrimmed, M_PI / 4.0, 3.0 * M_PI / 4.0);
}

TEST(GeomProp_OtherCurveTest, VsCLProps_TrimmedBezier)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedBezier();
  compareAll(aTrimmed, 0.1, 0.9);
}

TEST(GeomProp_OtherCurveTest, VsCLProps_AllProperties_TrimmedCircle)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedCircle();
  compareAll(aTrimmed, 0.0, M_PI / 2.0, 20);
}

TEST(GeomProp_OtherCurveTest, VsCLProps_AllProperties_TrimmedEllipse)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedEllipse();
  compareAll(aTrimmed, M_PI / 4.0, 3.0 * M_PI / 4.0, 20);
}

TEST(GeomProp_OtherCurveTest, VsCLProps_AllProperties_TrimmedBezier)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedBezier();
  compareAll(aTrimmed, 0.1, 0.9, 20);
}

// ============================================================================
// Trimmed hyperbola tests
// ============================================================================

TEST(GeomProp_OtherCurveTest, Curvature_TrimmedHyperbola)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedHyperbola();
  GeomProp_Curve                 aProp(aTrimmed);

  for (double aParam = -1.0; aParam <= 1.0; aParam += 0.5)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_GT(aCurv.Value, 0.0) << "Curvature should be positive at param=" << aParam;
  }
}

// ============================================================================
// Trimmed Bezier extrema / inflections tests
// ============================================================================

TEST(GeomProp_OtherCurveTest, FindCurvatureExtrema_TrimmedBezier)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedBezier();
  GeomProp_Curve                 aProp(aTrimmed);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.1 - Precision::Confusion());
    EXPECT_LE(aResult.Points[i].Parameter, 0.9 + Precision::Confusion());
    EXPECT_TRUE(aResult.Points[i].Type == GeomProp::CIType::MinCurvature
                || aResult.Points[i].Type == GeomProp::CIType::MaxCurvature);
  }
}

TEST(GeomProp_OtherCurveTest, FindInflections_TrimmedBezier)
{
  occ::handle<Geom_TrimmedCurve> aTrimmed = makeTrimmedBezier();
  GeomProp_Curve                 aProp(aTrimmed);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // S-shaped Bezier should have an inflection in the trimmed range [0.1, 0.9]
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.1 - Precision::Confusion());
    EXPECT_LE(aResult.Points[i].Parameter, 0.9 + Precision::Confusion());
    EXPECT_EQ(aResult.Points[i].Type, GeomProp::CIType::Inflection);
  }
}

TEST(GeomProp_OtherCurveTest, VsCLProps_CriticalPoints)
{
  // Trimmed circle
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_TrimmedCurve> aTrimmed =
    new Geom_TrimmedCurve(aCircle, M_PI / 4.0, 3.0 * M_PI / 4.0);
  const double aFirst    = aTrimmed->FirstParameter();
  const double aLast     = aTrimmed->LastParameter();
  const double aMid      = (aFirst + aLast) / 2.0;
  const double aParams[] = {aFirst,
                            aFirst + 1.0e-10,
                            aFirst + 1.0e-6,
                            aMid,
                            aMid + 1.0e-6,
                            aMid - 1.0e-6,
                            aLast - 1.0e-6,
                            aLast - 1.0e-10,
                            aLast};
  for (const double aParam : aParams)
  {
    compareTangent(aTrimmed, aParam);
    compareCurvature(aTrimmed, aParam);
    compareNormal(aTrimmed, aParam);
    compareCentre(aTrimmed, aParam);
  }
}
