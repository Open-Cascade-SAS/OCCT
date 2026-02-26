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

// Unit tests for GeomProp_Curve with Ellipse geometry.

#include <Geom_Ellipse.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
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

  //! Analytical curvature of an ellipse at parameter t.
  //! k(t) = a*b / (a^2*sin^2(t) + b^2*cos^2(t))^(3/2)
  double ellipseCurvature(const double theMajor, const double theMinor, const double theParam)
  {
    const double aSin = std::sin(theParam);
    const double aCos = std::cos(theParam);
    const double aDenom =
      std::pow(theMajor * theMajor * aSin * aSin + theMinor * theMinor * aCos * aCos, 1.5);
    return theMajor * theMinor / aDenom;
  }

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

// ============================================================================
// Analytical reference tests
// ============================================================================

TEST(GeomProp_EllipseTest, Curvature_AtMajorVertex)
{
  // At t=0 (major vertex): k = a*b / b^3 = a / b^2
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const double                    aExpected = aMajor / (aMinor * aMinor);
  const GeomProp::CurvatureResult aCurv     = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, aExpected, THE_CURV_TOL);
}

TEST(GeomProp_EllipseTest, Curvature_AtMinorVertex)
{
  // At t=PI/2 (minor vertex): k = a*b / a^3 = b / a^2
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const double                    aExpected = aMinor / (aMajor * aMajor);
  const GeomProp::CurvatureResult aCurv     = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, aExpected, THE_CURV_TOL);
}

TEST(GeomProp_EllipseTest, Curvature_Symmetry)
{
  // k(t) = k(-t) = k(PI - t) for an ellipse centered at origin.
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const double aTestParams[] = {0.3, 0.7, 1.1, 1.5};
  for (const double aT : aTestParams)
  {
    const GeomProp::CurvatureResult aCurvT    = aProp.Curvature(aT, THE_LIN_TOL);
    const GeomProp::CurvatureResult aCurvNegT = aProp.Curvature(-aT, THE_LIN_TOL);
    const GeomProp::CurvatureResult aCurvPiT  = aProp.Curvature(M_PI - aT, THE_LIN_TOL);
    ASSERT_TRUE(aCurvT.IsDefined);
    ASSERT_TRUE(aCurvNegT.IsDefined);
    ASSERT_TRUE(aCurvPiT.IsDefined);
    EXPECT_NEAR(aCurvT.Value, aCurvNegT.Value, THE_CURV_TOL) << "k(t) != k(-t) at t=" << aT;
    EXPECT_NEAR(aCurvT.Value, aCurvPiT.Value, THE_CURV_TOL) << "k(t) != k(PI-t) at t=" << aT;
  }
}

TEST(GeomProp_EllipseTest, Tangent_AtMajorVertex)
{
  // At param=0, point is (a,0,0), tangent should be along Y: (0,1,0).
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_EllipseTest, Tangent_AtMinorVertex)
{
  // At param=PI/2, point is (0,b,0), tangent should be along -X: (-1,0,0).
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::TangentResult aTan = aProp.Tangent(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(std::abs(aTan.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_EllipseTest, Normal_AtMajorVertex)
{
  // At param=0, normal should point from (a,0,0) toward center (0,0,0): (-1,0,0).
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), -1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_EllipseTest, Normal_AtMinorVertex)
{
  // At param=PI/2, normal should point from (0,b,0) toward center: (0,-1,0).
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::NormalResult aNorm = aProp.Normal(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), -1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_EllipseTest, Centre_AtMajorVertex)
{
  // At t=0, radius of curvature R = b^2/a. Centre = point + R * normal = (a - b^2/a, 0, 0).
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const double                 aRoc    = aMinor * aMinor / aMajor; // b^2/a
  const double                 aExpX   = aMajor - aRoc;            // a - b^2/a
  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  EXPECT_NEAR(aCentre.Centre.X(), aExpX, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Y(), 0.0, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Z(), 0.0, THE_POINT_TOL);
}

TEST(GeomProp_EllipseTest, Centre_AtMinorVertex)
{
  // At t=PI/2, radius of curvature R = a^2/b. Centre = (0, b - a^2/b, 0).
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const double                 aRoc    = aMajor * aMajor / aMinor; // a^2/b
  const double                 aExpY   = aMinor - aRoc;            // b - a^2/b
  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  EXPECT_NEAR(aCentre.Centre.X(), 0.0, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Y(), aExpY, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Z(), 0.0, THE_POINT_TOL);
}

TEST(GeomProp_EllipseTest, FindCurvatureExtrema_FourPoints)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 4);

  // Verify parameters are at approximately 0, PI/2, PI, 3*PI/2.
  const double aExpected[] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};
  for (int i = 0; i < 4; ++i)
  {
    EXPECT_NEAR(aResult.Points[i].Parameter, aExpected[i], 1.0e-6)
      << "Extremum parameter mismatch at index " << i;
  }
}

TEST(GeomProp_EllipseTest, FindInflections_Empty)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_EllipseTest, GetType_IsEllipse)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Ellipse);
}

TEST(GeomProp_EllipseTest, Curvature_HighEccentricity)
{
  // Very elongated ellipse: a=100, b=1.
  const double              aMajor = 100.0;
  const double              aMinor = 1.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  // At major vertex: k = a/b^2 = 100
  const GeomProp::CurvatureResult aCurvMajor = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvMajor.IsDefined);
  EXPECT_NEAR(aCurvMajor.Value, aMajor / (aMinor * aMinor), THE_CURV_TOL);

  // At minor vertex: k = b/a^2 = 0.0001
  const GeomProp::CurvatureResult aCurvMinor = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvMinor.IsDefined);
  EXPECT_NEAR(aCurvMinor.Value, aMinor / (aMajor * aMajor), THE_CURV_TOL);
}

TEST(GeomProp_EllipseTest, Curvature_NearlyCircular)
{
  // Nearly circular: a=5, b=4.99.
  const double              aMajor = 5.0;
  const double              aMinor = 4.99;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  const GeomProp::CurvatureResult aCurvMajor = aProp.Curvature(0.0, THE_LIN_TOL);
  const GeomProp::CurvatureResult aCurvMinor = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvMajor.IsDefined);
  ASSERT_TRUE(aCurvMinor.IsDefined);

  // When nearly circular, curvatures at both vertices should be close to 1/R ~ 0.2.
  EXPECT_NEAR(aCurvMajor.Value, aMajor / (aMinor * aMinor), THE_CURV_TOL);
  EXPECT_NEAR(aCurvMinor.Value, aMinor / (aMajor * aMajor), THE_CURV_TOL);

  // Difference should be small.
  EXPECT_LT(std::abs(aCurvMajor.Value - aCurvMinor.Value), 0.01);
}

TEST(GeomProp_EllipseTest, Curvature_MonotonicBetweenExtrema)
{
  // Curvature should be monotonically decreasing from major vertex (t=0)
  // to minor vertex (t=PI/2) since max curvature is at major vertex.
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  double aPrevCurv = 0.0;
  for (int i = 0; i <= 20; ++i)
  {
    const double                    aParam = i * M_PI / (2.0 * 20);
    const GeomProp::CurvatureResult aCurv  = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    if (i > 0)
    {
      EXPECT_LE(aCurv.Value, aPrevCurv + THE_CURV_TOL)
        << "Curvature not monotonically decreasing at param=" << aParam;
    }
    aPrevCurv = aCurv.Value;
  }
}

// ============================================================================
// Cross-validation tests vs GeomLProp_CLProps
// ============================================================================

TEST(GeomProp_EllipseTest, VsCLProps_Standard)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  compareAll(anEllipse, 0.0, 2.0 * M_PI, 20);
}

TEST(GeomProp_EllipseTest, VsCLProps_HighEccentricity)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 100.0, 1.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  compareAll(anEllipse, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_EllipseTest, VsCLProps_AllProperties)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 8.0, 3.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  compareAll(anEllipse, 0.0, 2.0 * M_PI, 40);
}

TEST(GeomProp_EllipseTest, VsCLProps_OffCenter)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(5, 10, 15), gp_Dir(0, 0, 1)), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  compareAll(anEllipse, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_EllipseTest, VsCLProps_TiltedPlane)
{
  // Ellipse in a tilted plane with axis not along Z.
  const gp_Dir              anAxis(1, 1, 1);
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), anAxis), 10.0, 5.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  compareAll(anEllipse, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_EllipseTest, Curvature_MatchesAnalytical)
{
  // Verify curvature against the analytical formula at several params.
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  GeomProp_Curve            aProp(anEllipse);

  for (int i = 0; i <= 20; ++i)
  {
    const double                    aParam    = i * 2.0 * M_PI / 20.0;
    const double                    aExpected = ellipseCurvature(aMajor, aMinor, aParam);
    const GeomProp::CurvatureResult aCurv     = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_NEAR(aCurv.Value, aExpected, THE_CURV_TOL)
      << "Curvature mismatch vs analytical at param=" << aParam;
  }
}

TEST(GeomProp_EllipseTest, VsCLProps_CriticalPoints)
{
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 3.0);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);
  const double              aParams[] = {0.0,
                                         1.0e-10,
                                         -1.0e-10,
                                         M_PI / 2.0,
                                         M_PI / 2.0 + 1.0e-10,
                                         M_PI / 2.0 - 1.0e-10,
                                         M_PI,
                                         M_PI + 1.0e-6,
                                         3.0 * M_PI / 2.0,
                                         3.0 * M_PI / 2.0 + 1.0e-6,
                                         M_PI / 4.0,
                                         3.0 * M_PI / 4.0,
                                         2.0 * M_PI - 1.0e-10};
  for (const double aParam : aParams)
  {
    compareTangent(anEllipse, aParam);
    compareCurvature(anEllipse, aParam);
    compareNormal(anEllipse, aParam);
    compareCentre(anEllipse, aParam);
  }
}
