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

// Tests for Geom2dProp_Curve local differential properties on 2D circles.
// Circles have constant curvature 1/R, tangent perpendicular to radius,
// normal pointing to center, and centre of curvature at the circle center.

#include <Geom2d_Circle.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
  Standard_ENABLE_DEPRECATION_WARNINGS
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
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

  void compareTangent(Geom2dProp_Curve & theProp,
                      Geom2dLProp_CLProps2d & theOld,
                      const double theParam)
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

  void compareCurvature(Geom2dProp_Curve & theProp,
                        Geom2dLProp_CLProps2d & theOld,
                        const double theParam)
  {
    theOld.SetParameter(theParam);
    const Geom2dProp::CurvatureResult aNew     = theProp.Curvature(theParam, THE_LIN_TOL);
    const double                      aOldCurv = theOld.Curvature();
    if (aNew.IsDefined && !aNew.IsInfinite)
    {
      EXPECT_NEAR(aNew.Value, aOldCurv, THE_CURV_TOL);
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
      EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
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
      EXPECT_NEAR(aNew.Centre.X(), anOldCentre.X(), THE_POINT_TOL);
      EXPECT_NEAR(aNew.Centre.Y(), anOldCentre.Y(), THE_POINT_TOL);
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
} // namespace

// Test 1: Curvature is constant 1/R
TEST(Geom2dProp_CircleTest, Curvature_IsConstant)
{
  const double               aRadius = 5.0;
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.Value), 1.0 / aRadius, THE_CURV_TOL);
}

// Test 2: Curvature constant at 12 equally spaced params
TEST(Geom2dProp_CircleTest, Curvature_AllParams)
{
  const double               aRadius = 7.0;
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  for (int i = 0; i < 12; ++i)
  {
    const double                      u     = i * M_PI / 6.0;
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "at u=" << u;
    EXPECT_NEAR(std::abs(aCurv.Value), 1.0 / aRadius, THE_CURV_TOL) << "at u=" << u;
  }
}

// Test 3: Curvature for small radius
TEST(Geom2dProp_CircleTest, Curvature_SmallRadius)
{
  const double               aRadius = 0.01;
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.Value), 100.0, THE_CURV_TOL);
}

// Test 4: Curvature for large radius
TEST(Geom2dProp_CircleTest, Curvature_LargeRadius)
{
  const double               aRadius = 1000.0;
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), aRadius);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.Value), 0.001, THE_CURV_TOL);
}

// Test 5: Tangent at param=0
TEST(Geom2dProp_CircleTest, Tangent_AtZero)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // At param=0, point is on positive X axis, tangent should be along Y
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
}

// Test 6: Tangent at PI/2
TEST(Geom2dProp_CircleTest, Tangent_AtPiOver2)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // At PI/2, point is on positive Y axis, tangent should be along -X
  EXPECT_NEAR(std::abs(aTan.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);
}

// Test 7: Tangent at PI
TEST(Geom2dProp_CircleTest, Tangent_AtPi)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(M_PI, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  // At PI, point is on negative X axis, tangent should be along -Y
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, THE_DIR_TOL);
}

// Test 8: Normal points toward center at param=0
TEST(Geom2dProp_CircleTest, Normal_PointsToCenter)
{
  const gp_Pnt2d             aCenter(0.0, 0.0);
  gp_Circ2d                  aCirc(gp_Ax2d(aCenter, gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aNorm.IsDefined);
  // At param=0, point is at (5,0), normal toward center (0,0) is (-1,0)
  EXPECT_NEAR(std::abs(aNorm.Direction.X()), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aNorm.Direction.Y(), 0.0, THE_DIR_TOL);
}

// Test 9: Normal always points toward center at all params
TEST(Geom2dProp_CircleTest, Normal_AllParams)
{
  const gp_Pnt2d             aCenter(0.0, 0.0);
  const double               aRadius = 5.0;
  gp_Circ2d                  aCirc(gp_Ax2d(aCenter, gp_Dir2d(1.0, 0.0)), aRadius);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  for (int i = 0; i < 12; ++i)
  {
    const double                   u     = i * M_PI / 6.0;
    const Geom2dProp::NormalResult aNorm = aProp.Normal(u, THE_LIN_TOL);
    ASSERT_TRUE(aNorm.IsDefined) << "at u=" << u;

    // Point on circle
    const double aPx = aRadius * std::cos(u);
    const double aPy = aRadius * std::sin(u);
    // Direction from point to center
    const double aDx  = aCenter.X() - aPx;
    const double aDy  = aCenter.Y() - aPy;
    const double aLen = std::sqrt(aDx * aDx + aDy * aDy);
    // Normal should be parallel to this direction
    const double aDot = aNorm.Direction.X() * (aDx / aLen) + aNorm.Direction.Y() * (aDy / aLen);
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "at u=" << u;
  }
}

// Test 10: Centre of curvature is the circle center at all params
TEST(Geom2dProp_CircleTest, Centre_IsCircleCenter)
{
  const gp_Pnt2d             aCenter(0.0, 0.0);
  gp_Circ2d                  aCirc(gp_Ax2d(aCenter, gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  for (int i = 0; i < 12; ++i)
  {
    const double                   u       = i * M_PI / 6.0;
    const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined) << "at u=" << u;
    EXPECT_NEAR(aCentre.Centre.X(), aCenter.X(), THE_POINT_TOL) << "at u=" << u;
    EXPECT_NEAR(aCentre.Centre.Y(), aCenter.Y(), THE_POINT_TOL) << "at u=" << u;
  }
}

// Test 11: Centre of curvature for off-center circle
TEST(Geom2dProp_CircleTest, Centre_OffCenter)
{
  const gp_Pnt2d             aCenter(3.0, 4.0);
  gp_Circ2d                  aCirc(gp_Ax2d(aCenter, gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  for (int i = 0; i < 8; ++i)
  {
    const double                   u       = i * M_PI / 4.0;
    const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCentre.IsDefined) << "at u=" << u;
    EXPECT_NEAR(aCentre.Centre.X(), aCenter.X(), THE_POINT_TOL) << "at u=" << u;
    EXPECT_NEAR(aCentre.Centre.Y(), aCenter.Y(), THE_POINT_TOL) << "at u=" << u;
  }
}

// Test 12: FindCurvatureExtrema returns empty (constant curvature)
TEST(Geom2dProp_CircleTest, FindCurvatureExtrema_Empty)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::CurveAnalysis aAnalysis = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aAnalysis.IsDone);
  EXPECT_EQ(aAnalysis.Points.Length(), 0);
}

// Test 13: FindInflections returns empty
TEST(Geom2dProp_CircleTest, FindInflections_Empty)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  const Geom2dProp::CurveAnalysis aAnalysis = aProp.FindInflections();
  ASSERT_TRUE(aAnalysis.IsDone);
  EXPECT_EQ(aAnalysis.Points.Length(), 0);
}

// Test 14: GetType returns GeomAbs_Circle
TEST(Geom2dProp_CircleTest, GetType_IsCircle)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);
}

// Test 15: Full circle comparison vs CLProps2d
TEST(Geom2dProp_CircleTest, VsCLProps2d_AllProperties)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 24);
}

// Test 16: Small radius comparison vs CLProps2d
TEST(Geom2dProp_CircleTest, VsCLProps2d_SmallRadius)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.01);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 12);
}

// Test 17: Large radius comparison vs CLProps2d
TEST(Geom2dProp_CircleTest, VsCLProps2d_LargeRadius)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1000.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 12);
}

// Test 18: Off-center circle comparison vs CLProps2d
TEST(Geom2dProp_CircleTest, VsCLProps2d_OffCenter)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(3.0, 4.0), gp_Dir2d(1.0, 0.0)), 7.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, 0.0, 2.0 * M_PI, 12);
}

// Test 19: Tangent is perpendicular to radius direction
TEST(Geom2dProp_CircleTest, Tangent_Perpendicular)
{
  const gp_Pnt2d             aCenter(0.0, 0.0);
  const double               aRadius = 5.0;
  gp_Circ2d                  aCirc(gp_Ax2d(aCenter, gp_Dir2d(1.0, 0.0)), aRadius);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dProp_Curve           aProp(aCircle);

  for (int i = 0; i < 12; ++i)
  {
    const double                    u    = i * M_PI / 6.0;
    const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined) << "at u=" << u;

    // Radius direction at this point
    const double aRx = std::cos(u);
    const double aRy = std::sin(u);
    // Tangent should be perpendicular to radius: dot product ~= 0
    const double aDot = aTan.Direction.X() * aRx + aTan.Direction.Y() * aRy;
    EXPECT_NEAR(aDot, 0.0, THE_DIR_TOL) << "at u=" << u;
  }
}

// Test 20: Compare FindCurvatureExtrema vs old CurAndInf2d
TEST(Geom2dProp_CircleTest, VsCurAndInf2d_NoExtrema)
{
  gp_Circ2d                  aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dLProp_CurAndInf2d anOld;
  anOld.PerformCurExt(aCircle);
  ASSERT_TRUE(anOld.IsDone());
  EXPECT_EQ(anOld.NbPoints(), 0);

  Geom2dProp_Curve                aProp(aCircle);
  const Geom2dProp::CurveAnalysis aNew = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aNew.IsDone);
  EXPECT_EQ(aNew.Points.Length(), 0);
}

TEST(Geom2dProp_CircleTest, VsCLProps2d_CriticalPoints)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  Geom2dProp_Curve      aProp(aCircle);
  Geom2dLProp_CLProps2d aOld(aCircle, 2, THE_LIN_TOL);
  const double          aParams[] = {0.0,
                                     1.0e-10,
                                     M_PI / 4.0,
                                     M_PI / 2.0,
                                     M_PI,
                                     3.0 * M_PI / 2.0,
                                     2.0 * M_PI - 1.0e-10,
                                     M_PI / 2.0 + 1.0e-6,
                                     M_PI / 2.0 - 1.0e-6};
  for (const double aParam : aParams)
  {
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
