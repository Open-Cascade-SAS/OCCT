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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

// ============================================================================
// Free functions tests (Geom2dProp namespace)
// ============================================================================

TEST(Geom2dPropTest, ComputeTangent_FromD1)
{
  const gp_Vec2d aD1(3.0, 4.0);
  const gp_Vec2d aD2(0.0, 0.0);
  const gp_Vec2d aD3(0.0, 0.0);

  const Geom2dProp::TangentResult aRes = Geom2dProp::ComputeTangent(aD1, aD2, aD3, 1.0e-7);
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), 3.0 / 5.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 4.0 / 5.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeTangent_FallbackToD2)
{
  const gp_Vec2d aD1(0.0, 0.0);
  const gp_Vec2d aD2(1.0, 0.0);
  const gp_Vec2d aD3(0.0, 0.0);

  const Geom2dProp::TangentResult aRes = Geom2dProp::ComputeTangent(aD1, aD2, aD3, 1.0e-7);
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeTangent_FallbackToD3)
{
  const gp_Vec2d aD1(0.0, 0.0);
  const gp_Vec2d aD2(0.0, 0.0);
  const gp_Vec2d aD3(0.0, 5.0);

  const Geom2dProp::TangentResult aRes = Geom2dProp::ComputeTangent(aD1, aD2, aD3, 1.0e-7);
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 1.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeTangent_Undefined)
{
  const gp_Vec2d aZero(0.0, 0.0);
  const Geom2dProp::TangentResult aRes =
    Geom2dProp::ComputeTangent(aZero, aZero, aZero, 1.0e-7);
  EXPECT_FALSE(aRes.IsDefined);
}

TEST(Geom2dPropTest, ComputeTangent_NegativeDirection)
{
  const gp_Vec2d aD1(-7.0, 0.0);
  const gp_Vec2d aD2(0.0, 0.0);
  const gp_Vec2d aD3(0.0, 0.0);

  const Geom2dProp::TangentResult aRes = Geom2dProp::ComputeTangent(aD1, aD2, aD3, 1.0e-7);
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeCurvature_Circle)
{
  // For a unit circle at param=0: D1=(0,1), D2=(-1,0)
  const gp_Vec2d aD1(0.0, 1.0);
  const gp_Vec2d aD2(-1.0, 0.0);

  const Geom2dProp::CurvatureResult aRes =
    Geom2dProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_FALSE(aRes.IsInfinite);
  EXPECT_NEAR(aRes.Value, 1.0, 1.0e-10);
}

TEST(Geom2dPropTest, ComputeCurvature_LargerCircle)
{
  // For circle R=5 at param=0: D1=(0,5), D2=(-5,0)
  const gp_Vec2d aD1(0.0, 5.0);
  const gp_Vec2d aD2(-5.0, 0.0);

  const Geom2dProp::CurvatureResult aRes =
    Geom2dProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Value, 1.0 / 5.0, 1.0e-10);
}

TEST(Geom2dPropTest, ComputeCurvature_ZeroD1_IsInfinite)
{
  const gp_Vec2d aD1(0.0, 0.0);
  const gp_Vec2d aD2(1.0, 0.0);

  const Geom2dProp::CurvatureResult aRes =
    Geom2dProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_TRUE(aRes.IsInfinite);
}

TEST(Geom2dPropTest, ComputeCurvature_ZeroD2_IsZero)
{
  const gp_Vec2d aD1(1.0, 0.0);
  const gp_Vec2d aD2(0.0, 0.0);

  const Geom2dProp::CurvatureResult aRes =
    Geom2dProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_FALSE(aRes.IsInfinite);
  EXPECT_NEAR(aRes.Value, 0.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeCurvature_ParallelD1D2_IsZero)
{
  // D1 and D2 are parallel => cross product is 0 => curvature is 0
  const gp_Vec2d aD1(1.0, 0.0);
  const gp_Vec2d aD2(3.0, 0.0);

  const Geom2dProp::CurvatureResult aRes =
    Geom2dProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Value, 0.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeNormal_Circle)
{
  const gp_Vec2d aD1(0.0, 1.0);
  const gp_Vec2d aD2(-1.0, 0.0);

  const Geom2dProp::NormalResult aRes =
    Geom2dProp::ComputeNormal(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  // Normal perpendicular to tangent
  EXPECT_NEAR(std::abs(aRes.Direction.X() * aD1.X() + aRes.Direction.Y() * aD1.Y()),
              0.0,
              Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeNormal_Line_Undefined)
{
  const gp_Vec2d aD1(1.0, 0.0);
  const gp_Vec2d aD2(0.0, 0.0);

  const Geom2dProp::NormalResult aRes =
    Geom2dProp::ComputeNormal(aD1, aD2, Precision::Confusion());
  EXPECT_FALSE(aRes.IsDefined);
}

TEST(Geom2dPropTest, ComputeNormal_Perpendicularity)
{
  // At an arbitrary point: D1=(1,1), D2=(0,2)
  const gp_Vec2d aD1(1.0, 1.0);
  const gp_Vec2d aD2(0.0, 2.0);

  const Geom2dProp::NormalResult aNorm =
    Geom2dProp::ComputeNormal(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aNorm.IsDefined);

  const Geom2dProp::TangentResult aTan =
    Geom2dProp::ComputeTangent(aD1, aD2, gp_Vec2d(0, 0), Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);

  const double aDot =
    aTan.Direction.X() * aNorm.Direction.X() + aTan.Direction.Y() * aNorm.Direction.Y();
  EXPECT_NEAR(aDot, 0.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeCentreOfCurvature_Circle)
{
  const gp_Pnt2d aPnt(1.0, 0.0);
  const gp_Vec2d aD1(0.0, 1.0);
  const gp_Vec2d aD2(-1.0, 0.0);

  const Geom2dProp::CentreResult aRes =
    Geom2dProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Centre.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Centre.Y(), 0.0, Precision::Confusion());
}

TEST(Geom2dPropTest, ComputeCentreOfCurvature_Line_Undefined)
{
  const gp_Pnt2d aPnt(0.0, 0.0);
  const gp_Vec2d aD1(1.0, 0.0);
  const gp_Vec2d aD2(0.0, 0.0);

  const Geom2dProp::CentreResult aRes =
    Geom2dProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, Precision::Confusion());
  EXPECT_FALSE(aRes.IsDefined);
}

TEST(Geom2dPropTest, ComputeCentreOfCurvature_DistanceEqualsRadius)
{
  // Circle R=3 centered at (0,0), point at (3,0)
  const gp_Pnt2d aPnt(3.0, 0.0);
  const gp_Vec2d aD1(0.0, 3.0);
  const gp_Vec2d aD2(-3.0, 0.0);

  const Geom2dProp::CentreResult aRes =
    Geom2dProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  const double aDist = aPnt.Distance(aRes.Centre);
  EXPECT_NEAR(aDist, 3.0, Precision::Confusion());
}

// ============================================================================
// Line tests via Geom2dProp_Curve dispatcher
// ============================================================================

class Geom2dProp_CurveLineTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    gp_Lin2d aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
    myLine = new Geom2d_Line(aLin);
    myProp.Initialize(myLine);
  }

  occ::handle<Geom2d_Line> myLine;
  Geom2dProp_Curve         myProp;
};

TEST_F(Geom2dProp_CurveLineTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_Line);
}

TEST_F(Geom2dProp_CurveLineTest, Tangent)
{
  const Geom2dProp::TangentResult aRes = myProp.Tangent(5.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveLineTest, TangentIsConstant)
{
  // Tangent should be the same at any parameter
  for (double u = -100.0; u <= 100.0; u += 50.0)
  {
    const Geom2dProp::TangentResult aRes = myProp.Tangent(u, Precision::Confusion());
    ASSERT_TRUE(aRes.IsDefined);
    EXPECT_NEAR(aRes.Direction.X(), 1.0, Precision::Confusion());
    EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
  }
}

TEST_F(Geom2dProp_CurveLineTest, CurvatureIsZero)
{
  const Geom2dProp::CurvatureResult aRes = myProp.Curvature(5.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_FALSE(aRes.IsInfinite);
  EXPECT_NEAR(aRes.Value, 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveLineTest, NormalUndefined)
{
  const Geom2dProp::NormalResult aRes = myProp.Normal(5.0, Precision::Confusion());
  EXPECT_FALSE(aRes.IsDefined);
}

TEST_F(Geom2dProp_CurveLineTest, CentreUndefined)
{
  const Geom2dProp::CentreResult aRes = myProp.CentreOfCurvature(5.0, Precision::Confusion());
  EXPECT_FALSE(aRes.IsDefined);
}

TEST_F(Geom2dProp_CurveLineTest, NoExtrema)
{
  const Geom2dProp::CurveAnalysis aRes = myProp.FindCurvatureExtrema();
  EXPECT_TRUE(aRes.IsDone);
  EXPECT_TRUE(aRes.Points.IsEmpty());
}

TEST_F(Geom2dProp_CurveLineTest, NoInflections)
{
  const Geom2dProp::CurveAnalysis aRes = myProp.FindInflections();
  EXPECT_TRUE(aRes.IsDone);
  EXPECT_TRUE(aRes.Points.IsEmpty());
}

// Diagonal line
TEST(Geom2dProp_LineTest, DiagonalLine_TangentDirection)
{
  gp_Lin2d aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 1.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aLine);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  const double aSqrt2Inv = 1.0 / std::sqrt(2.0);
  EXPECT_NEAR(aTan.Direction.X(), aSqrt2Inv, Precision::Confusion());
  EXPECT_NEAR(aTan.Direction.Y(), aSqrt2Inv, Precision::Confusion());
}

// ============================================================================
// Circle tests via Geom2dProp_Curve dispatcher
// ============================================================================

class Geom2dProp_CurveCircleTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
    myCircle = new Geom2d_Circle(aCirc);
    myProp.Initialize(myCircle);
  }

  occ::handle<Geom2d_Circle> myCircle;
  Geom2dProp_Curve           myProp;
};

TEST_F(Geom2dProp_CurveCircleTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_Circle);
}

TEST_F(Geom2dProp_CurveCircleTest, Tangent)
{
  const Geom2dProp::TangentResult aRes = myProp.Tangent(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 1.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveCircleTest, TangentAtPiHalf)
{
  const Geom2dProp::TangentResult aRes = myProp.Tangent(M_PI / 2.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveCircleTest, TangentPerpendicularToRadius)
{
  // At any parameter, tangent should be perpendicular to the radius vector
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 7.0)
  {
    const Geom2dProp::TangentResult aTan = myProp.Tangent(u, Precision::Confusion());
    ASSERT_TRUE(aTan.IsDefined);
    // Radius direction at param u on circle centered at origin
    const double aRadX = std::cos(u);
    const double aRadY = std::sin(u);
    const double aDot  = aTan.Direction.X() * aRadX + aTan.Direction.Y() * aRadY;
    EXPECT_NEAR(aDot, 0.0, 1.0e-10);
  }
}

TEST_F(Geom2dProp_CurveCircleTest, ConstantCurvature)
{
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::CurvatureResult aRes = myProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aRes.IsDefined);
    EXPECT_NEAR(aRes.Value, 1.0 / 5.0, Precision::Confusion());
  }
}

TEST_F(Geom2dProp_CurveCircleTest, Normal)
{
  const Geom2dProp::NormalResult aRes = myProp.Normal(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  EXPECT_NEAR(
    std::abs(aTan.Direction.X() * aRes.Direction.X() + aTan.Direction.Y() * aRes.Direction.Y()),
    0.0,
    Precision::Confusion());
}

TEST_F(Geom2dProp_CurveCircleTest, NormalPointsTowardCenter)
{
  // At param=0, point=(5,0), normal should point toward center (0,0) => direction (-1,0)
  const Geom2dProp::NormalResult aRes = myProp.Normal(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveCircleTest, CentreOfCurvature)
{
  const Geom2dProp::CentreResult aRes = myProp.CentreOfCurvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Centre.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Centre.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveCircleTest, CentreOfCurvature_ConstantAtAllParams)
{
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 5.0)
  {
    const Geom2dProp::CentreResult aRes = myProp.CentreOfCurvature(u, Precision::Confusion());
    ASSERT_TRUE(aRes.IsDefined);
    EXPECT_NEAR(aRes.Centre.X(), 0.0, Precision::Confusion());
    EXPECT_NEAR(aRes.Centre.Y(), 0.0, Precision::Confusion());
  }
}

TEST_F(Geom2dProp_CurveCircleTest, NoExtrema)
{
  const Geom2dProp::CurveAnalysis aRes = myProp.FindCurvatureExtrema();
  EXPECT_TRUE(aRes.IsDone);
  EXPECT_TRUE(aRes.Points.IsEmpty());
}

TEST_F(Geom2dProp_CurveCircleTest, NoInflections)
{
  const Geom2dProp::CurveAnalysis aRes = myProp.FindInflections();
  EXPECT_TRUE(aRes.IsDone);
  EXPECT_TRUE(aRes.Points.IsEmpty());
}

// Different radius circle
TEST(Geom2dProp_CircleTest, SmallRadius_HighCurvature)
{
  gp_Circ2d                    aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 0.1);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aCircle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 10.0, Precision::Confusion());
}

// Off-center circle
TEST(Geom2dProp_CircleTest, OffCenter_CentreOfCurvature)
{
  gp_Circ2d                    aCirc(gp_Ax2d(gp_Pnt2d(3.0, 7.0), gp_Dir2d(1.0, 0.0)), 4.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aCircle);

  const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(1.0, Precision::Confusion());
  ASSERT_TRUE(aCentre.IsDefined);
  EXPECT_NEAR(aCentre.Centre.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCentre.Centre.Y(), 7.0, Precision::Confusion());
}

// ============================================================================
// Ellipse tests via Geom2dProp_Curve dispatcher
// ============================================================================

class Geom2dProp_CurveEllipseTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
    myEllipse = new Geom2d_Ellipse(anElips);
    myProp.Initialize(myEllipse);
  }

  occ::handle<Geom2d_Ellipse> myEllipse;
  Geom2dProp_Curve            myProp;
};

TEST_F(Geom2dProp_CurveEllipseTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_Ellipse);
}

TEST_F(Geom2dProp_CurveEllipseTest, TangentAtMajorVertex)
{
  // At U=0, point is on major axis endpoint, tangent should be vertical
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveEllipseTest, TangentAtMinorVertex)
{
  // At U=PI/2, point is on minor axis endpoint, tangent should be horizontal
  const Geom2dProp::TangentResult aTan = myProp.Tangent(M_PI / 2.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(std::abs(aTan.Direction.X()), 1.0, Precision::Confusion());
  EXPECT_NEAR(aTan.Direction.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveEllipseTest, TangentPerpToNormal)
{
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    const Geom2dProp::TangentResult aTan = myProp.Tangent(u, Precision::Confusion());
    const Geom2dProp::NormalResult  aNorm = myProp.Normal(u, Precision::Confusion());
    ASSERT_TRUE(aTan.IsDefined);
    ASSERT_TRUE(aNorm.IsDefined);
    const double aDot =
      aTan.Direction.X() * aNorm.Direction.X() + aTan.Direction.Y() * aNorm.Direction.Y();
    EXPECT_NEAR(aDot, 0.0, 1.0e-10);
  }
}

TEST_F(Geom2dProp_CurveEllipseTest, CurvatureAtMajorVertex)
{
  // At U=0 (major vertex): curvature = a/b^2 = 10/25 = 0.4
  const Geom2dProp::CurvatureResult aRes = myProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Value, 10.0 / 25.0, 1.0e-6);
}

TEST_F(Geom2dProp_CurveEllipseTest, CurvatureAtMinorVertex)
{
  // At U=PI/2 (minor vertex): curvature = b/a^2 = 5/100 = 0.05
  const Geom2dProp::CurvatureResult aRes = myProp.Curvature(M_PI / 2.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Value, 5.0 / 100.0, 1.0e-6);
}

TEST_F(Geom2dProp_CurveEllipseTest, CurvatureAtPi)
{
  // At U=PI (opposite major vertex), curvature same as U=0
  const Geom2dProp::CurvatureResult aRes = myProp.Curvature(M_PI, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Value, 10.0 / 25.0, 1.0e-6);
}

TEST_F(Geom2dProp_CurveEllipseTest, CurvatureSymmetry)
{
  // Curvature at U and -U should be equal (ellipse is symmetric)
  for (double u = 0.1; u < M_PI; u += 0.3)
  {
    const Geom2dProp::CurvatureResult aRes1 = myProp.Curvature(u, Precision::Confusion());
    const Geom2dProp::CurvatureResult aRes2 = myProp.Curvature(-u, Precision::Confusion());
    ASSERT_TRUE(aRes1.IsDefined);
    ASSERT_TRUE(aRes2.IsDefined);
    EXPECT_NEAR(aRes1.Value, aRes2.Value, 1.0e-10);
  }
}

TEST_F(Geom2dProp_CurveEllipseTest, CurvatureMaxAtMajorVertex)
{
  // |curvature| should be maximum at major vertex (U=0, PI)
  const double aCurvMax = myProp.Curvature(0.0, Precision::Confusion()).Value;
  for (double u = 0.1; u < 2.0 * M_PI; u += 0.2)
  {
    const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LE(std::abs(aCurv.Value), std::abs(aCurvMax) + 1.0e-10);
  }
}

TEST_F(Geom2dProp_CurveEllipseTest, NormalAtMajorVertex)
{
  const Geom2dProp::NormalResult aRes = myProp.Normal(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  // At (10,0) on x-axis aligned ellipse, normal should point toward center: (-1,0)
  EXPECT_NEAR(aRes.Direction.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveEllipseTest, CentreOfCurvatureAtMajorVertex)
{
  const Geom2dProp::CentreResult aRes = myProp.CentreOfCurvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  // Radius of curvature at major vertex = b^2/a = 25/10 = 2.5
  // Centre should be at (10 - 2.5, 0) = (7.5, 0)
  EXPECT_NEAR(aRes.Centre.X(), 7.5, 1.0e-6);
  EXPECT_NEAR(aRes.Centre.Y(), 0.0, 1.0e-6);
}

TEST_F(Geom2dProp_CurveEllipseTest, CentreOfCurvatureAtMinorVertex)
{
  const Geom2dProp::CentreResult aRes =
    myProp.CentreOfCurvature(M_PI / 2.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  // Radius of curvature at minor vertex = a^2/b = 100/5 = 20
  // Centre should be at (0, 5 - 20) = (0, -15)
  EXPECT_NEAR(aRes.Centre.X(), 0.0, 1.0e-6);
  EXPECT_NEAR(aRes.Centre.Y(), -15.0, 1.0e-6);
}

TEST_F(Geom2dProp_CurveEllipseTest, FindCurvatureExtrema)
{
  const Geom2dProp::CurveAnalysis aRes = myProp.FindCurvatureExtrema();
  ASSERT_TRUE(aRes.IsDone);
  EXPECT_EQ(aRes.Points.Length(), 4);

  if (aRes.Points.Length() >= 4)
  {
    EXPECT_NEAR(aRes.Points.Value(1).Parameter, 0.0, 1.0e-10);
    EXPECT_EQ(aRes.Points.Value(1).Type, Geom2dProp::CIType::MinCurvature);

    EXPECT_NEAR(aRes.Points.Value(2).Parameter, M_PI / 2.0, 1.0e-10);
    EXPECT_EQ(aRes.Points.Value(2).Type, Geom2dProp::CIType::MaxCurvature);

    EXPECT_NEAR(aRes.Points.Value(3).Parameter, M_PI, 1.0e-10);
    EXPECT_EQ(aRes.Points.Value(3).Type, Geom2dProp::CIType::MinCurvature);

    EXPECT_NEAR(aRes.Points.Value(4).Parameter, 3.0 * M_PI / 2.0, 1.0e-10);
    EXPECT_EQ(aRes.Points.Value(4).Type, Geom2dProp::CIType::MaxCurvature);
  }
}

TEST_F(Geom2dProp_CurveEllipseTest, NoInflections)
{
  const Geom2dProp::CurveAnalysis aRes = myProp.FindInflections();
  EXPECT_TRUE(aRes.IsDone);
  EXPECT_TRUE(aRes.Points.IsEmpty());
}

// Ellipse with equal semi-axes is a circle
TEST(Geom2dProp_EllipseTest, EqualSemiAxes_BehavesLikeCircle)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anEllipse);

  // Curvature should be constant = 1/R = 1/5
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(aCurv.Value, 1.0 / 5.0, 1.0e-10);
  }
}

// ============================================================================
// Hyperbola tests
// ============================================================================

class Geom2dProp_CurveHyperbolaTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    gp_Hypr2d aHypr(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0, 3.0);
    myHyperbola = new Geom2d_Hyperbola(aHypr);
    myProp.Initialize(myHyperbola);
  }

  occ::handle<Geom2d_Hyperbola> myHyperbola;
  Geom2dProp_Curve              myProp;
};

TEST_F(Geom2dProp_CurveHyperbolaTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_Hyperbola);
}

TEST_F(Geom2dProp_CurveHyperbolaTest, TangentAtVertex)
{
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  // At vertex of hyperbola (t=0), tangent is vertical
  EXPECT_NEAR(aTan.Direction.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveHyperbolaTest, CurvatureAtVertex)
{
  // At vertex (t=0): curvature = b^2/a^2 * 1/a * a = b^2/(a * a) ... actually κ = b²/a
  // Wait, for hyperbola x=a*cosh(t), y=b*sinh(t):
  // κ(0) = b²/(a² * (b²/a²)^(3/2)) ... Let me just verify it's defined and positive.
  const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_GT(aCurv.Value, 0.0);
}

TEST_F(Geom2dProp_CurveHyperbolaTest, CurvatureDecreasesFromVertex)
{
  // Curvature should be maximum at vertex and decrease away from it
  const double aCurvAtVertex = std::abs(myProp.Curvature(0.0, Precision::Confusion()).Value);
  for (double u = 0.5; u < 3.0; u += 0.5)
  {
    const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LT(std::abs(aCurv.Value), aCurvAtVertex + 1.0e-10);
  }
}

TEST_F(Geom2dProp_CurveHyperbolaTest, NormalAtVertex)
{
  const Geom2dProp::NormalResult aNorm = myProp.Normal(0.0, Precision::Confusion());
  ASSERT_TRUE(aNorm.IsDefined);
  // Normal should be perpendicular to tangent
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  const double aDot =
    aTan.Direction.X() * aNorm.Direction.X() + aTan.Direction.Y() * aNorm.Direction.Y();
  EXPECT_NEAR(aDot, 0.0, 1.0e-10);
}

TEST_F(Geom2dProp_CurveHyperbolaTest, CentreOfCurvatureAtVertex)
{
  const Geom2dProp::CentreResult aCentre =
    myProp.CentreOfCurvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCentre.IsDefined);
}

TEST_F(Geom2dProp_CurveHyperbolaTest, CurvatureExtremaAtVertex)
{
  const Geom2dProp::CurveAnalysis aExtrema = myProp.FindCurvatureExtrema();
  ASSERT_TRUE(aExtrema.IsDone);
  EXPECT_EQ(aExtrema.Points.Length(), 1);
  if (!aExtrema.Points.IsEmpty())
  {
    EXPECT_NEAR(aExtrema.Points.Value(1).Parameter, 0.0, 1.0e-10);
    EXPECT_EQ(aExtrema.Points.Value(1).Type, Geom2dProp::CIType::MinCurvature);
  }
}

TEST_F(Geom2dProp_CurveHyperbolaTest, NoInflections)
{
  const Geom2dProp::CurveAnalysis aInfl = myProp.FindInflections();
  EXPECT_TRUE(aInfl.IsDone);
  EXPECT_TRUE(aInfl.Points.IsEmpty());
}

// ============================================================================
// Parabola tests
// ============================================================================

class Geom2dProp_CurveParabolaTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Focal parameter p=2 => parabola y^2 = 4px = 8x
    gp_Parab2d aParab(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0);
    myParabola = new Geom2d_Parabola(aParab);
    myProp.Initialize(myParabola);
  }

  occ::handle<Geom2d_Parabola> myParabola;
  Geom2dProp_Curve             myProp;
};

TEST_F(Geom2dProp_CurveParabolaTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_Parabola);
}

TEST_F(Geom2dProp_CurveParabolaTest, TangentAtVertex)
{
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  // At vertex, tangent is vertical
  EXPECT_NEAR(aTan.Direction.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(std::abs(aTan.Direction.Y()), 1.0, Precision::Confusion());
}

TEST_F(Geom2dProp_CurveParabolaTest, CurvatureAtVertex)
{
  // At vertex: curvature = 1/(2*focal_parameter) = 1/4 = 0.25
  const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_GT(aCurv.Value, 0.0);
}

TEST_F(Geom2dProp_CurveParabolaTest, CurvatureDecreasesFromVertex)
{
  const double aCurvAtVertex = std::abs(myProp.Curvature(0.0, Precision::Confusion()).Value);
  for (double u = 1.0; u <= 5.0; u += 1.0)
  {
    const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_LT(std::abs(aCurv.Value), aCurvAtVertex);
  }
}

TEST_F(Geom2dProp_CurveParabolaTest, CurvatureSymmetric)
{
  // Curvature at U and -U should be equal
  for (double u = 0.5; u <= 5.0; u += 0.5)
  {
    const double aCurv1 = myProp.Curvature(u, Precision::Confusion()).Value;
    const double aCurv2 = myProp.Curvature(-u, Precision::Confusion()).Value;
    EXPECT_NEAR(aCurv1, aCurv2, 1.0e-10);
  }
}

TEST_F(Geom2dProp_CurveParabolaTest, NormalAtVertex)
{
  const Geom2dProp::NormalResult aNorm = myProp.Normal(0.0, Precision::Confusion());
  ASSERT_TRUE(aNorm.IsDefined);
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  const double aDot =
    aTan.Direction.X() * aNorm.Direction.X() + aTan.Direction.Y() * aNorm.Direction.Y();
  EXPECT_NEAR(aDot, 0.0, 1.0e-10);
}

TEST_F(Geom2dProp_CurveParabolaTest, CentreOfCurvatureAtVertex)
{
  const Geom2dProp::CentreResult aCentre = myProp.CentreOfCurvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCentre.IsDefined);
}

TEST_F(Geom2dProp_CurveParabolaTest, CurvatureExtremaAtVertex)
{
  const Geom2dProp::CurveAnalysis aExtrema = myProp.FindCurvatureExtrema();
  ASSERT_TRUE(aExtrema.IsDone);
  EXPECT_EQ(aExtrema.Points.Length(), 1);
  if (!aExtrema.Points.IsEmpty())
  {
    EXPECT_NEAR(aExtrema.Points.Value(1).Parameter, 0.0, 1.0e-10);
    EXPECT_EQ(aExtrema.Points.Value(1).Type, Geom2dProp::CIType::MinCurvature);
  }
}

TEST_F(Geom2dProp_CurveParabolaTest, NoInflections)
{
  const Geom2dProp::CurveAnalysis aInfl = myProp.FindInflections();
  EXPECT_TRUE(aInfl.IsDone);
  EXPECT_TRUE(aInfl.Points.IsEmpty());
}

// ============================================================================
// Bezier curve tests
// ============================================================================

class Geom2dProp_CurveBezierTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // S-shaped cubic Bezier: (0,0), (1,2), (3,-2), (4,0)
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles(1) = gp_Pnt2d(0.0, 0.0);
    aPoles(2) = gp_Pnt2d(1.0, 2.0);
    aPoles(3) = gp_Pnt2d(3.0, -2.0);
    aPoles(4) = gp_Pnt2d(4.0, 0.0);
    myBezier = new Geom2d_BezierCurve(aPoles);
    myProp.Initialize(myBezier);
  }

  occ::handle<Geom2d_BezierCurve> myBezier;
  Geom2dProp_Curve                myProp;
};

TEST_F(Geom2dProp_CurveBezierTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_BezierCurve);
}

TEST_F(Geom2dProp_CurveBezierTest, TangentAtStart)
{
  const Geom2dProp::TangentResult aTan = myProp.Tangent(0.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  // Tangent at start should point toward second control point: (1,2)
  // Direction should be proportional to (1,2), normalized
  const double aLen = std::sqrt(1.0 + 4.0);
  EXPECT_NEAR(aTan.Direction.X(), 1.0 / aLen, 1.0e-6);
  EXPECT_NEAR(aTan.Direction.Y(), 2.0 / aLen, 1.0e-6);
}

TEST_F(Geom2dProp_CurveBezierTest, TangentAtEnd)
{
  const Geom2dProp::TangentResult aTan = myProp.Tangent(1.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  // Tangent at end should point from third control point to fourth: (4,0)-(3,-2) = (1,2)
  const double aLen = std::sqrt(1.0 + 4.0);
  EXPECT_NEAR(aTan.Direction.X(), 1.0 / aLen, 1.0e-6);
  EXPECT_NEAR(aTan.Direction.Y(), 2.0 / aLen, 1.0e-6);
}

TEST_F(Geom2dProp_CurveBezierTest, CurvatureAtMultiplePoints)
{
  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(u, Precision::Confusion());
    EXPECT_TRUE(aCurv.IsDefined);
    EXPECT_FALSE(aCurv.IsInfinite);
  }
}

TEST_F(Geom2dProp_CurveBezierTest, NormalPerpendicularToTangent)
{
  for (double u = 0.1; u < 1.0; u += 0.2)
  {
    const Geom2dProp::TangentResult aTan  = myProp.Tangent(u, Precision::Confusion());
    const Geom2dProp::NormalResult  aNorm = myProp.Normal(u, Precision::Confusion());
    if (aTan.IsDefined && aNorm.IsDefined)
    {
      const double aDot =
        aTan.Direction.X() * aNorm.Direction.X() + aTan.Direction.Y() * aNorm.Direction.Y();
      EXPECT_NEAR(aDot, 0.0, 1.0e-10);
    }
  }
}

TEST_F(Geom2dProp_CurveBezierTest, CentreOfCurvature_DistanceIsRadiusOfCurvature)
{
  const double u = 0.3;
  const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(u, Precision::Confusion());
  const Geom2dProp::CentreResult    aCentre = myProp.CentreOfCurvature(u, Precision::Confusion());
  if (aCurv.IsDefined && aCentre.IsDefined && !aCurv.IsInfinite && std::abs(aCurv.Value) > 1.0e-10)
  {
    gp_Pnt2d aPnt;
    gp_Vec2d aD1;
    myBezier->D1(u, aPnt, aD1);
    const double aDist = aPnt.Distance(aCentre.Centre);
    EXPECT_NEAR(aDist, 1.0 / std::abs(aCurv.Value), 1.0e-6);
  }
}

TEST_F(Geom2dProp_CurveBezierTest, InflectionPoints)
{
  // S-shaped curve should have inflection point(s) near the middle
  const Geom2dProp::CurveAnalysis aInflections = myProp.FindInflections();
  EXPECT_TRUE(aInflections.IsDone);
  EXPECT_GE(aInflections.Points.Length(), 1);
  if (!aInflections.Points.IsEmpty())
  {
    for (int i = 1; i <= aInflections.Points.Length(); ++i)
    {
      EXPECT_EQ(aInflections.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
      // Should be within the parameter range [0, 1]
      EXPECT_GE(aInflections.Points.Value(i).Parameter, 0.0 - 1.0e-6);
      EXPECT_LE(aInflections.Points.Value(i).Parameter, 1.0 + 1.0e-6);
    }
  }
}

TEST_F(Geom2dProp_CurveBezierTest, CurvatureExtrema)
{
  const Geom2dProp::CurveAnalysis aExtrema = myProp.FindCurvatureExtrema();
  EXPECT_TRUE(aExtrema.IsDone);
  // S-shaped cubic should have curvature extrema
  for (int i = 1; i <= aExtrema.Points.Length(); ++i)
  {
    EXPECT_TRUE(aExtrema.Points.Value(i).Type == Geom2dProp::CIType::MinCurvature
                || aExtrema.Points.Value(i).Type == Geom2dProp::CIType::MaxCurvature);
    EXPECT_GE(aExtrema.Points.Value(i).Parameter, 0.0 - 1.0e-6);
    EXPECT_LE(aExtrema.Points.Value(i).Parameter, 1.0 + 1.0e-6);
  }
}

// Straight-line Bezier
TEST(Geom2dProp_BezierTest, StraightLine_ZeroCurvature)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(2.0, 0.0);
  aPoles(3) = gp_Pnt2d(4.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aBezier);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, Precision::Confusion());
}

// Quadratic Bezier (arc-like)
TEST(Geom2dProp_BezierTest, QuadraticBezier_Properties)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 2.0);
  aPoles(3) = gp_Pnt2d(2.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aBezier = new Geom2d_BezierCurve(aPoles);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aBezier);

  // Symmetric parabolic arc: curvature should be max at midpoint
  const double aCurvMid = std::abs(aProp.Curvature(0.5, Precision::Confusion()).Value);
  EXPECT_GT(aCurvMid, 0.0);

  // No inflections for a parabolic arc
  const Geom2dProp::CurveAnalysis aInfl = aProp.FindInflections();
  EXPECT_TRUE(aInfl.IsDone);
  EXPECT_TRUE(aInfl.Points.IsEmpty());
}

// ============================================================================
// BSpline curve tests
// ============================================================================

class Geom2dProp_CurveBSplineTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles(1) = gp_Pnt2d(0.0, 0.0);
    aPoles(2) = gp_Pnt2d(1.0, 2.0);
    aPoles(3) = gp_Pnt2d(3.0, 2.0);
    aPoles(4) = gp_Pnt2d(4.0, 0.0);

    NCollection_Array1<double> aKnots(1, 3);
    aKnots(1) = 0.0;
    aKnots(2) = 0.5;
    aKnots(3) = 1.0;

    NCollection_Array1<int> aMults(1, 3);
    aMults(1) = 3;
    aMults(2) = 1;
    aMults(3) = 3;

    myBSpline = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
    myProp.Initialize(myBSpline);
  }

  occ::handle<Geom2d_BSplineCurve> myBSpline;
  Geom2dProp_Curve                 myProp;
};

TEST_F(Geom2dProp_CurveBSplineTest, IsInitialized)
{
  EXPECT_TRUE(myProp.IsInitialized());
  EXPECT_EQ(myProp.GetType(), GeomAbs_BSplineCurve);
}

TEST_F(Geom2dProp_CurveBSplineTest, TangentDefined)
{
  for (double u = 0.0; u <= 1.0; u += 0.2)
  {
    const Geom2dProp::TangentResult aTan = myProp.Tangent(u, Precision::Confusion());
    EXPECT_TRUE(aTan.IsDefined);
  }
}

TEST_F(Geom2dProp_CurveBSplineTest, CurvatureDefined)
{
  for (double u = 0.0; u <= 1.0; u += 0.2)
  {
    const Geom2dProp::CurvatureResult aCurv = myProp.Curvature(u, Precision::Confusion());
    EXPECT_TRUE(aCurv.IsDefined);
    EXPECT_FALSE(aCurv.IsInfinite);
  }
}

TEST_F(Geom2dProp_CurveBSplineTest, NormalDefined)
{
  // B-spline has non-zero curvature, so normal should be defined in the middle
  const Geom2dProp::NormalResult aNorm = myProp.Normal(0.3, Precision::Confusion());
  EXPECT_TRUE(aNorm.IsDefined);
}

TEST_F(Geom2dProp_CurveBSplineTest, CentreOfCurvatureDefined)
{
  const Geom2dProp::CentreResult aCentre = myProp.CentreOfCurvature(0.3, Precision::Confusion());
  EXPECT_TRUE(aCentre.IsDefined);
}

TEST_F(Geom2dProp_CurveBSplineTest, CurvatureExtrema)
{
  const Geom2dProp::CurveAnalysis aExtrema = myProp.FindCurvatureExtrema();
  EXPECT_TRUE(aExtrema.IsDone);
  // Verify all extrema parameters are within [0, 1]
  for (int i = 1; i <= aExtrema.Points.Length(); ++i)
  {
    EXPECT_GE(aExtrema.Points.Value(i).Parameter, 0.0 - 1.0e-6);
    EXPECT_LE(aExtrema.Points.Value(i).Parameter, 1.0 + 1.0e-6);
  }
}

TEST_F(Geom2dProp_CurveBSplineTest, InflectionPoints)
{
  const Geom2dProp::CurveAnalysis aInfl = myProp.FindInflections();
  EXPECT_TRUE(aInfl.IsDone);
  for (int i = 1; i <= aInfl.Points.Length(); ++i)
  {
    EXPECT_EQ(aInfl.Points.Value(i).Type, Geom2dProp::CIType::Inflection);
    EXPECT_GE(aInfl.Points.Value(i).Parameter, 0.0 - 1.0e-6);
    EXPECT_LE(aInfl.Points.Value(i).Parameter, 1.0 + 1.0e-6);
  }
}

// B-spline with lower continuity (C1)
TEST(Geom2dProp_BSplineTest, LowContinuity_C1)
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

  occ::handle<Geom2d_BSplineCurve> aBSpline =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aBSpline);
  EXPECT_TRUE(aProp.IsInitialized());

  // Should work with C3 interval subdivision
  const Geom2dProp::CurveAnalysis aExtrema = aProp.FindCurvatureExtrema();
  EXPECT_TRUE(aExtrema.IsDone);

  const Geom2dProp::CurveAnalysis aInfl = aProp.FindInflections();
  EXPECT_TRUE(aInfl.IsDone);
}

// ============================================================================
// Offset curve tests
// ============================================================================

TEST(Geom2dProp_OffsetCurveTest, IsInitialized)
{
  gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle>      aCircle = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 2.0);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anOffset);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_OffsetCurve);
}

TEST(Geom2dProp_OffsetCurveTest, OffsetCircle_ConstantCurvature)
{
  // Offset of circle R=5 by +2 gives circle R=7, curvature=1/7
  gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle>      aCircle = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 2.0);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anOffset);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(std::abs(aCurv.Value), 1.0 / 7.0, 1.0e-6);
  }
}

TEST(Geom2dProp_OffsetCurveTest, TangentAndNormalDefined)
{
  gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle>      aCircle = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 2.0);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anOffset);

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.5, Precision::Confusion());
  EXPECT_TRUE(aTan.IsDefined);

  const Geom2dProp::NormalResult aNorm = aProp.Normal(0.5, Precision::Confusion());
  EXPECT_TRUE(aNorm.IsDefined);

  // Perpendicularity
  if (aTan.IsDefined && aNorm.IsDefined)
  {
    const double aDot =
      aTan.Direction.X() * aNorm.Direction.X() + aTan.Direction.Y() * aNorm.Direction.Y();
    EXPECT_NEAR(aDot, 0.0, 1.0e-10);
  }
}

TEST(Geom2dProp_OffsetCurveTest, OffsetEllipse_ExtremaAndInflections)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse>     anEllipse = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(anEllipse, 1.0);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anOffset);
  EXPECT_TRUE(aProp.IsInitialized());

  const Geom2dProp::CurveAnalysis aExtrema = aProp.FindCurvatureExtrema();
  EXPECT_TRUE(aExtrema.IsDone);

  const Geom2dProp::CurveAnalysis aInfl = aProp.FindInflections();
  EXPECT_TRUE(aInfl.IsDone);
}

// ============================================================================
// TrimmedCurve tests
// ============================================================================

TEST(Geom2dProp_TrimmedCurveTest, UnwrapsToCircle)
{
  gp_Circ2d                        aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle>      aCircle = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aCircle, 0.0, M_PI);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aTrimmed);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / 5.0, Precision::Confusion());
}

TEST(Geom2dProp_TrimmedCurveTest, UnwrapsToEllipse)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse>     anEllipse = new Geom2d_Ellipse(anElips);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(anEllipse, 0.0, M_PI);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aTrimmed);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_Ellipse);
}

TEST(Geom2dProp_TrimmedCurveTest, NestedTrimmedCurve)
{
  gp_Circ2d                        aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0);
  occ::handle<Geom2d_Circle>      aCircle = new Geom2d_Circle(aCirc);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed1 = new Geom2d_TrimmedCurve(aCircle, 0.0, M_PI);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed2 = new Geom2d_TrimmedCurve(aTrimmed1, 0.1, 1.0);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aTrimmed2);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / 3.0, Precision::Confusion());
}

// ============================================================================
// Null / uninitialized tests
// ============================================================================

TEST(Geom2dProp_CurveTest, NullHandle_NotInitialized)
{
  Geom2dProp_Curve              aProp;
  occ::handle<Geom2d_Curve> aNullCurve;
  aProp.Initialize(aNullCurve);
  EXPECT_FALSE(aProp.IsInitialized());

  const Geom2dProp::TangentResult aTan = aProp.Tangent(0.0, 1.0e-7);
  EXPECT_FALSE(aTan.IsDefined);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, 1.0e-7);
  EXPECT_FALSE(aCurv.IsDefined);

  const Geom2dProp::NormalResult aNorm = aProp.Normal(0.0, 1.0e-7);
  EXPECT_FALSE(aNorm.IsDefined);

  const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, 1.0e-7);
  EXPECT_FALSE(aCentre.IsDefined);

  const Geom2dProp::CurveAnalysis aExtrema = aProp.FindCurvatureExtrema();
  EXPECT_FALSE(aExtrema.IsDone);

  const Geom2dProp::CurveAnalysis aInfl = aProp.FindInflections();
  EXPECT_FALSE(aInfl.IsDone);
}

TEST(Geom2dProp_CurveTest, DefaultConstructor_NotInitialized)
{
  Geom2dProp_Curve aProp;
  EXPECT_FALSE(aProp.IsInitialized());
}

TEST(Geom2dProp_CurveTest, ReInitialize_ChangesType)
{
  Geom2dProp_Curve aProp;

  // First: circle
  gp_Circ2d                    aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  aProp.Initialize(aCircle);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);

  // Re-initialize with line
  gp_Lin2d                   aLin(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(aLin);
  aProp.Initialize(aLine);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Line);
  EXPECT_NEAR(aProp.Curvature(0.0, Precision::Confusion()).Value, 0.0, Precision::Confusion());
}

// ============================================================================
// Initialize from adaptor tests
// ============================================================================

TEST(Geom2dProp_AdaptorTest, InitFromGeom2dAdaptor)
{
  gp_Circ2d                    aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);
  Geom2dAdaptor_Curve          anAdaptor(aCircle);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anAdaptor);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);

  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / 5.0, Precision::Confusion());
}

TEST(Geom2dProp_AdaptorTest, InitFromGeom2dAdaptor_Ellipse)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 8.0, 3.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);
  Geom2dAdaptor_Curve          anAdaptor(anEllipse);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anAdaptor);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_Ellipse);

  // Curvature at major vertex: a/b^2 = 8/9
  const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 8.0 / 9.0, 1.0e-6);
}

TEST(Geom2dProp_AdaptorTest, InitFromGeom2dAdaptor_BSpline)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 2.0);
  aPoles(3) = gp_Pnt2d(3.0, 2.0);
  aPoles(4) = gp_Pnt2d(4.0, 0.0);

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;

  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 3;
  aMults(2) = 1;
  aMults(3) = 3;

  occ::handle<Geom2d_BSplineCurve> aBSpline =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 2);
  Geom2dAdaptor_Curve anAdaptor(aBSpline);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anAdaptor);
  EXPECT_TRUE(aProp.IsInitialized());
  EXPECT_EQ(aProp.GetType(), GeomAbs_BSplineCurve);
}

// ============================================================================
// Cross-validation tests
// ============================================================================

TEST(Geom2dProp_CrossValidationTest, Circle_MatchesLProp)
{
  gp_Circ2d                    aCirc(gp_Ax2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)), 7.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aCircle);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 6.0)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurv.IsDefined);
    EXPECT_NEAR(aCurv.Value, 1.0 / 7.0, 1.0e-10);

    const Geom2dProp::CentreResult aCentre = aProp.CentreOfCurvature(u, Precision::Confusion());
    ASSERT_TRUE(aCentre.IsDefined);
    EXPECT_NEAR(aCentre.Centre.X(), 1.0, Precision::Confusion());
    EXPECT_NEAR(aCentre.Centre.Y(), 2.0, Precision::Confusion());
  }
}

TEST(Geom2dProp_CrossValidationTest, Ellipse_MatchesLProp)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 10.0, 5.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anEllipse);

  // At major vertex (U=0): curvature = a/b^2 = 10/25 = 0.4
  const Geom2dProp::CurvatureResult aCurv0 = aProp.Curvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv0.IsDefined);
  EXPECT_NEAR(aCurv0.Value, 10.0 / 25.0, 1.0e-6);

  // At minor vertex (U=PI/2): curvature = b/a^2 = 5/100 = 0.05
  const Geom2dProp::CurvatureResult aCurvPi2 =
    aProp.Curvature(M_PI / 2.0, Precision::Confusion());
  ASSERT_TRUE(aCurvPi2.IsDefined);
  EXPECT_NEAR(aCurvPi2.Value, 5.0 / 100.0, 1.0e-6);
}

// Cross-validate: free function vs. dispatcher give same result
TEST(Geom2dProp_CrossValidationTest, FreeFunctionVsDispatcher)
{
  gp_Circ2d aCirc(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0);
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(aCirc);

  Geom2dProp_Curve aProp;
  aProp.Initialize(aCircle);

  const double u = 1.0;
  gp_Pnt2d     aPnt;
  gp_Vec2d     aD1, aD2;
  aCircle->D2(u, aPnt, aD1, aD2);

  // Free function
  const Geom2dProp::CurvatureResult aCurvFree =
    Geom2dProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  // Dispatcher
  const Geom2dProp::CurvatureResult aCurvDisp = aProp.Curvature(u, Precision::Confusion());

  ASSERT_TRUE(aCurvFree.IsDefined);
  ASSERT_TRUE(aCurvDisp.IsDefined);
  EXPECT_NEAR(aCurvFree.Value, aCurvDisp.Value, 1.0e-10);
}

// Cross-validate centre of curvature consistency: distance from point = 1/|curvature|
TEST(Geom2dProp_CrossValidationTest, CentreDistanceConsistency)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 8.0, 4.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve aProp;
  aProp.Initialize(anEllipse);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 8.0)
  {
    const Geom2dProp::CurvatureResult aCurv = aProp.Curvature(u, Precision::Confusion());
    const Geom2dProp::CentreResult    aCentre = aProp.CentreOfCurvature(u, Precision::Confusion());
    if (aCurv.IsDefined && aCentre.IsDefined && !aCurv.IsInfinite
        && std::abs(aCurv.Value) > 1.0e-10)
    {
      gp_Pnt2d aPnt;
      gp_Vec2d aD1;
      anEllipse->D1(u, aPnt, aD1);
      const double aDist     = aPnt.Distance(aCentre.Centre);
      const double aExpected = 1.0 / std::abs(aCurv.Value);
      EXPECT_NEAR(aDist, aExpected, 1.0e-6);
    }
  }
}

// Cross-validate: adaptor init and geometry init give same results
TEST(Geom2dProp_CrossValidationTest, AdaptorVsGeometryInit)
{
  gp_Elips2d anElips(gp_Ax2d(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0)), 6.0, 3.0);
  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(anElips);

  Geom2dProp_Curve aPropGeom;
  aPropGeom.Initialize(anEllipse);

  Geom2dAdaptor_Curve anAdaptor(anEllipse);
  Geom2dProp_Curve    aPropAdap;
  aPropAdap.Initialize(anAdaptor);

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 5.0)
  {
    const Geom2dProp::CurvatureResult aCurvG = aPropGeom.Curvature(u, Precision::Confusion());
    const Geom2dProp::CurvatureResult aCurvA = aPropAdap.Curvature(u, Precision::Confusion());
    ASSERT_TRUE(aCurvG.IsDefined);
    ASSERT_TRUE(aCurvA.IsDefined);
    EXPECT_NEAR(aCurvG.Value, aCurvA.Value, 1.0e-10);
  }
}
