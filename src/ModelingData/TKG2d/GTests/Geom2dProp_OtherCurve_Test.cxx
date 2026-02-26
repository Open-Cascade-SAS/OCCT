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

// Tests for Geom2dProp_Curve with trimmed curves and other non-standard 2D
// curve types: local differential properties and global curve analysis.

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
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
  constexpr double THE_PARAM_TOL = 1.0e-4;

  void compareTangent(Geom2dProp_Curve & theProp,
                      Geom2dLProp_CLProps2d & theOld,
                      const double theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::TangentResult aNew = theProp.Tangent(theParam, THE_LIN_TOL);
    if (theOld.IsTangentDefined())
    {
      ASSERT_TRUE(aNew.IsDefined) << "Tangent undefined at U=" << theParam;
      gp_Dir2d anOldDir;
      theOld.Tangent(anOldDir);
      const double aDot = aNew.Direction.X() * anOldDir.X() + aNew.Direction.Y() * anOldDir.Y();
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Tangent mismatch at U=" << theParam;
    }
  }

  void compareCurvature(Geom2dProp_Curve & theProp,
                        Geom2dLProp_CLProps2d & theOld,
                        const double theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::CurvatureResult aNew     = theProp.Curvature(theParam, THE_LIN_TOL);
    const double                      aOldCurv = theOld.Curvature();
    if (aNew.IsDefined && !aNew.IsInfinite)
    {
      EXPECT_NEAR(aNew.Value, aOldCurv, THE_CURV_TOL) << "Curvature mismatch at U=" << theParam;
    }
  }

  void compareNormal(Geom2dProp_Curve & theProp,
                     Geom2dLProp_CLProps2d & theOld,
                     const double theParam)
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
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Normal mismatch at U=" << theParam;
    }
  }

  void compareCentre(Geom2dProp_Curve & theProp,
                     Geom2dLProp_CLProps2d & theOld,
                     const double theParam)
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
      EXPECT_NEAR(aNew.Centre.X(), anOldCentre.X(), THE_POINT_TOL)
        << "Centre X mismatch at U=" << theParam;
      EXPECT_NEAR(aNew.Centre.Y(), anOldCentre.Y(), THE_POINT_TOL)
        << "Centre Y mismatch at U=" << theParam;
    }
  }

  void compareAll(Geom2dProp_Curve & theProp,
                  Geom2dLProp_CLProps2d & theOld,
                  const double theFirst,
                  const double theLast,
                  const int    theNbSamples = 10)
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
                      const double                     theTol = THE_PARAM_TOL)
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

  // Helper: create trimmed circle.
  occ::handle<Geom2d_TrimmedCurve> makeTrimmedCircle(const double theRadius,
                                                     const double theFirst,
                                                     const double theLast)
  {
    gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), theRadius);
    occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
    return new Geom2d_TrimmedCurve(aCircle, theFirst, theLast);
  }

  // Helper: create trimmed ellipse.
  occ::handle<Geom2d_TrimmedCurve> makeTrimmedEllipse(const double theMajor,
                                                      const double theMinor,
                                                      const double theFirst,
                                                      const double theLast)
  {
    gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), theMajor, theMinor);
    occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
    return new Geom2d_TrimmedCurve(anEllipse, theFirst, theLast);
  }

  // Helper: create cubic S-shaped Bezier for trimming.
  occ::handle<Geom2d_BezierCurve> makeSBezier()
  {
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles(1) = gp_Pnt2d(0.0, 0.0);
    aPoles(2) = gp_Pnt2d(1.0, 3.0);
    aPoles(3) = gp_Pnt2d(3.0, -1.0);
    aPoles(4) = gp_Pnt2d(4.0, 1.0);
    return new Geom2d_BezierCurve(aPoles);
  }

} // namespace

// ============================================================================
// Trimmed circle tests
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, Tangent_TrimmedCircle)
{
  const double                     aRadius = 5.0;
  occ::handle<Geom2d_TrimmedCurve> aCurve  = makeTrimmedCircle(aRadius, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);

  for (double u = 0.5; u <= 2.5; u += 0.5)
  {
    const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined) << "Tangent undefined at U=" << u;
  }
}

TEST(Geom2dProp_OtherCurveTest, Curvature_TrimmedCircle)
{
  const double                     aRadius = 5.0;
  occ::handle<Geom2d_TrimmedCurve> aCurve  = makeTrimmedCircle(aRadius, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);

  const double aExpectedCurv = 1.0 / aRadius;
  for (double u = 0.5; u <= 2.5; u += 0.5)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at U=" << u;
    EXPECT_NEAR(aCurv.Value, aExpectedCurv, THE_CURV_TOL) << "Curvature mismatch at U=" << u;
  }
}

TEST(Geom2dProp_OtherCurveTest, Normal_TrimmedCircle)
{
  const double                     aRadius = 5.0;
  occ::handle<Geom2d_TrimmedCurve> aCurve  = makeTrimmedCircle(aRadius, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);

  for (double u = 0.5; u <= 2.5; u += 0.5)
  {
    const Geom2dProp::NormalResult aNorm = aProp.Normal(u, THE_LIN_TOL);
    ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at U=" << u;
    // Normal should point toward center (0,0)
    // Point on circle: (R*cos(u), R*sin(u)), so normal toward center is (-cos(u), -sin(u))
    const double aExpX = -std::cos(u);
    const double aExpY = -std::sin(u);
    const double aDot  = aNorm.Direction.X() * aExpX + aNorm.Direction.Y() * aExpY;
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Normal direction mismatch at U=" << u;
  }
}

TEST(Geom2dProp_OtherCurveTest, Centre_TrimmedCircle)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedCircle(5.0, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);

  for (double u = 0.5; u <= 2.5; u += 0.5)
  {
    const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined) << "Centre undefined at U=" << u;
    EXPECT_NEAR(aCentre.Centre.X(), 0.0, THE_POINT_TOL);
    EXPECT_NEAR(aCentre.Centre.Y(), 0.0, THE_POINT_TOL);
  }
}

TEST(Geom2dProp_OtherCurveTest, FindCurvatureExtrema_TrimmedCircle)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedCircle(5.0, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(Geom2dProp_OtherCurveTest, FindInflections_TrimmedCircle)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedCircle(5.0, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// ============================================================================
// Trimmed ellipse tests
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, Tangent_TrimmedEllipse)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);
  Geom2dProp_Curve                 aProp(aCurve);

  for (double u = 0.0; u <= M_PI; u += M_PI / 6.0)
  {
    const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
    EXPECT_TRUE(aTan.IsDefined) << "Tangent undefined at U=" << u;
  }
}

TEST(Geom2dProp_OtherCurveTest, Curvature_TrimmedEllipse)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);
  Geom2dProp_Curve                 aProp(aCurve);

  // Curvature at u=0 (major vertex) and u=pi/2 (minor vertex) should differ
  const Geom2dProp::CurvatureResult aCurv0    = aProp.Curvature(0.0, THE_LIN_TOL);
  const Geom2dProp::CurvatureResult aCurvHalf = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv0.IsDefined);
  ASSERT_TRUE(aCurvHalf.IsDefined);
  EXPECT_NE(aCurv0.Value, aCurvHalf.Value);
}

TEST(Geom2dProp_OtherCurveTest, FindCurvatureExtrema_TrimmedEllipse)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);
  Geom2dProp_Curve                 aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Half-ellipse should have extrema (at major and minor vertices)
  EXPECT_GT(aResult.Points.Length(), 0);

  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points.Value(i).Parameter, 0.0);
    EXPECT_LE(aResult.Points.Value(i).Parameter, M_PI);
  }
}

TEST(Geom2dProp_OtherCurveTest, FindInflections_TrimmedEllipse)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);
  Geom2dProp_Curve                 aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // Ellipse has no inflections
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// ============================================================================
// GetType test
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, GetType_IsOtherCurve)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedCircle(5.0, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);
  // Trimmed curves typically resolve to OtherCurve or the underlying type
  // depending on adaptor resolution. Check that it returns a valid type.
  const GeomAbs_CurveType aType = aProp.GetType();
  // Trimmed circle should be recognized as Circle by the adaptor
  EXPECT_EQ(aType, GeomAbs_Circle);
}

// ============================================================================
// Cross-validation vs CLProps2d
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_TrimmedCircle)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedCircle(5.0, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);
  Geom2dLProp_CLProps2d            aOld(aCurve, 2, THE_LIN_TOL);

  for (double u = 0.5; u <= 2.5; u += 0.25)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_TrimmedEllipse)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);
  Geom2dProp_Curve                 aProp(aCurve);
  Geom2dLProp_CLProps2d            aOld(aCurve, 2, THE_LIN_TOL);

  for (double u = 0.0; u <= M_PI; u += M_PI / 8.0)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_TrimmedBezier)
{
  occ::handle<Geom2d_BezierCurve>  aBezier  = makeSBezier();
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aBezier, 0.2, 0.8);

  Geom2dProp_Curve      aProp(aTrimmed);
  Geom2dLProp_CLProps2d aOld(aTrimmed, 2, THE_LIN_TOL);

  for (double u = 0.2; u <= 0.8; u += 0.1)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_AllProperties_TrimmedCircle)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedCircle(5.0, 0.5, 2.5);
  Geom2dProp_Curve                 aProp(aCurve);
  Geom2dLProp_CLProps2d            aOld(aCurve, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.5, 2.5, 20);
}

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_AllProperties_TrimmedEllipse)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);
  Geom2dProp_Curve                 aProp(aCurve);
  Geom2dLProp_CLProps2d            aOld(aCurve, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, M_PI, 20);
}

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_AllProperties_TrimmedBezier)
{
  occ::handle<Geom2d_BezierCurve>  aBezier  = makeSBezier();
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aBezier, 0.2, 0.8);

  Geom2dProp_Curve      aProp(aTrimmed);
  Geom2dLProp_CLProps2d aOld(aTrimmed, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.2, 0.8, 20);
}

// ============================================================================
// Trimmed hyperbola
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, Curvature_TrimmedHyperbola)
{
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola  = new Geom2d_Hyperbola(anHypr);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aHyperbola, -1.0, 1.0);

  Geom2dProp_Curve aProp(aTrimmed);

  // Curvature at vertex (u=0) should be maximum for hyperbola
  const Geom2dProp::CurvatureResult aCurvVertex = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvVertex.IsDefined);
  EXPECT_GT(std::abs(aCurvVertex.Value), 0.0);

  // Curvature should decrease away from vertex
  const Geom2dProp::CurvatureResult aCurvAway = aProp.Curvature(0.8, THE_LIN_TOL);
  ASSERT_TRUE(aCurvAway.IsDefined);
  EXPECT_LT(std::abs(aCurvAway.Value), std::abs(aCurvVertex.Value));
}

// ============================================================================
// Cross-validation vs CurAndInf2d
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, VsCurAndInf2d_TrimmedEllipse_Extrema)
{
  occ::handle<Geom2d_TrimmedCurve> aCurve = makeTrimmedEllipse(10.0, 5.0, 0.0, M_PI);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aCurve);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aCurve);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

// ============================================================================
// Trimmed Bezier inflections (S-curve)
// ============================================================================

TEST(Geom2dProp_OtherCurveTest, FindInflections_TrimmedBezier)
{
  occ::handle<Geom2d_BezierCurve>  aBezier  = makeSBezier();
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aBezier, 0.1, 0.9);

  Geom2dProp_Curve aProp(aTrimmed);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);

  // S-shaped Bezier should have inflection(s) within [0.1, 0.9]
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points.Value(i).Parameter, 0.1);
    EXPECT_LE(aResult.Points.Value(i).Parameter, 0.9);
    EXPECT_EQ(aResult.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
  }
}

TEST(Geom2dProp_OtherCurveTest, VsCLProps2d_CriticalPoints)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed =
    new Geom2d_TrimmedCurve(aCircle, M_PI / 4.0, 3.0 * M_PI / 4.0);
  Geom2dProp_Curve      aProp(aTrimmed);
  Geom2dLProp_CLProps2d aOld(aTrimmed, 2, THE_LIN_TOL);
  const double          aFirst    = aTrimmed->FirstParameter();
  const double          aLast     = aTrimmed->LastParameter();
  const double          aMid      = (aFirst + aLast) / 2.0;
  const double          aParams[] = {aFirst,
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
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
