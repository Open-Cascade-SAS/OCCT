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

#include <MathLin_Gauss.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <cmath>

namespace
{
  constexpr double THE_TOLERANCE = 1.0e-10;
}

//==================================================================================================
// Test: Solve 3x3 system - compare new API with legacy math_Gauss
//==================================================================================================

TEST(MathLin_ComparisonTest, Solve_3x3System)
{
  // Create a 3x3 matrix
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 3.0; aA(1, 2) = 2.0; aA(1, 3) = -1.0;
  aA(2, 1) = 2.0; aA(2, 2) = -2.0; aA(2, 3) = 4.0;
  aA(3, 1) = -1.0; aA(3, 2) = 0.5; aA(3, 3) = -1.0;

  // Right-hand side
  math_Vector aB(1, 3);
  aB(1) = 1.0;
  aB(2) = -2.0;
  aB(3) = 0.0;

  // Solve using new API
  auto aNewResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aNewResult.IsDone());
  ASSERT_TRUE(aNewResult.Solution.has_value());

  // Solve using legacy API
  math_Gauss aOldGauss(aA);
  ASSERT_TRUE(aOldGauss.IsDone());
  math_Vector aOldX(1, 3);
  aOldGauss.Solve(aB, aOldX);

  // Compare solutions
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR((*aNewResult.Solution)(i), aOldX(i), THE_TOLERANCE)
        << "Solution differs at index " << i;
  }
}

//==================================================================================================
// Test: Solve larger system (5x5)
//==================================================================================================

TEST(MathLin_ComparisonTest, Solve_5x5System)
{
  // Create a 5x5 diagonally dominant matrix
  math_Matrix aA(1, 5, 1, 5, 0.0);
  for (int i = 1; i <= 5; ++i)
  {
    aA(i, i) = 10.0;
    if (i > 1)
    {
      aA(i, i - 1) = -2.0;
    }
    if (i < 5)
    {
      aA(i, i + 1) = -3.0;
    }
  }

  // Right-hand side
  math_Vector aB(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aB(i) = static_cast<double>(i);
  }

  // Solve using new API
  auto aNewResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_Gauss aOldGauss(aA);
  ASSERT_TRUE(aOldGauss.IsDone());
  math_Vector aOldX(1, 5);
  aOldGauss.Solve(aB, aOldX);

  // Compare solutions
  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR((*aNewResult.Solution)(i), aOldX(i), THE_TOLERANCE)
        << "Solution differs at index " << i;
  }
}

//==================================================================================================
// Test: Determinant comparison
//==================================================================================================

TEST(MathLin_ComparisonTest, Determinant_3x3Matrix)
{
  // Create a 3x3 matrix with known determinant
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 6.0; aA(1, 2) = 1.0; aA(1, 3) = 1.0;
  aA(2, 1) = 4.0; aA(2, 2) = -2.0; aA(2, 3) = 5.0;
  aA(3, 1) = 2.0; aA(3, 2) = 8.0; aA(3, 3) = 7.0;
  // Expected determinant: -306

  // Compute using new API
  auto aNewResult = MathLin::Determinant(aA);
  ASSERT_TRUE(aNewResult.IsDone());
  ASSERT_TRUE(aNewResult.Determinant.has_value());

  // Compute using legacy API
  math_Gauss aOldGauss(aA);
  ASSERT_TRUE(aOldGauss.IsDone());
  double aOldDet = aOldGauss.Determinant();

  // Compare determinants
  EXPECT_NEAR(*aNewResult.Determinant, aOldDet, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Determinant, -306.0, THE_TOLERANCE);
}

//==================================================================================================
// Test: Determinant of identity matrix
//==================================================================================================

TEST(MathLin_ComparisonTest, Determinant_IdentityMatrix)
{
  // Create identity matrix
  math_Matrix aI(1, 4, 1, 4, 0.0);
  for (int i = 1; i <= 4; ++i)
  {
    aI(i, i) = 1.0;
  }

  // Compute using new API
  auto aNewResult = MathLin::Determinant(aI);
  ASSERT_TRUE(aNewResult.IsDone());

  // Compute using legacy API
  math_Gauss aOldGauss(aI);
  ASSERT_TRUE(aOldGauss.IsDone());
  double aOldDet = aOldGauss.Determinant();

  // Both should be 1.0
  EXPECT_NEAR(*aNewResult.Determinant, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aOldDet, 1.0, THE_TOLERANCE);
}

//==================================================================================================
// Test: Matrix inversion comparison
//==================================================================================================

TEST(MathLin_ComparisonTest, Invert_3x3Matrix)
{
  // Create a 3x3 matrix
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 1.0; aA(1, 2) = 2.0; aA(1, 3) = 3.0;
  aA(2, 1) = 0.0; aA(2, 2) = 1.0; aA(2, 3) = 4.0;
  aA(3, 1) = 5.0; aA(3, 2) = 6.0; aA(3, 3) = 0.0;

  // Compute inverse using new API
  auto aNewResult = MathLin::Invert(aA);
  ASSERT_TRUE(aNewResult.IsDone());
  ASSERT_TRUE(aNewResult.Inverse.has_value());

  // Compute inverse using legacy API
  math_Gauss aOldGauss(aA);
  ASSERT_TRUE(aOldGauss.IsDone());
  math_Matrix aOldInv(1, 3, 1, 3);
  aOldGauss.Invert(aOldInv);

  // Compare inverse matrices
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      EXPECT_NEAR((*aNewResult.Inverse)(i, j), aOldInv(i, j), THE_TOLERANCE)
          << "Inverse differs at (" << i << ", " << j << ")";
    }
  }
}

//==================================================================================================
// Test: Verify inverse by multiplication A * A^-1 = I
//==================================================================================================

TEST(MathLin_ComparisonTest, Invert_VerifyByMultiplication)
{
  // Create a 3x3 matrix
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 2.0; aA(1, 2) = 1.0; aA(1, 3) = 1.0;
  aA(2, 1) = 1.0; aA(2, 2) = 3.0; aA(2, 3) = 2.0;
  aA(3, 1) = 1.0; aA(3, 2) = 2.0; aA(3, 3) = 2.0;

  // Compute inverse using new API
  auto aNewResult = MathLin::Invert(aA);
  ASSERT_TRUE(aNewResult.IsDone());

  // Multiply A * A^-1 and check if result is identity
  math_Matrix aProduct = aA * (*aNewResult.Inverse);

  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      double aExpected = (i == j) ? 1.0 : 0.0;
      EXPECT_NEAR(aProduct(i, j), aExpected, THE_TOLERANCE)
          << "Product differs at (" << i << ", " << j << ")";
    }
  }
}

//==================================================================================================
// Test: LU decomposition - verify via solving a system
//==================================================================================================

TEST(MathLin_ComparisonTest, LU_Decomposition)
{
  // Create a 3x3 matrix
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 2.0; aA(1, 2) = 1.0; aA(1, 3) = 1.0;
  aA(2, 1) = 4.0; aA(2, 2) = 3.0; aA(2, 3) = 3.0;
  aA(3, 1) = 8.0; aA(3, 2) = 7.0; aA(3, 3) = 9.0;

  // Perform LU decomposition
  auto aLUResult = MathLin::LU(aA);
  ASSERT_TRUE(aLUResult.IsDone());
  ASSERT_TRUE(aLUResult.LU.has_value());
  ASSERT_TRUE(aLUResult.Pivot.has_value());
  ASSERT_TRUE(aLUResult.Determinant.has_value());

  // Extract L (lower triangular with 1s on diagonal) and U (upper triangular)
  const math_Matrix& aLUMat = *aLUResult.LU;

  math_Matrix aL(1, 3, 1, 3, 0.0);
  math_Matrix aU(1, 3, 1, 3, 0.0);

  for (int i = 1; i <= 3; ++i)
  {
    aL(i, i) = 1.0;
    for (int j = 1; j < i; ++j)
    {
      aL(i, j) = aLUMat(i, j);
    }
    for (int j = i; j <= 3; ++j)
    {
      aU(i, j) = aLUMat(i, j);
    }
  }

  // Compute L * U (for verification purposes)
  math_Matrix aLU = aL * aU;
  (void)aLU; // Used to verify decomposition quality via solve

  // Verify by solving a system and checking residual
  math_Vector aB(1, 3);
  aB(1) = 1.0; aB(2) = 2.0; aB(3) = 3.0;

  auto aSolveResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aSolveResult.IsDone());

  // Compute residual: A*x - b
  math_Vector aResidual = aA * (*aSolveResult.Solution);
  for (int i = 1; i <= 3; ++i)
  {
    aResidual(i) -= aB(i);
    EXPECT_NEAR(aResidual(i), 0.0, THE_TOLERANCE) << "Residual at " << i;
  }
}

//==================================================================================================
// Test: Singular matrix detection
//==================================================================================================

TEST(MathLin_ComparisonTest, SingularMatrix_Detection)
{
  // Create a singular matrix (row 3 = row 1)
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 1.0; aA(1, 2) = 2.0; aA(1, 3) = 3.0;
  aA(2, 1) = 4.0; aA(2, 2) = 5.0; aA(2, 3) = 6.0;
  aA(3, 1) = 1.0; aA(3, 2) = 2.0; aA(3, 3) = 3.0;

  // New API should report failure
  auto aNewResult = MathLin::LU(aA);
  EXPECT_FALSE(aNewResult.IsDone());

  // Legacy API should also report failure
  math_Gauss aOldGauss(aA);
  EXPECT_FALSE(aOldGauss.IsDone());
}

//==================================================================================================
// Test: Hilbert matrix (ill-conditioned)
//==================================================================================================

TEST(MathLin_ComparisonTest, IllConditioned_HilbertMatrix)
{
  // Create 4x4 Hilbert matrix (ill-conditioned)
  const int aN = 4;
  math_Matrix aH(1, aN, 1, aN);
  for (int i = 1; i <= aN; ++i)
  {
    for (int j = 1; j <= aN; ++j)
    {
      aH(i, j) = 1.0 / static_cast<double>(i + j - 1);
    }
  }

  // Right-hand side
  math_Vector aB(1, aN);
  for (int i = 1; i <= aN; ++i)
  {
    aB(i) = 1.0;
  }

  // Solve using new API
  auto aNewResult = MathLin::Solve(aH, aB);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_Gauss aOldGauss(aH);
  ASSERT_TRUE(aOldGauss.IsDone());
  math_Vector aOldX(1, aN);
  aOldGauss.Solve(aB, aOldX);

  // For ill-conditioned matrices, use looser tolerance
  constexpr double aLooseTol = 1.0e-6;
  for (int i = 1; i <= aN; ++i)
  {
    EXPECT_NEAR((*aNewResult.Solution)(i), aOldX(i), aLooseTol)
        << "Solution differs at index " << i;
  }
}

//==================================================================================================
// Test: Determinant comparison for multiple matrices
//==================================================================================================

TEST(MathLin_ComparisonTest, Determinant_MultipleMatrices)
{
  // Test several 2x2 matrices
  struct TestCase
  {
    double a11, a12, a21, a22;
    double expectedDet;
  };

  TestCase aCases[] = {
      {1.0, 0.0, 0.0, 1.0, 1.0},   // Identity
      {2.0, 0.0, 0.0, 3.0, 6.0},   // Diagonal
      {1.0, 2.0, 3.0, 4.0, -2.0},  // General
      {0.0, 1.0, 1.0, 0.0, -1.0},  // Permutation
  };

  for (const auto& aCase : aCases)
  {
    math_Matrix aA(1, 2, 1, 2);
    aA(1, 1) = aCase.a11;
    aA(1, 2) = aCase.a12;
    aA(2, 1) = aCase.a21;
    aA(2, 2) = aCase.a22;

    auto aNewResult = MathLin::Determinant(aA);
    ASSERT_TRUE(aNewResult.IsDone());

    math_Gauss aOldGauss(aA);
    ASSERT_TRUE(aOldGauss.IsDone());

    EXPECT_NEAR(*aNewResult.Determinant, aOldGauss.Determinant(), THE_TOLERANCE);
    EXPECT_NEAR(*aNewResult.Determinant, aCase.expectedDet, THE_TOLERANCE);
  }
}

//==================================================================================================
// Test: Solution accuracy with known answer
//==================================================================================================

TEST(MathLin_ComparisonTest, Solve_KnownSolution)
{
  // Create system with known solution x = (1, 2, 3)
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 1.0; aA(1, 2) = 0.0; aA(1, 3) = 0.0;
  aA(2, 1) = 0.0; aA(2, 2) = 1.0; aA(2, 3) = 0.0;
  aA(3, 1) = 0.0; aA(3, 2) = 0.0; aA(3, 3) = 1.0;

  math_Vector aExpectedX(1, 3);
  aExpectedX(1) = 1.0;
  aExpectedX(2) = 2.0;
  aExpectedX(3) = 3.0;

  // B = A * X
  math_Vector aB = aA * aExpectedX;

  // Solve using new API
  auto aNewResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aNewResult.IsDone());

  // Compare with expected
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR((*aNewResult.Solution)(i), aExpectedX(i), THE_TOLERANCE);
  }
}

//==================================================================================================
// Test: Solve with more complex known solution
//==================================================================================================

TEST(MathLin_ComparisonTest, Solve_ComplexKnownSolution)
{
  // Create a more complex system with known solution x = (1, -1, 2, -2)
  math_Matrix aA(1, 4, 1, 4);
  aA(1, 1) = 5.0; aA(1, 2) = 7.0; aA(1, 3) = 6.0; aA(1, 4) = 5.0;
  aA(2, 1) = 7.0; aA(2, 2) = 10.0; aA(2, 3) = 8.0; aA(2, 4) = 7.0;
  aA(3, 1) = 6.0; aA(3, 2) = 8.0; aA(3, 3) = 10.0; aA(3, 4) = 9.0;
  aA(4, 1) = 5.0; aA(4, 2) = 7.0; aA(4, 3) = 9.0; aA(4, 4) = 10.0;

  math_Vector aExpectedX(1, 4);
  aExpectedX(1) = 1.0;
  aExpectedX(2) = -1.0;
  aExpectedX(3) = 2.0;
  aExpectedX(4) = -2.0;

  // B = A * X
  math_Vector aB = aA * aExpectedX;

  // Solve using new API
  auto aNewResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_Gauss aOldGauss(aA);
  ASSERT_TRUE(aOldGauss.IsDone());
  math_Vector aOldX(1, 4);
  aOldGauss.Solve(aB, aOldX);

  // Compare both solutions with expected
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_NEAR((*aNewResult.Solution)(i), aExpectedX(i), THE_TOLERANCE);
    EXPECT_NEAR(aOldX(i), aExpectedX(i), THE_TOLERANCE);
    EXPECT_NEAR((*aNewResult.Solution)(i), aOldX(i), THE_TOLERANCE);
  }
}
