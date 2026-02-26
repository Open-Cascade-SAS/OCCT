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

// Tests for Geom2dProp_Curve with 2D offset curves: local differential
// properties and global curve analysis for offset circles and offset ellipses.

#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_OffsetCurve.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
    Standard_DISABLE_DEPRECATION_WARNINGS
#include <LProp_CIType.hxx>
      Standard_ENABLE_DEPRECATION_WARNINGS
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

  // Helper: create offset circle with given radius and offset distance.
  occ::handle<Geom2d_OffsetCurve> makeOffsetCircle(const double theRadius, const double theOffset)
  {
    gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), theRadius);
    occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
    return new Geom2d_OffsetCurve(aCircle, theOffset);
  }

  // Helper: create offset ellipse.
  occ::handle<Geom2d_OffsetCurve> makeOffsetEllipse(const double theMajor,
                                                    const double theMinor,
                                                    const double theOffset)
  {
    gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), theMajor, theMinor);
    occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
    return new Geom2d_OffsetCurve(anEllipse, theOffset);
  }

} // namespace

// ============================================================================
// Offset circle: curvature tests
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, Curvature_OffsetCircle_Constant)
{
  const double                    aRadius  = 5.0;
  const double                    anOffset = 2.0;
  occ::handle<Geom2d_OffsetCurve> aCurve   = makeOffsetCircle(aRadius, anOffset);
  Geom2dProp_Curve                aProp(aCurve);

  // Offset of circle by d gives circle with radius R+d, curvature = 1/(R+d)
  const double aExpectedCurv = 1.0 / (aRadius + anOffset);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at U=" << u;
    EXPECT_NEAR(aCurv.Value, aExpectedCurv, THE_CURV_TOL) << "Curvature mismatch at U=" << u;
  }
}

TEST(Geom2dProp_OffsetCurveTest, Curvature_OffsetCircle_Positive)
{
  const double                    aRadius  = 3.0;
  const double                    anOffset = 5.0;
  occ::handle<Geom2d_OffsetCurve> aCurve   = makeOffsetCircle(aRadius, anOffset);
  Geom2dProp_Curve                aProp(aCurve);

  const double                      aExpectedCurv = 1.0 / (aRadius + anOffset);
  const Geom2dProp::CurvatureResult aCurv         = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, aExpectedCurv, THE_CURV_TOL);
}

TEST(Geom2dProp_OffsetCurveTest, Curvature_OffsetCircle_Negative)
{
  const double                    aRadius  = 10.0;
  const double                    anOffset = -3.0;
  occ::handle<Geom2d_OffsetCurve> aCurve   = makeOffsetCircle(aRadius, anOffset);
  Geom2dProp_Curve                aProp(aCurve);

  // Negative offset reduces effective radius
  const double                      aExpectedCurv = 1.0 / (aRadius + anOffset);
  const Geom2dProp::CurvatureResult aCurv         = aProp.Curvature(M_PI / 3.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, aExpectedCurv, THE_CURV_TOL);
}

// ============================================================================
// Offset circle: tangent, normal, centre
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, Tangent_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);

  // At u=0 on a standard circle, tangent is in Y direction
  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // Tangent should be roughly (0, 1) or (0, -1) at u=0
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, THE_DIR_TOL);
}

TEST(Geom2dProp_OffsetCurveTest, Normal_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::NormalResult aNorm = aProp.Normal(u, THE_LIN_TOL);
    ASSERT_TRUE(aNorm.IsDefined) << "Normal undefined at U=" << u;
    // Normal should be perpendicular to tangent
    const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    const double aDot =
      aNorm.Direction.X() * aTan.Direction.X() + aNorm.Direction.Y() * aTan.Direction.Y();
    EXPECT_NEAR(aDot, 0.0, THE_DIR_TOL);
  }
}

TEST(Geom2dProp_OffsetCurveTest, Centre_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);

  // Centre of curvature for offset circle should be the circle center (0,0)
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined) << "Centre undefined at U=" << u;
    EXPECT_NEAR(aCentre.Centre.X(), 0.0, THE_POINT_TOL);
    EXPECT_NEAR(aCentre.Centre.Y(), 0.0, THE_POINT_TOL);
  }
}

// ============================================================================
// Offset circle: global analysis
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, FindCurvatureExtrema_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Constant curvature means no extrema
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(Geom2dProp_OffsetCurveTest, FindInflections_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // Circle has no inflections
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// ============================================================================
// GetType test
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, GetType_IsOffsetCurve)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);
  EXPECT_EQ(aProp.GetType(), GeomAbs_OffsetCurve);
}

// ============================================================================
// Cross-validation vs CLProps2d: offset circle
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, VsCLProps2d_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);
  Geom2dLProp_CLProps2d           aOld(aCurve, 2, THE_LIN_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_OffsetCurveTest, VsCLProps2d_AllProperties_OffsetCircle)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);
  Geom2dProp_Curve                aProp(aCurve);
  Geom2dLProp_CLProps2d           aOld(aCurve, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI - 0.01, 20);
}

// ============================================================================
// Offset ellipse tests
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, Curvature_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);

  // Curvature should vary along the ellipse offset
  const Geom2dProp::CurvatureResult aCurv0  = aProp.Curvature(0.0, THE_LIN_TOL);
  const Geom2dProp::CurvatureResult aCurvPi = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv0.IsDefined);
  ASSERT_TRUE(aCurvPi.IsDefined);
  EXPECT_NE(aCurv0.Value, aCurvPi.Value);
}

TEST(Geom2dProp_OffsetCurveTest, FindCurvatureExtrema_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Offset ellipse should have curvature extrema
  EXPECT_GT(aResult.Points.Length(), 0);
}

TEST(Geom2dProp_OffsetCurveTest, FindInflections_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // Offset of ellipse by small offset: no inflections expected
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_EQ(aResult.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
  }
}

TEST(Geom2dProp_OffsetCurveTest, Tangent_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
    EXPECT_TRUE(aTan.IsDefined) << "Tangent undefined at U=" << u;
  }
}

TEST(Geom2dProp_OffsetCurveTest, Normal_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::NormalResult aNorm = aProp.Normal(u, THE_LIN_TOL);
    if (aNorm.IsDefined)
    {
      const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
      ASSERT_TRUE(aTan.IsDefined);
      const double aDot =
        aNorm.Direction.X() * aTan.Direction.X() + aNorm.Direction.Y() * aTan.Direction.Y();
      EXPECT_NEAR(aDot, 0.0, THE_DIR_TOL) << "Normal not perpendicular at U=" << u;
    }
  }
}

// ============================================================================
// Cross-validation vs CLProps2d: offset ellipse
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, VsCLProps2d_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);
  Geom2dLProp_CLProps2d           aOld(aCurve, 2, THE_LIN_TOL);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_OffsetCurveTest, VsCLProps2d_AllProperties_OffsetEllipse)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);
  Geom2dProp_Curve                aProp(aCurve);
  Geom2dLProp_CLProps2d           aOld(aCurve, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI - 0.01, 20);
}

// ============================================================================
// Cross-validation vs CurAndInf2d
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, VsCurAndInf2d_OffsetEllipse_Extrema)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetEllipse(10.0, 5.0, 1.0);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aCurve);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aCurve);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_OffsetCurveTest, VsCurAndInf2d_OffsetCircle_NoExtrema)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(5.0, 2.0);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aCurve);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aCurve);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  EXPECT_EQ(aNew.Points.Length(), anOld.NbPoints());
}

TEST(Geom2dProp_OffsetCurveTest, VsCLProps2d_CriticalPoints)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 2.0);
  Geom2dProp_Curve                aProp(anOffset);
  Geom2dLProp_CLProps2d           aOld(anOffset, 2, THE_LIN_TOL);
  const double                    aParams[] = {0.0,
                                               1.0e-10,
                                               M_PI / 4.0,
                                               M_PI / 2.0,
                                               M_PI / 2.0 + 1.0e-6,
                                               M_PI,
                                               3.0 * M_PI / 2.0,
                                               2.0 * M_PI - 1.0e-10,
                                               M_PI / 6.0,
                                               5.0 * M_PI / 6.0};
  for (const double aParam : aParams)
  {
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
