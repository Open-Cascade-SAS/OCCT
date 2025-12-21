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
#include <MathPoly_Quartic.hxx>

#include <cmath>

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
  MathUtils::PolyResult aResult = MathPoly::Linear(2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
}

TEST(MathPoly_LinearTest, ZeroCoefficient_InfiniteSolutions)
{
  // 0*x + 0 = 0 -> infinite solutions
  MathUtils::PolyResult aResult = MathPoly::Linear(0.0, 0.0);
  EXPECT_EQ(aResult.Status, MathUtils::Status::InfiniteSolutions);
}

TEST(MathPoly_LinearTest, ZeroCoefficient_NoSolution)
{
  // 0*x + 5 = 0 -> no solution
  MathUtils::PolyResult aResult = MathPoly::Linear(0.0, 5.0);
  EXPECT_EQ(aResult.Status, MathUtils::Status::NoSolution);
}

// ============================================================================
// Quadratic equation tests
// ============================================================================

TEST(MathPoly_QuadraticTest, TwoDistinctRoots)
{
  // x^2 - 5x + 6 = 0 -> roots: 2, 3
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, -5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, DoubleRoot)
{
  // x^2 - 4x + 4 = 0 -> root: 2 (double)
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, -4.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, NoRealRoots)
{
  // x^2 + 1 = 0 -> no real roots
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathPoly_QuadraticTest, NegativeRoots)
{
  // x^2 + 5x + 6 = 0 -> roots: -3, -2
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, 5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], -2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, MixedSignRoots)
{
  // x^2 - 1 = 0 -> roots: -1, 1
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, 0.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], -1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 1.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, ReducesToLinear)
{
  // 0*x^2 + 2x + 4 = 0 -> x = -2
  MathUtils::PolyResult aResult = MathPoly::Quadratic(0.0, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, LargeCoefficients)
{
  // 1e6*x^2 - 2e6*x + 1e6 = 0 -> root: 1 (double)
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0e6, -2.0e6, 1.0e6);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, SmallCoefficients)
{
  // 1e-6*x^2 - 5e-6*x + 6e-6 = 0 -> roots: 2, 3
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0e-6, -5.0e-6, 6.0e-6);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_QuadraticTest, RootsAreSorted)
{
  // 2x^2 + 3x - 2 = 0 -> roots: -2, 0.5
  MathUtils::PolyResult aResult = MathPoly::Quadratic(2.0, 3.0, -2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_LT(aResult.Roots[0], aResult.Roots[1]);
}

// ============================================================================
// Cubic equation tests
// ============================================================================

TEST(MathPoly_CubicTest, ThreeDistinctRoots)
{
  // x^3 - 6x^2 + 11x - 6 = 0 -> roots: 1, 2, 3
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, -6.0, 11.0, -6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, OneRealRoot)
{
  // x^3 + x + 1 = 0 -> one real root approximately -0.6824
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, 0.0, 1.0, 1.0);
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
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, -3.0, 3.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, OneSimpleOneDouble)
{
  // x^3 - 5x^2 + 8x - 4 = 0 -> roots: 1, 2 (double)
  // This is (x-1)(x-2)^2
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, -5.0, 8.0, -4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 2);
  // Verify roots satisfy the equation
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aValue = EvalCubic(1.0, -5.0, 8.0, -4.0, aResult.Roots[i]);
    EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
  }
}

TEST(MathPoly_CubicTest, ReducesToQuadratic)
{
  // 0*x^3 + x^2 - 5x + 6 = 0 -> roots: 2, 3
  MathUtils::PolyResult aResult = MathPoly::Cubic(0.0, 1.0, -5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, NegativeRoots)
{
  // x^3 + 6x^2 + 11x + 6 = 0 -> roots: -3, -2, -1
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, 6.0, 11.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], -1.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, DepressedCubic)
{
  // x^3 - 7x + 6 = 0 -> roots: -3, 1, 2
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, 0.0, -7.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 2.0, THE_TOLERANCE);
}

TEST(MathPoly_CubicTest, RootsAreSorted)
{
  MathUtils::PolyResult aResult = MathPoly::Cubic(1.0, -6.0, 11.0, -6.0);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 1; i < aResult.NbRoots; ++i)
  {
    EXPECT_LE(aResult.Roots[i - 1], aResult.Roots[i]);
  }
}

// ============================================================================
// Quartic equation tests
// ============================================================================

TEST(MathPoly_QuarticTest, FourDistinctRoots)
{
  // (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24 = 0
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
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
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, 0.0, -5.0, 0.0, 4.0);
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
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, 0.0, 0.0, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathPoly_QuarticTest, Biquadratic)
{
  // x^4 - 5x^2 + 4 = 0 is biquadratic (no x^3 or x term)
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, 0.0, -5.0, 0.0, 4.0);
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
  MathUtils::PolyResult aResult = MathPoly::Quartic(0.0, 1.0, -6.0, 11.0, -6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 3);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
}

TEST(MathPoly_QuarticTest, QuadrupleRoot)
{
  // (x-2)^4 = x^4 - 8x^3 + 24x^2 - 32x + 16 = 0
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, -8.0, 24.0, -32.0, 16.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
}

TEST(MathPoly_QuarticTest, TwoDoubleRoots)
{
  // (x-1)^2 * (x-3)^2 = x^4 - 8x^3 + 22x^2 - 24x + 9 = 0
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, -8.0, 22.0, -24.0, 9.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 2);
  // Verify roots satisfy equation
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aValue = EvalQuartic(1.0, -8.0, 22.0, -24.0, 9.0, aResult.Roots[i]);
    EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
  }
}

TEST(MathPoly_QuarticTest, RootsAreSorted)
{
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 1; i < aResult.NbRoots; ++i)
  {
    EXPECT_LE(aResult.Roots[i - 1], aResult.Roots[i]);
  }
}

TEST(MathPoly_QuarticTest, VerifyRootsSatisfyEquation)
{
  // General quartic with all roots
  MathUtils::PolyResult aResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aValue = EvalQuartic(1.0, -10.0, 35.0, -50.0, 24.0, aResult.Roots[i]);
    EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
  }
}

// ============================================================================
// Boolean conversion tests
// ============================================================================

TEST(MathPoly_BoolConversionTest, SuccessfulResultIsTrue)
{
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, -5.0, 6.0);
  EXPECT_TRUE(static_cast<bool>(aResult));
}

TEST(MathPoly_BoolConversionTest, NoSolutionResultIsFalse)
{
  MathUtils::PolyResult aResult = MathPoly::Linear(0.0, 5.0);
  EXPECT_FALSE(static_cast<bool>(aResult));
}

// ============================================================================
// Indexing operator tests
// ============================================================================

TEST(MathPoly_IndexingTest, BracketOperator)
{
  MathUtils::PolyResult aResult = MathPoly::Quadratic(1.0, -5.0, 6.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult[0], aResult.Roots[0]);
  EXPECT_EQ(aResult[1], aResult.Roots[1]);
}
