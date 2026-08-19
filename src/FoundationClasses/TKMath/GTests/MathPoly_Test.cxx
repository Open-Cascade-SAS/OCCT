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

#include <gtest/gtest.h>

#include <MathPoly_Quadratic.hxx>
#include <MathPoly_Cubic.hxx>
#include <MathPoly_Laguerre.hxx>
#include <MathPoly_Quartic.hxx>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-10;

//! Helper to verify a root satisfies the polynomial equation.
double EvalCubic(double theA, double theB, double theC, double theD, double theX)
{
  return theA * theX * theX * theX + theB * theX * theX + theC * theX + theD;
}

double EvalQuartic(double theA, double theB, double theC, double theD, double theE, double theX)
{
  const double aX2 = theX * theX;
  return theA * aX2 * aX2 + theB * aX2 * theX + theC * aX2 + theD * theX + theE;
}
} // namespace

// ============================================================================
// Linear equation tests
// ============================================================================

TEST(MathPoly_LinearTest, SimpleLinear)
{
  // 2x + 4 = 0 -> x = -2
  MathPoly::PolyResult aResult = MathPoly::Linear(2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
}

TEST(MathPoly_LinearTest, ZeroCoefficient_InfiniteSolutions)
{
  // 0*x + 0 = 0 -> infinite solutions
  MathPoly::PolyResult aResult = MathPoly::Linear(0.0, 0.0);
  EXPECT_EQ(aResult.Status, MathUtils::Status::InfiniteSolutions);
}

TEST(MathPoly_LinearTest, ZeroCoefficient_NoSolution)
{
  // 0*x + 5 = 0 -> no solution
  MathPoly::PolyResult aResult = MathPoly::Linear(0.0, 5.0);
  EXPECT_EQ(aResult.Status, MathUtils::Status::NoSolution);
}

TEST(MathPoly_LinearTest, UnrepresentableFiniteRootIsNumericalError)
{
  const MathPoly::PolyResult aResult =
    MathPoly::Linear(std::numeric_limits<double>::denorm_min(),
                     std::numeric_limits<double>::max());
  EXPECT_EQ(aResult.Status, MathUtils::Status::NumericalError);
  EXPECT_EQ(aResult.NbRoots, 0u);
}

// ============================================================================
// Quadratic equation tests
// ============================================================================

TEST(MathPoly_QuadraticTest, TwoDistinctRoots)
{
  // x^2 - 5x + 6 = 0 -> roots: 2, 3
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, -5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, DoubleRoot)
{
  // x^2 - 4x + 4 = 0 -> root: 2 (double)
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, -4.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_EQ(aResult.Multiplicities[0], 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, NoRealRoots)
{
  // x^2 + 1 = 0 -> no real roots
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathPoly_QuadraticTest, NegativeRoots)
{
  // x^2 + 5x + 6 = 0 -> roots: -3, -2
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, 5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], -2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, MixedSignRoots)
{
  // x^2 - 1 = 0 -> roots: -1, 1
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, 0.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], -1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 1.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, ReducesToLinear)
{
  // 0*x^2 + 2x + 4 = 0 -> x = -2
  MathPoly::PolyResult aResult = MathPoly::Quadratic(0.0, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, LargeCoefficients)
{
  // 1e6*x^2 - 2e6*x + 1e6 = 0 -> root: 1 (double)
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0e6, -2.0e6, 1.0e6);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, SmallCoefficients)
{
  // 1e-6*x^2 - 5e-6*x + 6e-6 = 0 -> roots: 2, 3
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0e-6, -5.0e-6, 6.0e-6);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, RootsAreSorted)
{
  // 2x^2 + 3x - 2 = 0 -> roots: -2, 0.5
  MathPoly::PolyResult aResult = MathPoly::Quadratic(2.0, 3.0, -2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_LT(aResult.Roots[0], aResult.Roots[1]);
}

TEST(MathPoly_QuadraticTest, UnequalNonIntegralRoots)
{
  // 3.7*x^2 - 2.1*x - 4.3 = 0
  const MathPoly::PolyResult aResult = MathPoly::Quadratic(3.7, -2.1, -4.3);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_EQ(aResult.NbRoots, size_t{2});
  EXPECT_LT(aResult.Roots[0], aResult.Roots[1]);
  EXPECT_EQ(aResult.Multiplicities[0], size_t{1});
  EXPECT_EQ(aResult.Multiplicities[1], size_t{1});
  EXPECT_NEAR(aResult.Roots[0], -0.83097870127785634, 1.0e-13);
  EXPECT_NEAR(aResult.Roots[1], 1.3985462688454240, 1.0e-13);
}

TEST(MathPoly_QuadraticTest, GlobalCoefficientScalingDoesNotChangeRoots)
{
  for (double aScale : {1.0e-300, 1.0e300})
  {
    MathPoly::PolyResult aResult =
      MathPoly::Quadratic(aScale, -5.0 * aScale, 6.0 * aScale);
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_EQ(aResult.NbRoots, 2u);
    EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
    EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
  }
}

TEST(MathPoly_QuadraticTest, ExtremeRootMagnitudes)
{
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, -1.0e150, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 2u);
  EXPECT_NEAR(aResult.Roots[0] / 1.0e-150, 1.0, 1.0e-12);
  EXPECT_NEAR(aResult.Roots[1] / 1.0e150, 1.0, 1.0e-12);
}

TEST(MathPoly_QuadraticTest, RejectsNonFiniteCoefficients)
{
  const double aNan = std::numeric_limits<double>::quiet_NaN();
  const double anInfinity = std::numeric_limits<double>::infinity();
  EXPECT_EQ(MathPoly::Quadratic(aNan, 1.0, 1.0).Status, MathUtils::Status::InvalidInput);
  EXPECT_EQ(MathPoly::Quadratic(1.0, anInfinity, 1.0).Status, MathUtils::Status::InvalidInput);
}

// ============================================================================
// Cubic equation tests
// ============================================================================

TEST(MathPoly_CubicTest, ThreeDistinctRoots)
{
  // x^3 - 6x^2 + 11x - 6 = 0 -> roots: 1, 2, 3
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, -6.0, 11.0, -6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, MixedSignFractionalRoots)
{
  const MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, -1.5, -5.5, 3.0);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, size_t{3});
  constexpr double THE_EXPECTED_ROOTS[] = {-2.0, 0.5, 3.0};
  for (size_t anIndex = 0; anIndex < aResult.NbRoots; ++anIndex)
  {
    EXPECT_NEAR(aResult.Roots[anIndex], THE_EXPECTED_ROOTS[anIndex], 1.0e-13);
  }
}

TEST(MathPoly_CubicTest, OneRealRoot)
{
  // x^3 + x + 1 = 0 -> one real root approximately -0.6824
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, 0.0, 1.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  // Verify the root satisfies the equation
  double aValue = EvalCubic(1.0, 0.0, 1.0, 1.0, aResult.Roots[0]);
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, TripleRoot)
{
  // x^3 - 3x^2 + 3x - 1 = 0 -> root: 1 (triple)
  // This is (x-1)^3
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, -3.0, 3.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 1u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.Multiplicities[0], 3);
}

TEST(MathPoly_CubicTest, OneSimpleOneDouble)
{
  // x^3 - 5x^2 + 8x - 4 = 0 -> roots: 1, 2 (double)
  // This is (x-1)(x-2)^2
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, -5.0, 8.0, -4.0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 2u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.Multiplicities[0], 1u);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.Multiplicities[1], 2u);
}

TEST(MathPoly_CubicTest, ReducesToQuadratic)
{
  // 0*x^3 + x^2 - 5x + 6 = 0 -> roots: 2, 3
  MathPoly::PolyResult aResult = MathPoly::Cubic(0.0, 1.0, -5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, NegativeRoots)
{
  // x^3 + 6x^2 + 11x + 6 = 0 -> roots: -3, -2, -1
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, 6.0, 11.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], -1.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, DepressedCubic)
{
  // x^3 - 7x + 6 = 0 -> roots: -3, 1, 2
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, 0.0, -7.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 2.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, RootsAreSorted)
{
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, -6.0, 11.0, -6.0);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 1; i < aResult.NbRoots; ++i)
  {
    EXPECT_LE(aResult.Roots[i - 1], aResult.Roots[i]);
  }
}

TEST(MathPoly_CubicTest, ClusteredRootsRemainDistinct)
{
  // (x-1)(x-1.001)(x-2)
  MathPoly::PolyResult aResult = MathPoly::Cubic(1.0, -4.001, 5.003, -2.002);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 3u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, 1.0e-11);
  EXPECT_NEAR(aResult.Roots[1], 1.001, 1.0e-11);
  EXPECT_NEAR(aResult.Roots[2], 2.0, 1.0e-11);
}

TEST(MathPoly_CubicTest, ThreeUnevenRealRoots)
{
  // 1.7*x^3 - 2.3*x^2 - 4.8*x + 1.1 = 0
  const MathPoly::PolyResult aResult = MathPoly::Cubic(1.7, -2.3, -4.8, 1.1);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_EQ(aResult.NbRoots, size_t{3});
  constexpr double THE_EXPECTED_ROOTS[] = {
    -1.2704399324056863,
    0.21113914549269189,
    2.4122419633835825,
  };
  for (size_t anIndex = 0; anIndex < aResult.NbRoots; ++anIndex)
  {
    EXPECT_EQ(aResult.Multiplicities[anIndex], size_t{1});
    EXPECT_NEAR(aResult.Roots[anIndex], THE_EXPECTED_ROOTS[anIndex], 1.0e-12);
    if (anIndex > 0)
    {
      EXPECT_LT(aResult.Roots[anIndex - 1], aResult.Roots[anIndex]);
    }
  }
}

TEST(MathPoly_CubicTest, FractionalTangentialRoot)
{
  // x^3 - 2.125*x^2 + 0.171875*x + 1.181640625 = 0
  const MathPoly::PolyResult aResult =
    MathPoly::Cubic(1.0, -2.125, 0.171875, 1.181640625);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_EQ(aResult.NbRoots, size_t{2});
  EXPECT_LT(aResult.Roots[0], aResult.Roots[1]);
  EXPECT_NEAR(aResult.Roots[0], -0.625, 1.0e-13);
  EXPECT_EQ(aResult.Multiplicities[0], size_t{1});
  EXPECT_NEAR(aResult.Roots[1], 1.375, 1.0e-12);
  EXPECT_EQ(aResult.Multiplicities[1], size_t{2});
}

TEST(MathPoly_CubicTest, LargeScaleIrregularEquation)
{
  // 2.3e80*x^3 - 1.7e80*x^2 - 3.9e80*x + 6.5e79 = 0
  const MathPoly::PolyResult aResult =
    MathPoly::Cubic(2.3e80, -1.7e80, -3.9e80, 6.5e79);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_EQ(aResult.NbRoots, size_t{3});
  constexpr double THE_EXPECTED_ROOTS[] = {
    -1.0776640490606961,
    0.15810155136079423,
    1.6586929324825108,
  };
  for (size_t anIndex = 0; anIndex < aResult.NbRoots; ++anIndex)
  {
    EXPECT_EQ(aResult.Multiplicities[anIndex], size_t{1});
    EXPECT_NEAR(aResult.Roots[anIndex], THE_EXPECTED_ROOTS[anIndex], 1.0e-11);
    if (anIndex > 0)
    {
      EXPECT_LT(aResult.Roots[anIndex - 1], aResult.Roots[anIndex]);
    }
  }
}

// ============================================================================
// Quartic equation tests
// ============================================================================

TEST(MathPoly_QuarticTest, FourDistinctRoots)
{
  // (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24 = 0
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 4);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[3], 4.0, THE_TOLERANCE);
}

TEST(MathPoly_QuarticTest, TwoRealRoots)
{
  // x^4 - 5x^2 + 4 = 0 -> roots: -2, -1, 1, 2
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, 0.0, -5.0, 0.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 4);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], -1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[3], 2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuarticTest, NoRealRoots)
{
  // x^4 + 1 = 0 -> no real roots
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, 0.0, 0.0, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathPoly_QuarticTest, Biquadratic)
{
  // x^4 - 5x^2 + 4 = 0 is biquadratic (no x^3 or x term)
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, 0.0, -5.0, 0.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 4);
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aValue = EvalQuartic(1.0, 0.0, -5.0, 0.0, 4.0, aResult.Roots[i]);
    EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
  }
}

TEST(MathPoly_QuarticTest, ReducesToCubic)
{
  // 0*x^4 + x^3 - 6x^2 + 11x - 6 = 0 -> roots: 1, 2, 3
  MathPoly::PolyResult aResult = MathPoly::Quartic(0.0, 1.0, -6.0, 11.0, -6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_QuarticTest, QuadrupleRoot)
{
  // (x-2)^4 = x^4 - 8x^3 + 24x^2 - 32x + 16 = 0
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, -8.0, 24.0, -32.0, 16.0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 1u);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.Multiplicities[0], 4);
}

TEST(MathPoly_QuarticTest, TwoDoubleRoots)
{
  // (x-1)^2 * (x-3)^2 = x^4 - 8x^3 + 22x^2 - 24x + 9 = 0
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, -8.0, 22.0, -24.0, 9.0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 2u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.Multiplicities[0], 2u);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.Multiplicities[1], 2u);
}

TEST(MathPoly_QuarticTest, RootsAreSorted)
{
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 1; i < aResult.NbRoots; ++i)
  {
    EXPECT_LE(aResult.Roots[i - 1], aResult.Roots[i]);
  }
}

TEST(MathPoly_QuarticTest, VerifyRootsSatisfyEquation)
{
  // General quartic with all roots
  MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aValue = EvalQuartic(1.0, -10.0, 35.0, -50.0, 24.0, aResult.Roots[i]);
    EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
  }
}

TEST(MathPoly_QuarticTest, GloballyScaledRepeatedEquation)
{
  for (double aScale : {1.0e-280, 1.0e280})
  {
    MathPoly::PolyResult aResult = MathPoly::Quartic(aScale,
                                                     -8.0 * aScale,
                                                     22.0 * aScale,
                                                     -24.0 * aScale,
                                                     9.0 * aScale);
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_EQ(aResult.NbRoots, 2u);
    EXPECT_NEAR(aResult.Roots[0], 1.0, 1.0e-10);
    EXPECT_NEAR(aResult.Roots[1], 3.0, 1.0e-10);
    EXPECT_EQ(aResult.Multiplicities[0], 2);
    EXPECT_EQ(aResult.Multiplicities[1], 2);
  }
}

TEST(MathPoly_QuarticTest, FourUnevenRealRoots)
{
  // x^4 + 0.7*x^3 - 5.2*x^2 - 1.3*x + 3.4 = 0
  const MathPoly::PolyResult aResult = MathPoly::Quartic(1.0, 0.7, -5.2, -1.3, 3.4);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_EQ(aResult.NbRoots, size_t{4});
  constexpr double THE_EXPECTED_ROOTS[] = {
    -2.3964634709491510,
    -0.97207148860835169,
    0.76791933673826718,
    1.9006156228192357,
  };
  for (size_t anIndex = 0; anIndex < aResult.NbRoots; ++anIndex)
  {
    EXPECT_EQ(aResult.Multiplicities[anIndex], size_t{1});
    EXPECT_NEAR(aResult.Roots[anIndex], THE_EXPECTED_ROOTS[anIndex], 1.0e-10);
    if (anIndex > 0)
    {
      EXPECT_LT(aResult.Roots[anIndex - 1], aResult.Roots[anIndex]);
    }
  }
}

TEST(MathPoly_ComplexRootsTest, QuarticWithRealAndComplexRoots)
{
  // 1.3*x^4 - 0.4*x^3 + 0.7*x^2 - 2.2*x - 1.1 = 0
  const double               aCoefficients[] = {-1.1, -2.2, 0.7, -0.4, 1.3};
  const MathPoly::PolyResult aResult          = MathPoly::Laguerre(aCoefficients, 4);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_EQ(aResult.NbRoots, size_t{2});
  EXPECT_LT(aResult.Roots[0], aResult.Roots[1]);
  EXPECT_NEAR(aResult.Roots[0], -0.41478958662056115, 1.0e-10);
  EXPECT_NEAR(aResult.Roots[1], 1.2936081173279923, 1.0e-10);
  EXPECT_EQ(aResult.Multiplicities[0], size_t{1});
  EXPECT_EQ(aResult.Multiplicities[1], size_t{1});

  ASSERT_EQ(aResult.NbComplexRoots, size_t{2});
  EXPECT_NEAR(aResult.ComplexRoots[0].real(), -0.28556311150756175, 1.0e-10);
  EXPECT_NEAR(aResult.ComplexRoots[0].imag(), 1.2228682399186652, 1.0e-10);
  EXPECT_NEAR(aResult.ComplexRoots[1].real(), -0.28556311150756175, 1.0e-10);
  EXPECT_NEAR(aResult.ComplexRoots[1].imag(), -1.2228682399186652, 1.0e-10);
  EXPECT_EQ(aResult.ComplexMultiplicities[0], size_t{1});
  EXPECT_EQ(aResult.ComplexMultiplicities[1], size_t{1});
}

// ============================================================================
// Boolean conversion tests
// ============================================================================

TEST(MathPoly_BoolConversionTest, SuccessfulResultIsTrue)
{
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, -5.0, 6.0);
  EXPECT_TRUE(static_cast<bool>(aResult));
}

TEST(MathPoly_BoolConversionTest, NoSolutionResultIsFalse)
{
  MathPoly::PolyResult aResult = MathPoly::Linear(0.0, 5.0);
  EXPECT_FALSE(static_cast<bool>(aResult));
}

// ============================================================================
// Indexing operator tests
// ============================================================================

TEST(MathPoly_IndexingTest, BracketOperator)
{
  MathPoly::PolyResult aResult = MathPoly::Quadratic(1.0, -5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult[0], aResult.Roots[0]);
  EXPECT_EQ(aResult[1], aResult.Roots[1]);
}
