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

// Unit tests for GeomProp_Curve with Line geometry.

#include <Geom_Line.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
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

// ============================================================================
// Analytical reference tests
// ============================================================================

TEST(GeomProp_LineTest, Tangent_AlongX)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 1.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), 0.0, THE_DIR_TOL);
}

TEST(GeomProp_LineTest, Tangent_AlongDiagonal)
{
  const double           aInvSqrt3 = 1.0 / std::sqrt(3.0);
  occ::handle<Geom_Line> aLine     = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTan = aProp.Tangent(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), aInvSqrt3, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Y(), aInvSqrt3, THE_DIR_TOL);
  EXPECT_NEAR(aTan.Direction.Z(), aInvSqrt3, THE_DIR_TOL);
}

TEST(GeomProp_LineTest, Tangent_NegativeParam)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTanPos = aProp.Tangent(3.0, THE_LIN_TOL);
  const GeomProp::TangentResult aTanNeg = aProp.Tangent(-3.0, THE_LIN_TOL);
  ASSERT_TRUE(aTanPos.IsDefined);
  ASSERT_TRUE(aTanNeg.IsDefined);
  const double aDot = aTanPos.Direction.Dot(aTanNeg.Direction);
  EXPECT_NEAR(aDot, 1.0, THE_DIR_TOL);
}

TEST(GeomProp_LineTest, Tangent_AtOrigin)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTan = aProp.Tangent(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.Z(), 1.0, THE_DIR_TOL);
}

TEST(GeomProp_LineTest, Curvature_IsZero)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(1.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_FALSE(aCurv.IsInfinite);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
}

TEST(GeomProp_LineTest, Curvature_MultipleParams)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const double aParams[] = {-10.0, -1.0, 0.0, 0.5, 1.0, 10.0, 100.0};
  for (const double aParam : aParams)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL) << "Non-zero curvature at param=" << aParam;
  }
}

TEST(GeomProp_LineTest, Normal_Undefined)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::NormalResult aNorm = aProp.Normal(0.5, THE_LIN_TOL);
  EXPECT_FALSE(aNorm.IsDefined);
}

TEST(GeomProp_LineTest, Centre_Undefined)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(0.5, THE_LIN_TOL);
  EXPECT_FALSE(aCentre.IsDefined);
}

TEST(GeomProp_LineTest, FindCurvatureExtrema_Empty)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_LineTest, FindInflections_Empty)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_LineTest, GetType_IsLine)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  EXPECT_EQ(aProp.GetType(), GeomAbs_Line);
}

TEST(GeomProp_LineTest, Line_OffCenter)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(10, 20, 30), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTan = aProp.Tangent(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 1.0, THE_DIR_TOL);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(5.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);

  const GeomProp::NormalResult aNorm = aProp.Normal(5.0, THE_LIN_TOL);
  EXPECT_FALSE(aNorm.IsDefined);

  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(5.0, THE_LIN_TOL);
  EXPECT_FALSE(aCentre.IsDefined);
}

TEST(GeomProp_LineTest, Line_ArbitraryDirection)
{
  const gp_Dir           aDir(3, 4, 5);
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), aDir);
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTan = aProp.Tangent(2.5, THE_LIN_TOL);
  ASSERT_TRUE(aTan.IsDefined);
  const double aDot = aTan.Direction.Dot(aDir);
  EXPECT_NEAR(std::abs(aDot), 1.0, THE_DIR_TOL);
}

TEST(GeomProp_LineTest, Line_LargeParams)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  for (const double aParam : {1000.0, -1000.0})
  {
    const GeomProp::TangentResult aTan = aProp.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined) << "Tangent undefined at param=" << aParam;
    EXPECT_NEAR(aTan.Direction.X(), 1.0, THE_DIR_TOL);

    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_NEAR(aCurv.Value, 0.0, THE_CURV_TOL);
  }
}

TEST(GeomProp_LineTest, Tangent_ConsistentSign)
{
  const gp_Dir           aDir(0, 1, 0);
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), aDir);
  GeomProp_Curve         aProp(aLine);

  // Tangent direction should be consistent with the line direction at all params.
  for (const double aParam : {-5.0, 0.0, 5.0})
  {
    const GeomProp::TangentResult aTan = aProp.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined);
    const double aDot = aTan.Direction.Dot(aDir);
    EXPECT_NEAR(aDot, 1.0, THE_DIR_TOL)
      << "Tangent sign inconsistent with line direction at param=" << aParam;
  }
}

TEST(GeomProp_LineTest, Curvature_ExactlyZero)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_EQ(aCurv.Value, 0.0);
}

// ============================================================================
// Cross-validation tests vs GeomLProp_CLProps
// ============================================================================

TEST(GeomProp_LineTest, VsCLProps_Tangent)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 0));

  const double aStep = 1.0;
  for (int i = 0; i <= 10; ++i)
  {
    const double aParam = -5.0 + i * aStep;
    compareTangent(aLine, aParam);
  }
}

TEST(GeomProp_LineTest, VsCLProps_Curvature)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

  const double aStep = 1.0;
  for (int i = 0; i <= 10; ++i)
  {
    const double aParam = -5.0 + i * aStep;
    compareCurvature(aLine, aParam);
  }
}

TEST(GeomProp_LineTest, VsCLProps_AllProperties)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  compareAll(aLine, -5.0, 5.0, 20);
}

TEST(GeomProp_LineTest, VsCLProps_OffCenter)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(10, 20, 30), gp_Dir(3, 4, 5));
  compareAll(aLine, -10.0, 10.0);
}

TEST(GeomProp_LineTest, Normal_Undefined_MultipleParams)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const double aParams[] = {-100.0, -1.0, 0.0, 1.0, 100.0};
  for (const double aParam : aParams)
  {
    const GeomProp::NormalResult aNorm = aProp.Normal(aParam, THE_LIN_TOL);
    EXPECT_FALSE(aNorm.IsDefined) << "Normal should be undefined at param=" << aParam;
  }
}

TEST(GeomProp_LineTest, Centre_Undefined_MultipleParams)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);

  const double aParams[] = {-100.0, -1.0, 0.0, 1.0, 100.0};
  for (const double aParam : aParams)
  {
    const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(aParam, THE_LIN_TOL);
    EXPECT_FALSE(aCentre.IsDefined) << "Centre should be undefined at param=" << aParam;
  }
}

TEST(GeomProp_LineTest, VsCLProps_CriticalPoints)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 1, 1));
  const double aParams[] = {0.0, 1.0e-10, -1.0e-10, 1.0e-6, -1.0e-6, 1.0e6, -1.0e6, 0.5, -0.5};
  for (const double aParam : aParams)
  {
    compareTangent(aLine, aParam);
    compareCurvature(aLine, aParam);
    compareNormal(aLine, aParam);
    compareCentre(aLine, aParam);
  }
}
