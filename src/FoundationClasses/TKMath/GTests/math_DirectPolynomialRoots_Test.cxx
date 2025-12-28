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

// Test fixture for math_DirectPolynomialRoots testing
class math_DirectPolynomialRootsTest : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = 1.0e-10; }

  double myTolerance;

  //! Verify polynomial evaluation at root gives near-zero result
  void verifyRoot(double                     theRoot,
                  const std::vector<double>& theCoeffs,
                  int                        theSolutionIndex,
                  double                     theTolerance = 1.0e-10) const
  {
    double aResult = 0.0;
    double aPower  = 1.0;

    // Evaluate polynomial: coefficients are in descending degree order (a0*x^n + a1*x^(n-1) + ... +
    // an*x^0)
    for (int i = static_cast<int>(theCoeffs.size()) - 1; i >= 0; i--)
    {
      aResult += theCoeffs[i] * aPower;
      aPower *= theRoot;
    }

    EXPECT_NEAR(aResult, 0.0, theTolerance)
      << "Root " << theSolutionIndex << " should satisfy polynomial equation";
  }
};

TEST_F(math_DirectPolynomialRootsTest, QuadraticRoots)
{
  // Test quadratic: x^2 - 5x + 6 = 0, roots should be 2 and 3
  math_DirectPolynomialRoots aRoots(1.0, -5.0, 6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Quadratic root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Quadratic should have 2 roots";

  // Natural algorithm order from quadratic solver
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);

  EXPECT_NEAR(aRoot1, 3.0, myTolerance) << "First root should be 3";
  EXPECT_NEAR(aRoot2, 2.0, myTolerance) << "Second root should be 2";

  // Verify roots satisfy the equation x^2 - 5x + 6 = 0
  verifyRoot(aRoot1, {1.0, -5.0, 6.0}, 1);
  verifyRoot(aRoot2, {1.0, -5.0, 6.0}, 2);
}

TEST_F(math_DirectPolynomialRootsTest, QuadraticNoRealRoots)
{
  // Test quadratic: x^2 + x + 1 = 0, no real roots (discriminant < 0)
  math_DirectPolynomialRoots aRoots(1.0, 1.0, 1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should complete even with no real roots";
  EXPECT_EQ(aRoots.NbSolutions(), 0) << "Should have no real roots";
}

TEST_F(math_DirectPolynomialRootsTest, QuadraticDoubleRoot)
{
  // Test quadratic: (x-1)^2 = x^2 - 2x + 1 = 0, double root at x = 1
  math_DirectPolynomialRoots aRoots(1.0, -2.0, 1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Double root case should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 2)
    << "Double root is reported as 2 solutions in the current implementation";

  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  EXPECT_NEAR(aRoot1, 1.0, myTolerance) << "First double root should be 1.0";
  EXPECT_NEAR(aRoot2, 1.0, myTolerance) << "Second double root should be 1.0";

  // Verify roots satisfy the equation x^2 - 2x + 1 = 0
  verifyRoot(aRoot1, {1.0, -2.0, 1.0}, 1);
  verifyRoot(aRoot2, {1.0, -2.0, 1.0}, 2);
}

TEST_F(math_DirectPolynomialRootsTest, CubicRoots)
{
  // Test cubic: (x-1)(x-2)(x-3) = x^3 - 6x^2 + 11x - 6 = 0
  // Roots should be 1, 2, 3
  math_DirectPolynomialRoots aRoots(1.0, -6.0, 11.0, -6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Cubic root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 3) << "Cubic should have 3 real roots";

  // With our root ordering fix, roots should come in descending order: 3, 2, 1
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);

  EXPECT_NEAR(aRoot1, 3.0, myTolerance) << "First root should be 3";
  EXPECT_NEAR(aRoot2, 2.0, myTolerance) << "Second root should be 2";
  EXPECT_NEAR(aRoot3, 1.0, myTolerance) << "Third root should be 1";

  // Verify roots satisfy the equation x^3 - 6x^2 + 11x - 6 = 0
  verifyRoot(aRoot1, {1.0, -6.0, 11.0, -6.0}, 1);
  verifyRoot(aRoot2, {1.0, -6.0, 11.0, -6.0}, 2);
  verifyRoot(aRoot3, {1.0, -6.0, 11.0, -6.0}, 3);
}

TEST_F(math_DirectPolynomialRootsTest, CubicOneRealRoot)
{
  // Test cubic: x^3 + x + 1 = 0, should have one real root
  math_DirectPolynomialRoots aRoots(1.0, 0.0, 1.0, 1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Cubic with one real root should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 1) << "Should have exactly one real root";

  double aRoot = aRoots.Value(1);
  // Verify the root by equation verification x^3 + x + 1 = 0
  verifyRoot(aRoot, {1.0, 0.0, 1.0, 1.0}, 1);
}

TEST_F(math_DirectPolynomialRootsTest, QuarticRoots)
{
  // Test quartic: (x-1)(x-2)(x+1)(x+2) = (x^2-1)(x^2-4) = x^4 - 5x^2 + 4 = 0
  // Roots should be -2, -1, 1, 2
  math_DirectPolynomialRoots aRoots(1.0, 0.0, -5.0, 0.0, 4.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Quartic root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Quartic should have 4 real roots";

  // Natural algorithm order from Ferrari's method: -2, -1, 2, 1
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);
  double aRoot4 = aRoots.Value(4);

  EXPECT_NEAR(aRoot1, -2.0, myTolerance) << "First root should be -2";
  EXPECT_NEAR(aRoot2, -1.0, myTolerance) << "Second root should be -1";
  EXPECT_NEAR(aRoot3, 2.0, myTolerance) << "Third root should be 2";
  EXPECT_NEAR(aRoot4, 1.0, myTolerance) << "Fourth root should be 1";

  // Verify roots satisfy the equation x^4 - 5x^2 + 4 = 0
  std::vector<double> aCoeffs = {1.0, 0.0, -5.0, 0.0, 4.0};
  verifyRoot(aRoot1, aCoeffs, 1);
  verifyRoot(aRoot2, aCoeffs, 2);
  verifyRoot(aRoot3, aCoeffs, 3);
  verifyRoot(aRoot4, aCoeffs, 4);
}

TEST_F(math_DirectPolynomialRootsTest, LinearCase)
{
  // Test linear: 2x - 6 = 0, root should be x = 3
  math_DirectPolynomialRoots aRoots(2.0, -6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Linear root finding should succeed";
  EXPECT_EQ(aRoots.NbSolutions(), 1) << "Linear should have 1 root";

  double aRoot = aRoots.Value(1);
  EXPECT_NEAR(aRoot, 3.0, myTolerance) << "Linear root value";

  // Verify root satisfies the equation 2x - 6 = 0
  verifyRoot(aRoot, {2.0, -6.0}, 1);
}

TEST_F(math_DirectPolynomialRootsTest, DegenerateLinearCase)
{
  // Test degenerate linear: 0x + 5 = 0 (no solution)
  math_DirectPolynomialRoots aRoots(0.0, 5.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Degenerate linear case should complete";
  EXPECT_EQ(aRoots.NbSolutions(), 0) << "0x + 5 = 0 should have no solutions";
}

TEST_F(math_DirectPolynomialRootsTest, PolynomialEvaluation)
{
  // Test root verification by polynomial evaluation
  math_DirectPolynomialRoots aRoots(1.0, -3.0, 2.0); // x^2 - 3x + 2 = 0, roots: 1, 2

  EXPECT_TRUE(aRoots.IsDone()) << "Should find roots successfully";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Should have 2 roots";

  // Verify both roots satisfy the equation x^2 - 3x + 2 = 0 using helper method
  for (int i = 1; i <= aRoots.NbSolutions(); i++)
  {
    double aRoot = aRoots.Value(i);
    verifyRoot(aRoot, {1.0, -3.0, 2.0}, i);
  }
}

TEST_F(math_DirectPolynomialRootsTest, NearZeroCoefficients)
{
  // Test with very small leading coefficient (effectively lower degree)
  math_DirectPolynomialRoots aRoots(1.0e-15, 1.0, -2.0); // Effectively linear: x - 2 = 0

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle near-zero leading coefficient";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Should find 2 roots for this quadratic";

  // With very small leading coefficient, we get one small positive root near 2 and one very large
  // negative root
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);

  // One root should be near 2 (the meaningful solution from linear approximation)
  bool aFoundNearTwo = (std::abs(aRoot1 - 2.0) < 1.0e-6) || (std::abs(aRoot2 - 2.0) < 1.0e-6);
  EXPECT_TRUE(aFoundNearTwo) << "Should find one root near x = 2";
}

TEST_F(math_DirectPolynomialRootsTest, BiQuadraticPolynomial)
{
  // Test biquadratic: x^4 - 10x^2 + 9 = (x^2-1)(x^2-9) = 0
  // Roots: -3, -1, 1, 3
  math_DirectPolynomialRoots aRoots(1.0, 0.0, -10.0, 0.0, 9.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should solve biquadratic polynomial";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find 4 real roots";

  // Natural algorithm order for biquadratic: -3, -1, 3, 1
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);
  double aRoot4 = aRoots.Value(4);

  EXPECT_NEAR(aRoot1, -3.0, 1.0e-8) << "First root should be -3";
  EXPECT_NEAR(aRoot2, -1.0, 1.0e-8) << "Second root should be -1";
  EXPECT_NEAR(aRoot3, 3.0, 1.0e-8) << "Third root should be 3";
  EXPECT_NEAR(aRoot4, 1.0, 1.0e-8) << "Fourth root should be 1";

  // Verify roots satisfy the equation x^4 - 10x^2 + 9 = 0
  std::vector<double> aCoeffs = {1.0, 0.0, -10.0, 0.0, 9.0};
  verifyRoot(aRoot1, aCoeffs, 1, 1.0e-8);
  verifyRoot(aRoot2, aCoeffs, 2, 1.0e-8);
  verifyRoot(aRoot3, aCoeffs, 3, 1.0e-8);
  verifyRoot(aRoot4, aCoeffs, 4, 1.0e-8);
}

TEST_F(math_DirectPolynomialRootsTest, RepeatedRoots)
{
  // Test polynomial with repeated roots: (x-1)^3 = x^3 - 3x^2 + 3x - 1 = 0
  math_DirectPolynomialRoots aRoots(1.0, -3.0, 3.0, -1.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle repeated roots";
  EXPECT_EQ(aRoots.NbSolutions(), 3)
    << "Triple root is reported as 3 solutions in the current implementation";

  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);
  EXPECT_NEAR(aRoot1, 1.0, 1.0e-8) << "First triple root should be 1.0";
  EXPECT_NEAR(aRoot2, 1.0, 1.0e-8) << "Second triple root should be 1.0";
  EXPECT_NEAR(aRoot3, 1.0, 1.0e-8) << "Third triple root should be 1.0";

  // Verify roots satisfy the equation x^3 - 3x^2 + 3x - 1 = 0
  verifyRoot(aRoot1, {1.0, -3.0, 3.0, -1.0}, 1, 1.0e-8);
  verifyRoot(aRoot2, {1.0, -3.0, 3.0, -1.0}, 2, 1.0e-8);
  verifyRoot(aRoot3, {1.0, -3.0, 3.0, -1.0}, 3, 1.0e-8);
}

// ====================================================================================================
// Tests for the modernized C++17 implementation with aggressive Newton-Raphson refinement
// ====================================================================================================

TEST_F(math_DirectPolynomialRootsTest, ProblematicQuarticCaseDetailed)
{
  // The specific problematic case that motivated the implementation rework
  // Coefficients that caused issues with previous implementation
  const double a = 1.0000000000000004;
  const double b = -2.2737367544323211e-13;
  const double c = -17361733.368892364;
  const double d = 28.998342463569099;
  const double e = 75357446168894.516;

  // Expected roots from external solver verification (in descending order)
  // These values are used for verification in the test below

  math_DirectPolynomialRoots aRoots(a, b, c, d, e);

  EXPECT_TRUE(aRoots.IsDone()) << "Problematic quartic should be solved successfully";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find all 4 real roots";
  EXPECT_FALSE(aRoots.InfiniteRoots()) << "Should not report infinite roots";

  // Collect actual roots for analysis
  double actualRoots[4];
  for (int i = 1; i <= 4; ++i)
  {
    actualRoots[i - 1] = aRoots.Value(i);
  }

  // Current implementation order for problematic quartic case
  EXPECT_NEAR(actualRoots[0], -2946.425490, 1.0e-3) << "First root";
  EXPECT_NEAR(actualRoots[1], -2946.236617, 1.0e-3) << "Second root";
  EXPECT_NEAR(actualRoots[2], 2946.394276, 1.0e-3) << "Third root";
  EXPECT_NEAR(actualRoots[3], 2946.267830, 1.0e-3) << "Fourth root";

  // Verify residuals are extremely small (main goal of the rework)
  for (int i = 1; i <= 4; ++i)
  {
    const double root = aRoots.Value(i);
    const double residual =
      a * root * root * root * root + b * root * root * root + c * root * root + d * root + e;

    EXPECT_LT(std::abs(residual), 1.0)
      << "Root " << i << " residual should be much smaller than previous ~85000";
  }
}

TEST_F(math_DirectPolynomialRootsTest, ModernImplementationBiquadraticDetection)
{
  // Test case that should be detected as biquadratic (q ~= 0 after Ferrari transformation)
  // x^4 - 5x^2 + 4 = 0, which factors as (x^2-1)(x^2-4) = 0
  // Roots should be: +/-1, +/-2
  math_DirectPolynomialRoots aRoots(1.0, 0.0, -5.0, 0.0, 4.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Biquadratic should be solved";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find 4 real roots";

  // Natural algorithm order for biquadratic: -2, -1, 2, 1
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);
  double aRoot4 = aRoots.Value(4);

  EXPECT_NEAR(aRoot1, -2.0, 1.0e-12) << "First root should be -2";
  EXPECT_NEAR(aRoot2, -1.0, 1.0e-12) << "Second root should be -1";
  EXPECT_NEAR(aRoot3, 2.0, 1.0e-12) << "Third root should be 2";
  EXPECT_NEAR(aRoot4, 1.0, 1.0e-12) << "Fourth root should be 1";
}

TEST_F(math_DirectPolynomialRootsTest, FullFerrariMethodRequired)
{
  // Test case that requires full Ferrari method (not biquadratic)
  // x^4 + x^3 - 4x^2 - 4x = 0, which factors as x(x+2)(x^2-x-2) = x(x+2)(x-2)(x+1)
  // Roots should be: 0, -2, 2, -1
  math_DirectPolynomialRoots aRoots(1.0, 1.0, -4.0, -4.0, 0.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Full Ferrari method should work";
  EXPECT_EQ(aRoots.NbSolutions(), 4) << "Should find 4 real roots";

  // Current implementation order for Ferrari method: -2, -1, 2, 0
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);
  double aRoot4 = aRoots.Value(4);

  EXPECT_NEAR(aRoot1, -2.0, 1.0e-10) << "First root should be -2";
  EXPECT_NEAR(aRoot2, -1.0, 1.0e-10) << "Second root should be -1";
  EXPECT_NEAR(aRoot3, 2.0, 1.0e-10) << "Third root should be 2";
  EXPECT_NEAR(aRoot4, 0.0, 1.0e-10) << "Fourth root should be 0";
}

TEST_F(math_DirectPolynomialRootsTest, AggressiveRefinementEffectiveness)
{
  // Test that aggressive Newton-Raphson refinement provides excellent residuals
  // Using a case known to benefit from refinement
  const double a = 1.0;
  const double b = 0.0;
  const double c = -1000000.0; // Large coefficient to test numerical stability
  const double d = 0.0;
  const double e = 999999.0; // Nearly equal magnitude

  math_DirectPolynomialRoots aRoots(a, b, c, d, e);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle large coefficients";

  // Check that all residuals are very small
  for (int i = 1; i <= aRoots.NbSolutions(); ++i)
  {
    const double root = aRoots.Value(i);
    const double residual =
      a * root * root * root * root + b * root * root * root + c * root * root + d * root + e;

    EXPECT_LT(std::abs(residual), 1.0e-3)
      << "Aggressive refinement should achieve good residuals even with large coefficients";
  }
}

TEST_F(math_DirectPolynomialRootsTest, CubicWithAggressiveRefinement)
{
  // Test cubic case with aggressive refinement
  // x^3 - 6x^2 + 11x - 6 = 0, which factors as (x-1)(x-2)(x-3)
  // Roots should be: 1, 2, 3
  math_DirectPolynomialRoots aRoots(1.0, -6.0, 11.0, -6.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Cubic with aggressive refinement should work";
  EXPECT_EQ(aRoots.NbSolutions(), 3) << "Should find 3 real roots";

  // With deterministic ordering, roots come in descending order: 3, 2, 1
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);
  double aRoot3 = aRoots.Value(3);

  EXPECT_NEAR(aRoot1, 3.0, 1.0e-12) << "First root should be 3";
  EXPECT_NEAR(aRoot2, 2.0, 1.0e-12) << "Second root should be 2";
  EXPECT_NEAR(aRoot3, 1.0, 1.0e-12) << "Third root should be 1";

  // Verify excellent residuals
  for (int i = 1; i <= 3; ++i)
  {
    const double root     = aRoots.Value(i);
    const double residual = root * root * root - 6.0 * root * root + 11.0 * root - 6.0;
    EXPECT_LT(std::abs(residual), 1.0e-12) << "Cubic residual should be excellent";
  }
}

TEST_F(math_DirectPolynomialRootsTest, ExtremeCoefficientsStability)
{
  // Test numerical stability with extreme coefficient ranges
  // Similar to the problematic case but with even more extreme values
  const double a = 1.0;
  const double b = 1.0e-15; // Very small
  const double c = -1.0e8;  // Large negative
  const double d = 1.0e-10; // Very small positive
  const double e = 1.0e15;  // Very large positive

  math_DirectPolynomialRoots aRoots(a, b, c, d, e);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle extreme coefficient ranges";

  // At minimum, verify that residuals are reasonable (not astronomical)
  for (int i = 1; i <= aRoots.NbSolutions(); ++i)
  {
    const double root = aRoots.Value(i);
    const double residual =
      a * root * root * root * root + b * root * root * root + c * root * root + d * root + e;

    // With extreme coefficients, we expect residuals to be much better than old implementation
    EXPECT_LT(std::abs(residual), 1.0e6)
      << "Even with extreme coefficients, residuals should be controlled";
  }
}

TEST_F(math_DirectPolynomialRootsTest, CloseToZeroCoefficients)
{
  // Test robustness when coefficients are close to zero (degree reduction)
  // x^4 + 0*x^3 + 0*x^2 + 0*x - 16 = 0, essentially x^4 = 16
  // Roots should be +/-2, +/-2i (but we only get real roots: +/-2)
  const double               epsilon = 1.0e-14;
  math_DirectPolynomialRoots aRoots(1.0, epsilon, epsilon, epsilon, -16.0);

  EXPECT_TRUE(aRoots.IsDone()) << "Should handle near-zero coefficients";
  EXPECT_EQ(aRoots.NbSolutions(), 2) << "Should find 2 real roots";

  // Natural algorithm order for near-zero coefficients: 2, -2
  double aRoot1 = aRoots.Value(1);
  double aRoot2 = aRoots.Value(2);

  EXPECT_NEAR(aRoot1, 2.0, 1.0e-10) << "First root should be 2";
  EXPECT_NEAR(aRoot2, -2.0, 1.0e-10) << "Second root should be -2";
}
