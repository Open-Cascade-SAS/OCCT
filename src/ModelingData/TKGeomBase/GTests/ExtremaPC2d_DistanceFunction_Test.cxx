// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#include <ExtremaPC2d_DistanceFunction.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

#include <cmath>

//=================================================================================================

TEST(ExtremaPC2d_DistanceFunctionTest, LineValuesAreExact)
{
  occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  Geom2dAdaptor_Curve anAdaptor(aLine, -4.0, 7.0);
  ExtremaPC2d_DistanceFunction aFunction(anAdaptor, gp_Pnt2d(2.0, 3.0));
  double aValue = 0.0;
  double aDerivative = 0.0;
  ASSERT_TRUE(aFunction.Values(5.0, aValue, aDerivative));
  EXPECT_NEAR(aValue, 3.0, 1.0e-12);
  EXPECT_NEAR(aDerivative, 1.0, 1.0e-12);
  EXPECT_EQ(&aFunction.Curve(), &anAdaptor);
  EXPECT_NEAR(aFunction.FirstParameter(), -4.0, 1.0e-12);
  EXPECT_NEAR(aFunction.LastParameter(), 7.0, 1.0e-12);
}

//=================================================================================================

TEST(ExtremaPC2d_DistanceFunctionTest, CircleNormalizedFormulaIsExact)
{
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(
    gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), true),
    5.0);
  Geom2dAdaptor_Curve anAdaptor(aCircle);
  ExtremaPC2d_DistanceFunction aFunction(anAdaptor, gp_Pnt2d(3.0, -4.0));
  constexpr double aU = 0.7;
  double aValue = 0.0;
  double aDerivative = 0.0;
  ASSERT_TRUE(aFunction.Values(aU, aValue, aDerivative));
  EXPECT_NEAR(aValue, 3.0 * std::sin(aU) + 4.0 * std::cos(aU), 1.0e-11);
  EXPECT_NEAR(aDerivative, 3.0 * std::cos(aU) - 4.0 * std::sin(aU), 1.0e-11);
}

//=================================================================================================

TEST(ExtremaPC2d_DistanceFunctionTest, DerivativeMatchesCenteredDifference)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(1.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 1.0);
  aPoles(3) = gp_Pnt2d(0.0, 1.0);
  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = M_SQRT1_2;
  aWeights(3) = 1.0;
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles, aWeights);
  Geom2dAdaptor_Curve anAdaptor(aCurve);
  ExtremaPC2d_DistanceFunction aFunction(anAdaptor, gp_Pnt2d(2.0, 0.3));
  constexpr double aU = 0.37;
  constexpr double aStep = 1.0e-6;
  double aLeft = 0.0;
  double aRight = 0.0;
  double aDerivative = 0.0;
  ASSERT_TRUE(aFunction.Value(aU - aStep, aLeft));
  ASSERT_TRUE(aFunction.Value(aU + aStep, aRight));
  ASSERT_TRUE(aFunction.Derivative(aU, aDerivative));
  EXPECT_NEAR(aDerivative, (aRight - aLeft) / (2.0 * aStep), 1.0e-6);
}

//=================================================================================================

TEST(ExtremaPC2d_DistanceFunctionTest, ZeroSpeedReturnsDefinedZeros)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(0.0, 0.0);
  aPoles(3) = gp_Pnt2d(2.0, 1.0);
  aPoles(4) = gp_Pnt2d(3.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve anAdaptor(aCurve);
  ExtremaPC2d_DistanceFunction aFunction(anAdaptor, gp_Pnt2d(1.0, 1.0));
  double aValue = 1.0;
  double aDerivative = 1.0;
  ASSERT_TRUE(aFunction.Values(0.0, aValue, aDerivative));
  EXPECT_EQ(aValue, 0.0);
  EXPECT_EQ(aDerivative, 0.0);
}
