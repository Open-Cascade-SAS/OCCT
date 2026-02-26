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

// Tests for Geom2dProp_Curve local differential properties on 2D ellipses.
// Ellipse with semi-axes a (major) and b (minor):
// - Curvature at major vertex (param=0):   k = a / b^2
// - Curvature at minor vertex (param=PI/2): k = b / a^2
// - Analytical curvature: k(t) = a*b / (a^2*sin^2(t) + b^2*cos^2(t))^(3/2)
// - 4 curvature extrema, no inflections

#include <Geom2d_Ellipse.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <LProp_CIType.hxx>
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

//! Map LProp_CIType to Geom2dProp::CIType for comparison.
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

//! Compare extrema results from old and new APIs.
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
} // namespace

// Test 1: Curvature at major vertex (param=0): k = a/b^2
TEST(Geom2dProp_EllipseTest, Curvature_AtMajorVertex)
{
  const double                a = 10.0;
  const double                b = 5.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.Value), a / (b * b), THE_CURV_TOL);
}

// Test 2: Curvature at minor vertex (param=PI/2): k = b/a^2
TEST(Geom2dProp_EllipseTest, Curvature_AtMinorVertex)
{
  const double                a = 10.0;
  const double                b = 5.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.Value), b / (a * a), THE_CURV_TOL);
}

// Test 3: Curvature symmetry: k(t) = k(-t)
TEST(Geom2dProp_EllipseTest, Curvature_Symmetry)
{
  const double                a = 10.0;
  const double                b = 5.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  for (double t = 0.1; t < M_PI; t += 0.3)
  {
    const Geom2dProp::CurvatureResult aCurvPos = aProp.Curvature(t, THE_LIN_TOL);
    const Geom2dProp::CurvatureResult aCurvNeg = aProp.Curvature(-t, THE_LIN_TOL);
    ASSERT_TRUE(aCurvPos.IsDefined);
    ASSERT_TRUE(aCurvNeg.IsDefined);
    EXPECT_NEAR(std::abs(aCurvPos.Value), std::abs(aCurvNeg.Value), THE_CURV_TOL) << "at t=" << t;
  }
}

// Test 4: Tangent at major vertex is perpendicular to major axis
TEST(Geom2dProp_EllipseTest, Tangent_AtMajorVertex)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // At param=0, tangent perpendicular to major axis (1,0) => tangent along Y
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, THE_DIR_TOL);
}

// Test 5: Tangent at minor vertex is perpendicular to minor axis
TEST(Geom2dProp_EllipseTest, Tangent_AtMinorVertex)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // At param=PI/2, tangent perpendicular to minor axis (0,1) => tangent along X
  EXPECT_NEAR(std::abs(aTan.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);
}

// Test 6: Normal at major vertex is along major axis toward center
TEST(Geom2dProp_EllipseTest, Normal_AtMajorVertex)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  // At param=0, point is at (a,0), normal toward center => (-1,0)
  EXPECT_NEAR(std::abs(aNorm.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
}

// Test 7: Normal at minor vertex is along minor axis toward center
TEST(Geom2dProp_EllipseTest, Normal_AtMinorVertex)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::NormalResult aNorm = aProp.Normal(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  // At param=PI/2, point is at (0,b), normal toward center => (0,-1)
  EXPECT_NEAR(aNorm.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(std::abs(aNorm.Direction.Y()), 1.0, THE_DIR_TOL);
}

// Test 8: Centre at major vertex: radius of curvature = b^2/a
TEST(Geom2dProp_EllipseTest, Centre_AtMajorVertex)
{
  const double                a = 10.0;
  const double                b = 5.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  // Centre is at distance b^2/a from the point along the normal toward center
  // Point at (a,0), centre at (a - b^2/a, 0)
  const double aExpectedX = a - (b * b) / a;
  EXPECT_NEAR(aCentre.Centre.X(), aExpectedX, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Y(), 0.0, THE_POINT_TOL);
}

// Test 9: Centre at minor vertex: radius of curvature = a^2/b
TEST(Geom2dProp_EllipseTest, Centre_AtMinorVertex)
{
  const double                a = 10.0;
  const double                b = 5.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  // Point at (0,b), centre at (0, b - a^2/b)
  const double aExpectedY = b - (a * a) / b;
  EXPECT_NEAR(aCentre.Centre.X(), 0.0, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Y(), aExpectedY, THE_POINT_TOL);
}

// Test 10: FindCurvatureExtrema returns 4 points
TEST(Geom2dProp_EllipseTest, FindCurvatureExtrema_FourPoints)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CurveAnalysis aAnalysis = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aAnalysis.IsDone);
  EXPECT_EQ(aAnalysis.Points.Length(), 4);
}

// Test 11: FindInflections returns empty (no inflections)
TEST(Geom2dProp_EllipseTest, FindInflections_Empty)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CurveAnalysis aAnalysis = aProp.FindInflections();
  ASSERT_TRUE(aAnalysis.IsDone);
  EXPECT_EQ(aAnalysis.Points.Length(), 0);
}

// Test 12: GetType returns GeomAbs_Ellipse
TEST(Geom2dProp_EllipseTest, GetType_IsEllipse)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Ellipse);
}

// Test 13: Curvature for high eccentricity ellipse (a=100, b=1)
TEST(Geom2dProp_EllipseTest, Curvature_HighEccentricity)
{
  const double                a = 100.0;
  const double                b = 1.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  // At major vertex: k = a/b^2 = 100
  const Geom2dProp::CurvatureResult aCurvMaj = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvMaj.IsDefined);
  EXPECT_NEAR(std::abs(aCurvMaj.Value), a / (b * b), 1.0e-6);

  // At minor vertex: k = b/a^2 = 0.0001
  const Geom2dProp::CurvatureResult aCurvMin = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvMin.IsDefined);
  EXPECT_NEAR(std::abs(aCurvMin.Value), b / (a * a), 1.0e-10);
}

// Test 14: Curvature for nearly circular ellipse (a=5, b=4.99)
TEST(Geom2dProp_EllipseTest, Curvature_NearlyCircular)
{
  const double                a = 5.0;
  const double                b = 4.99;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  const Geom2dProp::CurvatureResult aCurvMaj = aProp.Curvature(0.0, THE_LIN_TOL);
  const Geom2dProp::CurvatureResult aCurvMin = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurvMaj.IsDefined);
  ASSERT_TRUE(aCurvMin.IsDefined);
  // For nearly circular, curvatures at vertices should be close
  EXPECT_NEAR(std::abs(aCurvMaj.Value), a / (b * b), THE_CURV_TOL);
  EXPECT_NEAR(std::abs(aCurvMin.Value), b / (a * a), THE_CURV_TOL);
}

// Test 15: Standard ellipse (a=10, b=5) comparison vs CLProps2d
TEST(Geom2dProp_EllipseTest, VsCLProps2d_Standard)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 24);
}

// Test 16: High eccentricity (a=100, b=1) comparison vs CLProps2d
TEST(Geom2dProp_EllipseTest, VsCLProps2d_HighEccentricity)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 100.0, 1.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 24);
}

// Test 17: Dense comparison of all properties vs CLProps2d
TEST(Geom2dProp_EllipseTest, VsCLProps2d_AllProperties)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 48);
}

// Test 18: Off-center ellipse comparison vs CLProps2d
TEST(Geom2dProp_EllipseTest, VsCLProps2d_OffCenter)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(100.0, -50.0), gp_Dir2d(1.0, 0.0)), 8.0, 3.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve      aProp(anEllipse);
  Geom2dLProp_CLProps2d aOld(anEllipse, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 24);
}

// Test 19: Compare FindCurvatureExtrema vs old CurAndInf2d
TEST(Geom2dProp_EllipseTest, VsCurAndInf2d_Extrema)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(anEllipse);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(anEllipse);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

// Test 20: Compare FindInflections vs old CurAndInf2d (no inflections)
TEST(Geom2dProp_EllipseTest, VsCurAndInf2d_NoInflections)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(anEllipse);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(anEllipse);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

// Test 21: Curvature matches analytical formula k(t) = ab/(a^2*sin^2(t) + b^2*cos^2(t))^(3/2)
TEST(Geom2dProp_EllipseTest, Curvature_MatchesAnalytical)
{
  const double                a = 10.0;
  const double                b = 5.0;
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), a, b);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);

  for (double t = 0.0; t < 2.0 * M_PI; t += M_PI / 12.0)
  {
    const double aSin       = std::sin(t);
    const double aCos       = std::cos(t);
    const double aDenom     = a * a * aSin * aSin + b * b * aCos * aCos;
    const double aExpectedK = (a * b) / (aDenom * std::sqrt(aDenom));

    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(t, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "at t=" << t;
    EXPECT_NEAR(std::abs(aCurv.Value), aExpectedK, THE_CURV_TOL) << "at t=" << t;
  }
}

TEST(Geom2dProp_EllipseTest, VsCLProps2d_CriticalPoints)
{
  gp_Elips2d                  anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 3.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dProp_Curve            aProp(anEllipse);
  Geom2dLProp_CLProps2d       aOld(anEllipse, 2, THE_LIN_TOL);
  const double                aParams[] = {0.0,
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
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
