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

#include <math_DirectPolynomialRoots.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Precision.hxx>

#include <cmath>
#include <algorithm>

namespace
{

TEST(MathDirectPolynomialRootsTest, QuadraticRoots)
{
  // Test quadratic: x^2 - 5x + 6 = 0, roots should be 2 and 3
  math_DirectPolynomialRoots aRoots(1.0, -5.0, 6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Quadratic root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Quadratic should have 2 roots";

  Standard_Real aRoot1 = aRoots.Value(1);
  Standard_Real aRoot2 = aRoots.Value(2);

  // Sort roots for comparison
  if (aRoot1 > aRoot2)
  {
    std::swap(aRoot1, aRoot2);
  }

  EXPECT_NEAR(aRoot1, 2.0, 1.0e-10) << "First quadratic root";
  EXPECT_NEAR(aRoot2, 3.0, 1.0e-10) << "Second quadratic root";
}

TEST(MathDirectPolynomialRootsTest, QuadraticNoRealRoots)
{
  // Test quadratic: x^2 + x + 1 = 0, no real roots (discriminant < 0)
  math_DirectPolynomialRoots aRoots(1.0, 1.0, 1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should complete even with no real roots";
  EXPECT_EQ(aRoots.NbSolutions(), 0) << "Should have no real roots";
}

TEST(MathDirectPolynomialRootsTest, QuadraticDoubleRoot)
{
  // Test quadratic: (x-1)^2 = x^2 - 2x + 1 = 0, double root at x = 1
  math_DirectPolynomialRoots aRoots(1.0, -2.0, 1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Double root case should succeed";
  // Implementation may report double root as 1 or 2 solutions
  EXPECT_GE(aRoots.NbSolutions(), 1) << "Should have at least one solution";
  EXPECT_LE(aRoots.NbSolutions(), 2) << "Should have at most two solutions";

  // All reported roots should be 1.0
  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
  {
    EXPECT_NEAR(aRoots.Value(i), 1.0, 1.0e-10) << "Double root value " << i;
  }
}

TEST(MathDirectPolynomialRootsTest, CubicRoots)
{
  // Test cubic: (x-1)(x-2)(x-3) = x^3 - 6x^2 + 11x - 6 = 0
  // Roots should be 1, 2, 3
  math_DirectPolynomialRoots aRoots(1.0, -6.0, 11.0, -6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Cubic root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 3) << "Cubic should have 3 real roots";

  // Collect and sort roots
  std::vector<Standard_Real> aFoundRoots;
  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
  {
    aFoundRoots.push_back(aRoots.Value(i));
  }
  std::sort(aFoundRoots.begin(), aFoundRoots.end());

  EXPECT_NEAR(aFoundRoots[0], 1.0, 1.0e-10) << "First cubic root";
  EXPECT_NEAR(aFoundRoots[1], 2.0, 1.0e-10) << "Second cubic root";
  EXPECT_NEAR(aFoundRoots[2], 3.0, 1.0e-10) << "Third cubic root";
}

TEST(MathDirectPolynomialRootsTest, CubicOneRealRoot)
{
  // Test cubic: x^3 + x + 1 = 0, should have one real root
  math_DirectPolynomialRoots aRoots(1.0, 0.0, 1.0, 1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Cubic with one real root should succeed";
  EXPECT_GE(aRoots.NbSolutions(), 1) << "Should have at least one real root";

  // Verify the root by substitution
  Standard_Real aRoot  = aRoots.Value(1);
  Standard_Real aValue = aRoot * aRoot * aRoot + aRoot + 1.0;
  EXPECT_NEAR(aValue, 0.0, 1.0e-10) << "Root should satisfy the equation";
}

TEST(MathDirectPolynomialRootsTest, QuarticRoots)
{
  // Test quartic: (x-1)(x-2)(x+1)(x+2) = (x^2-1)(x^2-4) = x^4 - 5x^2 + 4 = 0
  // Roots should be -2, -1, 1, 2
  math_DirectPolynomialRoots aRoots(1.0, 0.0, -5.0, 0.0, 4.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Quartic root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Quartic should have 4 real roots";

  // Collect and sort roots
  std::vector<Standard_Real> aFoundRoots;
  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
  {
    aFoundRoots.push_back(aRoots.Value(i));
  }
  std::sort(aFoundRoots.begin(), aFoundRoots.end());

  EXPECT_NEAR(aFoundRoots[0], -2.0, 1.0e-10) << "First quartic root";
  EXPECT_NEAR(aFoundRoots[1], -1.0, 1.0e-10) << "Second quartic root";
  EXPECT_NEAR(aFoundRoots[2], 1.0, 1.0e-10) << "Third quartic root";
  EXPECT_NEAR(aFoundRoots[3], 2.0, 1.0e-10) << "Fourth quartic root";
}

TEST(MathDirectPolynomialRootsTest, LinearCase)
{
  // Test linear: 2x - 6 = 0, root should be x = 3
  math_DirectPolynomialRoots aRoots(2.0, -6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Linear root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 1) << "Linear should have 1 root";

  EXPECT_NEAR(aRoots.Value(1), 3.0, 1.0e-10) << "Linear root value";
}

TEST(MathDirectPolynomialRootsTest, DegenerateLinearCase)
{
  // Test degenerate linear: 0x + 5 = 0 (no solution)
  math_DirectPolynomialRoots aRoots(0.0, 5.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Degenerate linear case should complete";
  EXPECT_EQ(aRoots.NbSolutions(), 0) << "0x + 5 = 0 should have no solutions";
}

TEST(MathDirectPolynomialRootsTest, PolynomialEvaluation)
{
  // Test root verification by polynomial evaluation
  math_DirectPolynomialRoots aRoots(1.0, -3.0, 2.0); // x^2 - 3x + 2 = 0, roots: 1, 2

  EXPECT_TRUE(aRoots.IsDone()) << "Should find roots successfully";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Should have 2 roots";

  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
  {
    Standard_Real aRoot  = aRoots.Value(i);
    Standard_Real aValue = aRoot * aRoot - 3.0 * aRoot + 2.0;
    EXPECT_NEAR(aValue, 0.0, 1.0e-10) << "Root " << i << " should satisfy equation";
  }
}

TEST(MathDirectPolynomialRootsTest, NearZeroCoefficients)
{
  // Test with very small leading coefficient (effectively lower degree)
  math_DirectPolynomialRoots aRoots(1.0e-15, 1.0, -2.0); // Effectively linear: x - 2 = 0

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle near-zero leading coefficient";

  if (aRoots.NbSolutions() > 0)
  {
    // Should find root near x = 2
    bool aFoundNearTwo = false;
    for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
    {
      if (std::abs(aRoots.Value(i) - 2.0) < 1.0e-6)
      {
        aFoundNearTwo = true;
        break;
      }
    }
    EXPECT_TRUE(aFoundNearTwo) << "Should find root near x = 2";
  }
}

TEST(MathDirectPolynomialRootsTest, BiQuadraticPolynomial)
{
  // Test biquadratic: x^4 - 10x^2 + 9 = (x^2-1)(x^2-9) = 0
  // Roots: -3, -1, 1, 3
  math_DirectPolynomialRoots aRoots(1.0, 0.0, -10.0, 0.0, 9.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should solve biquadratic polynomial";

  if (aRoots.NbSolutions() == 4)
  {
    std::vector<Standard_Real> aFoundRoots;
    for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
    {
      aFoundRoots.push_back(aRoots.Value(i));
    }
    std::sort(aFoundRoots.begin(), aFoundRoots.end());

    EXPECT_NEAR(aFoundRoots[0], -3.0, 1.0e-8) << "Root -3";
    EXPECT_NEAR(aFoundRoots[1], -1.0, 1.0e-8) << "Root -1";
    EXPECT_NEAR(aFoundRoots[2], 1.0, 1.0e-8) << "Root 1";
    EXPECT_NEAR(aFoundRoots[3], 3.0, 1.0e-8) << "Root 3";
  }
}

TEST(MathDirectPolynomialRootsTest, RepeatedRoots)
{
  // Test polynomial with repeated roots: (x-1)^3 = x^3 - 3x^2 + 3x - 1 = 0
  math_DirectPolynomialRoots aRoots(1.0, -3.0, 3.0, -1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle repeated roots";

  // Implementation may report repeated roots differently
  EXPECT_GE(aRoots.NbSolutions(), 1) << "Should find at least one root";

  // All reported roots should be near 1
  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); i++)
  {
    EXPECT_NEAR(aRoots.Value(i), 1.0, 1.0e-8) << "Repeated root should be near 1";
  }
}

// ====================================================================================================
// Tests for the modernized C++17 implementation with aggressive Newton-Raphson refinement
// ====================================================================================================

TEST(MathDirectPolynomialRootsTest, ProblematicQuarticCaseDetailed)
{
  // The specific problematic case that motivated the implementation rework
  // Coefficients that caused issues with previous implementation
  const Standard_Real a = 1.0000000000000004;
  const Standard_Real b = -2.2737367544323211e-13;
  const Standard_Real c = -17361733.368892364;
  const Standard_Real d = 28.998342463569099;
  const Standard_Real e = 75357446168894.516;

  // Expected roots from external solver verification
  const Standard_Real expectedRoots[4] = {2946.394276649265,
                                          2946.267830747863,
                                          -2946.236617290974,
                                          -2946.425490106154};

  math_DirectPolynomialRoots aRoots(a, b, c, d, e);

  EXPECT_TRUE(aRoots.IsDone()) << "Problematic quartic should be solved successfully";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find all 4 real roots";
  EXPECT_FALSE(aRoots.InfiniteRoots()) << "Should not report infinite roots";

  // Collect actual roots for analysis
  Standard_Real actualRoots[4];
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    actualRoots[i - 1] = aRoots.Value(i);
  }

  // Sort both arrays for comparison
  std::sort(actualRoots, actualRoots + 4);
  Standard_Real sortedExpected[4];
  std::copy(expectedRoots, expectedRoots + 4, sortedExpected);
  std::sort(sortedExpected, sortedExpected + 4);

  // Verify each root with high precision requirement
  for (Standard_Integer i = 0; i < 4; ++i)
  {
    EXPECT_NEAR(actualRoots[i], sortedExpected[i], 1.0e-6)
      << "Root " << (i + 1) << " should match expected value with high precision";
  }

  // Verify residuals are extremely small (main goal of the rework)
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    const Standard_Real root = aRoots.Value(i);
    const Standard_Real residual =
      a * root * root * root * root + b * root * root * root + c * root * root + d * root + e;

    EXPECT_LT(std::abs(residual), 1.0)
      << "Root " << i << " residual should be much smaller than previous ~85000";
  }
}

TEST(MathDirectPolynomialRootsTest, ModernImplementationBiquadraticDetection)
{
  // Test case that should be detected as biquadratic (q ≈ 0 after Ferrari transformation)
  // x^4 - 5x^2 + 4 = 0, which factors as (x^2-1)(x^2-4) = 0
  // Roots should be: ±1, ±2
  math_DirectPolynomialRoots aRoots(1.0, 0.0, -5.0, 0.0, 4.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Biquadratic should be solved";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find 4 real roots";

  // Collect and sort roots
  Standard_Real roots[4];
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    roots[i - 1] = aRoots.Value(i);
  }
  std::sort(roots, roots + 4);

  // Expected: -2, -1, 1, 2
  EXPECT_NEAR(roots[0], -2.0, 1.0e-12) << "First root";
  EXPECT_NEAR(roots[1], -1.0, 1.0e-12) << "Second root";
  EXPECT_NEAR(roots[2], 1.0, 1.0e-12) << "Third root";
  EXPECT_NEAR(roots[3], 2.0, 1.0e-12) << "Fourth root";
}

TEST(MathDirectPolynomialRootsTest, FullFerrariMethodRequired)
{
  // Test case that requires full Ferrari method (not biquadratic)
  // x^4 + x^3 - 4x^2 - 4x = 0, which factors as x(x+2)(x^2-x-2) = x(x+2)(x-2)(x+1)
  // Roots should be: 0, -2, 2, -1
  math_DirectPolynomialRoots aRoots(1.0, 1.0, -4.0, -4.0, 0.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Full Ferrari method should work";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find 4 real roots";

  // Collect and sort roots
  Standard_Real roots[4];
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    roots[i - 1] = aRoots.Value(i);
  }
  std::sort(roots, roots + 4);

  // Expected: -2, -1, 0, 2
  EXPECT_NEAR(roots[0], -2.0, 1.0e-10) << "First root";
  EXPECT_NEAR(roots[1], -1.0, 1.0e-10) << "Second root";
  EXPECT_NEAR(roots[2], 0.0, 1.0e-10) << "Third root";
  EXPECT_NEAR(roots[3], 2.0, 1.0e-10) << "Fourth root";
}

TEST(MathDirectPolynomialRootsTest, AggressiveRefinementEffectiveness)
{
  // Test that aggressive Newton-Raphson refinement provides excellent residuals
  // Using a case known to benefit from refinement
  const Standard_Real a = 1.0;
  const Standard_Real b = 0.0;
  const Standard_Real c = -1000000.0; // Large coefficient to test numerical stability
  const Standard_Real d = 0.0;
  const Standard_Real e = 999999.0; // Nearly equal magnitude

  math_DirectPolynomialRoots aRoots(a, b, c, d, e);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle large coefficients";

  // Check that all residuals are very small
  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); ++i)
  {
    const Standard_Real root = aRoots.Value(i);
    const Standard_Real residual =
      a * root * root * root * root + b * root * root * root + c * root * root + d * root + e;

    EXPECT_LT(std::abs(residual), 1.0e-3)
      << "Aggressive refinement should achieve good residuals even with large coefficients";
  }
}

TEST(MathDirectPolynomialRootsTest, CubicWithAggressiveRefinement)
{
  // Test cubic case with aggressive refinement
  // x^3 - 6x^2 + 11x - 6 = 0, which factors as (x-1)(x-2)(x-3)
  // Roots should be: 1, 2, 3
  math_DirectPolynomialRoots aRoots(1.0, -6.0, 11.0, -6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Cubic with aggressive refinement should work";
  EXPECT_EQ(aRoots.NbSolutions(), 3) << "Should find 3 real roots";

  // Collect and sort roots
  Standard_Real roots[3];
  for (Standard_Integer i = 1; i <= 3; ++i)
  {
    roots[i - 1] = aRoots.Value(i);
  }
  std::sort(roots, roots + 3);

  EXPECT_NEAR(roots[0], 1.0, 1.0e-12) << "First cubic root";
  EXPECT_NEAR(roots[1], 2.0, 1.0e-12) << "Second cubic root";
  EXPECT_NEAR(roots[2], 3.0, 1.0e-12) << "Third cubic root";

  // Verify excellent residuals
  for (Standard_Integer i = 1; i <= 3; ++i)
  {
    const Standard_Real root     = aRoots.Value(i);
    const Standard_Real residual = root * root * root - 6.0 * root * root + 11.0 * root - 6.0;
    EXPECT_LT(std::abs(residual), 1.0e-12) << "Cubic residual should be excellent";
  }
}

TEST(MathDirectPolynomialRootsTest, ExtremeCoefficientsStability)
{
  // Test numerical stability with extreme coefficient ranges
  // Similar to the problematic case but with even more extreme values
  const Standard_Real a = 1.0;
  const Standard_Real b = 1.0e-15; // Very small
  const Standard_Real c = -1.0e8;  // Large negative
  const Standard_Real d = 1.0e-10; // Very small positive
  const Standard_Real e = 1.0e15;  // Very large positive

  math_DirectPolynomialRoots aRoots(a, b, c, d, e);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle extreme coefficient ranges";

  // At minimum, verify that residuals are reasonable (not astronomical)
  for (Standard_Integer i = 1; i <= aRoots.NbSolutions(); ++i)
  {
    const Standard_Real root = aRoots.Value(i);
    const Standard_Real residual =
      a * root * root * root * root + b * root * root * root + c * root * root + d * root + e;

    // With extreme coefficients, we expect residuals to be much better than old implementation
    EXPECT_LT(std::abs(residual), 1.0e6)
      << "Even with extreme coefficients, residuals should be controlled";
  }
}

TEST(MathDirectPolynomialRootsTest, CloseToZeroCoefficients)
{
  // Test robustness when coefficients are close to zero (degree reduction)
  // x^4 + 0*x^3 + 0*x^2 + 0*x - 16 = 0, essentially x^4 = 16
  // Roots should be ±2, ±2i (but we only get real roots: ±2)
  const Standard_Real        epsilon = 1.0e-14;
  math_DirectPolynomialRoots aRoots(1.0, epsilon, epsilon, epsilon, -16.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle near-zero coefficients";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Should find 2 real roots";

  // Collect roots
  Standard_Real root1 = aRoots.Value(1);
  Standard_Real root2 = aRoots.Value(2);
  if (root1 > root2)
    std::swap(root1, root2);

  EXPECT_NEAR(root1, -2.0, 1.0e-10) << "Negative root";
  EXPECT_NEAR(root2, 2.0, 1.0e-10) << "Positive root";
}

} // anonymous namespace