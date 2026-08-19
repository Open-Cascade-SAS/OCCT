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

#include <MathLin_SVD.hxx>
#include <MathLin_Householder.hxx>
#include <MathLin_Jacobi.hxx>
#include <MathLin_LeastSquares.hxx>
#include <MathLin_Gauss.hxx>
#include <MathLin_Crout.hxx>
#include <math_SVD.hxx>
#include <math_Householder.hxx>
#include <math_Jacobi.hxx>
#include <math_GaussLeastSquare.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-10;

//! Create identity matrix.
math_Matrix CreateIdentity(int theN)
{
  math_Matrix aMat(1, theN, 1, theN, 0.0);
  for (int i = 1; i <= theN; ++i)
  {
    aMat(i, i) = 1.0;
  }
  return aMat;
}

//! Create symmetric positive definite matrix.
math_Matrix CreateSPD(int theN)
{
  math_Matrix aMat(1, theN, 1, theN, 0.0);
  for (int i = 1; i <= theN; ++i)
  {
    for (int j = 1; j <= theN; ++j)
    {
      aMat(i, j) = 1.0 / (i + j - 1); // Hilbert matrix
    }
    aMat(i, i) += static_cast<double>(theN); // Make well-conditioned
  }
  return aMat;
}

//! Create random matrix.
math_Matrix CreateRandom(int theM, int theN, int theSeed = 42)
{
  math_Matrix aMat(1, theM, 1, theN);
  srand(static_cast<unsigned int>(theSeed));
  for (int i = 1; i <= theM; ++i)
  {
    for (int j = 1; j <= theN; ++j)
    {
      aMat(i, j) = static_cast<double>(rand()) / RAND_MAX * 2.0 - 1.0;
    }
  }
  return aMat;
}

//! Compute L2 norm of vector.
double VectorNorm(const math_Vector& theVec)
{
  double aNorm = 0.0;
  for (size_t i = 0; i < theVec.Size(); ++i)
  {
    aNorm += theVec.At(i) * theVec.At(i);
  }
  return std::sqrt(aNorm);
}

//! Matrix multiplication A*B.
math_Matrix MatMul(const math_Matrix& theA, const math_Matrix& theB)
{
  const size_t aM = theA.RowSize();
  const size_t aN = theB.ColSize();
  const size_t aK = theA.ColSize();
  math_Matrix  aResult(1, static_cast<int>(aM), 1, static_cast<int>(aN), 0.0);
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      for (size_t k = 0; k < aK; ++k)
      {
        aResult.ChangeAt(i, j) += theA.At(i, k) * theB.At(k, j);
      }
    }
  }
  return aResult;
}

//! Transpose matrix.
math_Matrix Transpose(const math_Matrix& theMat)
{
  const size_t aM = theMat.RowSize();
  const size_t aN = theMat.ColSize();
  math_Matrix  aResult(1, static_cast<int>(aN), 1, static_cast<int>(aM));
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aResult.ChangeAt(j, i) = theMat.At(i, j);
    }
  }
  return aResult;
}

} // namespace

// ============================================================================
// SVD tests
// ============================================================================

TEST(MathLin_SVD_Test, BasicDecomposition_2x2)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 3.0;
  aMat(1, 2) = 2.0;
  aMat(2, 1) = 2.0;
  aMat(2, 2) = 3.0;

  auto aResult = MathLin::SVD(aMat);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Rank, 2);

  // Verify U * S * V^T = A
  const math_Matrix& aU = *aResult.U;
  const math_Vector& aS = *aResult.SingularValues;
  const math_Matrix& aV = *aResult.V;

  // Construct diagonal matrix from singular values
  math_Matrix aSigma(1, 2, 1, 2, 0.0);
  aSigma(1, 1) = aS.At(0);
  aSigma(2, 2) = aS.At(1);

  // Compute U * Sigma * V^T
  math_Matrix aUSigma        = MatMul(aU, aSigma);
  math_Matrix aVt            = Transpose(aV);
  math_Matrix aReconstructed = MatMul(aUSigma, aVt);

  // Check reconstruction
  for (int i = 1; i <= 2; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      EXPECT_NEAR(aReconstructed(i, j), aMat(i, j), THE_TOLERANCE);
    }
  }
}

TEST(MathLin_SVD_Test, SingularValues)
{
  math_Matrix aMat(1, 3, 1, 3);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 2.0;
  aMat(1, 3) = 3.0;
  aMat(2, 1) = 4.0;
  aMat(2, 2) = 5.0;
  aMat(2, 3) = 6.0;
  aMat(3, 1) = 7.0;
  aMat(3, 2) = 8.0;
  aMat(3, 3) = 9.0;

  auto aResult = MathLin::SVD(aMat);

  ASSERT_TRUE(aResult.IsDone());

  // Singular values should be non-negative and in descending order
  const math_Vector& aS = *aResult.SingularValues;
  for (size_t i = 0; i + 1 < aS.Size(); ++i)
  {
    EXPECT_GE(aS.At(i), 0.0);
    EXPECT_GE(aS.At(i), aS.At(i + 1));
  }

  // This matrix is rank-deficient (rank 2)
  EXPECT_LE(aResult.Rank, 2);
}

TEST(MathLin_SVD_Test, SolveSystem)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 3.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;

  math_Vector aB(1, 2);
  aB(1) = 9.0;
  aB(2) = 8.0;

  auto aResult = MathLin::SolveSVD(aMat, aB);

  ASSERT_TRUE(aResult.IsDone());

  // Check solution: Ax = b
  const math_Vector& aX      = *aResult.Solution;
  double             aCheck1 = aMat(1, 1) * aX.At(0) + aMat(1, 2) * aX.At(1);
  double             aCheck2 = aMat(2, 1) * aX.At(0) + aMat(2, 2) * aX.At(1);

  EXPECT_NEAR(aCheck1, aB(1), THE_TOLERANCE);
  EXPECT_NEAR(aCheck2, aB(2), THE_TOLERANCE);
}

TEST(MathLin_SVD_Test, PseudoInverse)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 2.0;
  aMat(2, 1) = 3.0;
  aMat(2, 2) = 4.0;

  auto aPinv = MathLin::PseudoInverse(aMat);

  ASSERT_TRUE(aPinv.IsDone());

  // A * A+ * A = A
  math_Matrix aTemp  = MatMul(aMat, *aPinv.Inverse);
  math_Matrix aCheck = MatMul(aTemp, aMat);

  for (int i = 1; i <= 2; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      EXPECT_NEAR(aCheck(i, j), aMat(i, j), THE_TOLERANCE);
    }
  }
}

TEST(MathLin_SVD_Test, PseudoInverseUsesEffectiveSVDCutoff)
{
  math_Matrix aA(2, 2, 0.0);
  aA.ChangeAt(0, 0) = 1.0;
  aA.ChangeAt(1, 1) = std::numeric_limits<double>::epsilon();

  const MathLin::SVDResult aSVD = MathLin::SVD(aA, 0.0);
  ASSERT_TRUE(aSVD.IsDone());
  ASSERT_EQ(aSVD.Rank, 1);

  math_Vector                    aB(size_t{2}, 1.0);
  const MathUtils::LinearResult  aSolve   = MathLin::SolveSVD(aA, aB, 0.0);
  const MathUtils::InverseResult aInverse = MathLin::PseudoInverse(aA, 0.0);
  ASSERT_TRUE(aSolve.IsDone());
  ASSERT_TRUE(aInverse.IsDone());
  EXPECT_DOUBLE_EQ(aSolve.Solution->At(1), 0.0);
  EXPECT_DOUBLE_EQ(aInverse.Inverse->At(1, 1), 0.0);
}

TEST(MathLin_SVD_Test, PseudoInverseRejectsNonFiniteOutput)
{
  math_Matrix aA(2, 2, 0.0);
  aA.ChangeAt(0, 0) = 1.0e-307;
  aA.ChangeAt(1, 1) = 1.0e-309;

  EXPECT_EQ(MathLin::PseudoInverse(aA, 0.0).Status, MathUtils::Status::NumericalError);
}

TEST(MathLin_SVD_Test, ConditionNumber)
{
  // Well-conditioned identity matrix
  math_Matrix aI     = CreateIdentity(3);
  double      aCondI = MathLin::ConditionNumber(aI);
  EXPECT_NEAR(aCondI, 1.0, THE_TOLERANCE);

  // Ill-conditioned matrix
  math_Matrix aHilbert(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aHilbert(i, j) = 1.0 / (i + j - 1);
    }
  }
  double aCondH = MathLin::ConditionNumber(aHilbert);
  EXPECT_GT(aCondH, 100.0); // Hilbert matrices are ill-conditioned
}

TEST(MathLin_SVD_Test, RankDeficiencyBoundsAndSortedReconstruction)
{
  math_Matrix aA(-2, 0, 7, 8);
  aA(-2, 7) = 1.0;
  aA(-2, 8) = 2.0;
  aA(-1, 7) = 2.0;
  aA(-1, 8) = 4.0;
  aA(0, 7)  = -1.0;
  aA(0, 8)  = -2.0;

  const MathLin::SVDResult aSVD = MathLin::SVD(aA);
  ASSERT_TRUE(aSVD.IsDone());
  EXPECT_EQ(aSVD.Rank, 1);
  EXPECT_TRUE(std::isinf(aSVD.ConditionNumber));
  EXPECT_GE(aSVD.SingularValues->At(0), aSVD.SingularValues->At(1));
  for (size_t i = 0; i < aSVD.U->RowSize(); ++i)
  {
    for (size_t j = 0; j < aSVD.V->RowSize(); ++j)
    {
      double aValue = 0.0;
      for (size_t k = 0; k < aSVD.SingularValues->Size(); ++k)
      {
        aValue += aSVD.U->At(i, k) * aSVD.SingularValues->At(k) * aSVD.V->At(j, k);
      }
      EXPECT_NEAR(aValue, aA(-2 + static_cast<int>(i), 7 + static_cast<int>(j)), 1.0e-12);
    }
  }
}

TEST(MathLin_SVD_Test, WideMatrixReconstructionAndMinimumNorm)
{
  math_Matrix aA(2, 3, 0.0);
  aA.ChangeAt(0, 0) = 1.0;
  aA.ChangeAt(0, 2) = 1.0;
  aA.ChangeAt(1, 1) = 1.0;
  aA.ChangeAt(1, 2) = 1.0;

  const MathLin::SVDResult aSVD = MathLin::SVD(aA);
  ASSERT_TRUE(aSVD.IsDone());
  ASSERT_EQ(aSVD.U->RowSize(), 2u);
  ASSERT_EQ(aSVD.U->ColSize(), 3u);
  EXPECT_EQ(aSVD.Rank, 2u);
  for (size_t i = 0; i < aSVD.Rank; ++i)
  {
    double aNormSq = 0.0;
    for (size_t k = 0; k < aSVD.U->RowSize(); ++k)
    {
      aNormSq += aSVD.U->At(k, i) * aSVD.U->At(k, i);
    }
    EXPECT_NEAR(aNormSq, 1.0, 1.0e-12);
    for (size_t j = i + 1; j < aSVD.Rank; ++j)
    {
      double aDot = 0.0;
      for (size_t k = 0; k < aSVD.U->RowSize(); ++k)
      {
        aDot += aSVD.U->At(k, i) * aSVD.U->At(k, j);
      }
      EXPECT_NEAR(aDot, 0.0, 1.0e-12);
    }
  }
  for (size_t i = 0; i < aA.RowSize(); ++i)
  {
    for (size_t j = 0; j < aA.ColSize(); ++j)
    {
      double aValue = 0.0;
      for (size_t k = 0; k < aSVD.SingularValues->Size(); ++k)
      {
        aValue += aSVD.U->At(i, k) * aSVD.SingularValues->At(k) * aSVD.V->At(j, k);
      }
      EXPECT_NEAR(aValue, aA.At(i, j), 1.0e-12);
    }
  }

  math_Vector aB(size_t{2}, 1.0);
  const MathUtils::LinearResult aSolve = MathLin::SolveSVD(aA, aB);
  ASSERT_TRUE(aSolve.IsDone());
  EXPECT_NEAR(aSolve.Solution->At(0), 1.0 / 3.0, 1.0e-12);
  EXPECT_NEAR(aSolve.Solution->At(1), 1.0 / 3.0, 1.0e-12);
  EXPECT_NEAR(aSolve.Solution->At(2), 2.0 / 3.0, 1.0e-12);

  const MathUtils::InverseResult aInverse = MathLin::PseudoInverse(aA);
  ASSERT_TRUE(aInverse.IsDone());
  const math_Matrix aReconstructed = MatMul(MatMul(aA, *aInverse.Inverse), aA);
  for (size_t i = 0; i < aA.RowSize(); ++i)
  {
    for (size_t j = 0; j < aA.ColSize(); ++j)
    {
      EXPECT_NEAR(aReconstructed.At(i, j), aA.At(i, j), 1.0e-12);
    }
  }
}

// ============================================================================
// Multi-RHS linear solve tests
// ============================================================================

TEST(MathLin_Gauss_Test, SolveMultiple_ReturnsFullMatrix)
{
  math_Matrix aA(1, 3, 1, 3);
  aA(1, 1) = 4.0;
  aA(1, 2) = 1.0;
  aA(1, 3) = 2.0;
  aA(2, 1) = 0.0;
  aA(2, 2) = 3.0;
  aA(2, 3) = 1.0;
  aA(3, 1) = 2.0;
  aA(3, 2) = 1.0;
  aA(3, 3) = 5.0;

  math_Matrix aXExpected(1, 3, 1, 2);
  aXExpected(1, 1) = 1.0;
  aXExpected(2, 1) = 2.0;
  aXExpected(3, 1) = -1.0;
  aXExpected(1, 2) = -2.0;
  aXExpected(2, 2) = 0.5;
  aXExpected(3, 2) = 3.0;

  const math_Matrix aB = MatMul(aA, aXExpected);

  auto aResult = MathLin::SolveMultiple(aA, aB);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solutions.has_value());

  const math_Matrix& aX = *aResult.Solutions;
  for (size_t i = 0; i < aX.RowSize(); ++i)
  {
    for (size_t j = 0; j < aX.ColSize(); ++j)
    {
      EXPECT_NEAR(aX.At(i, j), aXExpected.At(i, j), THE_TOLERANCE);
    }
  }

  const math_Matrix aCheckB = MatMul(aA, aX);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      EXPECT_NEAR(aCheckB(i, j), aB(i, j), THE_TOLERANCE);
    }
  }
}

TEST(MathLin_Gauss_Test, SolveMultiple_DimensionMismatch)
{
  math_Matrix aA(1, 3, 1, 3, 0.0);
  for (int i = 1; i <= 3; ++i)
  {
    aA(i, i) = 1.0;
  }

  math_Matrix aBWrong(1, 2, 1, 1, 0.0);
  auto        aResult = MathLin::SolveMultiple(aA, aBWrong);
  EXPECT_EQ(aResult.Status, MathUtils::Status::InvalidInput);
}

TEST(MathLin_Gauss_Test, ThreeByThreePivotedSystem)
{
  math_Matrix aMatrix(3, 3);
  aMatrix.ChangeAt(0, 0) = 4.0;
  aMatrix.ChangeAt(0, 1) = 2.0;
  aMatrix.ChangeAt(0, 2) = -1.0;
  aMatrix.ChangeAt(1, 0) = 1.0;
  aMatrix.ChangeAt(1, 1) = 5.0;
  aMatrix.ChangeAt(1, 2) = 2.0;
  aMatrix.ChangeAt(2, 0) = 2.0;
  aMatrix.ChangeAt(2, 1) = -1.0;
  aMatrix.ChangeAt(2, 2) = 3.0;

  math_Vector aRightHandSide(3);
  aRightHandSide.ChangeAt(0) = 7.0;
  aRightHandSide.ChangeAt(1) = -4.0;
  aRightHandSide.ChangeAt(2) = 5.0;
  const MathUtils::LinearResult aResult = MathLin::Solve(aMatrix, aRightHandSide);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  constexpr double THE_EXPECTED_SOLUTION[] = {2.2716049382716048,
                                               -1.1604938271604937,
                                               -0.23456790123456786};
  for (size_t anIndex = 0; anIndex < 3; ++anIndex)
  {
    EXPECT_NEAR(aResult.Solution->At(anIndex), THE_EXPECTED_SOLUTION[anIndex], 1.0e-13);
  }
}

TEST(MathLin_Gauss_Test, TwoByTwoNonsymmetricSystem)
{
  math_Matrix aA(size_t{2}, size_t{2});
  aA.ChangeAt(0, 0) = 0.2;
  aA.ChangeAt(0, 1) = 3.1;
  aA.ChangeAt(1, 0) = -2.4;
  aA.ChangeAt(1, 1) = 1.7;

  math_Vector aB(size_t{2});
  aB.ChangeAt(0) = 7.3;
  aB.ChangeAt(1) = -1.2;

  const MathUtils::LinearResult aResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Determinant.has_value());

  constexpr double THE_EXPECTED_SOLUTION[2] = {2.0732647814910026, 2.2210796915167097};
  for (size_t i = 0; i < 2; ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(i), THE_EXPECTED_SOLUTION[i], 1.0e-13);
    double aResidual = -aB.At(i);
    for (size_t j = 0; j < 2; ++j)
    {
      aResidual += aA.At(i, j) * aResult.Solution->At(j);
    }
    EXPECT_NEAR(aResidual, 0.0, 1.0e-13);
  }
  EXPECT_NEAR(*aResult.Determinant, 7.78, 1.0e-13);
}

TEST(MathLin_Gauss_Test, FourByFourNonsymmetricSystem)
{
  constexpr double THE_A[4][4] = {{3.0, -2.0, 0.5, 1.0},
                                   {1.0, 4.0, -1.0, 2.0},
                                   {-2.0, 0.75, 5.0, -1.0},
                                   {0.5, -3.0, 2.0, 6.0}};
  constexpr double THE_B[4] = {4.5, -2.0, 7.25, 1.0};
  math_Matrix      aA(size_t{4}, size_t{4});
  math_Vector      aB(size_t{4});
  for (size_t i = 0; i < 4; ++i)
  {
    aB.ChangeAt(i) = THE_B[i];
    for (size_t j = 0; j < 4; ++j)
    {
      aA.ChangeAt(i, j) = THE_A[i][j];
    }
  }

  const MathUtils::LinearResult aResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Determinant.has_value());

  constexpr double THE_EXPECTED_SOLUTION[4] = {1.3449494949494951,
                                                -0.065151515151515113,
                                                1.877020202020202,
                                                -0.60366161616161618};
  for (size_t i = 0; i < 4; ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(i), THE_EXPECTED_SOLUTION[i], 1.0e-13);
    double aResidual = -aB.At(i);
    for (size_t j = 0; j < 4; ++j)
    {
      aResidual += aA.At(i, j) * aResult.Solution->At(j);
    }
    EXPECT_NEAR(aResidual, 0.0, 1.0e-12);
  }
  EXPECT_NEAR(*aResult.Determinant, 495.0, 1.0e-11);
}

TEST(MathLin_Gauss_Test, FiveByFiveColumnScaledSystem)
{
  constexpr double THE_A[5][5] = {{2.5, 0.0012, -3.0, 1.5, 0.2},
                                   {-1.0, 0.0021, 2.0, 0.5, -0.7},
                                   {3.0, -0.0008, 1.0, -2.0, 0.4},
                                   {0.5, 0.0032, -1.0, 4.0, 1.1},
                                   {-2.0, 0.0017, 0.5, 1.0, 3.0}};
  constexpr double THE_B[5] = {1.2, -3.4, 5.6, 0.75, -2.1};
  math_Matrix      aA(size_t{5}, size_t{5});
  math_Vector      aB(size_t{5});
  for (size_t i = 0; i < 5; ++i)
  {
    aB.ChangeAt(i) = THE_B[i];
    for (size_t j = 0; j < 5; ++j)
    {
      aA.ChangeAt(i, j) = THE_A[i][j];
    }
  }

  const MathUtils::LinearResult aResult = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Determinant.has_value());

  constexpr double THE_EXPECTED_SOLUTION[5] = {1.8626125409601375,
                                                -1955.7312378710285,
                                                1.2160500111347952,
                                                1.5690929914421161,
                                                0.92428339643050317};
  for (size_t i = 0; i < 5; ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(i), THE_EXPECTED_SOLUTION[i], 1.0e-9);
    double aResidual = -aB.At(i);
    for (size_t j = 0; j < 5; ++j)
    {
      aResidual += aA.At(i, j) * aResult.Solution->At(j);
    }
    EXPECT_NEAR(aResidual, 0.0, 1.0e-11);
  }
  EXPECT_NEAR(*aResult.Determinant, 0.23574749999999986, 1.0e-13);
}

TEST(MathLin_Gauss_Test, ArbitraryMismatchedBoundsAndScaledSystem)
{
  math_Matrix  aA(-3, -1, 5, 7);
  const double aScale = 1.0e-200;
  aA(-3, 5)           = 4.0 * aScale;
  aA(-3, 6)           = 1.0 * aScale;
  aA(-3, 7)           = 2.0 * aScale;
  aA(-2, 5)           = 1.0 * aScale;
  aA(-2, 6)           = 5.0 * aScale;
  aA(-2, 7)           = 1.0 * aScale;
  aA(-1, 5)           = 2.0 * aScale;
  aA(-1, 6)           = 1.0 * aScale;
  aA(-1, 7)           = 6.0 * aScale;
  math_Vector aB(20, 22);
  aB(20) = 12.0 * aScale;
  aB(21) = 14.0 * aScale;
  aB(22) = 22.0 * aScale;

  const MathUtils::LinearResult aSolve = MathLin::Solve(aA, aB);
  ASSERT_TRUE(aSolve.IsDone());
  ASSERT_EQ(aSolve.Solution->Lower(), 0);
  EXPECT_NEAR(aSolve.Solution->At(0), 1.0, 1.0e-12);
  EXPECT_NEAR(aSolve.Solution->At(1), 2.0, 1.0e-12);
  EXPECT_NEAR(aSolve.Solution->At(2), 3.0, 1.0e-12);

  const MathLin::LUResult aFactor = MathLin::LU(aA);
  ASSERT_TRUE(aFactor.IsDone());
  math_Matrix aPermuted(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPermuted(i, j) = aA(-4 + i, 4 + j);
    }
  }
  for (size_t k = 0; k < aFactor.Pivot->Size(); ++k)
  {
    const size_t aPivot = aFactor.Pivot->At(k);
    for (size_t j = 0; j < aFactor.LU->ColSize(); ++j)
    {
      std::swap(aPermuted.ChangeAt(k, j), aPermuted.ChangeAt(aPivot, j));
    }
  }
  math_Matrix aL(1, 3, 1, 3, 0.0);
  math_Matrix aU(1, 3, 1, 3, 0.0);
  for (int i = 1; i <= 3; ++i)
  {
    aL(i, i) = 1.0;
    for (int j = 1; j <= 3; ++j)
    {
      if (i > j)
      {
        aL(i, j) = aFactor.LU->At(static_cast<size_t>(i - 1), static_cast<size_t>(j - 1));
      }
      else
      {
        aU(i, j) = aFactor.LU->At(static_cast<size_t>(i - 1), static_cast<size_t>(j - 1));
      }
    }
  }
  const math_Matrix aReconstructed = aL * aU;
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      EXPECT_NEAR(aReconstructed(i, j) / aScale, aPermuted(i, j) / aScale, 1.0e-12);
    }
  }
}

TEST(MathLin_Gauss_Test, RejectsDeterminantOverflow)
{
  math_Matrix aA(2, 2, 0.0);
  aA.ChangeAt(0, 0) = 1.0e200;
  aA.ChangeAt(1, 1) = 1.0e200;

  const MathLin::LUResult aLU = MathLin::LU(aA);
  EXPECT_EQ(aLU.Status, MathUtils::Status::NumericalError);
  EXPECT_FALSE(aLU.Determinant.has_value());
  const MathUtils::LinearResult aDeterminant = MathLin::Determinant(aA);
  EXPECT_EQ(aDeterminant.Status, MathUtils::Status::NumericalError);
  EXPECT_FALSE(aDeterminant.Determinant.has_value());
}

TEST(MathLin_Gauss_Test, ThreeByThreeDeterminantAndInverse)
{
  constexpr double THE_A[3][3] = {{4.0, -1.0, 2.0},
                                   {0.5, 3.0, -0.75},
                                   {-2.0, 1.0, 5.0}};
  constexpr double THE_EXPECTED_INVERSE[3][3] = {
    {0.20454545454545453, 0.090909090909090912, -0.068181818181818177},
    {-0.012987012987012986, 0.31168831168831168, 0.051948051948051945},
    {0.084415584415584416, -0.025974025974025972, 0.16233766233766234}};
  math_Matrix aA(size_t{3}, size_t{3});
  for (size_t i = 0; i < 3; ++i)
  {
    for (size_t j = 0; j < 3; ++j)
    {
      aA.ChangeAt(i, j) = THE_A[i][j];
    }
  }

  const MathUtils::InverseResult aResult = MathLin::Invert(aA);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Inverse.has_value());
  ASSERT_TRUE(aResult.Determinant.has_value());
  EXPECT_NEAR(*aResult.Determinant, 77.0, 1.0e-13);

  for (size_t i = 0; i < 3; ++i)
  {
    for (size_t j = 0; j < 3; ++j)
    {
      EXPECT_NEAR(aResult.Inverse->At(i, j), THE_EXPECTED_INVERSE[i][j], 1.0e-13);
      double aResidual = i == j ? -1.0 : 0.0;
      for (size_t k = 0; k < 3; ++k)
      {
        aResidual += aA.At(i, k) * aResult.Inverse->At(k, j);
      }
      EXPECT_NEAR(aResidual, 0.0, 1.0e-13);
    }
  }
}

TEST(MathLin_Crout_Test, MismatchedBoundsFactorAndSolve)
{
  math_Matrix  aA(-1, 1, 4, 6);
  const double aValues[3][3] = {{4.0, 1.0, 1.0}, {1.0, 3.0, 0.5}, {1.0, 0.5, 2.0}};
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      aA(-1 + i, 4 + j) = aValues[i][j];
    }
  }
  math_Vector aB(10, 12);
  for (int i = 0; i < 3; ++i)
  {
    aB(10 + i) = aValues[i][0] + 2.0 * aValues[i][1] - aValues[i][2];
  }
  const MathLin::CroutResult aFactor = MathLin::Crout(aA);
  ASSERT_TRUE(aFactor.IsDone());
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      double aValue = 0.0;
      for (int k = 0; k < 3; ++k)
      {
        const double aLik = i >= k ? aFactor.L->At(i, k) : 0.0;
        const double aLjk = j >= k ? aFactor.L->At(j, k) : 0.0;
        aValue += aLik * aFactor.D->At(k) * aLjk;
      }
      EXPECT_NEAR(aValue, aValues[i][j], 1.0e-12);
    }
  }
  const MathUtils::LinearResult aSolve = MathLin::SolveCrout(aA, aB);
  ASSERT_TRUE(aSolve.IsDone());
  EXPECT_EQ(aSolve.Solution->Lower(), 0);
  EXPECT_NEAR(aSolve.Solution->At(0), 1.0, 1.0e-12);
  EXPECT_NEAR(aSolve.Solution->At(1), 2.0, 1.0e-12);
  EXPECT_NEAR(aSolve.Solution->At(2), -1.0, 1.0e-12);
}

TEST(MathLin_Crout_Test, SymmetricIndefiniteUsesLowerTriangle)
{
  math_Matrix aMatrix(size_t{2}, size_t{2}, 0.0);
  aMatrix.ChangeAt(0, 0) = 1.0;
  aMatrix.ChangeAt(1, 0) = 2.0;
  aMatrix.ChangeAt(1, 1) = 1.0;
  aMatrix.ChangeAt(0, 1) = std::numeric_limits<double>::quiet_NaN();

  const MathLin::CroutResult aResult = MathLin::Crout(aMatrix);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Inverse.has_value());
  EXPECT_NEAR(aResult.Inverse->At(0, 0), -1.0 / 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Inverse->At(0, 1), 2.0 / 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Inverse->At(1, 0), 2.0 / 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Inverse->At(1, 1), -1.0 / 3.0, THE_TOLERANCE);
}

TEST(MathLin_Householder_Test, SolveQRMultiple_ReturnsFullMatrix)
{
  math_Matrix aA(1, 3, 1, 2);
  aA(1, 1) = 1.0;
  aA(1, 2) = 2.0;
  aA(2, 1) = 3.0;
  aA(2, 2) = 1.0;
  aA(3, 1) = -1.0;
  aA(3, 2) = 1.0;

  math_Matrix aXExpected(1, 2, 1, 2);
  aXExpected(1, 1) = 2.0;
  aXExpected(2, 1) = -1.0;
  aXExpected(1, 2) = -0.5;
  aXExpected(2, 2) = 3.0;

  const math_Matrix aB = MatMul(aA, aXExpected);

  auto aResult = MathLin::SolveQRMultiple(aA, aB);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solutions.has_value());

  const math_Matrix& aX = *aResult.Solutions;
  for (size_t i = 0; i < aX.RowSize(); ++i)
  {
    for (size_t j = 0; j < aX.ColSize(); ++j)
    {
      EXPECT_NEAR(aX.At(i, j), aXExpected.At(i, j), THE_TOLERANCE);
    }
  }

  const math_Matrix aCheckB = MatMul(aA, aX);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      EXPECT_NEAR(aCheckB(i, j), aB(i, j), THE_TOLERANCE);
    }
  }
}

TEST(MathLin_Householder_Test, SolveQRMultiple_DimensionMismatch)
{
  math_Matrix aA(1, 3, 1, 2, 0.0);
  aA(1, 1) = 1.0;
  aA(2, 2) = 1.0;

  math_Matrix aBWrong(1, 2, 1, 2, 0.0);
  auto        aResult = MathLin::SolveQRMultiple(aA, aBWrong);
  EXPECT_EQ(aResult.Status, MathUtils::Status::InvalidInput);
}

// ============================================================================
// Householder QR tests
// ============================================================================

TEST(MathLin_Householder_Test, BasicQR_2x2)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 2.0;
  aMat(2, 1) = 3.0;
  aMat(2, 2) = 4.0;

  auto aResult = MathLin::QR(aMat);

  ASSERT_TRUE(aResult.IsDone());

  const math_Matrix& aQ = *aResult.Q;
  const math_Matrix& aR = *aResult.R;

  // Check Q is orthogonal: Q * Q^T = I
  math_Matrix aQQt = MatMul(aQ, Transpose(aQ));
  for (int i = 1; i <= 2; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      double aExpected = (i == j) ? 1.0 : 0.0;
      EXPECT_NEAR(aQQt(i, j), aExpected, THE_TOLERANCE);
    }
  }

  // Check R is upper triangular
  EXPECT_NEAR(aR.At(1, 0), 0.0, THE_TOLERANCE);

  // Check Q * R = A
  math_Matrix aQR = MatMul(aQ, aR);
  for (int i = 1; i <= 2; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      EXPECT_NEAR(aQR(i, j), aMat(i, j), THE_TOLERANCE);
    }
  }
}

TEST(MathLin_Householder_Test, SolveSystem)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 3.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;

  math_Vector aB(1, 2);
  aB(1) = 9.0;
  aB(2) = 8.0;

  auto aResult = MathLin::SolveQR(aMat, aB);

  ASSERT_TRUE(aResult.IsDone());

  // Check solution: Ax = b
  const math_Vector& aX      = *aResult.Solution;
  double             aCheck1 = aMat(1, 1) * aX.At(0) + aMat(1, 2) * aX.At(1);
  double             aCheck2 = aMat(2, 1) * aX.At(0) + aMat(2, 2) * aX.At(1);

  EXPECT_NEAR(aCheck1, aB(1), THE_TOLERANCE);
  EXPECT_NEAR(aCheck2, aB(2), THE_TOLERANCE);
}

TEST(MathLin_Householder_Test, Overdetermined)
{
  // 3x2 system (overdetermined)
  math_Matrix aMat(1, 3, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;
  aMat(3, 1) = 1.0;
  aMat(3, 2) = 3.0;

  math_Vector aB(1, 3);
  aB(1) = 1.0;
  aB(2) = 2.0;
  aB(3) = 3.0;

  auto aResult = MathLin::SolveQR(aMat, aB);

  ASSERT_TRUE(aResult.IsDone());

  // This is a least squares solution
  EXPECT_EQ(aResult.Solution->Size(), 2u);
}

TEST(MathLin_Householder_Test, BoundsScalingRankAndReconstruction)
{
  math_Matrix  aA(-4, -2, 3, 4);
  const double aScale         = 1.0e150;
  aA(-4, 3)                   = 1.0 * aScale;
  aA(-4, 4)                   = 2.0 * aScale;
  aA(-3, 3)                   = 2.0 * aScale;
  aA(-3, 4)                   = -1.0 * aScale;
  aA(-2, 3)                   = 3.0 * aScale;
  aA(-2, 4)                   = 1.0 * aScale;
  const MathLin::QRResult aQR = MathLin::QR(aA);
  ASSERT_TRUE(aQR.IsDone());
  EXPECT_EQ(aQR.Rank, 2);
  for (size_t i = 0; i < aQR.Q->RowSize(); ++i)
  {
    for (size_t j = 0; j < aQR.R->ColSize(); ++j)
    {
      double aValue = 0.0;
      for (size_t k = 0; k < aQR.Q->ColSize(); ++k)
      {
        aValue += aQR.Q->At(i, k) * aQR.R->At(k, j);
      }
      EXPECT_NEAR(aValue / aScale,
                  aA(-4 + static_cast<int>(i), 3 + static_cast<int>(j)) / aScale,
                  1.0e-12);
    }
  }

  aA(-4, 4) = 2.0 * aA(-4, 3);
  aA(-3, 4) = 2.0 * aA(-3, 3);
  aA(-2, 4) = 2.0 * aA(-2, 3);
  math_Vector aB(10, 12, 1.0);
  EXPECT_EQ(MathLin::QR(aA).Rank, 1);
  EXPECT_EQ(MathLin::SolveQR(aA, aB).Status, MathUtils::Status::Singular);
}

// ============================================================================
// Jacobi eigenvalue tests
// ============================================================================

TEST(MathLin_Jacobi_Test, Eigenvalues_Diagonal)
{
  math_Matrix aMat(1, 3, 1, 3, 0.0);
  aMat(1, 1) = 3.0;
  aMat(2, 2) = 1.0;
  aMat(3, 3) = 2.0;

  auto aResult = MathLin::Jacobi(aMat, true);

  ASSERT_TRUE(aResult.IsDone());

  const math_Vector& aEigenVals = *aResult.EigenValues;

  // Eigenvalues of diagonal matrix are the diagonal elements
  // Sorted in descending order
  EXPECT_NEAR(aEigenVals.At(0), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aEigenVals.At(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aEigenVals.At(2), 1.0, THE_TOLERANCE);
}

TEST(MathLin_Jacobi_Test, Eigenvalues_Symmetric)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 3.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 3.0;

  auto aResult = MathLin::Jacobi(aMat, true);

  ASSERT_TRUE(aResult.IsDone());

  const math_Vector& aEigenVals = *aResult.EigenValues;

  // Eigenvalues are 4 and 2
  EXPECT_NEAR(aEigenVals.At(0), 4.0, THE_TOLERANCE);
  EXPECT_NEAR(aEigenVals.At(1), 2.0, THE_TOLERANCE);
}

TEST(MathLin_Jacobi_Test, DenseFourByFourSymmetricSystem)
{
  constexpr double THE_A[4][4] = {{6.0, 2.0, -1.0, 0.5},
                                   {2.0, 3.0, 0.75, -1.0},
                                   {-1.0, 0.75, 4.0, 1.5},
                                   {0.5, -1.0, 1.5, 2.0}};
  math_Matrix aA(size_t{4}, size_t{4});
  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 4; ++j)
    {
      aA.ChangeAt(i, j) = THE_A[i][j];
    }
  }

  const MathUtils::EigenResult aResult = MathLin::Jacobi(aA, true);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.EigenValues.has_value());
  ASSERT_TRUE(aResult.EigenVectors.has_value());

  constexpr double THE_EXPECTED_EIGENVALUES[4] = {7.1223130768175151,
                                                   4.7817499346384418,
                                                   3.0874669645313992,
                                                   0.0084700240126456498};
  for (size_t j = 0; j < 4; ++j)
  {
    const double anEigenvalue = aResult.EigenValues->At(j);
    EXPECT_NEAR(anEigenvalue, THE_EXPECTED_EIGENVALUES[j], 1.0e-11);
    double aResidualSq = 0.0;
    double aNormSq     = 0.0;
    for (size_t i = 0; i < 4; ++i)
    {
      double aResidual = -anEigenvalue * aResult.EigenVectors->At(i, j);
      for (size_t k = 0; k < 4; ++k)
      {
        aResidual += aA.At(i, k) * aResult.EigenVectors->At(k, j);
      }
      aResidualSq += aResidual * aResidual;
      aNormSq += aResult.EigenVectors->At(i, j) * aResult.EigenVectors->At(i, j);
    }
    EXPECT_NEAR(std::sqrt(aResidualSq), 0.0, 1.0e-11);
    EXPECT_NEAR(aNormSq, 1.0, 1.0e-12);
  }
}

TEST(MathLin_Jacobi_Test, EigenvectorsOrthogonal)
{
  math_Matrix aMat = CreateSPD(3);

  auto aResult = MathLin::Jacobi(aMat, false);

  ASSERT_TRUE(aResult.IsDone());

  const math_Matrix& aV = *aResult.EigenVectors;

  // Eigenvectors should be orthogonal: V^T * V = I
  math_Matrix aVtV = MatMul(Transpose(aV), aV);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      double aExpected = (i == j) ? 1.0 : 0.0;
      EXPECT_NEAR(aVtV(i, j), aExpected, 1.0e-8);
    }
  }
}

TEST(MathLin_Jacobi_Test, SpectralDecomposition)
{
  math_Matrix aMat = CreateSPD(3);

  auto aResult = MathLin::SpectralDecomposition(aMat);

  ASSERT_TRUE(aResult.IsDone());

  const math_Vector& aD = *aResult.EigenValues;
  const math_Matrix& aV = *aResult.EigenVectors;

  // Reconstruct: A = V * D * V^T
  math_Matrix aDiag(1, 3, 1, 3, 0.0);
  for (size_t i = 0; i < aD.Size(); ++i)
  {
    aDiag.ChangeAt(i, i) = aD.At(i);
  }

  math_Matrix aVD            = MatMul(aV, aDiag);
  math_Matrix aReconstructed = MatMul(aVD, Transpose(aV));

  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      EXPECT_NEAR(aReconstructed(i, j), aMat(i, j), 1.0e-8);
    }
  }
}

TEST(MathLin_Jacobi_Test, MatrixSqrt)
{
  math_Matrix aMat = CreateSPD(2);

  auto aSqrt = MathLin::MatrixSqrt(aMat);

  ASSERT_TRUE(aSqrt.has_value());

  // sqrt(A) * sqrt(A) = A
  math_Matrix aCheck = MatMul(*aSqrt, *aSqrt);

  for (int i = 1; i <= 2; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      EXPECT_NEAR(aCheck(i, j), aMat(i, j), 1.0e-8);
    }
  }
}

TEST(MathLin_Jacobi_Test, MismatchedBounds)
{
  math_Matrix aA(-1, 0, 5, 6);
  aA(-1, 5)                           = 3.0e200;
  aA(-1, 6)                           = 1.0e200;
  aA(0, 5)                            = 1.0e200;
  aA(0, 6)                            = 3.0e200;
  const MathUtils::EigenResult aEigen = MathLin::Jacobi(aA);
  ASSERT_TRUE(aEigen.IsDone());
  EXPECT_NEAR(aEigen.EigenValues->At(0) / 1.0e200, 4.0, 1.0e-12);
  EXPECT_NEAR(aEigen.EigenValues->At(1) / 1.0e200, 2.0, 1.0e-12);
  const std::optional<math_Matrix> aSqrt = MathLin::MatrixSqrt(aA);
  ASSERT_TRUE(aSqrt.has_value());
  EXPECT_EQ(aSqrt->LowerRow(), 0);
  EXPECT_EQ(aSqrt->LowerCol(), 0);
}

TEST(MathLin_Jacobi_Test, SubnormalScalingAndInvalidMatrixPowers)
{
  const double aSubnormal = std::numeric_limits<double>::denorm_min();
  math_Matrix  aMatrix(size_t{1}, size_t{1}, aSubnormal);

  const MathUtils::EigenResult aEigen = MathLin::Jacobi(aMatrix);
  ASSERT_TRUE(aEigen.IsDone()) << "status=" << static_cast<int>(aEigen.Status)
                               << ", rows=" << aMatrix.RowSize() << ", cols=" << aMatrix.ColSize();
  EXPECT_EQ(aEigen.EigenValues->At(0), aSubnormal);

  const MathLin::SVDResult aSVD = MathLin::SVD(aMatrix);
  ASSERT_TRUE(aSVD.IsDone());
  EXPECT_EQ(aSVD.SingularValues->At(0), aSubnormal);

  aMatrix.ChangeAt(0, 0) = std::numeric_limits<double>::max();
  EXPECT_FALSE(MathLin::MatrixPower(aMatrix, 2.0).has_value());
  aMatrix.ChangeAt(0, 0) = 0.0;
  EXPECT_FALSE(MathLin::MatrixInvSqrt(aMatrix).has_value());
}

// ============================================================================
// Least squares tests
// ============================================================================

TEST(MathLin_LeastSquares_Test, SquareSystem)
{
  math_Matrix aMat(1, 2, 1, 2);
  aMat(1, 1) = 3.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;

  math_Vector aB(1, 2);
  aB(1) = 9.0;
  aB(2) = 8.0;

  auto aResult = MathLin::LeastSquares(aMat, aB, MathLin::LeastSquaresMethod::QR);

  ASSERT_TRUE(aResult.IsDone());

  // For square systems, residual should be near zero
  EXPECT_LT(*aResult.Residual, THE_TOLERANCE);
}

TEST(MathLin_LeastSquares_Test, Overdetermined)
{
  // 4x2 overdetermined system
  math_Matrix aMat(1, 4, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;
  aMat(3, 1) = 1.0;
  aMat(3, 2) = 3.0;
  aMat(4, 1) = 1.0;
  aMat(4, 2) = 4.0;

  // Perfect line: y = 1 + x
  math_Vector aB(1, 4);
  aB(1) = 2.0;
  aB(2) = 3.0;
  aB(3) = 4.0;
  aB(4) = 5.0;

  auto aResult = MathLin::LeastSquares(aMat, aB, MathLin::LeastSquaresMethod::QR);

  ASSERT_TRUE(aResult.IsDone());

  const math_Vector& aX = *aResult.Solution;

  // Solution should be approximately [1, 1] (intercept=1, slope=1)
  EXPECT_NEAR(aX.At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aX.At(1), 1.0, THE_TOLERANCE);

  // Residual should be near zero for consistent system
  EXPECT_LT(*aResult.Residual, THE_TOLERANCE);
}

TEST(MathLin_LeastSquares_Test, NoisyQuadraticFit)
{
  constexpr double THE_A[7][3] = {{1.0, -2.0, 4.0},
                                   {1.0, -1.0, 1.0},
                                   {1.0, -0.25, 0.0625},
                                   {1.0, 0.5, 0.25},
                                   {1.0, 1.0, 1.0},
                                   {1.0, 1.75, 3.0625},
                                   {1.0, 2.5, 6.25}};
  constexpr double THE_B[7] = {7.1, 2.4, 0.85, 1.2, 2.05, 4.8, 9.7};
  math_Matrix      aA(size_t{7}, size_t{3});
  math_Vector      aB(size_t{7});
  for (size_t i = 0; i < 7; ++i)
  {
    aB.ChangeAt(i) = THE_B[i];
    for (size_t j = 0; j < 3; ++j)
    {
      aA.ChangeAt(i, j) = THE_A[i][j];
    }
  }

  const MathLin::LeastSquaresResult aResult =
    MathLin::LeastSquares(aA, aB, MathLin::LeastSquaresMethod::QR);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Residual.has_value());
  ASSERT_TRUE(aResult.ResidualSq.has_value());
  EXPECT_EQ(aResult.Rank, 3u);

  constexpr double THE_EXPECTED_SOLUTION[3] = {0.74941012717216748,
                                                -0.18756729578259318,
                                                1.4926750303520839};
  for (size_t i = 0; i < 3; ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(i), THE_EXPECTED_SOLUTION[i], 1.0e-12);
  }
  EXPECT_NEAR(*aResult.Residual, 0.2774950969170783, 1.0e-12);
  EXPECT_NEAR(*aResult.ResidualSq, 0.077003528813018682, 1.0e-12);

  for (size_t j = 0; j < 3; ++j)
  {
    double aNormalResidual = 0.0;
    for (size_t i = 0; i < 7; ++i)
    {
      double aRowResidual = -aB.At(i);
      for (size_t k = 0; k < 3; ++k)
      {
        aRowResidual += aA.At(i, k) * aResult.Solution->At(k);
      }
      aNormalResidual += aA.At(i, j) * aRowResidual;
    }
    EXPECT_NEAR(aNormalResidual, 0.0, 1.0e-11);
  }
}

TEST(MathLin_LeastSquares_Test, MethodComparison)
{
  math_Matrix aMat = CreateRandom(5, 3);
  math_Vector aB(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aB(i) = static_cast<double>(i);
  }

  auto aResultNE  = MathLin::LeastSquares(aMat, aB, MathLin::LeastSquaresMethod::NormalEquations);
  auto aResultQR  = MathLin::LeastSquares(aMat, aB, MathLin::LeastSquaresMethod::QR);
  auto aResultSVD = MathLin::LeastSquares(aMat, aB, MathLin::LeastSquaresMethod::SVD);

  ASSERT_TRUE(aResultNE.IsDone());
  ASSERT_TRUE(aResultQR.IsDone());
  ASSERT_TRUE(aResultSVD.IsDone());

  // All methods should give similar results
  for (size_t i = 0; i < aResultNE.Solution->Size(); ++i)
  {
    EXPECT_NEAR(aResultNE.Solution->At(i), aResultQR.Solution->At(i), 1.0e-6);
    EXPECT_NEAR(aResultQR.Solution->At(i), aResultSVD.Solution->At(i), 1.0e-6);
  }
}

TEST(MathLin_LeastSquares_Test, WeightedLeastSquares)
{
  math_Matrix aMat(1, 3, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;
  aMat(3, 1) = 1.0;
  aMat(3, 2) = 3.0;

  math_Vector aB(1, 3);
  aB(1) = 2.0;
  aB(2) = 3.0;
  aB(3) = 4.5; // Slightly off from the line

  // Equal weights
  math_Vector aW1(1, 3, 1.0);
  auto        aResult1 = MathLin::WeightedLeastSquares(aMat, aB, aW1);

  // Higher weight on first two points
  math_Vector aW2(1, 3);
  aW2(1)        = 10.0;
  aW2(2)        = 10.0;
  aW2(3)        = 0.1;
  auto aResult2 = MathLin::WeightedLeastSquares(aMat, aB, aW2);

  ASSERT_TRUE(aResult1.IsDone());
  ASSERT_TRUE(aResult2.IsDone());

  // Weighted solution should fit first two points better
  // (Different weights should give different solutions)
  EXPECT_NE(aResult1.Solution->At(0), aResult2.Solution->At(0));
}

TEST(MathLin_LeastSquares_Test, RegularizedLeastSquares)
{
  // Ill-conditioned system
  math_Matrix aMat(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aMat(i, j) = 1.0 / (i + j - 1); // Hilbert matrix
    }
  }

  math_Vector aB(1, 3);
  aB(1) = 1.0;
  aB(2) = 0.5;
  aB(3) = 0.333;

  auto aResultNoReg = MathLin::LeastSquares(aMat, aB);
  auto aResultReg   = MathLin::RegularizedLeastSquares(aMat, aB, 0.01);

  ASSERT_TRUE(aResultNoReg.IsDone());
  ASSERT_TRUE(aResultReg.IsDone());

  // Regularized solution should have smaller norm
  double aNormNoReg = VectorNorm(*aResultNoReg.Solution);
  double aNormReg   = VectorNorm(*aResultReg.Solution);
  EXPECT_LT(aNormReg, aNormNoReg);
}

TEST(MathLin_LeastSquares_Test, ZeroRegularizationUsesSVDCutoff)
{
  math_Matrix aA(2, 2, 0.0);
  aA.ChangeAt(0, 0) = 1.0;
  aA.ChangeAt(1, 1) = std::numeric_limits<double>::epsilon();
  math_Vector aB(size_t{2}, 1.0);

  const MathLin::LeastSquaresResult aResult =
    MathLin::RegularizedLeastSquares(aA, aB, 0.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Rank, 1u);
  EXPECT_DOUBLE_EQ(aResult.Solution->At(0), 1.0);
  EXPECT_DOUBLE_EQ(aResult.Solution->At(1), 0.0);
}

TEST(MathLin_LeastSquares_Test, NonRepresentableResidualIsNumericalError)
{
  math_Matrix aA(2, 1, 0.0);
  math_Vector aB(size_t{2}, std::numeric_limits<double>::max());

  EXPECT_EQ(MathLin::LeastSquares(aA, aB, MathLin::LeastSquaresMethod::SVD).Status,
            MathUtils::Status::NumericalError);
  EXPECT_EQ(MathLin::RegularizedLeastSquares(aA, aB, 1.0).Status,
            MathUtils::Status::NumericalError);
}

TEST(MathLin_LeastSquares_Test, ArbitraryBoundsRankAndDiagnostics)
{
  math_Matrix aA(-2, 1, 5, 6);
  for (int i = 0; i < 4; ++i)
  {
    aA(-2 + i, 5) = 1.0;
    aA(-2 + i, 6) = static_cast<double>(i);
  }
  math_Vector aB(10, 13);
  for (int i = 0; i < 4; ++i)
  {
    aB(10 + i) = 2.0 + 3.0 * i;
  }
  const MathLin::LeastSquaresResult aResult =
    MathLin::LeastSquares(aA, aB, MathLin::LeastSquaresMethod::SVD);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Solution->Lower(), 0);
  EXPECT_EQ(aResult.Rank, 2);
  ASSERT_TRUE(aResult.ConditionNumber.has_value());
  EXPECT_TRUE(std::isfinite(*aResult.ConditionNumber));
  EXPECT_NEAR(aResult.Solution->At(0), 2.0, 1.0e-12);
  EXPECT_NEAR(aResult.Solution->At(1), 3.0, 1.0e-12);
  EXPECT_NEAR(*aResult.Residual, 0.0, 1.0e-12);
}

TEST(MathLin_LeastSquares_Test, GCVSelectionAndInvalidInput)
{
  math_Matrix aA(1, 8, 1, 3);
  math_Vector aB(-4, 3);
  for (int i = 1; i <= 8; ++i)
  {
    const double aX = static_cast<double>(i - 1) / 7.0;
    aA(i, 1)        = 1.0;
    aA(i, 2)        = aX;
    aA(i, 3)        = aX * aX;
    aB(-5 + i)      = 1.0 + 2.0 * aX + ((i % 2 == 0) ? 0.3 : -0.3);
  }
  const std::optional<double> aLambda =
    MathLin::OptimalRegularization(aA, aB, 1.0e-8, 1.0e1, 30);
  ASSERT_TRUE(aLambda.has_value());
  EXPECT_TRUE(std::isfinite(*aLambda));
  EXPECT_GE(*aLambda, 1.0e-8);
  EXPECT_LE(*aLambda, 1.0e1);
  EXPECT_FALSE(MathLin::OptimalRegularization(aA, aB, -1.0, 1.0, 10).has_value());

  aB.Init(std::numeric_limits<double>::max());
  EXPECT_TRUE(MathLin::OptimalRegularization(aA, aB, 1.0e-8, 1.0e1, 30).has_value());
}

// ============================================================================
// Comparison with old API tests
// ============================================================================

TEST(MathLin_Test, CompareWithOldAPI_SVD)
{
  math_Matrix aMat(1, 3, 1, 3);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 2.0;
  aMat(1, 3) = 3.0;
  aMat(2, 1) = 4.0;
  aMat(2, 2) = 5.0;
  aMat(2, 3) = 6.0;
  aMat(3, 1) = 7.0;
  aMat(3, 2) = 8.0;
  aMat(3, 3) = 10.0;

  math_Vector aB(1, 3);
  aB(1) = 1.0;
  aB(2) = 2.0;
  aB(3) = 3.0;

  // Old API
  math_SVD    anOldSVD(aMat);
  math_Vector anOldSol(1, 3);
  anOldSVD.Solve(aB, anOldSol);

  // New API
  auto aNewResult = MathLin::SolveSVD(aMat, aB);

  ASSERT_TRUE(anOldSVD.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Solutions should match
  for (size_t i = 0; i < aNewResult.Solution->Size(); ++i)
  {
    EXPECT_NEAR(anOldSol.At(i), aNewResult.Solution->At(i), 1.0e-8);
  }
}

TEST(MathLin_Test, CompareWithOldAPI_Householder)
{
  math_Matrix aMat(1, 3, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;
  aMat(3, 1) = 1.0;
  aMat(3, 2) = 3.0;

  math_Vector aB(1, 3);
  aB(1) = 2.0;
  aB(2) = 3.0;
  aB(3) = 4.0;

  // Old API
  math_Matrix aBMat(1, 3, 1, 1);
  aBMat(1, 1) = aB(1);
  aBMat(2, 1) = aB(2);
  aBMat(3, 1) = aB(3);
  math_Householder anOldHH(aMat, aBMat);

  // New API
  auto aNewResult = MathLin::SolveQR(aMat, aB);

  ASSERT_TRUE(anOldHH.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Get the old solution using the proper API
  math_Vector anOldSol(1, 2);
  anOldHH.Value(anOldSol, 1);

  // Solutions should match
  for (size_t i = 0; i < aNewResult.Solution->Size(); ++i)
  {
    EXPECT_NEAR(anOldSol.At(i), aNewResult.Solution->At(i), 1.0e-8);
  }
}

TEST(MathLin_Test, CompareWithOldAPI_Jacobi)
{
  math_Matrix aMat(1, 3, 1, 3);
  aMat(1, 1) = 3.0;
  aMat(1, 2) = 1.0;
  aMat(1, 3) = 0.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 3.0;
  aMat(2, 3) = 1.0;
  aMat(3, 1) = 0.0;
  aMat(3, 2) = 1.0;
  aMat(3, 3) = 3.0;

  // Old API
  math_Jacobi anOldJacobi(aMat);

  // New API
  auto aNewResult = MathLin::Jacobi(aMat, true);

  ASSERT_TRUE(anOldJacobi.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Eigenvalues should match (both sorted descending)
  const math_Vector& aNewEig = *aNewResult.EigenValues;
  for (size_t i = 0; i < aNewEig.Size(); ++i)
  {
    EXPECT_NEAR(anOldJacobi.Value(static_cast<int>(i) + 1), aNewEig.At(i), 1.0e-8);
  }
}

TEST(MathLin_Test, CompareWithOldAPI_GaussLeastSquare)
{
  math_Matrix aMat(1, 4, 1, 2);
  aMat(1, 1) = 1.0;
  aMat(1, 2) = 1.0;
  aMat(2, 1) = 1.0;
  aMat(2, 2) = 2.0;
  aMat(3, 1) = 1.0;
  aMat(3, 2) = 3.0;
  aMat(4, 1) = 1.0;
  aMat(4, 2) = 4.0;

  math_Vector aB(1, 4);
  aB(1) = 2.0;
  aB(2) = 3.0;
  aB(3) = 4.0;
  aB(4) = 5.0;

  // Old API
  math_GaussLeastSquare anOldLS(aMat);
  math_Vector           anOldSol(1, 2);
  anOldLS.Solve(aB, anOldSol);

  // New API
  auto aNewResult = MathLin::LeastSquares(aMat, aB);

  ASSERT_TRUE(anOldLS.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Solutions should match
  for (size_t i = 0; i < aNewResult.Solution->Size(); ++i)
  {
    EXPECT_NEAR(anOldSol.At(i), aNewResult.Solution->At(i), 1.0e-8);
  }
}
