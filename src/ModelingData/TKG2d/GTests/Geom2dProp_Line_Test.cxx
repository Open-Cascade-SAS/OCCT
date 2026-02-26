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

// Tests for Geom2dProp_Curve local differential properties on 2D lines.
// Lines have zero curvature, constant tangent, and no normal or centre of curvature.

#include <Geom2d_Line.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
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
} // namespace

// Test 1: Tangent along X axis
TEST(Geom2dProp_LineTest, Tangent_AlongX)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);
}

// Test 2: Tangent along diagonal (1,1)
TEST(Geom2dProp_LineTest, Tangent_AlongDiagonal)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  const double aSqrt2Inv = 1.0 / std::sqrt(2.0);
  EXPECT_NEAR(aTan.Direction.X(), aSqrt2Inv, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), aSqrt2Inv, THE_DIR_TOL);
}

// Test 3: Tangent at negative parameter - same as positive
TEST(Geom2dProp_LineTest, Tangent_NegativeParam)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTanNeg = aProp.Tangent(-10.0, THE_LIN_TOL);
  const Geom2dProp::TangentResult aTanPos = aProp.Tangent(10.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanNeg.IsDefined);
  ASSERT_TRUE(aTanPos.IsDefined);
  EXPECT_NEAR(aTanNeg.Direction.X(), aTanPos.Direction.X(), THE_DIR_TOL);
  EXPECT_NEAR(aTanNeg.Direction.Y(), aTanPos.Direction.Y(), THE_DIR_TOL);
}

// Test 4: Tangent at origin (param=0)
TEST(Geom2dProp_LineTest, Tangent_AtOrigin)
{
  gp_Lin2d                 aLin(gp_Pnt2d(5.0, 5.0), gp_Dir2d(0.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 1.0, THE_DIR_TOL);
}

// Test 5: Curvature is zero
TEST(Geom2dProp_LineTest, Curvature_IsZero)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
}

// Test 6: Curvature is zero at multiple params
TEST(Geom2dProp_LineTest, Curvature_MultipleParams)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  for (double u = -50.0; u <= 50.0; u += 10.0)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "at u=" << u;
    EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL) << "at u=" << u;
  }
}

// Test 7: Normal is undefined for a line
TEST(Geom2dProp_LineTest, Normal_Undefined)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::NormalResult aNorm = aProp.Normal(0.0, THE_LIN_TOL);
  EXPECT_FALSE(aNorm.IsDefined);
}

// Test 8: Centre of curvature is undefined for a line
TEST(Geom2dProp_LineTest, Centre_Undefined)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  EXPECT_FALSE(aCentre.IsDefined);
}

// Test 9: FindCurvatureExtrema returns empty
TEST(Geom2dProp_LineTest, FindCurvatureExtrema_Empty)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::CurveAnalysis aAnalysis = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aAnalysis.IsDone);
  EXPECT_EQ(aAnalysis.Points.Length(), 0);
}

// Test 10: FindInflections returns empty
TEST(Geom2dProp_LineTest, FindInflections_Empty)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::CurveAnalysis aAnalysis = aProp.FindInflections();
  ASSERT_TRUE(aAnalysis.IsDone);
  EXPECT_EQ(aAnalysis.Points.Length(), 0);
}

// Test 11: GetType returns GeomAbs_Line
TEST(Geom2dProp_LineTest, GetType_IsLine)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Line);
}

// Test 12: Tangent comparison vs CLProps2d at 11 params
TEST(Geom2dProp_LineTest, VsCLProps2d_Tangent)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve      aProp(aLine);
  Geom2dLProp_CLProps2d aOld(aLine, 2, THE_LIN_TOL);

  for (double u = -5.0; u <= 5.0; u += 1.0)
  {
    compareTangent(aProp, aOld, u);
  }
}

// Test 13: Curvature comparison vs CLProps2d at 11 params
TEST(Geom2dProp_LineTest, VsCLProps2d_Curvature)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve      aProp(aLine);
  Geom2dLProp_CLProps2d aOld(aLine, 2, THE_LIN_TOL);

  for (double u = -5.0; u <= 5.0; u += 1.0)
  {
    compareCurvature(aProp, aOld, u);
  }
}

// Test 14: All properties comparison vs CLProps2d (dense)
TEST(Geom2dProp_LineTest, VsCLProps2d_AllProperties)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve      aProp(aLine);
  Geom2dLProp_CLProps2d aOld(aLine, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -10.0, 10.0, 20);
}

// Test 15: Line not at origin
TEST(Geom2dProp_LineTest, Line_OffCenter)
{
  gp_Lin2d                 aLin(gp_Pnt2d(10.0, 20.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
}

// Test 16: Line with arbitrary direction (3,4)
TEST(Geom2dProp_LineTest, Line_ArbitraryDirection)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(3.0, 4.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 3.0 / 5.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 4.0 / 5.0, THE_DIR_TOL);
}

// Test 17: Line at large parameters
TEST(Geom2dProp_LineTest, Line_LargeParams)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(1000.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(1000.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
}

// Test 18: Tangent sign is consistent with line direction
TEST(Geom2dProp_LineTest, Tangent_ConsistentSign)
{
  gp_Dir2d                 aDir(3.0, 4.0);
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), aDir);
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  for (double u = -10.0; u <= 10.0; u += 2.0)
  {
    const Geom2dProp::TangentResult aTan = aProp.Tangent(u, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    const double aDot = aTan.Direction.X() * aDir.X() + aTan.Direction.Y() * aDir.Y();
    EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL) << "at u=" << u;
  }
}

// Test 19: Curvature is exactly zero (not just near-zero)
TEST(Geom2dProp_LineTest, Curvature_ExactlyZero)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_EQ(aCurv.Value, 0.0);
}

// Test 20: All properties comparison vs CLProps2d for off-center line
TEST(Geom2dProp_LineTest, VsCLProps2d_OffCenter)
{
  gp_Lin2d                 aLin(gp_Pnt2d(10.0, 20.0), gp_Dir2d(3.0, 4.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve      aProp(aLine);
  Geom2dLProp_CLProps2d aOld(aLine, 2, THE_LIN_TOL);

  compareAll(aProp, aOld, -10.0, 10.0, 20);
}

TEST(Geom2dProp_LineTest, VsCLProps2d_CriticalPoints)
{
  gp_Lin2d                 aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  Geom2dProp_Curve         aProp(aLine);
  Geom2dLProp_CLProps2d    aOld(aLine, 2, THE_LIN_TOL);
  const double aParams[] = {0.0, 1.0e-10, -1.0e-10, 1.0e-6, -1.0e-6, 1.0e6, -1.0e6, 0.5, -0.5};
  for (const double aParam : aParams)
  {
    compareTangent(aProp, aOld, aParam);
    compareCurvature(aProp, aOld, aParam);
    compareNormal(aProp, aOld, aParam);
    compareCentre(aProp, aOld, aParam);
  }
}
