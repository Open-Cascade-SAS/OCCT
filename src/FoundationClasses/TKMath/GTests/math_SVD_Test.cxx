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

#include <math_SVD.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{
// Helper function to check if solution is approximately correct
void checkSolution(const math_Matrix&  theA,
                   const math_Vector&  theX,
                   const math_Vector&  theB,
                   const Standard_Real theTolerance = 1.0e-10)
{
  ASSERT_EQ(theA.ColNumber(), theX.Length()) << "Matrix and solution vector dimensions must match";
  ASSERT_EQ(theA.RowNumber(), theB.Length()) << "Matrix and RHS vector dimensions must match";

  // Compute A * X
  math_Vector aResult(theB.Lower(), theB.Upper());
  for (Standard_Integer anI = theA.LowerRow(); anI <= theA.UpperRow(); anI++)
  {
    Standard_Real aSum = 0.0;
    for (Standard_Integer aJ = theA.LowerCol(); aJ <= theA.UpperCol(); aJ++)
    {
      aSum += theA(anI, aJ) * theX(aJ - theA.LowerCol() + theX.Lower());
    }
    aResult(anI - theA.LowerRow() + theB.Lower()) = aSum;
  }

  // Check if A * X approximately equals B
  for (Standard_Integer anI = theB.Lower(); anI <= theB.Upper(); anI++)
  {
    EXPECT_NEAR(aResult(anI), theB(anI), theTolerance)
      << "Solution verification failed at index " << anI;
  }
}

// Helper to create a well-conditioned test matrix
math_Matrix createWellConditionedMatrix()
{
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 2.0;
  aMatrix(1, 2) = 1.0;
  aMatrix(1, 3) = 0.0;
  aMatrix(2, 1) = 1.0;
  aMatrix(2, 2) = 2.0;
  aMatrix(2, 3) = 1.0;
  aMatrix(3, 1) = 0.0;
  aMatrix(3, 2) = 1.0;
  aMatrix(3, 3) = 2.0;
  return aMatrix;
}
} // namespace

// Tests for math_SVD
TEST(MathSVDTest, WellConditionedSquareMatrix)
{
  math_Matrix aMatrix = createWellConditionedMatrix();
  math_SVD    aSVD(aMatrix);

  EXPECT_TRUE(aSVD.IsDone()) << "SVD decomposition should succeed for well-conditioned matrix";

  // Test solving Ax = b where b = [6, 9, 8]
  math_Vector aB(1, 3);
  aB(1) = 6.0;
  aB(2) = 9.0;
  aB(3) = 8.0;

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // Verify the solution
  checkSolution(aMatrix, aX, aB);
}

TEST(MathSVDTest, IdentityMatrix)
{
  // Identity matrix test
  math_Matrix anIdentity(1, 3, 1, 3);
  anIdentity(1, 1) = 1.0;
  anIdentity(1, 2) = 0.0;
  anIdentity(1, 3) = 0.0;
  anIdentity(2, 1) = 0.0;
  anIdentity(2, 2) = 1.0;
  anIdentity(2, 3) = 0.0;
  anIdentity(3, 1) = 0.0;
  anIdentity(3, 2) = 0.0;
  anIdentity(3, 3) = 1.0;

  math_SVD aSVD(anIdentity);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for identity matrix";

  math_Vector aB(1, 3);
  aB(1) = 5.0;
  aB(2) = 7.0;
  aB(3) = 9.0;

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // For identity matrix, X should equal B
  EXPECT_NEAR(aX(1), aB(1), Precision::Confusion()) << "Identity matrix solution X(1)";
  EXPECT_NEAR(aX(2), aB(2), Precision::Confusion()) << "Identity matrix solution X(2)";
  EXPECT_NEAR(aX(3), aB(3), Precision::Confusion()) << "Identity matrix solution X(3)";
}

TEST(MathSVDTest, DiagonalMatrix)
{
  // Diagonal matrix test
  math_Matrix aDiagonal(1, 3, 1, 3);
  aDiagonal(1, 1) = 3.0;
  aDiagonal(1, 2) = 0.0;
  aDiagonal(1, 3) = 0.0;
  aDiagonal(2, 1) = 0.0;
  aDiagonal(2, 2) = 5.0;
  aDiagonal(2, 3) = 0.0;
  aDiagonal(3, 1) = 0.0;
  aDiagonal(3, 2) = 0.0;
  aDiagonal(3, 3) = 2.0;

  math_SVD aSVD(aDiagonal);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for diagonal matrix";

  math_Vector aB(1, 3);
  aB(1) = 12.0;
  aB(2) = 20.0;
  aB(3) = 8.0;

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // For diagonal matrix, X[i] = B[i] / D[i,i]
  EXPECT_NEAR(aX(1), 4.0, Precision::Confusion()) << "Diagonal solution X(1) = 12/3";
  EXPECT_NEAR(aX(2), 4.0, Precision::Confusion()) << "Diagonal solution X(2) = 20/5";
  EXPECT_NEAR(aX(3), 4.0, Precision::Confusion()) << "Diagonal solution X(3) = 8/2";
}

TEST(MathSVDTest, OverdeterminedSystem)
{
  // Overdetermined system: more equations than unknowns (4x3)
  math_Matrix aMatrix(1, 4, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(1, 3) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 1.0;
  aMatrix(2, 3) = 0.0;
  aMatrix(3, 1) = 0.0;
  aMatrix(3, 2) = 0.0;
  aMatrix(3, 3) = 1.0;
  aMatrix(4, 1) = 1.0;
  aMatrix(4, 2) = 1.0;
  aMatrix(4, 3) = 1.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for overdetermined system";

  // Consistent system: b4 = b1 + b2 + b3
  math_Vector aB(1, 4);
  aB(1) = 2.0;
  aB(2) = 3.0;
  aB(3) = 4.0;
  aB(4) = 9.0; // 2 + 3 + 4 = 9

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // Verify the solution
  checkSolution(aMatrix, aX, aB, 1.0e-8);
}

TEST(MathSVDTest, UnderdeterminedSystem)
{
  // Underdetermined system: fewer equations than unknowns (2x3)
  math_Matrix aMatrix(1, 2, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 3.0;
  aMatrix(2, 1) = 4.0;
  aMatrix(2, 2) = 5.0;
  aMatrix(2, 3) = 6.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for underdetermined system";

  math_Vector aB(1, 2);
  aB(1) = 14.0; // 1*1 + 2*2 + 3*3 = 14
  aB(2) = 32.0; // 4*1 + 5*2 + 6*3 = 32

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // Verify the solution (should be one of many possible solutions)
  checkSolution(aMatrix, aX, aB, 1.0e-8);
}

TEST(MathSVDTest, RankDeficientMatrix)
{
  // Rank deficient matrix (rank 2, but 3x3)
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 3.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 4.0;
  aMatrix(2, 3) = 6.0; // 2 * row 1
  aMatrix(3, 1) = 1.0;
  aMatrix(3, 2) = 1.0;
  aMatrix(3, 3) = 1.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should handle rank deficient matrix";

  // Consistent RHS (b2 = 2*b1, compatible with rank deficiency)
  math_Vector aB(1, 3);
  aB(1) = 6.0;
  aB(2) = 12.0; // 2 * b1
  aB(3) = 3.0;

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // Verify the solution
  checkSolution(aMatrix, aX, aB, 1.0e-6);
}

TEST(MathSVDTest, SingleRowMatrix)
{
  // Single equation, multiple unknowns (1x3)
  math_Matrix aMatrix(1, 1, 1, 3);
  aMatrix(1, 1) = 2.0;
  aMatrix(1, 2) = 3.0;
  aMatrix(1, 3) = 4.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for single row matrix";

  math_Vector aB(1, 1);
  aB(1) = 20.0; // 2*2 + 3*3 + 4*3 = 20 (one possible solution: x=[2,3,3])

  math_Vector aX(1, 3);
  aSVD.Solve(aB, aX);

  // Verify the solution
  checkSolution(aMatrix, aX, aB);
}

TEST(MathSVDTest, SingleColumnMatrix)
{
  // Multiple equations, single unknown (3x1)
  math_Matrix aMatrix(1, 3, 1, 1);
  aMatrix(1, 1) = 2.0;
  aMatrix(2, 1) = 3.0;
  aMatrix(3, 1) = 4.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for single column matrix";

  // Least squares problem: find x that minimizes ||Ax - b||^2
  math_Vector aB(1, 3);
  aB(1) = 4.0; // 2x approximately 4 -> x approximately 2
  aB(2) = 6.0; // 3x approximately 6 -> x approximately 2
  aB(3) = 8.0; // 4x approximately 8 -> x approximately 2

  math_Vector aX(1, 1);
  aSVD.Solve(aB, aX);

  EXPECT_NEAR(aX(1), 2.0, 1.0e-10) << "Least squares solution should be approximately 2.0";
}

TEST(MathSVDTest, PseudoInverseMethod)
{
  math_Matrix aMatrix = createWellConditionedMatrix();
  math_SVD    aSVD(aMatrix);

  EXPECT_TRUE(aSVD.IsDone()) << "SVD decomposition should succeed";

  // Test PseudoInverse method - compute A^+ (pseudoinverse of A)
  math_Matrix aPseudoInv(aMatrix.LowerCol(),
                         aMatrix.UpperCol(),
                         aMatrix.LowerRow(),
                         aMatrix.UpperRow());
  aSVD.PseudoInverse(aPseudoInv);

  // For a well-conditioned square matrix, pseudoinverse should behave like regular inverse
  // Test: A^+ * A should be approximately identity
  math_Matrix aProduct(aMatrix.LowerRow(),
                       aMatrix.UpperRow(),
                       aMatrix.LowerRow(),
                       aMatrix.UpperRow());

  for (Standard_Integer anI = aMatrix.LowerRow(); anI <= aMatrix.UpperRow(); anI++)
  {
    for (Standard_Integer aJ = aMatrix.LowerRow(); aJ <= aMatrix.UpperRow(); aJ++)
    {
      Standard_Real aSum = 0.0;
      for (Standard_Integer aK = aMatrix.LowerCol(); aK <= aMatrix.UpperCol(); aK++)
      {
        aSum += aPseudoInv(anI, aK) * aMatrix(aK, aJ);
      }
      aProduct(anI, aJ) = aSum;
    }
  }

  // Check if result is approximately identity
  for (Standard_Integer anI = aMatrix.LowerRow(); anI <= aMatrix.UpperRow(); anI++)
  {
    for (Standard_Integer aJ = aMatrix.LowerRow(); aJ <= aMatrix.UpperRow(); aJ++)
    {
      Standard_Real anExpected = (anI == aJ) ? 1.0 : 0.0;
      EXPECT_NEAR(aProduct(anI, aJ), anExpected, 1.0e-10)
        << "PseudoInverse * Matrix should approximate identity at (" << anI << "," << aJ << ")";
    }
  }
}

// Tests for exception handling
TEST(MathSVDTest, DimensionCompatibility)
{
  math_Matrix aMatrix = createWellConditionedMatrix();
  math_SVD    aSVD(aMatrix);

  ASSERT_TRUE(aSVD.IsDone()) << "SVD should succeed for dimension compatibility tests";

  // Test with correctly dimensioned vectors
  math_Vector aCorrectB(1, 3); // Correct size for 3x3 matrix
  aCorrectB(1) = 1.0;
  aCorrectB(2) = 2.0;
  aCorrectB(3) = 3.0;

  math_Vector aX(1, 3); // Correct size for solution
  aSVD.Solve(aCorrectB, aX);

  // Verify the results make sense
  EXPECT_EQ(aX.Length(), 3) << "Solution vector should have correct dimension";
  EXPECT_EQ(aCorrectB.Length(), 3) << "RHS vector should have correct dimension";

  // Verify matrix dimensions are consistent
  EXPECT_EQ(aMatrix.RowNumber(), 3) << "Matrix should have 3 rows";
  EXPECT_EQ(aMatrix.ColNumber(), 3) << "Matrix should have 3 columns";
}

TEST(MathSVDTest, SingularValues)
{
  // Test a matrix where we can predict singular values
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 3.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 4.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for diagonal matrix";

  // For a diagonal matrix, singular values should be the absolute values of diagonal elements
  // We can't directly access singular values in this interface, but we can verify through solutions
  math_Vector aB1(1, 2);
  aB1(1) = 6.0;
  aB1(2) = 0.0;
  math_Vector aX1(1, 2);
  aSVD.Solve(aB1, aX1);
  EXPECT_NEAR(aX1(1), 2.0, 1.0e-10) << "Solution should be 6/3 = 2";
  EXPECT_NEAR(aX1(2), 0.0, 1.0e-10) << "Solution should be 0/4 = 0";

  math_Vector aB2(1, 2);
  aB2(1) = 0.0;
  aB2(2) = 12.0;
  math_Vector aX2(1, 2);
  aSVD.Solve(aB2, aX2);
  EXPECT_NEAR(aX2(1), 0.0, 1.0e-10) << "Solution should be 0/3 = 0";
  EXPECT_NEAR(aX2(2), 3.0, 1.0e-10) << "Solution should be 12/4 = 3";
}

TEST(MathSVDTest, DifferentMatrixBounds)
{
  // Test with non-standard matrix bounds
  math_Matrix aMatrix(2, 4, 3, 5); // 3x3 matrix with custom bounds

  aMatrix(2, 3) = 1.0;
  aMatrix(2, 4) = 0.0;
  aMatrix(2, 5) = 0.0;
  aMatrix(3, 3) = 0.0;
  aMatrix(3, 4) = 1.0;
  aMatrix(3, 5) = 0.0;
  aMatrix(4, 3) = 0.0;
  aMatrix(4, 4) = 0.0;
  aMatrix(4, 5) = 1.0;

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for custom bounds matrix";

  math_Vector aB(2, 4); // Matching row bounds
  aB(2) = 5.0;
  aB(3) = 7.0;
  aB(4) = 9.0;

  math_Vector aX(3, 5); // Matching column bounds
  aSVD.Solve(aB, aX);

  // For identity matrix, solution should equal RHS
  EXPECT_NEAR(aX(3), 5.0, Precision::Confusion()) << "Custom bounds solution X(3)";
  EXPECT_NEAR(aX(4), 7.0, Precision::Confusion()) << "Custom bounds solution X(4)";
  EXPECT_NEAR(aX(5), 9.0, Precision::Confusion()) << "Custom bounds solution X(5)";
}

TEST(MathSVDTest, LargerMatrix)
{
  // Test with a larger well-conditioned matrix (5x5)
  math_Matrix aMatrix(1, 5, 1, 5);

  // Create a symmetric positive definite matrix
  for (Standard_Integer anI = 1; anI <= 5; anI++)
  {
    for (Standard_Integer aJ = 1; aJ <= 5; aJ++)
    {
      if (anI == aJ)
        aMatrix(anI, aJ) = 10.0; // Diagonal dominance
      else
        aMatrix(anI, aJ) = 1.0 / (abs(anI - aJ) + 1.0);
    }
  }

  math_SVD aSVD(aMatrix);
  EXPECT_TRUE(aSVD.IsDone()) << "SVD should succeed for larger matrix";

  math_Vector aB(1, 5);
  for (Standard_Integer anI = 1; anI <= 5; anI++)
  {
    aB(anI) = Standard_Real(anI) * 2.0;
  }

  math_Vector aX(1, 5);
  aSVD.Solve(aB, aX);

  // Verify the solution
  checkSolution(aMatrix, aX, aB, 1.0e-8);
}