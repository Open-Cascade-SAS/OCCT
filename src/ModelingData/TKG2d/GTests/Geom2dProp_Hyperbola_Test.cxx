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

#include <Geom2d_Hyperbola.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
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

} // namespace

TEST(Geom2dProp_HyperbolaTest, Curvature_AtVertex)
{
  // For hyperbola with semi-axes a (real) and b (imaginary),
  // curvature at vertex (t=0) is k = a / b^2.
  const double                  anA = 6.0;
  const double                  aB  = 3.0;
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), anA, aB);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve                  aProp(aHyperbola);
  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);

  ASSERT_TRUE(aCurv.IsDefined);
  ASSERT_FALSE(aCurv.IsInfinite);
  const double anExpected = anA / (aB * aB);
  EXPECT_NEAR(aCurv.Value, anExpected, THE_CURV_TOL);
}

TEST(Geom2dProp_HyperbolaTest, Curvature_Symmetric)
{
  // k(t) = k(-t) for a centered hyperbola.
  const double                  anA = 5.0;
  const double                  aB  = 2.0;
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), anA, aB);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve aProp(aHyperbola);
  for (double t = 0.1; t <= 2.0; t += 0.3)
  {
    const Geom2dProp::CurvatureResult aCurvPos = aProp.Curvature(t, THE_LIN_TOL);
    const Geom2dProp::CurvatureResult aCurvNeg = aProp.Curvature(-t, THE_LIN_TOL);
    ASSERT_TRUE(aCurvPos.IsDefined);
    ASSERT_TRUE(aCurvNeg.IsDefined);
    EXPECT_NEAR(aCurvPos.Value, aCurvNeg.Value, THE_CURV_TOL) << "Asymmetry at t=" << t;
  }
}

TEST(Geom2dProp_HyperbolaTest, Curvature_DecreasesFromVertex)
{
  // Curvature magnitude decreases as |t| increases from vertex.
  const double                  anA = 8.0;
  const double                  aB  = 4.0;
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), anA, aB);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve aProp(aHyperbola);
  double           aPrevCurv = 1.0e30;
  for (double t = 0.0; t <= 3.0; t += 0.5)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(t, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    const double anAbsCurv = std::abs(aCurv.Value);
    EXPECT_LE(anAbsCurv, aPrevCurv + THE_CURV_TOL) << "Curvature not decreasing at t=" << t;
    aPrevCurv = anAbsCurv;
  }
}

TEST(Geom2dProp_HyperbolaTest, Tangent_AtVertex)
{
  // At vertex (t=0), tangent is perpendicular to the real axis.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);

  ASSERT_TRUE(aTan.IsDefined);
  // Tangent at vertex should be along Y axis (perpendicular to real axis X).
  const double aDotX = aTan.Direction.X() * 1.0 + aTan.Direction.Y() * 0.0;
  EXPECT_NEAR(std::abs(aDotX), 0.0, THE_DIR_TOL);
}

TEST(Geom2dProp_HyperbolaTest, Tangent_Symmetric)
{
  // Tangent at t and -t: X-components equal, Y-components opposite (or vice versa).
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve aProp(aHyperbola);
  for (double t = 0.2; t <= 2.0; t += 0.4)
  {
    const Geom2dProp::TangentResult aTanPos = aProp.Tangent(t, THE_LIN_TOL);
    const Geom2dProp::TangentResult aTanNeg = aProp.Tangent(-t, THE_LIN_TOL);
    ASSERT_TRUE(aTanPos.IsDefined);
    ASSERT_TRUE(aTanNeg.IsDefined);
    // For hyperbola (a*cosh(t), b*sinh(t)), derivative is (a*sinh(t), b*cosh(t)).
    // At -t: (-a*sinh(t), b*cosh(t)), so X-components are opposite, Y-components are equal.
    EXPECT_NEAR(aTanPos.Direction.X(), -aTanNeg.Direction.X(), THE_DIR_TOL);
    EXPECT_NEAR(aTanPos.Direction.Y(), aTanNeg.Direction.Y(), THE_DIR_TOL);
  }
}

TEST(Geom2dProp_HyperbolaTest, Normal_AtVertex)
{
  // At vertex, normal should be along real axis toward center.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve               aProp(aHyperbola);
  const Geom2dProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);

  ASSERT_TRUE(aNorm.IsDefined);
  // Normal at vertex should be along X axis (toward center at origin).
  const double aDotX = aNorm.Direction.X() * 1.0 + aNorm.Direction.Y() * 0.0;
  EXPECT_NEAR(std::abs(aDotX), 1.0, THE_DIR_TOL);
}

TEST(Geom2dProp_HyperbolaTest, Centre_AtVertex)
{
  // Centre of curvature at vertex is at distance 1/k = b^2/a from vertex along axis.
  const double                  anA = 6.0;
  const double                  aB  = 3.0;
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), anA, aB);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve               aProp(aHyperbola);
  const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);

  ASSERT_TRUE(aCentre.IsDefined);
  // Vertex is at (a, 0). Radius of curvature = b^2/a.
  // The centre of curvature is on the convex side (away from hyperbola center),
  // so centre is at (a + b^2/a, 0) = ((a^2 + b^2)/a, 0).
  const double anExpectedX = (anA * anA + aB * aB) / anA;
  EXPECT_NEAR(aCentre.Centre.X(), anExpectedX, THE_POINT_TOL);
  EXPECT_NEAR(aCentre.Centre.Y(), 0.0, THE_POINT_TOL);
}

TEST(Geom2dProp_HyperbolaTest, FindCurvatureExtrema_OnePoint)
{
  // Hyperbola has exactly one curvature extremum at vertex (t=0).
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::CurveAnalysis aExtrema = aProp.FindCurvatureExtrema();

  ASSERT_TRUE(aExtrema.IsDone);
  EXPECT_EQ(aExtrema.Points.Length(), 1);
  if (aExtrema.Points.Length() == 1)
  {
    EXPECT_NEAR(aExtrema.Points.Value(0).Parameter, 0.0, 1.0e-6);
  }
}

TEST(Geom2dProp_HyperbolaTest, FindInflections_Empty)
{
  // Hyperbola has no inflection points.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::CurveAnalysis aInflections = aProp.FindInflections();

  ASSERT_TRUE(aInflections.IsDone);
  EXPECT_EQ(aInflections.Points.Length(), 0);
}

TEST(Geom2dProp_HyperbolaTest, GetType_IsHyperbola)
{
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve aProp(aHyperbola);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Hyperbola);
}

TEST(Geom2dProp_HyperbolaTest, Curvature_LargeParam)
{
  // Curvature approaches 0 for large |t|.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve                  aProp(aHyperbola);
  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(10.0, THE_LIN_TOL);

  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_LT(std::abs(aCurv.Value), 0.01);
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_Standard)
{
  // Cross-validate against deprecated CLProps2d with a=6, b=3.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -2.0, 2.0, 10);
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_AllProperties)
{
  // Dense comparison over [-2, 2].
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -2.0, 2.0, 40);
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_LargeAxes)
{
  // Cross-validate with a=100, b=50.
  gp_Hypr2d anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 100.0, 50.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -2.0, 2.0, 10);
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_SmallAxes)
{
  // Cross-validate with a=0.5, b=0.3.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.5, 0.3);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -2.0, 2.0, 10);
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_AsymmetricAxes)
{
  // Cross-validate with a=10, b=1.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 1.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -2.0, 2.0, 10);
}

TEST(Geom2dProp_HyperbolaTest, Curvature_NearVertex)
{
  // Fine-grained curvature check near vertex.
  const double                  anA = 6.0;
  const double                  aB  = 3.0;
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), anA, aB);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve aProp(aHyperbola);
  const double     anExpectedMax = anA / (aB * aB);

  for (double t = -0.5; t <= 0.5; t += 0.05)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(t, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LE(std::abs(aCurv.Value), anExpectedMax + THE_CURV_TOL)
      << "Curvature exceeds vertex maximum at t=" << t;
  }
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_OffCenter)
{
  // Cross-validate hyperbola centered at (5,5).
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(5.0, 5.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dProp_Curve      aProp(aHyperbola);
  Geom2dLProp_CLProps2d aOld(aHyperbola, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -2.0, 2.0, 10);
}

TEST(Geom2dProp_HyperbolaTest, VsCurAndInf2d_Extrema)
{
  // Compare curvature extrema with deprecated CurAndInf2d.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aHyperbola);
  ASSERT_TRUE(anOld.IsDone());

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);

  compareExtrema(aNew, anOld, 1.0e-6);
}

TEST(Geom2dProp_HyperbolaTest, VsCurAndInf2d_NoInflections)
{
  // Compare inflection results with deprecated CurAndInf2d.
  gp_Hypr2d                     anHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(anHypr);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformInf(aHyperbola);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(aHyperbola);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindInflections();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

TEST(Geom2dProp_HyperbolaTest, VsCLProps2d_CriticalPoints)
{
  gp_Hypr2d                     aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(aHypr);
  Geom2dProp_Curve              aProp(aHyperbola);
  Geom2dLProp_CLProps2d         aOld(aHyperbola, 2, THE_LIN_TOL);
  const double                  aParams[] = {0.0,
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
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
