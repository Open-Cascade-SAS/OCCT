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

// Unit tests for GeomProp_Curve with Circle geometry.

#include <Geom_Circle.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_CLProps.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
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

// ============================================================================
// Analytical reference tests
// ============================================================================

TEST(GeomProp_CircleTest, Curvature_IsConstant)
{
  const double             aRadius = 5.0;
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_FALSE(aCurv.IsInfinite);
  EXPECT_NEAR(aCurv.Value, 1.0 / aRadius, THE_CURV_TOL);
}

TEST(GeomProp_CircleTest, Curvature_AllParams)
{
  const double             aRadius = 7.0;
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const double aExpected = 1.0 / aRadius;
  for (int i = 0; i < 12; ++i)
  {
    const double                  aParam = i * M_PI / 6.0;
    const GeomProp::CurvatureResult aCurv  = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_NEAR(aCurv.Value, aExpected, THE_CURV_TOL)
      << "Curvature not constant at param=" << aParam;
  }
}

TEST(GeomProp_CircleTest, Curvature_SmallRadius)
{
  const double             aRadius = 0.01;
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 100.0, THE_CURV_TOL);
}

TEST(GeomProp_CircleTest, Curvature_LargeRadius)
{
  const double             aRadius = 1000.0;
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.001, THE_CURV_TOL);
}

TEST(GeomProp_CircleTest, Tangent_AtZero)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  // At param=0, point is (R,0,0), tangent should be (0,1,0).
  const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_CircleTest, Tangent_AtPiOver2)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  // At param=PI/2, point is (0,R,0), tangent should be (-1,0,0).
  const GeomProp::TangentResult aTan = aProp.Tangent(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), -1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_CircleTest, Tangent_AtPi)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  // At param=PI, point is (-R,0,0), tangent should be (0,-1,0).
  const GeomProp::TangentResult aTan = aProp.Tangent(M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), -1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_CircleTest, Normal_PointsToCenter)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  // At param=0, point is (5,0,0), normal toward center = (-1,0,0).
  const GeomProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), -1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_CircleTest, Normal_AllParams)
{
  const double             aRadius = 5.0;
  const gp_Pnt             aCenter(0, 0, 0);
  gp_Circ                  aCirc(gp_Ax2(aCenter, gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  // Normal should always point from the curve point toward the center.
  for (int i = 0; i < 12; ++i)
  {
    const double aParam = i * M_PI / 6.0;
    // Compute expected normal direction from the curve point toward the center.
    const double aCosT  = std::cos(aParam);
    const double aSinT  = std::sin(aParam);
    const gp_Pnt aPntOnCurve(aRadius * aCosT, aRadius * aSinT, 0.0);
    const gp_Dir aExpectedNorm(aCenter.X() - aPntOnCurve.X(),
                               aCenter.Y() - aPntOnCurve.Y(),
                               aCenter.Z() - aPntOnCurve.Z());

    const GeomProp::NormalResult aNorm = aProp.Normal(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at param=" << aParam;
    const double aDot = aNorm.Direction.Dot(aExpectedNorm);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL)
      << "Normal does not point toward center at param=" << aParam;
  }
}

TEST(GeomProp_CircleTest, Centre_IsCircleCenter)
{
  const gp_Pnt             aCenter(0, 0, 0);
  gp_Circ                  aCirc(gp_Ax2(aCenter, gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  for (int i = 0; i < 8; ++i)
  {
    const double                 aParam  = i * M_PI / 4.0;
    const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined) << "Centre undefined at param=" << aParam;
    EXPECT_NEAR(aCentre.Centre.Distance(aCenter), 0.0, THE_POINT_TOL)
      << "Centre not at circle center, param=" << aParam;
  }
}

TEST(GeomProp_CircleTest, Centre_OffCenter)
{
  const gp_Pnt             aCenter(1, 2, 3);
  gp_Circ                  aCirc(gp_Ax2(aCenter, gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(1.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  EXPECT_NEAR(aCentre.Centre.X(), 1.0, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Y(), 2.0, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Z(), 3.0, THE_POINT_TOL);
}

TEST(GeomProp_CircleTest, FindCurvatureExtrema_Empty)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_CircleTest, FindInflections_Empty)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_CircleTest, GetType_IsCircle)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);
}

TEST(GeomProp_CircleTest, Tangent_Perpendicular_ToRadius)
{
  const double             aRadius = 5.0;
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  GeomProp_Curve           aProp(aCircle);

  for (int i = 0; i < 12; ++i)
  {
    const double aParam = i * M_PI / 6.0;
    // Radius direction at this parameter.
    const double aCosT    = std::cos(aParam);
    const double aSinT    = std::sin(aParam);
    const gp_Dir aRadDir(aCosT, aSinT, 0.0);

    const GeomProp::TangentResult aTan = aProp.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    EXPECT_NEAR(aTan.Direction.Dot(aRadDir), 0.0, THE_DIR_TOL)
      << "Tangent not perpendicular to radius at param=" << aParam;
  }
}

// ============================================================================
// Cross-validation tests vs GeomLProp_CLProps
// ============================================================================

TEST(GeomProp_CircleTest, VsCLProps_AllProperties)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  compareAll(aCircle, 0.0, 2.0 * M_PI, 20);
}

TEST(GeomProp_CircleTest, VsCLProps_SmallRadius)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.01);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  compareAll(aCircle, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_CircleTest, VsCLProps_LargeRadius)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1000.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  compareAll(aCircle, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_CircleTest, VsCLProps_OffCenter)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  compareAll(aCircle, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_CircleTest, VsCLProps_TiltedPlane)
{
  // Circle in a tilted plane with axis not along Z.
  const gp_Dir             anAxis(1, 1, 1);
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), anAxis), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);
  compareAll(aCircle, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_CircleTest, VsCLProps_CriticalPoints)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  const double aParams[] = {0.0, 1.0e-10, M_PI / 4.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0,
                            2.0 * M_PI - 1.0e-10, M_PI / 2.0 + 1.0e-6, M_PI / 2.0 - 1.0e-6};
  for (const double aParam : aParams)
  {
    compareTangent(aCircle, aParam);
    compareCurvature(aCircle, aParam);
    compareNormal(aCircle, aParam);
    compareCentre(aCircle, aParam);
  }
}
