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

// Tests for GeomProp_Curve with Geom_Hyperbola curves.
// Validates curvature, tangent, normal, and centre of curvature properties
// against analytical formulas and cross-validates against GeomLProp_CLProps.

#include <Geom_Hyperbola.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
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

// Curvature at vertex (t=0): k = a / b^2
TEST(GeomProp_HyperbolaTest, Curvature_AtVertex)
{
  const double                    aA = 6.0;
  const double                    aB = 3.0;
  gp_Hypr                         anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aA, aB);
  occ::handle<Geom_Hyperbola>     aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve                  aProp(aHyperbola);
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  const double aExpected = aA / (aB * aB);
  EXPECT_NEAR(aCurv.Value, aExpected, THE_CURV_TOL);
}

// Curvature is symmetric: k(t) = k(-t)
TEST(GeomProp_HyperbolaTest, Curvature_Symmetric)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  for (double aT = 0.1; aT <= 2.0; aT += 0.3)
  {
    const GeomProp::CurvatureResult aPos = aProp.Curvature(aT, THE_LIN_TOL);
    const GeomProp::CurvatureResult aNeg = aProp.Curvature(-aT, THE_LIN_TOL);
    ASSERT_TRUE(aPos.IsDefined);
    ASSERT_TRUE(aNeg.IsDefined);
    EXPECT_NEAR(aPos.Value, aNeg.Value, THE_CURV_TOL) << "Asymmetry at t=" << aT;
  }
}

// Curvature decreases from vertex as |t| increases
TEST(GeomProp_HyperbolaTest, Curvature_DecreasesFromVertex)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::CurvatureResult aCurvVertex = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvVertex.IsDefined);

  double aPrevCurv = aCurvVertex.Value;
  for (double aT = 0.5; aT <= 3.0; aT += 0.5)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LT(aCurv.Value, aPrevCurv) << "Curvature did not decrease at t=" << aT;
    aPrevCurv = aCurv.Value;
  }
}

// Tangent at vertex is perpendicular to real axis
TEST(GeomProp_HyperbolaTest, Tangent_AtVertex)
{
  gp_Ax2                      anAx2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Hypr                     anHypr(anAx2, 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // Real axis is X direction of the coordinate system
  const gp_Dir aRealAxis = anAx2.XDirection();
  EXPECT_NEAR(std::abs(aTan.Direction.Dot(aRealAxis)), 0.0, THE_DIR_TOL);
}

// Tangent direction is symmetric about vertex
TEST(GeomProp_HyperbolaTest, Tangent_Symmetric)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  for (double aT = 0.2; aT <= 2.0; aT += 0.3)
  {
    const GeomProp::TangentResult aTanPos = aProp.Tangent(aT, THE_LIN_TOL);
    const GeomProp::TangentResult aTanNeg = aProp.Tangent(-aT, THE_LIN_TOL);
    ASSERT_TRUE(aTanPos.IsDefined);
    ASSERT_TRUE(aTanNeg.IsDefined);
    // Symmetric tangent: Y-component flips sign, X-component stays the same
    // Check that the X-components have the same magnitude
    EXPECT_NEAR(std::abs(aTanPos.Direction.X()), std::abs(aTanNeg.Direction.X()), THE_DIR_TOL)
      << "Tangent X asymmetry at t=" << aT;
  }
}

// Normal at vertex is along real axis toward center
TEST(GeomProp_HyperbolaTest, Normal_AtVertex)
{
  gp_Ax2                      anAx2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Hypr                     anHypr(anAx2, 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  // Normal at vertex should be along the real axis (X direction)
  const gp_Dir aRealAxis = anAx2.XDirection();
  EXPECT_NEAR(std::abs(aNorm.Direction.Dot(aRealAxis)), 1.0, THE_DIR_TOL);
}

// Centre at vertex is at distance 1/k from vertex along normal
TEST(GeomProp_HyperbolaTest, Centre_AtVertex)
{
  const double                aA = 6.0;
  const double                aB = 3.0;
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aA, aB);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::CentreResult    aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  const GeomProp::CurvatureResult aCurv   = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  ASSERT_TRUE(aCurv.IsDefined);

  // Vertex is at (a, 0, 0); centre should be at distance 1/k from vertex
  gp_Pnt aVertex;
  aHyperbola->D0(0.0, aVertex);
  const double aRadius = 1.0 / aCurv.Value;
  EXPECT_NEAR(aCentre.Centre.Distance(aVertex), std::abs(aRadius), THE_POINT_TOL);
}

// FindCurvatureExtrema returns 1 extremum at t=0
TEST(GeomProp_HyperbolaTest, FindCurvatureExtrema_OnePoint)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 1);
  EXPECT_NEAR(aResult.Points[0].Parameter, 0.0, Precision::Confusion());
}

// FindInflections returns no inflections
TEST(GeomProp_HyperbolaTest, FindInflections_Empty)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// GetType returns GeomAbs_Hyperbola
TEST(GeomProp_HyperbolaTest, GetType_IsHyperbola)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Hyperbola);
}

// Curvature approaches 0 for large |t|
TEST(GeomProp_HyperbolaTest, Curvature_LargeParam)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(10.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_LT(aCurv.Value, 0.01);
}

// Cross-validate vs CLProps: standard axes a=6, b=3
TEST(GeomProp_HyperbolaTest, VsCLProps_Standard)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -2.0, 2.0);
}

// Dense cross-validation on [-2, 2]
TEST(GeomProp_HyperbolaTest, VsCLProps_AllProperties)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -2.0, 2.0, 20);
}

// Cross-validate with large semi-axes
TEST(GeomProp_HyperbolaTest, VsCLProps_LargeAxes)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 100.0, 50.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -1.0, 1.0);
}

// Cross-validate with small semi-axes
TEST(GeomProp_HyperbolaTest, VsCLProps_SmallAxes)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.5, 0.3);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -2.0, 2.0);
}

// Cross-validate with asymmetric semi-axes (a >> b)
TEST(GeomProp_HyperbolaTest, VsCLProps_AsymmetricAxes)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 1.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -2.0, 2.0);
}

// Fine-grained curvature near vertex
TEST(GeomProp_HyperbolaTest, Curvature_NearVertex)
{
  const double                aA = 6.0;
  const double                aB = 3.0;
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aA, aB);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  const double aExpectedMax = aA / (aB * aB);
  for (double aT = 0.01; aT <= 0.1; aT += 0.01)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LT(aCurv.Value, aExpectedMax + THE_CURV_TOL)
      << "Curvature exceeds vertex maximum at t=" << aT;
  }
}

// Tangent far from vertex approaches asymptote direction
TEST(GeomProp_HyperbolaTest, Tangent_FarFromVertex)
{
  const double                aA = 6.0;
  const double                aB = 3.0;
  gp_Ax2                      anAx2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Hypr                     anHypr(anAx2, aA, aB);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  // For large t, the asymptote direction is (a*sinh(t), b*cosh(t), 0) -> (a, b, 0) normalized
  const gp_Dir aAsymptote(aA, aB, 0.0);

  const GeomProp::TangentResult aTan = aProp.Tangent(10.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(std::abs(aTan.Direction.Dot(aAsymptote)), 1.0, 1.0e-4);
}

// Cross-validate with off-center hyperbola
TEST(GeomProp_HyperbolaTest, VsCLProps_OffCenter)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(5.0, 5.0, 5.0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  compareAll(aHyperbola, -2.0, 2.0);
}

// Centre of curvature moves away from vertex as |t| increases
TEST(GeomProp_HyperbolaTest, Centre_MovesAwayFromVertex)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);
  GeomProp_Curve              aProp(aHyperbola);

  gp_Pnt aVertex;
  aHyperbola->D0(0.0, aVertex);

  const GeomProp::CentreResult aCentreAtVertex = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentreAtVertex.IsDefined);

  double aPrevDist = aCentreAtVertex.Centre.Distance(aVertex);
  for (double aT = 0.5; aT <= 2.5; aT += 0.5)
  {
    const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined);
    const double aDist = aCentre.Centre.Distance(aVertex);
    EXPECT_GT(aDist, aPrevDist - THE_POINT_TOL)
      << "Centre did not move away from vertex at t=" << aT;
    aPrevDist = aDist;
  }
}

TEST(GeomProp_HyperbolaTest, VsCLProps_CriticalPoints)
{
  gp_Hypr                     aHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(aHypr);
  const double                aParams[]  = {0.0,
                                            1.0e-10,
                                            -1.0e-10,
                                            1.0e-6,
                                            -1.0e-6,
                                            0.1,
                                            -0.1,
                                            0.5,
                                            -0.5,
                                            1.0,
                                            -1.0,
                                            3.0,
                                            -3.0,
                                            5.0,
                                            -5.0};
  for (const double aParam : aParams)
  {
    compareTangent(aHyperbola, aParam);
    compareCurvature(aHyperbola, aParam);
    compareNormal(aHyperbola, aParam);
    compareCentre(aHyperbola, aParam);
  }
}
