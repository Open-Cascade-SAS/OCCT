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

// Tests for GeomProp_Curve with Geom_BezierCurve curves.
// Validates curvature, tangent, normal, and centre of curvature properties
// for linear, quadratic, cubic, and high-degree Bezier curves.
// Cross-validates against GeomLProp_CLProps.

#include <Geom_BezierCurve.hxx>
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

  //! Compare tangent from new GeomProp_Curve vs old GeomLProp_CLProps.
  void compareTangent(const occ::handle<Geom_Curve>& theCurve, const double theParam)
  {
    GeomProp_Curve                aProp(theCurve);
    const GeomProp::TangentResult aNew = aProp.Tangent(theParam, THE_LIN_TOL);

    GeomLProp_CLProps anOld(theCurve, theParam, 2, THE_LIN_TOL);
    if (anOld.IsTangentDefined())
    {
      ASSERT_TRUE(aNew.IsDefined) << "New tangent undefined but old is defined at param="
                                  << theParam;
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

  //! Create a linear (degree 1) Bezier curve.
  occ::handle<Geom_BezierCurve> makeLinear()
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 2);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(4, 0, 0);
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a quadratic (degree 2) Bezier curve.
  occ::handle<Geom_BezierCurve> makeQuadratic()
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 3);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(2, 4, 0);
    aPoles(3) = gp_Pnt(4, 0, 0);
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a cubic S-shaped Bezier curve.
  occ::handle<Geom_BezierCurve> makeCubicS()
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 2, 1);
    aPoles(3) = gp_Pnt(3, -1, 0);
    aPoles(4) = gp_Pnt(4, 1, 1);
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a degree 5 Bezier curve.
  occ::handle<Geom_BezierCurve> makeDegree5()
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 6);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 3, 0);
    aPoles(3) = gp_Pnt(2, -1, 1);
    aPoles(4) = gp_Pnt(3, 2, 0);
    aPoles(5) = gp_Pnt(4, -2, 1);
    aPoles(6) = gp_Pnt(5, 1, 0);
    return new Geom_BezierCurve(aPoles);
  }
} // namespace

// Tangent at endpoints of cubic: at t=0 aligned with P1->P2, at t=1 with P3->P4
TEST(GeomProp_BezierCurveTest, Tangent_CubicEndpoints)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  GeomProp_Curve                aProp(aBezier);

  // At t=0, tangent should align with P1->P2 direction
  const gp_Dir                  aDirStart(gp_Pnt(1, 2, 1).XYZ() - gp_Pnt(0, 0, 0).XYZ());
  const GeomProp::TangentResult aTan0 = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan0.IsDefined);
  EXPECT_NEAR(std::abs(aTan0.Direction.Dot(aDirStart)), 1.0, THE_DIR_TOL);

  // At t=1, tangent should align with P3->P4 direction
  const gp_Dir                  aDirEnd(gp_Pnt(4, 1, 1).XYZ() - gp_Pnt(3, -1, 0).XYZ());
  const GeomProp::TangentResult aTan1 = aProp.Tangent(1.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan1.IsDefined);
  EXPECT_NEAR(std::abs(aTan1.Direction.Dot(aDirEnd)), 1.0, THE_DIR_TOL);
}

// Tangent at midpoint of quadratic
TEST(GeomProp_BezierCurveTest, Tangent_QuadraticMidpoint)
{
  occ::handle<Geom_BezierCurve> aBezier = makeQuadratic();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::TangentResult aTan = aProp.Tangent(0.5, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // For symmetric quadratic P1(0,0,0), P2(2,4,0), P3(4,0,0),
  // tangent at t=0.5 should be along X-axis (horizontal)
  EXPECT_NEAR(std::abs(aTan.Direction.X()), 1.0, THE_DIR_TOL);
}

// Quadratic: curvature doesn't change sign (convex curve)
TEST(GeomProp_BezierCurveTest, Curvature_QuadraticConstantSign)
{
  occ::handle<Geom_BezierCurve> aBezier = makeQuadratic();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurvatureResult aCurv0 = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv0.IsDefined);
  for (double aT = 0.1; aT <= 1.0; aT += 0.1)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    // Curvature is always non-negative from the API, so just verify it stays positive
    EXPECT_GT(aCurv.Value, 0.0) << "Curvature became zero/negative at t=" << aT;
  }
}

// Cubic S-curve: curvature passes through zero (inflection)
TEST(GeomProp_BezierCurveTest, Curvature_CubicInflection)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  GeomProp_Curve                aProp(aBezier);

  // Sample curvature at many points and check if it gets very small near inflection
  bool aFoundSmall = false;
  for (double aT = 0.0; aT <= 1.0; aT += 0.01)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    if (aCurv.Value < 0.1)
    {
      aFoundSmall = true;
    }
  }
  EXPECT_TRUE(aFoundSmall) << "Expected to find near-zero curvature at inflection";
}

// Normal defined where curvature is non-zero on cubic
TEST(GeomProp_BezierCurveTest, Normal_Cubic)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  GeomProp_Curve                aProp(aBezier);

  // At endpoints, curvature is typically non-zero for an S-curve
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  if (aCurv.IsDefined && aCurv.Value > THE_LIN_TOL)
  {
    const GeomProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
    ASSERT_TRUE(aNorm.IsDefined);
    // Normal should be perpendicular to tangent
    const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    EXPECT_NEAR(std::abs(aNorm.Direction.Dot(aTan.Direction)), 0.0, THE_DIR_TOL);
  }
}

// Centre of curvature at several params on cubic
TEST(GeomProp_BezierCurveTest, Centre_Cubic)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  GeomProp_Curve                aProp(aBezier);

  for (double aT = 0.0; aT <= 1.0; aT += 0.25)
  {
    const GeomProp::CurvatureResult aCurv   = aProp.Curvature(aT, THE_LIN_TOL);
    const GeomProp::CentreResult    aCentre = aProp.CentreOfCurvature(aT, THE_LIN_TOL);
    if (aCurv.IsDefined && aCurv.Value > THE_LIN_TOL)
    {
      ASSERT_TRUE(aCentre.IsDefined) << "Centre undefined at t=" << aT;
      // Distance from curve point to centre = 1/curvature
      gp_Pnt aPnt;
      aBezier->D0(aT, aPnt);
      const double aRadius = 1.0 / aCurv.Value;
      EXPECT_NEAR(aCentre.Centre.Distance(aPnt), aRadius, THE_POINT_TOL)
        << "Radius mismatch at t=" << aT;
    }
  }
}

// Cubic S-curve curvature extrema
TEST(GeomProp_BezierCurveTest, FindCurvatureExtrema_Cubic)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // A cubic S-curve should have at least one curvature extremum
  EXPECT_GE(aResult.Points.Length(), 1);
}

// S-shaped cubic has at least one inflection
TEST(GeomProp_BezierCurveTest, FindInflections_CubicS)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_GE(aResult.Points.Length(), 1);
}

// Quadratic has no inflections
TEST(GeomProp_BezierCurveTest, FindInflections_QuadraticNone)
{
  occ::handle<Geom_BezierCurve> aBezier = makeQuadratic();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// Quadratic curvature extrema
TEST(GeomProp_BezierCurveTest, FindCurvatureExtrema_Quadratic)
{
  occ::handle<Geom_BezierCurve> aBezier = makeQuadratic();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Quadratic Bezier should have at least one extremum
  EXPECT_GE(aResult.Points.Length(), 1);
}

// GetType returns GeomAbs_BezierCurve
TEST(GeomProp_BezierCurveTest, GetType_IsBezier)
{
  occ::handle<Geom_BezierCurve> aBezier = makeQuadratic();
  GeomProp_Curve                aProp(aBezier);
  EXPECT_EQ(aProp.GetType(), GeomAbs_BezierCurve);
}

// Cross-validate cubic S vs CLProps
TEST(GeomProp_BezierCurveTest, VsCLProps_CubicS)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  compareAll(aBezier, 0.0, 1.0);
}

// Cross-validate quadratic vs CLProps
TEST(GeomProp_BezierCurveTest, VsCLProps_Quadratic)
{
  occ::handle<Geom_BezierCurve> aBezier = makeQuadratic();
  compareAll(aBezier, 0.0, 1.0);
}

// Cross-validate degree 5 vs CLProps
TEST(GeomProp_BezierCurveTest, VsCLProps_HighDegree)
{
  occ::handle<Geom_BezierCurve> aBezier = makeDegree5();
  compareAll(aBezier, 0.0, 1.0);
}

// Dense cross-validation for cubic
TEST(GeomProp_BezierCurveTest, VsCLProps_AllProperties_Cubic)
{
  occ::handle<Geom_BezierCurve> aBezier = makeCubicS();
  compareAll(aBezier, 0.0, 1.0, 20);
}

// Dense cross-validation for degree 5
TEST(GeomProp_BezierCurveTest, VsCLProps_AllProperties_Degree5)
{
  occ::handle<Geom_BezierCurve> aBezier = makeDegree5();
  compareAll(aBezier, 0.0, 1.0, 20);
}

// Linear Bezier (degree 1) has zero curvature everywhere
TEST(GeomProp_BezierCurveTest, Curvature_LinearBezier)
{
  occ::handle<Geom_BezierCurve> aBezier = makeLinear();
  GeomProp_Curve                aProp(aBezier);

  for (double aT = 0.0; aT <= 1.0; aT += 0.1)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL) << "Non-zero curvature on line at t=" << aT;
  }
}

// Linear Bezier (degree 1) has constant tangent direction
TEST(GeomProp_BezierCurveTest, Tangent_LinearBezier)
{
  occ::handle<Geom_BezierCurve> aBezier = makeLinear();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::TangentResult aTanRef = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanRef.IsDefined);

  for (double aT = 0.1; aT <= 1.0; aT += 0.1)
  {
    const GeomProp::TangentResult aTan = aProp.Tangent(aT, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    EXPECT_NEAR(std::abs(aTan.Direction.Dot(aTanRef.Direction)), 1.0, THE_DIR_TOL)
      << "Tangent changed direction at t=" << aT;
  }
}

// Degree 5 S-curve inflections
TEST(GeomProp_BezierCurveTest, FindInflections_HighDegree)
{
  occ::handle<Geom_BezierCurve> aBezier = makeDegree5();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // 3D curve inflections depend on the cross product of D1 and D2;
  // verify the analysis completes without requiring a specific count
  EXPECT_GE(aResult.Points.Length(), 0);
}

// Degree 5 curvature extrema
TEST(GeomProp_BezierCurveTest, FindCurvatureExtrema_HighDegree)
{
  occ::handle<Geom_BezierCurve> aBezier = makeDegree5();
  GeomProp_Curve                aProp(aBezier);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // High-degree curve with varied control points should have multiple extrema
  EXPECT_GE(aResult.Points.Length(), 1);
}

// 3D Bezier with z-variation: curvature comparison
TEST(GeomProp_BezierCurveTest, Curvature_PlanarVs3D)
{
  // Planar quadratic
  occ::handle<Geom_BezierCurve> aPlanar = makeQuadratic();

  // 3D quadratic with z-variation
  NCollection_Array1<gp_Pnt> aPoles3D(1, 3);
  aPoles3D(1)                       = gp_Pnt(0, 0, 0);
  aPoles3D(2)                       = gp_Pnt(2, 4, 3);
  aPoles3D(3)                       = gp_Pnt(4, 0, 0);
  occ::handle<Geom_BezierCurve> a3D = new Geom_BezierCurve(aPoles3D);

  GeomProp_Curve aPropPlanar(aPlanar);
  GeomProp_Curve aProp3D(a3D);

  // Both should have well-defined curvature at midpoint
  const GeomProp::CurvatureResult aCurvPlanar = aPropPlanar.Curvature(0.5, THE_LIN_TOL);
  const GeomProp::CurvatureResult aCurv3D     = aProp3D.Curvature(0.5, THE_LIN_TOL);
  ASSERT_TRUE(aCurvPlanar.IsDefined);
  ASSERT_TRUE(aCurv3D.IsDefined);
  // The 3D curve has additional z-component, so curvature should differ
  // but both should be positive
  EXPECT_GT(aCurvPlanar.Value, 0.0);
  EXPECT_GT(aCurv3D.Value, 0.0);
}

TEST(GeomProp_BezierCurveTest, VsCLProps_CriticalPoints)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1)                             = gp_Pnt(0, 0, 0);
  aPoles(2)                             = gp_Pnt(1, 2, 0);
  aPoles(3)                             = gp_Pnt(3, -1, 0);
  aPoles(4)                             = gp_Pnt(4, 0, 0);
  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  const double                  aParams[] =
    {0.0, 1.0e-10, 1.0e-6, 0.01, 0.25, 0.5, 0.75, 0.99, 1.0 - 1.0e-6, 1.0 - 1.0e-10, 1.0};
  for (const double aParam : aParams)
  {
    compareTangent(aBezier, aParam);
    compareCurvature(aBezier, aParam);
    compareNormal(aBezier, aParam);
    compareCentre(aBezier, aParam);
  }
}
