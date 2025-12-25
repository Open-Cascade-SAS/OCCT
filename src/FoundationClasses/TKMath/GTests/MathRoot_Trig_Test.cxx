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

#include <MathRoot_Trig.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOL  = 1.0e-10;
constexpr double THE_PI   = 3.14159265358979323846;
constexpr double THE_2PI  = 2.0 * THE_PI;

//! Helper to verify that a root satisfies the equation
//! a*cos^2(x) + 2*b*cos(x)*sin(x) + c*cos(x) + d*sin(x) + e = 0
double evaluateEquation(double theA, double theB, double theC, double theD, double theE, double theX)
{
  double aCos = std::cos(theX);
  double aSin = std::sin(theX);
  return theA * aCos * aCos + 2.0 * theB * aCos * aSin + theC * aCos + theD * aSin + theE;
}

//! Helper to verify all roots satisfy the equation
void verifyRoots(const MathRoot::TrigResult& theResult,
                 double                      theA,
                 double                      theB,
                 double                      theC,
                 double                      theD,
                 double                      theE,
                 double                      theTol = 1.0e-10)
{
  for (int i = 0; i < theResult.NbRoots; ++i)
  {
    double aVal = evaluateEquation(theA, theB, theC, theD, theE, theResult.Roots[i]);
    EXPECT_NEAR(aVal, 0.0, theTol) << "Root " << i << " = " << theResult.Roots[i]
                                   << " gives f(x) = " << aVal;
  }
}

} // namespace

// ============================================================================
// Basic linear equations: d*sin(x) + e = 0
// ============================================================================

TEST(MathRoot_TrigTest, LinearSin_ZeroConstant)
{
  // sin(x) = 0 => x = 0, PI
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 0.0, 1.0, 0.0);
}

TEST(MathRoot_TrigTest, LinearSin_HalfValue)
{
  // sin(x) = 0.5 => x = PI/6, 5*PI/6
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, -0.5);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 0.0, 1.0, -0.5);
}

TEST(MathRoot_TrigTest, LinearSin_NegativeHalf)
{
  // sin(x) = -0.5 => x = -PI/6 + 2PI, PI + PI/6
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.5);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 0.0, 1.0, 0.5);
}

TEST(MathRoot_TrigTest, LinearSin_NoSolution)
{
  // sin(x) = 2 => no solution
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, -2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathRoot_TrigTest, LinearSin_BoundaryValue)
{
  // sin(x) = 1 => x = PI/2
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 1);
  verifyRoots(aResult, 0.0, 0.0, 0.0, 1.0, -1.0);
}

// ============================================================================
// Basic equations: c*cos(x) + e = 0
// ============================================================================

TEST(MathRoot_TrigTest, LinearCos_ZeroConstant)
{
  // cos(x) = 0 => x = PI/2, 3*PI/2
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 1.0, 0.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 1.0, 0.0, 0.0);
}

TEST(MathRoot_TrigTest, LinearCos_HalfValue)
{
  // cos(x) = 0.5 => x = PI/3, 5*PI/3
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 1.0, 0.0, -0.5);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 1.0, 0.0, -0.5);
}

TEST(MathRoot_TrigTest, LinearCos_NoSolution)
{
  // cos(x) = 2 => no solution
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 1.0, 0.0, -2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

// ============================================================================
// Linear combination: c*cos(x) + d*sin(x) + e = 0
// ============================================================================

TEST(MathRoot_TrigTest, CDE_CosPlusSin)
{
  // cos(x) + sin(x) = 0 => tan(x) = -1 => x = 3*PI/4, 7*PI/4
  MathRoot::TrigResult aResult = MathRoot::TrigonometricCDE(1.0, 1.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 1.0, 1.0, 0.0);
}

TEST(MathRoot_TrigTest, CDE_CosPlusSinEqualsSqrt2)
{
  // cos(x) + sin(x) = sqrt(2) => x = PI/4 (double root)
  // This is a challenging case where F(x) and F'(x) are both zero at the root
  MathRoot::TrigResult aResult = MathRoot::TrigonometricCDE(1.0, 1.0, -std::sqrt(2.0));
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 1);
  // With Halley's method fallback, precision should be much better
  verifyRoots(aResult, 0.0, 0.0, 1.0, 1.0, -std::sqrt(2.0), 1.0e-9);
}

TEST(MathRoot_TrigTest, CDE_CosMinusSin)
{
  // cos(x) - sin(x) = 0 => tan(x) = 1 => x = PI/4, 5*PI/4
  MathRoot::TrigResult aResult = MathRoot::TrigonometricCDE(1.0, -1.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 1.0, -1.0, 0.0);
}

TEST(MathRoot_TrigTest, CDE_Harmonic)
{
  // 3*cos(x) + 4*sin(x) = 5 => one solution at x = atan(4/3) ~= 0.927
  // This is R*cos(x - phi) = 5 where R = 5
  MathRoot::TrigResult aResult = MathRoot::TrigonometricCDE(3.0, 4.0, -5.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 1);
  verifyRoots(aResult, 0.0, 0.0, 3.0, 4.0, -5.0);
}

TEST(MathRoot_TrigTest, CDE_NoSolution)
{
  // cos(x) + sin(x) = 2 => no solution (max value is sqrt(2))
  MathRoot::TrigResult aResult = MathRoot::TrigonometricCDE(1.0, 1.0, -2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

// ============================================================================
// Quadratic in cos: a*cos^2(x) + c*cos(x) + e = 0
// ============================================================================

TEST(MathRoot_TrigTest, QuadraticCos_TwoSolutions)
{
  // cos^2(x) - 1 = 0 => cos(x) = ±1 => x = 0, PI
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, 0.0, 0.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 2);
  verifyRoots(aResult, 1.0, 0.0, 0.0, 0.0, -1.0);
}

TEST(MathRoot_TrigTest, QuadraticCos_FourSolutions)
{
  // cos^2(x) = 0.25 => cos(x) = ±0.5 => 4 solutions
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, 0.0, 0.0, -0.25);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 4);
  verifyRoots(aResult, 1.0, 0.0, 0.0, 0.0, -0.25);
}

// ============================================================================
// Mixed terms: a*cos^2(x) + 2*b*cos(x)*sin(x) = 0
// ============================================================================

TEST(MathRoot_TrigTest, CosSinProduct_AE_Zero)
{
  // 2*cos(x)*sin(x) = 0 => sin(2x) = 0 => x = 0, PI/2, PI, 3*PI/2
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 1.0, 0.0, 0.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 4);
  verifyRoots(aResult, 0.0, 1.0, 0.0, 0.0, 0.0);
}

TEST(MathRoot_TrigTest, Mixed_SinTimesCos)
{
  // cos(x)*sin(x) + sin(x) = sin(x)*(cos(x) + 1) = 0
  // Solutions: sin(x) = 0 => x = 0, PI
  //        or: cos(x) = -1 => x = PI
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.5, 0.0, 1.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.5, 0.0, 1.0, 0.0);
}

// ============================================================================
// General quartic cases
// ============================================================================

TEST(MathRoot_TrigTest, General_FourRoots)
{
  // A general equation with 4 roots
  // cos^2(x) + cos(x) - 0.5 = 0
  // cos(x) = (-1 ± sqrt(3))/2
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, 1.0, 0.0, -0.5);
  ASSERT_TRUE(aResult.IsDone());
  // This should have 2-4 roots
  EXPECT_GE(aResult.NbRoots, 2);
  verifyRoots(aResult, 1.0, 0.0, 1.0, 0.0, -0.5);
}

TEST(MathRoot_TrigTest, General_AllCoefficients)
{
  // Test with all coefficients non-zero
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.5, 0.3, 0.4, -0.2);
  ASSERT_TRUE(aResult.IsDone());
  // Just verify that any roots found satisfy the equation
  verifyRoots(aResult, 1.0, 0.5, 0.3, 0.4, -0.2, 1.0e-8);
}

// ============================================================================
// Bound constraint tests
// ============================================================================

TEST(MathRoot_TrigTest, Bounds_FirstQuadrant)
{
  // sin(x) = 0.5 in [0, PI/2]
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, -0.5, 0.0, THE_PI / 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_GE(aResult.Roots[0], 0.0 - THE_TOL);
  EXPECT_LE(aResult.Roots[0], THE_PI / 2.0 + THE_TOL);
  verifyRoots(aResult, 0.0, 0.0, 0.0, 1.0, -0.5);
}

TEST(MathRoot_TrigTest, Bounds_SecondQuadrant)
{
  // sin(x) = 0.5 in [PI/2, PI]
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, -0.5, THE_PI / 2.0, THE_PI);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_GE(aResult.Roots[0], THE_PI / 2.0 - THE_TOL);
  EXPECT_LE(aResult.Roots[0], THE_PI + THE_TOL);
  verifyRoots(aResult, 0.0, 0.0, 0.0, 1.0, -0.5);
}

TEST(MathRoot_TrigTest, Bounds_NarrowRange)
{
  // cos(x) = 0 in narrow range around PI/2
  MathRoot::TrigResult aResult =
    MathRoot::Trigonometric(0.0, 0.0, 1.0, 0.0, 0.0, THE_PI / 2.0 - 0.1, THE_PI / 2.0 + 0.1);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 1);
  EXPECT_NEAR(aResult.Roots[0], THE_PI / 2.0, THE_TOL);
}

TEST(MathRoot_TrigTest, Bounds_RootOutsideRange)
{
  // sin(x) = 0.5, roots at PI/6 and 5*PI/6
  // Looking in [PI, 2*PI] where these roots don't exist
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, -0.5, THE_PI, THE_2PI);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathRoot_TrigTest, Bounds_FullCircle)
{
  // sin(x) = 0 in [0, 2*PI]
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.0, 0.0, THE_2PI);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 2);
}

TEST(MathRoot_TrigTest, Bounds_MultipleCircles)
{
  // sin(x) = 0 in [0, 4*PI] - but we only look in one period
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.0, 0.0, 4.0 * THE_PI);
  ASSERT_TRUE(aResult.IsDone());
  // Should still only return 2 roots in one period
  EXPECT_EQ(aResult.NbRoots, 2);
}

// ============================================================================
// Degenerate cases
// ============================================================================

TEST(MathRoot_TrigTest, Degenerate_AllZero)
{
  // 0 = 0 => infinite solutions
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_TRUE(aResult.InfiniteRoots);
}

TEST(MathRoot_TrigTest, Degenerate_ConstantNonZero)
{
  // e = 0 where e != 0 => no solutions
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 0.0, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0);
}

TEST(MathRoot_TrigTest, Degenerate_NearZeroCoefficients)
{
  // Very small coefficients
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0e-14, 1.0e-14, 1.0, 0.0, -0.5);
  ASSERT_TRUE(aResult.IsDone());
  // Should behave like cos(x) = 0.5
  EXPECT_EQ(aResult.NbRoots, 2);
  verifyRoots(aResult, 1.0e-14, 1.0e-14, 1.0, 0.0, -0.5, 1.0e-6);
}

// ============================================================================
// Special case: PI as root
// ============================================================================

TEST(MathRoot_TrigTest, SpecialCase_PiRoot)
{
  // cos^2(x) - cos(x) = 0 => cos(x)*(cos(x) - 1) = 0
  // cos(x) = 0 => x = PI/2, 3*PI/2
  // cos(x) = 1 => x = 0
  // Note: A - C + E = 1 - 0 + 0 = 1 != 0, so PI is not a special root here
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, -1.0, 0.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots, 2);
  verifyRoots(aResult, 1.0, 0.0, -1.0, 0.0, 0.0);
}

TEST(MathRoot_TrigTest, SpecialCase_PiRootCheck)
{
  // A - C + E = 0 triggers special PI check
  // cos^2(x) + cos(x) = 0 => cos(x)*(cos(x) + 1) = 0
  // cos(x) = 0 => x = PI/2, 3*PI/2
  // cos(x) = -1 => x = PI
  // Here A=1, C=-1, E=0 => A - C + E = 1 - (-1) + 0 = 2 != 0... not the right case
  // Need A - C + E = 0: try A=1, C=1, E=0
  // cos^2(x) - cos(x) = 0: that's A=1, C=-1, E=0 => 1-(-1)+0=2
  // Let's try: A=1, C=0, E=-1 => 1-0+(-1)=0
  // cos^2(x) - 1 = 0 => cos(x) = ±1 => x = 0, PI
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, 0.0, 0.0, -1.0);
  ASSERT_TRUE(aResult.IsDone());
  // Check if PI is in the roots
  bool aFoundPi = false;
  for (int i = 0; i < aResult.NbRoots; ++i)
  {
    if (std::abs(aResult.Roots[i] - THE_PI) < THE_TOL)
    {
      aFoundPi = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundPi) << "PI should be a root of cos^2(x) - 1 = 0";
  verifyRoots(aResult, 1.0, 0.0, 0.0, 0.0, -1.0);
}

// ============================================================================
// Numerical stability tests
// ============================================================================

TEST(MathRoot_TrigTest, Stability_LargeCoefficients)
{
  // Equation with large coefficients
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1000.0, 500.0, 300.0, 400.0, -200.0);
  ASSERT_TRUE(aResult.IsDone());
  verifyRoots(aResult, 1000.0, 500.0, 300.0, 400.0, -200.0, 1.0e-6);
}

TEST(MathRoot_TrigTest, Stability_SmallCoefficients)
{
  // Equation with small (but not negligible) coefficients
  MathRoot::TrigResult aResult =
    MathRoot::Trigonometric(1.0e-6, 0.5e-6, 0.3e-6, 0.4e-6, -0.2e-6);
  ASSERT_TRUE(aResult.IsDone());
  // Verify roots if any found
  if (aResult.NbRoots > 0)
  {
    verifyRoots(aResult, 1.0e-6, 0.5e-6, 0.3e-6, 0.4e-6, -0.2e-6, 1.0e-4);
  }
}

TEST(MathRoot_TrigTest, Stability_MixedMagnitude)
{
  // Coefficients of very different magnitudes
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, 1.0e6, 0.0, -0.5e6);
  ASSERT_TRUE(aResult.IsDone());
  verifyRoots(aResult, 1.0, 0.0, 1.0e6, 0.0, -0.5e6, 1.0e-4);
}

// ============================================================================
// Root ordering and uniqueness tests
// ============================================================================

TEST(MathRoot_TrigTest, RootOrdering_Sorted)
{
  // sin(x) = 0 => x = 0, PI (should be sorted)
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i < aResult.NbRoots; ++i)
  {
    EXPECT_LE(aResult.Roots[i - 1], aResult.Roots[i])
      << "Roots should be in ascending order";
  }
}

TEST(MathRoot_TrigTest, RootUniqueness_NoDuplicates)
{
  // cos^2(x) = 0.25 => 4 distinct roots
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(1.0, 0.0, 0.0, 0.0, -0.25);
  ASSERT_TRUE(aResult.IsDone());
  for (int i = 0; i < aResult.NbRoots; ++i)
  {
    for (int j = i + 1; j < aResult.NbRoots; ++j)
    {
      EXPECT_GT(std::abs(aResult.Roots[i] - aResult.Roots[j]), 1.0e-10)
        << "Roots " << i << " and " << j << " are duplicates";
    }
  }
}

// ============================================================================
// Wrapper function tests
// ============================================================================

TEST(MathRoot_TrigTest, Wrapper_TrigonometricLinear)
{
  // Test TrigonometricLinear wrapper
  MathRoot::TrigResult aResult1 = MathRoot::TrigonometricLinear(2.0, -1.0);
  MathRoot::TrigResult aResult2 = MathRoot::Trigonometric(0.0, 0.0, 0.0, 2.0, -1.0);

  EXPECT_EQ(aResult1.NbRoots, aResult2.NbRoots);
  for (int i = 0; i < aResult1.NbRoots; ++i)
  {
    EXPECT_NEAR(aResult1.Roots[i], aResult2.Roots[i], THE_TOL);
  }
}

TEST(MathRoot_TrigTest, Wrapper_TrigonometricCDE)
{
  // Test TrigonometricCDE wrapper
  MathRoot::TrigResult aResult1 = MathRoot::TrigonometricCDE(1.0, 2.0, -0.5);
  MathRoot::TrigResult aResult2 = MathRoot::Trigonometric(0.0, 0.0, 1.0, 2.0, -0.5);

  EXPECT_EQ(aResult1.NbRoots, aResult2.NbRoots);
  for (int i = 0; i < aResult1.NbRoots; ++i)
  {
    EXPECT_NEAR(aResult1.Roots[i], aResult2.Roots[i], THE_TOL);
  }
}

// ============================================================================
// Result structure tests
// ============================================================================

TEST(MathRoot_TrigTest, Result_BoolConversion)
{
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.0);
  EXPECT_TRUE(static_cast<bool>(aResult));
  EXPECT_TRUE(aResult.IsDone());
}

TEST(MathRoot_TrigTest, Result_InfiniteRoots)
{
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_TRUE(aResult.InfiniteRoots);
  // IsDone() may or may not be true for infinite roots case
}

// ============================================================================
// Ellipse-related equation tests (from ExtremaPC use case)
// ============================================================================

TEST(MathRoot_TrigTest, Ellipse_PointOnMajorAxis)
{
  // Equation from ellipse extrema: (b²-a²)/2 * 2*cos*sin + a*X*sin = 0
  // For ellipse with a=20, b=10 and point at X=30, Y=0:
  // (100-400)/2 * 2*cos*sin + 20*30*sin = 0
  // -150 * 2*cos*sin + 600*sin = 0
  // sin * (-300*cos + 600) = 0
  // => sin=0 or cos=2 (impossible)
  // Roots at sin=0: x = 0, PI
  double aB2MinusA2Over2 = (100.0 - 400.0) / 2.0; // -150
  double aAX             = 20.0 * 30.0;            // 600
  double aBY             = 0.0;

  MathRoot::TrigResult aResult =
    MathRoot::Trigonometric(0.0, aB2MinusA2Over2, -aBY, aAX, 0.0, 0.0, THE_2PI);
  ASSERT_TRUE(aResult.IsDone());
  // Should find x=0 and x=PI as roots
  EXPECT_GE(aResult.NbRoots, 2);

  // Verify roots
  verifyRoots(aResult, 0.0, aB2MinusA2Over2, -aBY, aAX, 0.0);
}

TEST(MathRoot_TrigTest, Ellipse_GeneralPoint)
{
  // Equation from ellipse extrema with point not on axis
  // For ellipse with a=20, b=10 and point at X=15, Y=8:
  double aB2MinusA2Over2 = (100.0 - 400.0) / 2.0; // -150
  double aAX             = 20.0 * 15.0;            // 300
  double aBY             = 10.0 * 8.0;             // 80

  MathRoot::TrigResult aResult =
    MathRoot::Trigonometric(0.0, aB2MinusA2Over2, -aBY, aAX, 0.0, 0.0, THE_2PI);
  ASSERT_TRUE(aResult.IsDone());
  // Verify any roots found
  verifyRoots(aResult, 0.0, aB2MinusA2Over2, -aBY, aAX, 0.0);
}

// ============================================================================
// Negative parameter range tests
// ============================================================================

TEST(MathRoot_TrigTest, NegativeBounds_Basic)
{
  // sin(x) = 0 in [-PI, 0]
  MathRoot::TrigResult aResult = MathRoot::TrigonometricLinear(1.0, 0.0, -THE_PI, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  // Should find x = -PI and x = 0 (or just one depending on boundary handling)
  EXPECT_GE(aResult.NbRoots, 1);
  for (int i = 0; i < aResult.NbRoots; ++i)
  {
    EXPECT_GE(aResult.Roots[i], -THE_PI - THE_TOL);
    EXPECT_LE(aResult.Roots[i], THE_TOL);
  }
}

TEST(MathRoot_TrigTest, NegativeBounds_CosEquation)
{
  // cos(x) = 0.5 in [-PI, PI]
  MathRoot::TrigResult aResult = MathRoot::Trigonometric(0.0, 0.0, 1.0, 0.0, -0.5, -THE_PI, THE_PI);
  ASSERT_TRUE(aResult.IsDone());
  // cos(x) = 0.5 => x = ±PI/3
  EXPECT_GE(aResult.NbRoots, 2);
  verifyRoots(aResult, 0.0, 0.0, 1.0, 0.0, -0.5);
}
