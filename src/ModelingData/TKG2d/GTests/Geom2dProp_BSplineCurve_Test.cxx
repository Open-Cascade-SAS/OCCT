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

// Tests for Geom2dProp_Curve with 2D BSpline curves: local differential
// properties (tangent, curvature, normal, centre of curvature) and global
// curve analysis (curvature extrema and inflection points).

#include <Geom2d_BSplineCurve.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <LProp_CIType.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
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

  void compareTangent(Geom2dProp_Curve&      theProp,
                      Geom2dLProp_CLProps2d&  theOld,
                      const double            theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::TangentResult aNew = theProp.Tangent(theParam, THE_LIN_TOL);
    if (theOld.IsTangentDefined())
    {
      ASSERT_TRUE(aNew.IsDefined) << "Tangent undefined at U=" << theParam;
      gp_Dir2d anOldDir;
      theOld.Tangent(anOldDir);
      const double aDot = aNew.Direction.X() * anOldDir.X()
                        + aNew.Direction.Y() * anOldDir.Y();
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Tangent mismatch at U=" << theParam;
    }
  }

  void compareCurvature(Geom2dProp_Curve&      theProp,
                        Geom2dLProp_CLProps2d&  theOld,
                        const double            theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::CurvatureResult aNew = theProp.Curvature(theParam, THE_LIN_TOL);
    const double aOldCurv = theOld.Curvature();
    if (aNew.IsDefined && !aNew.IsInfinite)
    {
      EXPECT_NEAR(aNew.Value, aOldCurv, THE_CURV_TOL) << "Curvature mismatch at U=" << theParam;
    }
  }

  void compareNormal(Geom2dProp_Curve&      theProp,
                     Geom2dLProp_CLProps2d&  theOld,
                     const double            theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::NormalResult aNew = theProp.Normal(theParam, THE_LIN_TOL);
    const double aOldCurv = theOld.Curvature();
    if (std::abs(aOldCurv) < THE_LIN_TOL)
    {
      return;
    }
    if (aNew.IsDefined)
    {
      gp_Dir2d anOldNorm;
      theOld.Normal(anOldNorm);
      const double aDot = aNew.Direction.X() * anOldNorm.X()
                        + aNew.Direction.Y() * anOldNorm.Y();
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "Normal mismatch at U=" << theParam;
    }
  }

  void compareCentre(Geom2dProp_Curve&      theProp,
                     Geom2dLProp_CLProps2d&  theOld,
                     const double            theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::CentreResult aNew = theProp.CentreOfCurvature(theParam, THE_LIN_TOL);
    const double aOldCurv = theOld.Curvature();
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

  void compareAll(Geom2dProp_Curve&      theProp,
                  Geom2dLProp_CLProps2d&  theOld,
                  const double            theFirst,
                  const double            theLast,
                  const int               theNbSamples = 10)
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
      case LProp_Inflection: return Geom2dProp::CIType::Inflection;
      case LProp_MinCur:     return Geom2dProp::CIType::MinCurvature;
      case LProp_MaxCur:     return Geom2dProp::CIType::MaxCurvature;
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

  // Helper: create a cubic C2 BSpline with 6 poles.
  // Knots [0, 0.33, 0.66, 1], mults [4,1,1,4], degree 3.
  occ::handle<Geom2d_BSplineCurve> makeCubicBSpline()
  {
    NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
    aPoles(1) = gp_Pnt2d(0.0, 0.0);
    aPoles(2) = gp_Pnt2d(1.0, 3.0);
    aPoles(3) = gp_Pnt2d(2.0, 1.0);
    aPoles(4) = gp_Pnt2d(3.0, 4.0);
    aPoles(5) = gp_Pnt2d(4.0, 2.0);
    aPoles(6) = gp_Pnt2d(5.0, 0.0);

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

    return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
  }

  // Helper: create a quadratic C1 BSpline with 5 poles.
  // Knots [0, 0.33, 0.66, 1], mults [3,1,1,3], degree 2.
  occ::handle<Geom2d_BSplineCurve> makeQuadraticBSpline()
  {
    NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
    aPoles(1) = gp_Pnt2d(0.0, 0.0);
    aPoles(2) = gp_Pnt2d(1.0, 3.0);
    aPoles(3) = gp_Pnt2d(2.0, 1.0);
    aPoles(4) = gp_Pnt2d(3.0, 3.0);
    aPoles(5) = gp_Pnt2d(4.0, 0.0);

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

    return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
  }

  // Helper: create a degree 4 BSpline with 5 poles.
  // Knots [0, 1], mults [5, 5], degree 4.
  occ::handle<Geom2d_BSplineCurve> makeDegree4BSpline()
  {
    NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
    aPoles(1) = gp_Pnt2d(0.0, 0.0);
    aPoles(2) = gp_Pnt2d(1.0, 3.0);
    aPoles(3) = gp_Pnt2d(2.0, -1.0);
    aPoles(4) = gp_Pnt2d(3.0, 2.0);
    aPoles(5) = gp_Pnt2d(4.0, 0.0);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 5;
    aMults(2) = 5;

    return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 4);
  }

} // namespace

// ============================================================================
// Tangent tests
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, Tangent_CubicEndpoints)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  const Geom2dProp::TangentResult aTanFirst = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanFirst.IsDefined);
  // Tangent at start should point roughly toward second pole
  const double aDotFirst = aTanFirst.Direction.X() * 1.0 + aTanFirst.Direction.Y() * 3.0;
  EXPECT_GT(aDotFirst, 0.0);

  const Geom2dProp::TangentResult aTanLast = aProp.Tangent(1.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanLast.IsDefined);
  // Tangent at end should point roughly from 5th pole toward 6th pole
  const double aDotLast = aTanLast.Direction.X() * 1.0 + aTanLast.Direction.Y() * (-2.0);
  EXPECT_GT(aDotLast, 0.0);
}

TEST(Geom2dProp_BSplineCurveTest, Tangent_AtKnots)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  // Tangent should be defined at internal knots
  const Geom2dProp::TangentResult aTan1 = aProp.Tangent(0.33, THE_LIN_TOL);
  ASSERT_TRUE(aTan1.IsDefined);

  const Geom2dProp::TangentResult aTan2 = aProp.Tangent(0.66, THE_LIN_TOL);
  ASSERT_TRUE(aTan2.IsDefined);

  // Directions at different knots should generally differ
  const double aDot = aTan1.Direction.X() * aTan2.Direction.X()
                    + aTan1.Direction.Y() * aTan2.Direction.Y();
  EXPECT_LT(std::abs(aDot), 1.0 - 1.0e-10);
}

// ============================================================================
// Curvature tests
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, Curvature_CubicSmooth)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  for (double u = 0.0; u <= 1.0; u += 0.2)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, THE_LIN_TOL);
    EXPECT_TRUE(aCurv.IsDefined) << "Curvature undefined at U=" << u;
    EXPECT_FALSE(aCurv.IsInfinite) << "Curvature infinite at U=" << u;
  }
}

TEST(Geom2dProp_BSplineCurveTest, Curvature_AtKnots)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  // Curvature at internal knots should be defined for C2 cubic
  const Geom2dProp::CurvatureResult aCurv1 = aProp.Curvature(0.33, THE_LIN_TOL);
  EXPECT_TRUE(aCurv1.IsDefined);

  const Geom2dProp::CurvatureResult aCurv2 = aProp.Curvature(0.66, THE_LIN_TOL);
  EXPECT_TRUE(aCurv2.IsDefined);
}

// ============================================================================
// Normal tests
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, Normal_CubicSmooth)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  for (double u = 0.1; u <= 0.9; u += 0.2)
  {
    const Geom2dProp::NormalResult aNorm = aProp.Normal(u, THE_LIN_TOL);
    if (aNorm.IsDefined)
    {
      // Normal must be perpendicular to tangent
      const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
      ASSERT_TRUE(aTan.IsDefined);
      const double aDot = aNorm.Direction.X() * aTan.Direction.X()
                        + aNorm.Direction.Y() * aTan.Direction.Y();
      EXPECT_NEAR(aDot, 0.0, THE_DIR_TOL) << "Normal not perpendicular at U=" << u;
    }
  }
}

// ============================================================================
// Centre of curvature tests
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, Centre_CubicSmooth)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  for (double u = 0.1; u <= 0.9; u += 0.2)
  {
    const Geom2dProp::CurvatureResult aCurv   = aProp.Curvature(u, THE_LIN_TOL);
    const Geom2dProp::CentreResult    aCentre = aProp.CentreOfCurvature(u, THE_LIN_TOL);
    if (aCurv.IsDefined && !aCurv.IsInfinite && std::abs(aCurv.Value) > THE_LIN_TOL)
    {
      EXPECT_TRUE(aCentre.IsDefined) << "Centre undefined at U=" << u;
    }
  }
}

// ============================================================================
// Global analysis tests
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, FindCurvatureExtrema_Cubic)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);

  // All extrema must be within parameter range
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points.Value(i).Parameter, 0.0);
    EXPECT_LE(aResult.Points.Value(i).Parameter, 1.0);
    EXPECT_NE(aResult.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
  }
}

TEST(Geom2dProp_BSplineCurveTest, FindInflections_Cubic)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);

  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points.Value(i).Parameter, 0.0);
    EXPECT_LE(aResult.Points.Value(i).Parameter, 1.0);
    EXPECT_EQ(aResult.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
  }
}

TEST(Geom2dProp_BSplineCurveTest, FindCurvatureExtrema_LowContinuity)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeQuadraticBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);

  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points.Value(i).Parameter, 0.0);
    EXPECT_LE(aResult.Points.Value(i).Parameter, 1.0);
  }
}

TEST(Geom2dProp_BSplineCurveTest, FindInflections_LowContinuity)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeQuadraticBSpline();
  Geom2dProp_Curve aProp(aBSpline);

  const Geom2dProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);

  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_GE(aResult.Points.Value(i).Parameter, 0.0);
    EXPECT_LE(aResult.Points.Value(i).Parameter, 1.0);
    EXPECT_EQ(aResult.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
  }
}

// ============================================================================
// GetType test
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, GetType_IsBSpline)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve aProp(aBSpline);
  EXPECT_EQ(aProp.GetType(), GeomAbs_BSplineCurve);
}

// ============================================================================
// Cross-validation vs CLProps2d
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_CubicSmooth)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_Quadratic)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeQuadraticBSpline();
  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_Degree4)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeDegree4BSpline();
  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    compareTangent(aProp, aOld, u);
    compareCurvature(aProp, aOld, u);
  }
}

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_AllProperties_Cubic)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();
  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 20);
}

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_AllProperties_Degree4)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeDegree4BSpline();
  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 20);
}

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_LowContinuity)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeQuadraticBSpline();
  Geom2dProp_Curve      aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 1.0, 15);
}

// ============================================================================
// Cross-validation vs CurAndInf2d
// ============================================================================

TEST(Geom2dProp_BSplineCurveTest, VsCurAndInf2d_Cubic_Extrema)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_BSplineCurveTest, VsCurAndInf2d_Cubic_Inflections)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeCubicBSpline();

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld);
}

TEST(Geom2dProp_BSplineCurveTest, VsCurAndInf2d_Degree4_FullPerform)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline = makeDegree4BSpline();

  Geom2dLProp_CurAndInf2d anOld;
  anOld.Perform(aBSpline);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aBSpline);
  const Geom2dProp::CurveAnalysis aNewExt  = aProp.FindCurvatureExtrema();
  const Geom2dProp::CurveAnalysis aNewInfl = aProp.FindInflections();
  ASSERT_TRUE(aNewExt.IsDone);
  ASSERT_TRUE(aNewInfl.IsDone);

  const int aNewTotal = aNewExt.Points.Length() + aNewInfl.Points.Length();
  EXPECT_EQ(aNewTotal, anOld.NbPoints());
}

TEST(Geom2dProp_BSplineCurveTest, VsCLProps2d_CriticalPoints)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 2.0);
  aPoles(3) = gp_Pnt2d(2.0, -1.0);
  aPoles(4) = gp_Pnt2d(3.0, 1.0);
  aPoles(5) = gp_Pnt2d(4.0, -2.0);
  aPoles(6) = gp_Pnt2d(5.0, 0.0);
  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0; aKnots(2) = 0.33; aKnots(3) = 0.66; aKnots(4) = 1.0;
  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4; aMults(2) = 1; aMults(3) = 1; aMults(4) = 4;
  occ::handle<Geom2d_BSplineCurve> aBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
  Geom2dProp_Curve aProp(aBSpline);
  Geom2dLProp_CLProps2d aOld(aBSpline, 2, THE_LIN_TOL);
  const double aParams[] = {0.0, 1.0e-10, 1.0e-6,
                            0.33, 0.33 + 1.0e-6, 0.33 - 1.0e-6,
                            0.66, 0.66 + 1.0e-6, 0.66 - 1.0e-6,
                            1.0 - 1.0e-6, 1.0 - 1.0e-10, 1.0,
                            0.165, 0.495, 0.83};
  for (const double aParam : aParams)
  {
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
