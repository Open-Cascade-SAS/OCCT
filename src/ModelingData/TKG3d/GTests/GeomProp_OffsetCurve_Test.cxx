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

// Tests for GeomProp_Curve with offset curves: curves at constant distance
// from a base curve in a given direction. Properties computed numerically.

#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_OffsetCurve.hxx>
Standard_DISABLE_DEPRECATION_WARNINGS
#include <GeomLProp_CLProps.hxx>
Standard_ENABLE_DEPRECATION_WARNINGS
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
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

void compareTangent(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve                aProp(theCurve);
  const GeomProp::TangentResult aNew = aProp.Tangent(theParam, THE_LIN_TOL);
  GeomLProp_CLProps             anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Dir anOldDir;
    anOld.Tangent(anOldDir);
    EXPECT_NEAR(std::abs(aNew.Direction.Dot(anOldDir)), 1.0, THE_DIR_TOL);
  }
}

void compareCurvature(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve                  aProp(theCurve);
  const GeomProp::CurvatureResult aNew = aProp.Curvature(theParam, THE_LIN_TOL);
  GeomLProp_CLProps               anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined())
  {
    ASSERT_TRUE(aNew.IsDefined);
    EXPECT_NEAR(aNew.Value, anOld.Curvature(), THE_CURV_TOL);
  }
}

void compareNormal(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve               aProp(theCurve);
  const GeomProp::NormalResult aNew = aProp.Normal(theParam, THE_LIN_TOL);
  GeomLProp_CLProps            anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Dir anOldNorm;
    anOld.Normal(anOldNorm);
    EXPECT_NEAR(std::abs(aNew.Direction.Dot(anOldNorm)), 1.0, THE_DIR_TOL);
  }
}

void compareCentre(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  GeomProp_Curve               aProp(theCurve);
  const GeomProp::CentreResult aNew = aProp.CentreOfCurvature(theParam, THE_LIN_TOL);
  GeomLProp_CLProps            anOld(theCurve, theParam, 2, THE_LIN_TOL);
  if (anOld.IsTangentDefined() && std::abs(anOld.Curvature()) > THE_LIN_TOL)
  {
    ASSERT_TRUE(aNew.IsDefined);
    gp_Pnt anOldCentre;
    anOld.CentreOfCurvature(anOldCentre);
    EXPECT_NEAR(aNew.Centre.Distance(anOldCentre), 0.0, THE_POINT_TOL);
  }
}

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

//! Create a circle in the XY plane with given radius and center.
occ::handle<Geom_Circle> makeCircle(const double theRadius,
                                    const double theCX = 0.0,
                                    const double theCY = 0.0,
                                    const double theCZ = 0.0)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(theCX, theCY, theCZ), gp_Dir(0, 0, 1)), theRadius);
  return new Geom_Circle(aCirc);
}

//! Create an ellipse in the XY plane with given semi-axes.
occ::handle<Geom_Ellipse> makeEllipse(const double theMajor, const double theMinor)
{
  gp_Elips anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), theMajor, theMinor);
  return new Geom_Ellipse(anElips);
}

//! Create an offset curve from a base curve in the Z direction.
occ::handle<Geom_OffsetCurve> makeOffset(const occ::handle<Geom_Curve>& theBase,
                                          const double                   theOffset)
{
  return new Geom_OffsetCurve(theBase, theOffset, gp_Dir(0, 0, 1));
}

} // namespace

// ============================================================================
// Curvature tests - offset circle
// ============================================================================

TEST(GeomProp_OffsetCurveTest, Curvature_OffsetCircle_Constant)
{
  const double                      aRadius = 5.0;
  const double                      aDist   = 2.0;
  occ::handle<Geom_Circle>      aCircle = makeCircle(aRadius);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, aDist);

  GeomProp_Curve aProp(anOffset);

  // Offset of circle with radius R by distance d gives circle with radius R+d
  const double aExpectedCurvature = 1.0 / (aRadius + aDist);
  for (double aParam = 0.0; aParam < 2.0 * M_PI; aParam += M_PI / 4.0)
  {
    const GeomProp::CurvatureResult aCurv = aProp.Curvature(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aCurv.IsDefined) << "Curvature undefined at param=" << aParam;
    EXPECT_NEAR(aCurv.Value, aExpectedCurvature, THE_CURV_TOL)
      << "Curvature mismatch at param=" << aParam;
  }
}

TEST(GeomProp_OffsetCurveTest, Curvature_OffsetCircle_Positive)
{
  const double                      aRadius = 3.0;
  const double                      aDist   = 4.0;
  occ::handle<Geom_Circle>      aCircle = makeCircle(aRadius);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, aDist);

  GeomProp_Curve                  aProp(anOffset);
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / (aRadius + aDist), THE_CURV_TOL);
}

TEST(GeomProp_OffsetCurveTest, Curvature_OffsetCircle_Negative)
{
  const double                      aRadius = 5.0;
  const double                      aDist   = -2.0;
  occ::handle<Geom_Circle>      aCircle = makeCircle(aRadius);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, aDist);

  GeomProp_Curve                  aProp(anOffset);
  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv.IsDefined);
  // Negative offset gives smaller circle with radius R-|d|=3
  EXPECT_NEAR(aCurv.Value, 1.0 / (aRadius + aDist), THE_CURV_TOL);
}

// ============================================================================
// Tangent tests - offset circle
// ============================================================================

TEST(GeomProp_OffsetCurveTest, Tangent_OffsetCircle)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);

  GeomProp_Curve aPropBase(aCircle);
  GeomProp_Curve aPropOffset(anOffset);

  // Tangent of offset circle should be parallel to base circle tangent
  for (double aParam = 0.0; aParam < 2.0 * M_PI; aParam += M_PI / 3.0)
  {
    const GeomProp::TangentResult aTanBase   = aPropBase.Tangent(aParam, THE_LIN_TOL);
    const GeomProp::TangentResult aTanOffset = aPropOffset.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTanBase.IsDefined);
    ASSERT_TRUE(aTanOffset.IsDefined);
    EXPECT_NEAR(std::abs(aTanBase.Direction.Dot(aTanOffset.Direction)), 1.0, THE_DIR_TOL);
  }
}

// ============================================================================
// Normal tests - offset circle
// ============================================================================

TEST(GeomProp_OffsetCurveTest, Normal_OffsetCircle)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);

  GeomProp_Curve aPropBase(aCircle);
  GeomProp_Curve aPropOffset(anOffset);

  for (double aParam = 0.0; aParam < 2.0 * M_PI; aParam += M_PI / 3.0)
  {
    const GeomProp::NormalResult aNormBase   = aPropBase.Normal(aParam, THE_LIN_TOL);
    const GeomProp::NormalResult aNormOffset = aPropOffset.Normal(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aNormBase.IsDefined);
    ASSERT_TRUE(aNormOffset.IsDefined);
    EXPECT_NEAR(std::abs(aNormBase.Direction.Dot(aNormOffset.Direction)), 1.0, THE_DIR_TOL);
  }
}

// ============================================================================
// Centre of curvature - offset circle
// ============================================================================

TEST(GeomProp_OffsetCurveTest, Centre_OffsetCircle)
{
  const gp_Pnt                      aCenter(1.0, 2.0, 0.0);
  occ::handle<Geom_Circle>      aCircle = makeCircle(5.0, 1.0, 2.0, 0.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);

  GeomProp_Curve aProp(anOffset);

  // Centre of curvature of offset circle = center of the circle
  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, THE_LIN_TOL);
  ASSERT_TRUE(aCentre.IsDefined);
  EXPECT_NEAR(aCentre.Centre.Distance(aCenter), 0.0, THE_POINT_TOL);
}

// ============================================================================
// FindCurvatureExtrema / FindInflections - offset circle
// ============================================================================

TEST(GeomProp_OffsetCurveTest, FindCurvatureExtrema_OffsetCircle)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);

  GeomProp_Curve                aProp(anOffset);
  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Constant curvature => no extrema
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomProp_OffsetCurveTest, FindInflections_OffsetCircle)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);

  GeomProp_Curve                aProp(anOffset);
  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

// ============================================================================
// GetType test
// ============================================================================

TEST(GeomProp_OffsetCurveTest, GetType_IsOffsetCurve)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);

  GeomProp_Curve aProp(anOffset);
  EXPECT_EQ(aProp.GetType(), GeomAbs_OffsetCurve);
}

// ============================================================================
// VsCLProps comparison tests - offset circle
// ============================================================================

TEST(GeomProp_OffsetCurveTest, VsCLProps_OffsetCircle)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);
  compareAll(anOffset, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_OffsetCurveTest, VsCLProps_AllProperties_OffsetCircle)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 2.0);
  compareAll(anOffset, 0.0, 2.0 * M_PI, 20);
}

// ============================================================================
// Offset ellipse tests
// ============================================================================

TEST(GeomProp_OffsetCurveTest, Curvature_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);

  GeomProp_Curve aProp(anOffset);

  // Curvature should vary along the offset ellipse
  const GeomProp::CurvatureResult aCurv0    = aProp.Curvature(0.0, THE_LIN_TOL);
  const GeomProp::CurvatureResult aCurvPi2  = aProp.Curvature(M_PI / 2.0, THE_LIN_TOL);
  ASSERT_TRUE(aCurv0.IsDefined);
  ASSERT_TRUE(aCurvPi2.IsDefined);
  EXPECT_NE(aCurv0.Value, aCurvPi2.Value);
}

TEST(GeomProp_OffsetCurveTest, FindCurvatureExtrema_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);

  GeomProp_Curve                aProp(anOffset);
  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  // Offset ellipse should have curvature extrema (inherited from ellipse shape)
  EXPECT_GE(aResult.Points.Length(), 2);
}

TEST(GeomProp_OffsetCurveTest, FindInflections_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);

  GeomProp_Curve                aProp(anOffset);
  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  // Offset of convex ellipse with small offset should have no inflections
  for (int i = 0; i < aResult.Points.Length(); ++i)
  {
    EXPECT_EQ(aResult.Points[i].Type, GeomProp::CIType::Inflection);
  }
}

TEST(GeomProp_OffsetCurveTest, Tangent_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);

  GeomProp_Curve aProp(anOffset);

  for (double aParam = 0.0; aParam < 2.0 * M_PI; aParam += M_PI / 4.0)
  {
    const GeomProp::TangentResult aTan = aProp.Tangent(aParam, THE_LIN_TOL);
    ASSERT_TRUE(aTan.IsDefined) << "Tangent undefined at param=" << aParam;
    (void)aTan.Direction;
  }
}

TEST(GeomProp_OffsetCurveTest, Normal_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);

  GeomProp_Curve aProp(anOffset);

  for (double aParam = 0.0; aParam < 2.0 * M_PI; aParam += M_PI / 4.0)
  {
    const GeomProp::NormalResult aNorm = aProp.Normal(aParam, THE_LIN_TOL);
    if (aNorm.IsDefined)
    {
      (void)aNorm.Direction;
    }
  }
}

// ============================================================================
// VsCLProps comparison tests - offset ellipse
// ============================================================================

TEST(GeomProp_OffsetCurveTest, VsCLProps_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);
  compareAll(anOffset, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_OffsetCurveTest, VsCLProps_AllProperties_OffsetEllipse)
{
  occ::handle<Geom_Ellipse>     anEllipse = makeEllipse(10.0, 5.0);
  occ::handle<Geom_OffsetCurve> anOffset  = makeOffset(anEllipse, 1.0);
  compareAll(anOffset, 0.0, 2.0 * M_PI, 20);
}

TEST(GeomProp_OffsetCurveTest, VsCLProps_LargeOffset)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 50.0);
  compareAll(anOffset, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_OffsetCurveTest, VsCLProps_SmallOffset)
{
  occ::handle<Geom_Circle>      aCircle  = makeCircle(5.0);
  occ::handle<Geom_OffsetCurve> anOffset = makeOffset(aCircle, 0.001);
  compareAll(anOffset, 0.0, 2.0 * M_PI);
}

TEST(GeomProp_OffsetCurveTest, VsCLProps_CriticalPoints)
{
  // Offset of a circle
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_OffsetCurve> anOffset = new Geom_OffsetCurve(aCircle, 2.0, gp_Dir(0, 0, 1));
  const double aParams[] = {0.0, 1.0e-10, M_PI / 4.0, M_PI / 2.0, M_PI / 2.0 + 1.0e-6,
                            M_PI, 3.0 * M_PI / 2.0, 2.0 * M_PI - 1.0e-10,
                            M_PI / 6.0, 5.0 * M_PI / 6.0};
  for (const double aParam : aParams)
  {
    compareTangent(anOffset, aParam);
    compareCurvature(anOffset, aParam);
    compareNormal(anOffset, aParam);
    compareCentre(anOffset, aParam);
  }
}
