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

// New API
#include <MathPoly_Quadratic.hxx>
#include <MathPoly_Cubic.hxx>
#include <MathPoly_Quartic.hxx>

// Old API
#include <math_DirectPolynomialRoots.hxx>

#include <algorithm>
#include <cmath>
#include <vector>

namespace
{
  constexpr double THE_TOLERANCE = 1.0e-9;

  //! Helper to sort roots for comparison.
  std::vector<double> SortRoots(const std::vector<double>& theRoots)
  {
    std::vector<double> aSorted = theRoots;
    std::sort(aSorted.begin(), aSorted.end());
    return aSorted;
  }

  //! Extract roots from old API result.
  std::vector<double> GetOldRoots(const math_DirectPolynomialRoots& theSolver)
  {
    std::vector<double> aRoots;
    if (theSolver.IsDone())
    {
      for (int i = 1; i <= theSolver.NbSolutions(); ++i)
      {
        aRoots.push_back(theSolver.Value(i));
      }
    }
    return SortRoots(aRoots);
  }

  //! Extract roots from new API result.
  std::vector<double> GetNewRoots(const MathUtils::PolyResult& theResult)
  {
    std::vector<double> aRoots;
    if (theResult.IsDone())
    {
      for (size_t i = 0; i < theResult.NbRoots; ++i)
      {
        aRoots.push_back(theResult.Roots[i]);
      }
    }
    return SortRoots(aRoots);
  }

  //! Compare two root sets within tolerance.
  void CompareRoots(const std::vector<double>& theOld,
                    const std::vector<double>& theNew,
                    double                     theTolerance = THE_TOLERANCE)
  {
    ASSERT_EQ(theOld.size(), theNew.size()) << "Different number of roots";
    for (size_t i = 0; i < theOld.size(); ++i)
    {
      EXPECT_NEAR(theOld[i], theNew[i], theTolerance)
          << "Root " << i << " differs: old=" << theOld[i] << " new=" << theNew[i];
    }
  }
}

// ============================================================================
// Quadratic equation comparison tests
// ============================================================================

TEST(MathPoly_ComparisonTest, Quadratic_TwoDistinctRoots)
{
  // x^2 - 5x + 6 = 0 -> roots at 2 and 3
  const double aA = 1.0, aB = -5.0, aC = 6.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

TEST(MathPoly_ComparisonTest, Quadratic_DoubleRoot)
{
  // x^2 - 4x + 4 = 0 -> double root at 2
  const double aA = 1.0, aB = -4.0, aC = 4.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Note: Old API may return double root twice, new API returns it once
  // Both should find roots at 2.0
  auto aOldRoots = GetOldRoots(anOldSolver);
  auto aNewRoots = GetNewRoots(aNewResult);

  for (double aRoot : aOldRoots)
  {
    EXPECT_NEAR(aRoot, 2.0, THE_TOLERANCE);
  }
  for (double aRoot : aNewRoots)
  {
    EXPECT_NEAR(aRoot, 2.0, THE_TOLERANCE);
  }
}

TEST(MathPoly_ComparisonTest, Quadratic_NoRealRoots)
{
  // x^2 + 1 = 0 -> no real roots
  const double aA = 1.0, aB = 0.0, aC = 1.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  // Both should report no real solutions
  EXPECT_EQ(anOldSolver.NbSolutions(), 0);
  EXPECT_EQ(aNewResult.NbRoots, 0);
}

TEST(MathPoly_ComparisonTest, Quadratic_NegativeRoots)
{
  // x^2 + 5x + 6 = 0 -> roots at -2 and -3
  const double aA = 1.0, aB = 5.0, aC = 6.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

TEST(MathPoly_ComparisonTest, Quadratic_LargeCoefficients)
{
  // 1000x^2 - 3000x + 2000 = 0 -> roots at 1 and 2
  const double aA = 1000.0, aB = -3000.0, aC = 2000.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

// ============================================================================
// Cubic equation comparison tests
// ============================================================================

TEST(MathPoly_ComparisonTest, Cubic_ThreeDistinctRoots)
{
  // x^3 - 6x^2 + 11x - 6 = 0 -> roots at 1, 2, 3
  const double aA = 1.0, aB = -6.0, aC = 11.0, aD = -6.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD);
  MathUtils::PolyResult            aNewResult = MathPoly::Cubic(aA, aB, aC, aD);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

TEST(MathPoly_ComparisonTest, Cubic_OneRealRoot)
{
  // x^3 + x + 2 = 0 -> one real root near -1.0
  const double aA = 1.0, aB = 0.0, aC = 1.0, aD = 2.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD);
  MathUtils::PolyResult            aNewResult = MathPoly::Cubic(aA, aB, aC, aD);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find same number of real roots
  EXPECT_EQ(anOldSolver.NbSolutions(), aNewResult.NbRoots);

  if (anOldSolver.NbSolutions() > 0 && aNewResult.NbRoots > 0)
  {
    CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
  }
}

TEST(MathPoly_ComparisonTest, Cubic_TripleRoot)
{
  // x^3 - 3x^2 + 3x - 1 = 0 -> triple root at 1
  const double aA = 1.0, aB = -3.0, aC = 3.0, aD = -1.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD);
  MathUtils::PolyResult            aNewResult = MathPoly::Cubic(aA, aB, aC, aD);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // For triple roots, both should find roots near 1.0
  auto aOldRoots = GetOldRoots(anOldSolver);
  auto aNewRoots = GetNewRoots(aNewResult);

  // All roots should be near 1.0
  for (double aRoot : aOldRoots)
  {
    EXPECT_NEAR(aRoot, 1.0, 1.0e-6);
  }
  for (double aRoot : aNewRoots)
  {
    EXPECT_NEAR(aRoot, 1.0, 1.0e-6);
  }
}

TEST(MathPoly_ComparisonTest, Cubic_NegativeLeadingCoeff)
{
  // -x^3 + 6x^2 - 11x + 6 = 0 -> roots at 1, 2, 3
  const double aA = -1.0, aB = 6.0, aC = -11.0, aD = 6.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD);
  MathUtils::PolyResult            aNewResult = MathPoly::Cubic(aA, aB, aC, aD);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

// ============================================================================
// Quartic equation comparison tests
// ============================================================================

TEST(MathPoly_ComparisonTest, Quartic_FourDistinctRoots)
{
  // (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24
  const double aA = 1.0, aB = -10.0, aC = 35.0, aD = -50.0, aE = 24.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD, aE);
  MathUtils::PolyResult            aNewResult = MathPoly::Quartic(aA, aB, aC, aD, aE);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

TEST(MathPoly_ComparisonTest, Quartic_TwoRealRoots)
{
  // (x^2 - 1)(x^2 + 1) = x^4 - 1 has roots at -1 and 1 (real) and +/-i (complex)
  const double aA = 1.0, aB = 0.0, aC = 0.0, aD = 0.0, aE = -1.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD, aE);
  MathUtils::PolyResult            aNewResult = MathPoly::Quartic(aA, aB, aC, aD, aE);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find 2 real roots
  EXPECT_EQ(anOldSolver.NbSolutions(), 2);
  EXPECT_EQ(aNewResult.NbRoots, 2);

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

TEST(MathPoly_ComparisonTest, Quartic_Biquadratic)
{
  // x^4 - 5x^2 + 4 = 0 -> (x^2-1)(x^2-4) = 0 -> roots at -2, -1, 1, 2
  const double aA = 1.0, aB = 0.0, aC = -5.0, aD = 0.0, aE = 4.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD, aE);
  MathUtils::PolyResult            aNewResult = MathPoly::Quartic(aA, aB, aC, aD, aE);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));
}

TEST(MathPoly_ComparisonTest, Quartic_NoRealRoots)
{
  // x^4 + 1 = 0 has no real roots
  const double aA = 1.0, aB = 0.0, aC = 0.0, aD = 0.0, aE = 1.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD, aE);
  MathUtils::PolyResult            aNewResult = MathPoly::Quartic(aA, aB, aC, aD, aE);

  // Both should report no real solutions
  EXPECT_EQ(anOldSolver.NbSolutions(), 0);
  EXPECT_EQ(aNewResult.NbRoots, 0);
}

TEST(MathPoly_ComparisonTest, Quartic_QuadrupleRoot)
{
  // (x-2)^4 = x^4 - 8x^3 + 24x^2 - 32x + 16
  const double aA = 1.0, aB = -8.0, aC = 24.0, aD = -32.0, aE = 16.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD, aE);
  MathUtils::PolyResult            aNewResult = MathPoly::Quartic(aA, aB, aC, aD, aE);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // All roots should be near 2.0
  auto aOldRoots = GetOldRoots(anOldSolver);
  auto aNewRoots = GetNewRoots(aNewResult);

  for (double aRoot : aOldRoots)
  {
    EXPECT_NEAR(aRoot, 2.0, 1.0e-5);
  }
  for (double aRoot : aNewRoots)
  {
    EXPECT_NEAR(aRoot, 2.0, 1.0e-5);
  }
}

// ============================================================================
// Edge case comparison tests
// ============================================================================

TEST(MathPoly_ComparisonTest, Quadratic_SmallDiscriminant)
{
  // x^2 - 2x + 0.9999 = 0 -> roots very close together
  const double aA = 1.0, aB = -2.0, aC = 0.9999;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find 2 roots near 1.0
  EXPECT_EQ(anOldSolver.NbSolutions(), 2);
  EXPECT_EQ(aNewResult.NbRoots, 2);

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult), 1.0e-4);
}

TEST(MathPoly_ComparisonTest, Cubic_SmallCoefficients)
{
  // 0.001x^3 - 0.006x^2 + 0.011x - 0.006 = 0 -> roots at 1, 2, 3
  const double aA = 0.001, aB = -0.006, aC = 0.011, aD = -0.006;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD);
  MathUtils::PolyResult            aNewResult = MathPoly::Cubic(aA, aB, aC, aD);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult), 1.0e-6);
}

// ============================================================================
// Numerical stability comparison tests
// ============================================================================

TEST(MathPoly_ComparisonTest, Quadratic_NumericallyChallengingCase)
{
  // x^2 - 1e8*x + 1 = 0 -> roots at ~1e8 and ~1e-8 (challenging due to scale)
  const double aA = 1.0, aB = -1.0e8, aC = 1.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC);
  MathUtils::PolyResult            aNewResult = MathPoly::Quadratic(aA, aB, aC);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_EQ(anOldSolver.NbSolutions(), 2);
  EXPECT_EQ(aNewResult.NbRoots, 2);

  // Verify both find the same roots (with relative tolerance for large root)
  auto aOldRoots = GetOldRoots(anOldSolver);
  auto aNewRoots = GetNewRoots(aNewResult);

  // Small root comparison
  EXPECT_NEAR(aOldRoots[0], aNewRoots[0], 1.0e-14);
  // Large root - use relative tolerance
  EXPECT_NEAR(aOldRoots[1] / aNewRoots[1], 1.0, 1.0e-8);
}

TEST(MathPoly_ComparisonTest, Quartic_SymmetricRoots)
{
  // (x-1)(x+1)(x-2)(x+2) = (x^2-1)(x^2-4) = x^4 - 5x^2 + 4
  // Roots: -2, -1, 1, 2
  const double aA = 1.0, aB = 0.0, aC = -5.0, aD = 0.0, aE = 4.0;

  math_DirectPolynomialRoots anOldSolver(aA, aB, aC, aD, aE);
  MathUtils::PolyResult            aNewResult = MathPoly::Quartic(aA, aB, aC, aD, aE);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  CompareRoots(GetOldRoots(anOldSolver), GetNewRoots(aNewResult));

  // Also verify the exact expected roots
  auto aNewRoots = GetNewRoots(aNewResult);
  ASSERT_EQ(aNewRoots.size(), 4u);
  EXPECT_NEAR(aNewRoots[0], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aNewRoots[1], -1.0, THE_TOLERANCE);
  EXPECT_NEAR(aNewRoots[2], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aNewRoots[3], 2.0, THE_TOLERANCE);
}
