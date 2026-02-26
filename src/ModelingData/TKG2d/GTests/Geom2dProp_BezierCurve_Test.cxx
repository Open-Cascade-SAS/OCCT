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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <LProp_CIType.hxx>
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

void compareTangent(Geom2dProp_Curve& theProp, Geom2dLProp_CLProps2d& theOld, const double theParam)
{
  theOld.SetParameter(theParam);
  const Geom2dProp::TangentResult aNew = theProp.Tangent(theParam, THE_LIN_TOL);
  if (theOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Dir2d anOldDir;
    theOld.Tangent(anOldDir);
    const double aDot = aNew.Direction.X() * anOldDir.X() + aNew.Direction.Y() * anOldDir.Y();
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
  }
}

void compareCurvature(Geom2dProp_Curve&      theProp,
                      Geom2dLProp_CLProps2d& theOld,
                      const double           theParam)
{
  theOld.SetParameter(theParam);
  const Geom2dProp::CurvatureResult aNew     = theProp.Curvature(theParam, THE_LIN_TOL);
  const double                      aOldCurv = theOld.Curvature();
  if (aNew.IsDefined && !aNew.IsInfinite)
  {
    EXPECT_NEAR(aNew.Value, aOldCurv, THE_CURV_TOL);
  }
}

void compareNormal(Geom2dProp_Curve& theProp, Geom2dLProp_CLProps2d& theOld, const double theParam)
{
  theOld.SetParameter(theParam);
  const Geom2dProp::NormalResult aNew     = theProp.Normal(theParam, THE_LIN_TOL);
  const double                   aOldCurv = theOld.Curvature();
  if (std::abs(aOldCurv) < THE_LIN_TOL)
  {
    return;
  }
  if (aNew.IsDefined)
  {
    gp_Dir2d anOldNorm;
    theOld.Normal(anOldNorm);
    const double aDot = aNew.Direction.X() * anOldNorm.X() + aNew.Direction.Y() * anOldNorm.Y();
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
  }
}

void compareCentre(Geom2dProp_Curve& theProp, Geom2dLProp_CLProps2d& theOld, const double theParam)
{
  theOld.SetParameter(theParam);
  const Geom2dProp::CentreResult aNew     = theProp.CentreOfCurvature(theParam, THE_LIN_TOL);
  const double                   aOldCurv = theOld.Curvature();
  if (std::abs(aOldCurv) < THE_LIN_TOL)
  {
    return;
  }
  if (aNew.IsDefined)
  {
    gp_Pnt2d anOldCentre;
    theOld.CentreOfCurvature(anOldCentre);
    EXPECT_NEAR(aNew.Centre.X(), anOldCentre.X(), THE_POINT_TOL);
    EXPECT_NEAR(aNew.Centre.Y(), anOldCentre.Y(), THE_POINT_TOL);
  }
}

void compareAll(Geom2dProp_Curve&      theProp,
                Geom2dLProp_CLProps2d& theOld,
                const double           theFirst,
                const double           theLast,
                const int              theNbSamples = 10)
{
  const double aStep = (theLast - theFirst) / theNbSamples;
  for (int i = 0; i <= theNbSamples; ++i)
  {
    const double aParam = theFirst + i * aStep;
    compareTangent(theProp, theOld, aParam);
    compareCurvature(theProp, theOld, aParam);
    compareNormal(theProp, theOld, aParam);
    compareCentre(theProp, theOld, aParam);
  }
}

Geom2dProp::CIType mapLPropType(const LProp_CIType theType)
{
  switch (theType)
  {
    case LProp_Inflection:
      return Geom2dProp::CIType::Inflection;
    case LProp_MinCur:
      return Geom2dProp::CIType::MinCurvature;
    case LProp_MaxCur:
      return Geom2dProp::CIType::MaxCurvature;
  }
  return Geom2dProp::CIType::Inflection;
}

void compareExtrema(const Geom2dProp::CurveAnalysis& theNew,
                    const Geom2dLProp_CurAndInf2d&   theOld,
                    const double                     theTol = 1.0e-4)
{
  EXPECT_EQ(theNew.Points.Length(), theOld.NbPoints());
  const int aNb = std::min(theNew.Points.Length(), theOld.NbPoints());
  for (int i = 0; i < aNb; ++i)
  {
    EXPECT_NEAR(theNew.Points.Value(i).Parameter, theOld.Parameter(i + 1), theTol)
      << "Parameter mismatch at index " << i;
    EXPECT_EQ(theNew.Points.Value(i).Type, mapLPropType(theOld.Type(i + 1)))
      << "Type mismatch at index " << i;
  }
}

//! Create a linear Bezier (degree 1): P1(0,0), P2(4,0).
occ::handle<Geom2d_BezierCurve> makeLinearBezier()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(4.0, 0.0);
  return new Geom2d_BezierCurve(aPoles);
}

//! Create a quadratic Bezier: P1(0,0), P2(2,4), P3(4,0).
occ::handle<Geom2d_BezierCurve> makeQuadraticBezier()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(2.0, 4.0);
  aPoles(3) = gp_Pnt2d(4.0, 0.0);
  return new Geom2d_BezierCurve(aPoles);
}

//! Create a cubic S-shaped Bezier: P1(0,0), P2(1,2), P3(3,-1), P4(4,1).
occ::handle<Geom2d_BezierCurve> makeCubicSBezier()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 2.0);
  aPoles(3) = gp_Pnt2d(3.0, -1.0);
  aPoles(4) = gp_Pnt2d(4.0, 1.0);
  return new Geom2d_BezierCurve(aPoles);
}

//! Create a degree 5 Bezier: P1(0,0), P2(1,3), P3(2,-1), P4(3,2), P5(4,-2), P6(5,1).
occ::handle<Geom2d_BezierCurve> makeDegree5Bezier()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 3.0);
  aPoles(3) = gp_Pnt2d(2.0, -1.0);
  aPoles(4) = gp_Pnt2d(3.0, 2.0);
  aPoles(5) = gp_Pnt2d(4.0, -2.0);
  aPoles(6) = gp_Pnt2d(5.0, 1.0);
  return new Geom2d_BezierCurve(aPoles);
}

} // namespace

TEST(Geom2dProp_BezierCurveTest, Tangent_CubicEndpoints)
{
  // At t=0, tangent should be aligned with P1->P2; at t=1, with P3->P4.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();
  Geom2dProp_Curve                aProp(aBezier);

  // t=0: tangent along P1(0,0)->P2(1,2) = (1,2)
  const Geom2dProp::TangentResult aTan0 = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan0.IsDefined);
  const gp_Dir2d anExpDir0(1.0, 2.0);
  const double   aDot0 = aTan0.Direction.X() * anExpDir0.X() + aTan0.Direction.Y() * anExpDir0.Y();
  EXPECT_NEAR(std::abs(aDot0), 1.0, THE_DIR_TOL);

  // t=1: tangent along P3(3,-1)->P4(4,1) = (1,2)
  const Geom2dProp::TangentResult aTan1 = aProp.Tangent(1.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan1.IsDefined);
  const gp_Dir2d anExpDir1(1.0, 2.0);
  const double   aDot1 = aTan1.Direction.X() * anExpDir1.X() + aTan1.Direction.Y() * anExpDir1.Y();
  EXPECT_NEAR(std::abs(aDot1), 1.0, THE_DIR_TOL);
}

TEST(Geom2dProp_BezierCurveTest, Tangent_QuadraticMidpoint)
{
  // At midpoint t=0.5 of symmetric quadratic, tangent should be along X axis.
  occ::handle<Geom2d_BezierCurve> aBezier = makeQuadraticBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.5, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // By symmetry of P1(0,0), P2(2,4), P3(4,0), tangent at midpoint is horizontal.
  const double aDotX = aTan.Direction.X() * 1.0 + aTan.Direction.Y() * 0.0;
  EXPECT_NEAR(std::abs(aDotX), 1.0, THE_DIR_TOL);
}

TEST(Geom2dProp_BezierCurveTest, Curvature_QuadraticConstantSign)
{
  // Quadratic Bezier curvature should not change sign.
  occ::handle<Geom2d_BezierCurve> aBezier = makeQuadraticBezier();
  Geom2dProp_Curve                aProp(aBezier);

  int aPositiveCount = 0;
  int aNegativeCount = 0;
  for (double t = 0.0; t <= 1.0; t += 0.05)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(t, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    if (aCurv.Value > THE_CURV_TOL)
    {
      ++aPositiveCount;
    }
    else if (aCurv.Value < -THE_CURV_TOL)
    {
      ++aNegativeCount;
    }
  }
  // Should be all one sign (no inflection in a quadratic).
  EXPECT_TRUE(aPositiveCount == 0 || aNegativeCount == 0);
}

TEST(Geom2dProp_BezierCurveTest, Curvature_CubicInflection)
{
  // S-shaped cubic should have curvature near zero at inflection point.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();
  Geom2dProp_Curve                aProp(aBezier);

  // Find parameter where curvature is closest to zero.
  double aMinAbsCurv   = 1.0e30;
  double aMinCurvParam = 0.0;
  for (double t = 0.01; t <= 0.99; t += 0.01)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(t, THE_LIN_TOL);
    if (aCurv.IsDefined && std::abs(aCurv.Value) < aMinAbsCurv)
    {
      aMinAbsCurv   = std::abs(aCurv.Value);
      aMinCurvParam = t;
    }
  }
  // Curvature at inflection should be near zero.
  EXPECT_LT(aMinAbsCurv, 1.0) << "Inflection not found, min curvature at t=" << aMinCurvParam;
}

TEST(Geom2dProp_BezierCurveTest, Normal_Cubic)
{
  // Normal should be defined where curvature is non-zero.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();
  Geom2dProp_Curve                aProp(aBezier);

  // At t=0, curvature should be non-zero so normal is defined.
  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  if (std::abs(aCurv.Value) > THE_LIN_TOL)
  {
    const Geom2dProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
    ASSERT_TRUE(aNorm.IsDefined);

    // Normal should be perpendicular to tangent.
    const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    const double aDot =
      aNorm.Direction.X() * aTan.Direction.X() + aNorm.Direction.Y() * aTan.Direction.Y();
    EXPECT_NEAR(std::abs(aDot), 0.0, THE_DIR_TOL);
  }
}

TEST(Geom2dProp_BezierCurveTest, Centre_Cubic)
{
  // Centre of curvature should be defined where curvature is non-zero.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.2, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  if (std::abs(aCurv.Value) > THE_LIN_TOL)
  {
    const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(0.2, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined);

    // Radius of curvature should be 1/|k|.
    gp_Pnt2d aPnt;
    aBezier->D0(0.2, aPnt);
    const double aDist =
      std::sqrt((aCentre.Centre.X() - aPnt.X()) * (aCentre.Centre.X() - aPnt.X())
                + (aCentre.Centre.Y() - aPnt.Y()) * (aCentre.Centre.Y() - aPnt.Y()));
    const double anExpRadius = 1.0 / std::abs(aCurv.Value);
    EXPECT_NEAR(aDist, anExpRadius, THE_POINT_TOL);
  }
}

TEST(Geom2dProp_BezierCurveTest, FindCurvatureExtrema_Cubic)
{
  // Cubic S-shaped Bezier should have at least one curvature extremum.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const Geom2dProp::CurveAnalysis aExtrema = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aExtrema.IsDone);
  EXPECT_GE(aExtrema.Points.Length(), 1);
}

TEST(Geom2dProp_BezierCurveTest, FindInflections_CubicS)
{
  // S-shaped cubic should have at least one inflection point.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const Geom2dProp::CurveAnalysis aInflections = aProp.FindInflections();
  ASSERT_TRUE(aInflections.IsDone);
  EXPECT_GE(aInflections.Points.Length(), 1);

  // Inflection parameters should be in [0, 1].
  for (int i = 0; i < aInflections.Points.Length(); ++i)
  {
    EXPECT_GE(aInflections.Points.Value(i).Parameter, 0.0);
    EXPECT_LE(aInflections.Points.Value(i).Parameter, 1.0);
    EXPECT_EQ(aInflections.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
  }
}

TEST(Geom2dProp_BezierCurveTest, FindInflections_QuadraticNone)
{
  // Quadratic Bezier has no inflection points.
  occ::handle<Geom2d_BezierCurve> aBezier = makeQuadraticBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const Geom2dProp::CurveAnalysis aInflections = aProp.FindInflections();
  ASSERT_TRUE(aInflections.IsDone);
  EXPECT_EQ(aInflections.Points.Length(), 0);
}

TEST(Geom2dProp_BezierCurveTest, FindCurvatureExtrema_Quadratic)
{
  // Quadratic Bezier should have curvature extrema.
  occ::handle<Geom2d_BezierCurve> aBezier = makeQuadraticBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const Geom2dProp::CurveAnalysis aExtrema = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aExtrema.IsDone);
  EXPECT_GE(aExtrema.Points.Length(), 1);
}

TEST(Geom2dProp_BezierCurveTest, GetType_IsBezier)
{
  occ::handle<Geom2d_BezierCurve> aBezier = makeQuadraticBezier();
  Geom2dProp_Curve                aProp(aBezier);

  EXPECT_EQ(aProp.GetType(), GeomAbs_BezierCurve);
}

TEST(Geom2dProp_BezierCurveTest, VsCLProps2d_CubicS)
{
  // Cross-validate against deprecated CLProps2d for cubic S-curve.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 10);
}

TEST(Geom2dProp_BezierCurveTest, VsCLProps2d_Quadratic)
{
  // Cross-validate against deprecated CLProps2d for quadratic.
  occ::handle<Geom2d_BezierCurve> aBezier = makeQuadraticBezier();

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 10);
}

TEST(Geom2dProp_BezierCurveTest, VsCLProps2d_HighDegree)
{
  // Cross-validate against deprecated CLProps2d for degree 5.
  occ::handle<Geom2d_BezierCurve> aBezier = makeDegree5Bezier();

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 20);
}

TEST(Geom2dProp_BezierCurveTest, VsCLProps2d_AllProperties_Cubic)
{
  // Dense comparison for cubic S-curve.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 40);
}

TEST(Geom2dProp_BezierCurveTest, VsCLProps2d_AllProperties_Degree5)
{
  // Dense comparison for degree 5 curve.
  occ::handle<Geom2d_BezierCurve> aBezier = makeDegree5Bezier();

  Geom2dProp_Curve      aProp(aBezier);
  Geom2dLProp_CLProps2d aOld(aBezier, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 40);
}

TEST(Geom2dProp_BezierCurveTest, Curvature_LinearBezier)
{
  // Linear Bezier (degree 1) should have zero curvature everywhere.
  occ::handle<Geom2d_BezierCurve> aBezier = makeLinearBezier();
  Geom2dProp_Curve                aProp(aBezier);

  for (double t = 0.0; t <= 1.0; t += 0.1)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(t, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL) << "Non-zero curvature at t=" << t;
  }
}

TEST(Geom2dProp_BezierCurveTest, Tangent_LinearBezier)
{
  // Linear Bezier should have constant tangent along P1->P2.
  occ::handle<Geom2d_BezierCurve> aBezier = makeLinearBezier();
  Geom2dProp_Curve                aProp(aBezier);

  const gp_Dir2d anExpDir(1.0, 0.0); // P1(0,0)->P2(4,0) = (4,0) normalized = (1,0)
  for (double t = 0.0; t <= 1.0; t += 0.1)
  {
    const Geom2dProp::TangentResult aTan = aProp.Tangent(t, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    const double aDot = aTan.Direction.X() * anExpDir.X() + aTan.Direction.Y() * anExpDir.Y();
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Tangent not constant at t=" << t;
  }
}

TEST(Geom2dProp_BezierCurveTest, VsCurAndInf2d_CubicS_Inflections)
{
  // Compare inflection results with deprecated CurAndInf2d.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_BezierCurveTest, VsCurAndInf2d_CubicS_Extrema)
{
  // Compare curvature extrema with deprecated CurAndInf2d.
  occ::handle<Geom2d_BezierCurve> aBezier = makeCubicSBezier();

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_BezierCurveTest, VsCurAndInf2d_HighDegree_FullPerform)
{
  // Compare full perform (extrema + inflections) with deprecated CurAndInf2d for degree 5.
  occ::handle<Geom2d_BezierCurve> aBezier = makeDegree5Bezier();

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aBezier);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBezier);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

TEST(Geom2dProp_BezierCurveTest, VsCLProps2d_CriticalPoints)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                               = gp_Pnt2d(1.0, 2.0);
  aPoles(3)                               = gp_Pnt2d(3.0, -1.0);
  aPoles(4)                               = gp_Pnt2d(4.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);
  Geom2dProp_Curve                aProp(aBezier);
  Geom2dLProp_CLProps2d           aOld(aBezier, 2, THE_LIN_TOL);
  const double                    aParams[] =
    {0.0, 1.0e-10, 1.0e-6, 0.01, 0.25, 0.5, 0.75, 0.99, 1.0 - 1.0e-6, 1.0 - 1.0e-10, 1.0};
  for (const double aParam : aParams)
  {
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
