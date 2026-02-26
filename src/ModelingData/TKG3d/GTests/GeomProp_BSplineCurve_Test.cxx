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

// Tests for GeomProp_Curve with BSpline curves: piecewise polynomial curves
// with knots, span-by-span analysis, and numerical property evaluation.

#include <Geom_BSplineCurve.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Dir.hxx>
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

//! Create a cubic C2 BSpline with 6 poles, knots [0, 0.33, 0.66, 1], mults [4,1,1,4].
occ::handle<Geom_BSplineCurve> makeCubicC2()
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

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
}

//! Create a quadratic C1 BSpline with 5 poles, knots [0, 0.33, 0.66, 1], mults [3,1,1,3].
occ::handle<Geom_BSplineCurve> makeQuadraticC1()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, -1, 1);
  aPoles(4) = gp_Pnt(3, 3, 0);
  aPoles(5) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.66;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 3;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
}

//! Create a degree 4 single-span BSpline (Bezier-like), knots [0,1], mults [5,5].
occ::handle<Geom_BSplineCurve> makeDegree4()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 4, 0);
  aPoles(3) = gp_Pnt(2, -2, 1);
  aPoles(4) = gp_Pnt(3, 3, 0);
  aPoles(5) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 5;
  aMults(2) = 5;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 4);
}

//! Create a multi-span cubic BSpline with 8 poles and 6 knots.
occ::handle<Geom_BSplineCurve> makeMultiSpanCubic()
{
  NCollection_Array1<gp_Pnt> aPoles(1, 8);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(2, -1, 1);
  aPoles(4) = gp_Pnt(3, 2, 0);
  aPoles(5) = gp_Pnt(4, -2, 1);
  aPoles(6) = gp_Pnt(5, 3, 0);
  aPoles(7) = gp_Pnt(6, 1, 1);
  aPoles(8) = gp_Pnt(7, 0, 0);

  NCollection_Array1<double> aKnots(1, 6);
  aKnots(1) = 0.0;
  aKnots(2) = 0.2;
  aKnots(3) = 0.4;
  aKnots(4) = 0.6;
  aKnots(5) = 0.8;
  aKnots(6) = 1.0;

  NCollection_Array1<int> aMults(1, 6);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 1;
  aMults(4) = 1;
  aMults(5) = 1;
  aMults(6) = 4;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
}

} // namespace

// ============================================================================
// Tangent tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, Tangent_CubicEndpoints)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::TangentResult aTanFirst = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanFirst.IsDefined);
  // gp_Dir is always unit length; just verify it's defined
  (void)aTanFirst.Direction;

  const GeomProp::TangentResult aTanLast = aProp.Tangent(1.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanLast.IsDefined);
  (void)aTanLast.Direction;
}

TEST(GeomProp_BSplineCurveTest, Tangent_AtKnots)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  // Internal knots at 0.33 and 0.66
  const GeomProp::TangentResult aTan1 = aProp.Tangent(0.33, THE_LIN_TOL);
  ASSERT_TRUE(aTan1.IsDefined);

  const GeomProp::TangentResult aTan2 = aProp.Tangent(0.66, THE_LIN_TOL);
  ASSERT_TRUE(aTan2.IsDefined);
}

// ============================================================================
// Curvature tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, Curvature_CubicSmooth)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  for (double aParam = 0.0; aParam <= 1.0; aParam += 0.25)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_GE(aCurv.Value, 0.0) << "Curvature negative at param=" << aParam;
  }
}

TEST(GeomProp_BSplineCurveTest, Curvature_AtKnots)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurvatureResult aCurv1 = aProp.Curvature(0.33, THE_LIN_TOL);
  ASSERT_TRUE(aCurv1.IsDefined);

  const GeomProp::CurvatureResult aCurv2 = aProp.Curvature(0.66, THE_LIN_TOL);
  ASSERT_TRUE(aCurv2.IsDefined);
}

TEST(GeomProp_BSplineCurveTest, Curvature_Degree4_Bezier)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeDegree4();
  GeomProp_Curve                 aProp(aBSpline);

  for (double aParam = 0.0; aParam <= 1.0; aParam += 0.2)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_GE(aCurv.Value, 0.0) << "Curvature negative at param=" << aParam;
  }
}

// ============================================================================
// Normal tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, Normal_CubicSmooth)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  for (double aParam = 0.1; aParam <= 0.9; aParam += 0.2)
  {
    const GeomProp::NormalResult aNorm = aProp.Normal(aParam, THE_LIN_TOL);
    if (aNorm.IsDefined)
    {
      (void)aNorm.Direction;
    }
  }
}

// ============================================================================
// Centre of curvature tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, Centre_CubicSmooth)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  for (double aParam = 0.1; aParam <= 0.9; aParam += 0.2)
  {
    const GeomProp::CentreResult    aCentre = aProp.CentreOfCurvature(aParam, THE_LIN_TOL);
    const GeomProp::CurvatureResult aCurv   = aProp.Curvature(aParam, THE_LIN_TOL);
    if (aCentre.IsDefined && aCurv.IsDefined && aCurv.Value > THE_LIN_TOL)
    {
      // Centre should be at distance 1/curvature from the curve point
      gp_Pnt aPntOnCurve;
      aBSpline->D0(aParam, aPntOnCurve);
      const double aExpectedDist = 1.0 / aCurv.Value;
      EXPECT_NEAR(aCentre.Centre.Distance(aPntOnCurve), aExpectedDist, THE_POINT_TOL);
    }
  }
}

// ============================================================================
// FindCurvatureExtrema tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, FindCurvatureExtrema_Cubic)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Cubic BSpline with varying curvature should have at least one extremum
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.0);
    EXPECT_LE(aResult.Points[i].Parameter, 1.0);
    EXPECT_TRUE(aResult.Points[i].Type == GeomProp::CIType::MinCurvature
                || aResult.Points[i].Type == GeomProp::CIType::MaxCurvature);
  }
}

TEST(GeomProp_BSplineCurveTest, FindCurvatureExtrema_LowContinuity)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeQuadraticC1();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.0);
    EXPECT_LE(aResult.Points[i].Parameter, 1.0);
  }
}

TEST(GeomProp_BSplineCurveTest, FindCurvatureExtrema_MultipleSpans)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeMultiSpanCubic();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.0);
    EXPECT_LE(aResult.Points[i].Parameter, 1.0);
  }
}

// ============================================================================
// FindInflections tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, FindInflections_Cubic)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.0);
    EXPECT_LE(aResult.Points[i].Parameter, 1.0);
    EXPECT_EQ(aResult.Points[i].Type, GeomProp::CIType::Inflection);
  }
}

TEST(GeomProp_BSplineCurveTest, FindInflections_LowContinuity)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeQuadraticC1();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.0);
    EXPECT_LE(aResult.Points[i].Parameter, 1.0);
    EXPECT_EQ(aResult.Points[i].Type, GeomProp::CIType::Inflection);
  }
}

TEST(GeomProp_BSplineCurveTest, FindInflections_MultipleSpans)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeMultiSpanCubic();
  GeomProp_Curve                 aProp(aBSpline);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points[i].Parameter, 0.0);
    EXPECT_LE(aResult.Points[i].Parameter, 1.0);
    EXPECT_EQ(aResult.Points[i].Type, GeomProp::CIType::Inflection);
  }
}

// ============================================================================
// GetType test
// ============================================================================

TEST(GeomProp_BSplineCurveTest, GetType_IsBSpline)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  GeomProp_Curve                 aProp(aBSpline);
  EXPECT_EQ(aProp.GetType(), GeomAbs_BSplineCurve);
}

// ============================================================================
// VsCLProps comparison tests
// ============================================================================

TEST(GeomProp_BSplineCurveTest, VsCLProps_CubicSmooth)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  compareAll(aBSpline, 0.0, 1.0);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_Quadratic)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeQuadraticC1();
  compareAll(aBSpline, 0.0, 1.0);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_Degree4)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeDegree4();
  compareAll(aBSpline, 0.0, 1.0);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_AllProperties_Cubic)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeCubicC2();
  compareAll(aBSpline, 0.0, 1.0, 20);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_AllProperties_Degree4)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeDegree4();
  compareAll(aBSpline, 0.0, 1.0, 20);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_LowContinuity)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeQuadraticC1();
  compareAll(aBSpline, 0.0, 1.0);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_MultipleSpans)
{
  occ::handle<Geom_BSplineCurve> aBSpline = makeMultiSpanCubic();
  compareAll(aBSpline, 0.0, 1.0, 20);
}

TEST(GeomProp_BSplineCurveTest, VsCLProps_CriticalPoints)
{
  // Cubic C2 BSpline with uniform knots
  NCollection_Array1<gp_Pnt> aPoles(1, 6);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, -1, 1);
  aPoles(4) = gp_Pnt(3, 1, 0);
  aPoles(5) = gp_Pnt(4, -2, 1);
  aPoles(6) = gp_Pnt(5, 0, 0);
  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.33;
  aKnots(3) = 0.66;
  aKnots(4) = 1.0;
  NCollection_Array1<int> aMults(1, 4);
  aMults(1)                               = 4;
  aMults(2)                               = 1;
  aMults(3)                               = 1;
  aMults(4)                               = 4;
  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  // Critical: endpoints, knots, near-knots, span midpoints
  const double aParams[] = {0.0,
                            1.0e-10,
                            1.0e-6,
                            0.33,
                            0.33 + 1.0e-6,
                            0.33 - 1.0e-6,
                            0.66,
                            0.66 + 1.0e-6,
                            0.66 - 1.0e-6,
                            1.0 - 1.0e-6,
                            1.0 - 1.0e-10,
                            1.0,
                            0.165,
                            0.495,
                            0.83};
  for (const double aParam : aParams)
  {
    compareTangent(aBSpline, aParam);
    compareCurvature(aBSpline, aParam);
    compareNormal(aBSpline, aParam);
    compareCentre(aBSpline, aParam);
  }
}
