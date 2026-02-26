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

// Tests for GeomProp_Curve with Geom_Parabola curves.
// Validates curvature, tangent, normal, and centre of curvature properties
// against analytical formulas and cross-validates against GeomLProp_CLProps.

#include <Geom_Parabola.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_CLProps.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Parab.hxx>
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
} // namespace

// Curvature at vertex (t=0): k = 1/(2f)
TEST(GeomProp_ParabolaTest, Curvature_AtVertex)
{
  const double                    aFocal = 2.0;
  gp_Parab                        aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aFocal);
  occ::handle<Geom_Parabola>      aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve                   aProp(aParabola);
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  const double aExpected = 1.0 / (2.0 * aFocal);
  EXPECT_NEAR(aCurv.Value, aExpected, THE_CURV_TOL);
}

// Curvature is symmetric: k(t) = k(-t)
TEST(GeomProp_ParabolaTest, Curvature_Symmetric)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  for (double aT = 0.5; aT <= 5.0; aT += 0.5)
  {
    const GeomProp::CurvatureResult aPos = aProp.Curvature(aT, THE_LIN_TOL);
    const GeomProp::CurvatureResult aNeg = aProp.Curvature(-aT, THE_LIN_TOL);
    ASSERT_TRUE(aPos.IsDefined);
    ASSERT_TRUE(aNeg.IsDefined);
    EXPECT_NEAR(aPos.Value, aNeg.Value, THE_CURV_TOL) << "Asymmetry at t=" << aT;
  }
}

// Curvature decreases from vertex as |t| increases
TEST(GeomProp_ParabolaTest, Curvature_DecreasesFromVertex)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::CurvatureResult aCurvVertex = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvVertex.IsDefined);

  double aPrevCurv = aCurvVertex.Value;
  for (double aT = 1.0; aT <= 5.0; aT += 1.0)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LT(aCurv.Value, aPrevCurv) << "Curvature did not decrease at t=" << aT;
    aPrevCurv = aCurv.Value;
  }
}

// Tangent at vertex is perpendicular to axis
TEST(GeomProp_ParabolaTest, Tangent_AtVertex)
{
  gp_Ax2                     anAx2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Parab                   aParab(anAx2, 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // Axis of symmetry is X direction
  const gp_Dir anAxis = anAx2.XDirection();
  EXPECT_NEAR(std::abs(aTan.Direction.Dot(anAxis)), 0.0, THE_DIR_TOL);
}

// Normal at vertex along axis toward focus
TEST(GeomProp_ParabolaTest, Normal_AtVertex)
{
  gp_Ax2                     anAx2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Parab                   aParab(anAx2, 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  // Normal at vertex should be along the axis (X direction)
  const gp_Dir anAxis = anAx2.XDirection();
  EXPECT_NEAR(std::abs(aNorm.Direction.Dot(anAxis)), 1.0, THE_DIR_TOL);
}

// Centre at vertex is at distance 2f from vertex (coincides with focus)
TEST(GeomProp_ParabolaTest, Centre_AtVertex)
{
  const double               aFocal = 2.0;
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aFocal);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);

  // Vertex of parabola
  gp_Pnt aVertex;
  aParabola->D0(0.0, aVertex);

  // Radius of curvature at vertex = 1/k = 2f
  const double aRadiusOfCurv = 2.0 * aFocal;
  EXPECT_NEAR(aCentre.Centre.Distance(aVertex), aRadiusOfCurv, THE_POINT_TOL);
}

// FindCurvatureExtrema returns 1 extremum at t=0
TEST(GeomProp_ParabolaTest, FindCurvatureExtrema_OnePoint)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 1);
  EXPECT_NEAR(aResult.Points[0].Parameter, 0.0, Precision::Confusion());
}

// FindInflections returns no inflections
TEST(GeomProp_ParabolaTest, FindInflections_Empty)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// GetType returns GeomAbs_Parabola
TEST(GeomProp_ParabolaTest, GetType_IsParabola)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Parabola);
}

// Curvature approaches 0 for large |t|
TEST(GeomProp_ParabolaTest, Curvature_LargeParam)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(50.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_LT(aCurv.Value, 0.001);
}

// Cross-validate vs CLProps: f=2
TEST(GeomProp_ParabolaTest, VsCLProps_Standard)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -5.0, 5.0);
}

// Dense cross-validation on [-5, 5]
TEST(GeomProp_ParabolaTest, VsCLProps_AllProperties)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -5.0, 5.0, 20);
}

// Cross-validate with small focal distance
TEST(GeomProp_ParabolaTest, VsCLProps_SmallFocal)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.1);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -3.0, 3.0);
}

// Cross-validate with large focal distance
TEST(GeomProp_ParabolaTest, VsCLProps_LargeFocal)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -10.0, 10.0);
}

// Cross-validate with off-center parabola
TEST(GeomProp_ParabolaTest, VsCLProps_OffCenter)
{
  gp_Parab aParab(gp_Ax2(gp_Pnt(3.0, 4.0, 5.0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -5.0, 5.0);
}

// Tangent for large parameter approaches axis direction
TEST(GeomProp_ParabolaTest, Tangent_LargeParam)
{
  gp_Ax2                     anAx2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Parab                   aParab(anAx2, 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  // For large t, the parabola tangent approaches the axis direction (X direction)
  const gp_Dir anAxis = anAx2.XDirection();

  const GeomProp::TangentResult aTan = aProp.Tangent(100.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(std::abs(aTan.Direction.Dot(anAxis)), 1.0, 1.0e-3);
}

// Centre moves away from vertex as |t| increases
TEST(GeomProp_ParabolaTest, Centre_MovesAway)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  gp_Pnt aVertex;
  aParabola->D0(0.0, aVertex);

  const GeomProp::CentreResult aCentreAtVertex = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentreAtVertex.IsDefined);

  double aPrevDist = aCentreAtVertex.Centre.Distance(aVertex);
  for (double aT = 1.0; aT <= 5.0; aT += 1.0)
  {
    const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined);
    const double aDist = aCentre.Centre.Distance(aVertex);
    EXPECT_GT(aDist, aPrevDist - THE_POINT_TOL)
      << "Centre did not move away from vertex at t=" << aT;
    aPrevDist = aDist;
  }
}

// Fine-grained curvature near vertex
TEST(GeomProp_ParabolaTest, Curvature_NearVertex)
{
  const double               aFocal = 2.0;
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aFocal);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  const double aExpectedMax = 1.0 / (2.0 * aFocal);
  for (double aT = 0.01; aT <= 0.1; aT += 0.01)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LT(aCurv.Value, aExpectedMax + THE_CURV_TOL)
      << "Curvature exceeds vertex maximum at t=" << aT;
  }
}

// Cross-validate with tilted plane parabola
TEST(GeomProp_ParabolaTest, VsCLProps_TiltedPlane)
{
  gp_Ax2 anAx2(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  gp_Parab                   aParab(anAx2, 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  compareAll(aParabola, -5.0, 5.0);
}

// Normal directions are symmetric about vertex
TEST(GeomProp_ParabolaTest, Normal_Symmetric)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  GeomProp_Curve              aProp(aParabola);

  for (double aT = 1.0; aT <= 4.0; aT += 1.0)
  {
    const GeomProp::NormalResult aNormPos = aProp.Normal(aT, THE_LIN_TOL);
    const GeomProp::NormalResult aNormNeg = aProp.Normal(-aT, THE_LIN_TOL);
    ASSERT_TRUE(aNormPos.IsDefined);
    ASSERT_TRUE(aNormNeg.IsDefined);
    // For a parabola symmetric about X-axis:
    // Normal X-components should be equal, Y-components should be opposite in sign
    EXPECT_NEAR(aNormPos.Direction.X(), aNormNeg.Direction.X(), THE_DIR_TOL)
      << "Normal X asymmetry at t=" << aT;
  }
}

TEST(GeomProp_ParabolaTest, VsCLProps_CriticalPoints)
{
  gp_Parab aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);
  const double aParams[] = {0.0, 1.0e-10, -1.0e-10, 1.0e-6, -1.0e-6, 0.1, -0.1,
                            0.5, -0.5, 1.0, -1.0, 5.0, -5.0, 10.0, -10.0};
  for (const double aParam : aParams)
  {
    compareTangent(aParabola, aParam);
    compareCurvature(aParabola, aParam);
    compareNormal(aParabola, aParam);
    compareCentre(aParabola, aParam);
  }
}
