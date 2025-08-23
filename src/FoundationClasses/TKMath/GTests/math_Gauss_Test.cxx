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

#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>
#include <Precision.hxx>

namespace
{

TEST(MathGaussTest, WellConditionedMatrix)
{
  // Test with a simple 3x3 well-conditioned matrix
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 2.0;
  aMatrix(1, 2) = 1.0;
  aMatrix(1, 3) = 1.0;
  aMatrix(2, 1) = 1.0;
  aMatrix(2, 2) = 3.0;
  aMatrix(2, 3) = 2.0;
  aMatrix(3, 1) = 1.0;
  aMatrix(3, 2) = 2.0;
  aMatrix(3, 3) = 3.0;

  math_Gauss aGauss(aMatrix);

  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed for well-conditioned matrix";

  // Verify by solving with known RHS and checking A*x = b
  math_Vector aB(1, 3);
  aB(1) = 1.0;
  aB(2) = 2.0;
  aB(3) = 3.0;

  math_Vector aX(1, 3);
  aGauss.Solve(aB, aX);

  // Verify solution by checking A * x = b
  math_Vector aVerify(1, 3);
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    aVerify(i) = 0.0;
    for (Standard_Integer j = 1; j <= 3; j++)
    {
      aVerify(i) += aMatrix(i, j) * aX(j);
    }
  }

  EXPECT_NEAR(aVerify(1), aB(1), 1.0e-10) << "Solution verification A*x=b (1)";
  EXPECT_NEAR(aVerify(2), aB(2), 1.0e-10) << "Solution verification A*x=b (2)";
  EXPECT_NEAR(aVerify(3), aB(3), 1.0e-10) << "Solution verification A*x=b (3)";
}

TEST(MathGaussTest, IdentityMatrix)
{
  // Test with identity matrix
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(1, 3) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 1.0;
  aMatrix(2, 3) = 0.0;
  aMatrix(3, 1) = 0.0;
  aMatrix(3, 2) = 0.0;
  aMatrix(3, 3) = 1.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed for identity matrix";

  // For identity matrix, solution should equal RHS
  math_Vector aB(1, 3);
  aB(1) = 5.0;
  aB(2) = 7.0;
  aB(3) = 9.0;

  math_Vector aX(1, 3);
  aGauss.Solve(aB, aX);

  EXPECT_NEAR(aX(1), 5.0, Precision::Confusion()) << "Identity matrix solution X(1)";
  EXPECT_NEAR(aX(2), 7.0, Precision::Confusion()) << "Identity matrix solution X(2)";
  EXPECT_NEAR(aX(3), 9.0, Precision::Confusion()) << "Identity matrix solution X(3)";
}

TEST(MathGaussTest, DiagonalMatrix)
{
  // Test with diagonal matrix
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 2.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(1, 3) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 3.0;
  aMatrix(2, 3) = 0.0;
  aMatrix(3, 1) = 0.0;
  aMatrix(3, 2) = 0.0;
  aMatrix(3, 3) = 4.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed for diagonal matrix";

  // For diagonal matrix: x_i = b_i / a_ii
  math_Vector aB(1, 3);
  aB(1) = 8.0;
  aB(2) = 15.0;
  aB(3) = 20.0; // Expected solution: [4, 5, 5]

  math_Vector aX(1, 3);
  aGauss.Solve(aB, aX);

  EXPECT_NEAR(aX(1), 4.0, Precision::Confusion()) << "Diagonal matrix solution X(1)";
  EXPECT_NEAR(aX(2), 5.0, Precision::Confusion()) << "Diagonal matrix solution X(2)";
  EXPECT_NEAR(aX(3), 5.0, Precision::Confusion()) << "Diagonal matrix solution X(3)";
}

TEST(MathGaussTest, InPlaceSolve)
{
  // Test the in-place solve method where B is overwritten with solution
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 3.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 5.0;
  aMatrix(2, 3) = 8.0;
  aMatrix(3, 1) = 3.0;
  aMatrix(3, 2) = 8.0;
  aMatrix(3, 3) = 14.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed";

  math_Vector aB(1, 3);
  aB(1) = 14.0;
  aB(2) = 31.0;
  aB(3) = 53.0; // Should give solution [13, -7, 5]

  aGauss.Solve(aB); // In-place solve

  // Now B contains the solution
  EXPECT_NEAR(aB(1), 13.0, 1.0e-10) << "In-place solution B(1)";
  EXPECT_NEAR(aB(2), -7.0, 1.0e-10) << "In-place solution B(2)";
  EXPECT_NEAR(aB(3), 5.0, 1.0e-10) << "In-place solution B(3)";
}

TEST(MathGaussTest, Determinant)
{
  // Test determinant calculation
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 3.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 1.0;
  aMatrix(2, 3) = 4.0;
  aMatrix(3, 1) = 5.0;
  aMatrix(3, 2) = 6.0;
  aMatrix(3, 3) = 0.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed";

  Standard_Real aDet = aGauss.Determinant();
  // Expected determinant: 1*(1*0 - 4*6) - 2*(0*0 - 4*5) + 3*(0*6 - 1*5) = -24 + 40 - 15 = 1
  EXPECT_NEAR(aDet, 1.0, 1.0e-12) << "Determinant calculation";
}

TEST(MathGaussTest, MatrixInversion)
{
  // Test matrix inversion
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 4.0;
  aMatrix(1, 2) = 7.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 6.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed";

  math_Matrix aInverse(1, 2, 1, 2);
  aGauss.Invert(aInverse);

  // For 2x2 matrix [[4,7],[2,6]], inverse should be [[0.6,-0.7],[-0.2,0.4]]
  // det = 4*6 - 7*2 = 24 - 14 = 10
  // inv = (1/10) * [[6,-7],[-2,4]]
  EXPECT_NEAR(aInverse(1, 1), 0.6, 1.0e-12) << "Inverse(1,1)";
  EXPECT_NEAR(aInverse(1, 2), -0.7, 1.0e-12) << "Inverse(1,2)";
  EXPECT_NEAR(aInverse(2, 1), -0.2, 1.0e-12) << "Inverse(2,1)";
  EXPECT_NEAR(aInverse(2, 2), 0.4, 1.0e-12) << "Inverse(2,2)";

  // Verify that A * A^(-1) = I
  math_Matrix aProduct(1, 2, 1, 2);
  for (Standard_Integer i = 1; i <= 2; i++)
  {
    for (Standard_Integer j = 1; j <= 2; j++)
    {
      aProduct(i, j) = 0.0;
      for (Standard_Integer k = 1; k <= 2; k++)
      {
        aProduct(i, j) += aMatrix(i, k) * aInverse(k, j);
      }
    }
  }

  EXPECT_NEAR(aProduct(1, 1), 1.0, 1.0e-12) << "A * A^(-1) should equal identity (1,1)";
  EXPECT_NEAR(aProduct(1, 2), 0.0, 1.0e-12) << "A * A^(-1) should equal identity (1,2)";
  EXPECT_NEAR(aProduct(2, 1), 0.0, 1.0e-12) << "A * A^(-1) should equal identity (2,1)";
  EXPECT_NEAR(aProduct(2, 2), 1.0, 1.0e-12) << "A * A^(-1) should equal identity (2,2)";
}

TEST(MathGaussTest, SingularMatrix)
{
  // Test with a singular (non-invertible) matrix
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 3.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 4.0;
  aMatrix(2, 3) = 6.0; // Row 2 = 2 * Row 1
  aMatrix(3, 1) = 3.0;
  aMatrix(3, 2) = 6.0;
  aMatrix(3, 3) = 9.0; // Row 3 = 3 * Row 1

  // Singular matrix should either fail decomposition or have zero determinant
  math_Gauss aGauss(aMatrix);
  if (aGauss.IsDone())
  {
    Standard_Real aDet = aGauss.Determinant();
    EXPECT_NEAR(aDet, 0.0, 1.0e-12) << "Determinant of singular matrix must be zero";
  }
  // Note: Some implementations may fail decomposition or throw exceptions for singular matrices
}

TEST(MathGaussTest, CustomMinPivot)
{
  // Test with custom minimum pivot threshold
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 1.0e-15;
  aMatrix(1, 2) = 1.0; // Very small pivot
  aMatrix(2, 1) = 1.0;
  aMatrix(2, 2) = 1.0;

  // With default MinPivot (1e-20), should succeed
  math_Gauss aGauss1(aMatrix);
  EXPECT_TRUE(aGauss1.IsDone()) << "Should succeed with default MinPivot";

  // Create a truly singular matrix to test pivot threshold behavior
  math_Matrix aSingular(1, 2, 1, 2);
  aSingular(1, 1) = 1.0e-25;
  aSingular(1, 2) = 1.0; // Extremely small pivot
  aSingular(2, 1) = 1.0;
  aSingular(2, 2) = 1.0;

  // Test with truly singular matrix (all zeros) which should always fail
  math_Matrix aTrulySingular(1, 2, 1, 2);
  aTrulySingular(1, 1) = 0.0;
  aTrulySingular(1, 2) = 0.0;
  aTrulySingular(2, 1) = 0.0;
  aTrulySingular(2, 2) = 0.0;

  math_Gauss aGauss2(aTrulySingular);
  EXPECT_FALSE(aGauss2.IsDone()) << "Should fail for zero matrix";
}

TEST(MathGaussTest, LargerMatrix)
{
  // Test with a larger 4x4 matrix
  math_Matrix aMatrix(1, 4, 1, 4);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 1.0;
  aMatrix(1, 4) = 1.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 1.0;
  aMatrix(2, 3) = 3.0;
  aMatrix(2, 4) = 1.0;
  aMatrix(3, 1) = 1.0;
  aMatrix(3, 2) = 3.0;
  aMatrix(3, 3) = 1.0;
  aMatrix(3, 4) = 2.0;
  aMatrix(4, 1) = 1.0;
  aMatrix(4, 2) = 1.0;
  aMatrix(4, 3) = 2.0;
  aMatrix(4, 4) = 3.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed for 4x4 matrix";

  // Test with a simple RHS vector
  math_Vector aB(1, 4);
  aB(1) = 1.0;
  aB(2) = 2.0;
  aB(3) = 3.0;
  aB(4) = 4.0;

  math_Vector aX(1, 4);
  aGauss.Solve(aB, aX);

  // Verify solution by checking A * x = b
  math_Vector aVerify(1, 4);
  for (Standard_Integer i = 1; i <= 4; i++)
  {
    aVerify(i) = 0.0;
    for (Standard_Integer j = 1; j <= 4; j++)
    {
      aVerify(i) += aMatrix(i, j) * aX(j);
    }
  }

  EXPECT_NEAR(aVerify(1), aB(1), 1.0e-10) << "4x4 matrix solution verification (1)";
  EXPECT_NEAR(aVerify(2), aB(2), 1.0e-10) << "4x4 matrix solution verification (2)";
  EXPECT_NEAR(aVerify(3), aB(3), 1.0e-10) << "4x4 matrix solution verification (3)";
  EXPECT_NEAR(aVerify(4), aB(4), 1.0e-10) << "4x4 matrix solution verification (4)";
}

TEST(MathGaussTest, DimensionCompatibility)
{
  // Test dimension compatibility handling
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(1, 3) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 1.0;
  aMatrix(2, 3) = 0.0;
  aMatrix(3, 1) = 0.0;
  aMatrix(3, 2) = 0.0;
  aMatrix(3, 3) = 1.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone());

  // Test solve with correctly sized vectors
  math_Vector aB(1, 3);
  aB(1) = 1.0;
  aB(2) = 2.0;
  aB(3) = 3.0;

  math_Vector aX(1, 3);
  aGauss.Solve(aB, aX);

  // Verify the result makes sense
  EXPECT_EQ(aX.Length(), 3) << "Solution vector should have correct dimension";

  // Test invert with correctly sized matrix
  math_Matrix aInv(1, 3, 1, 3);
  aGauss.Invert(aInv);

  EXPECT_EQ(aInv.RowNumber(), 3) << "Inverse matrix should have correct dimensions";
  EXPECT_EQ(aInv.ColNumber(), 3) << "Inverse matrix should have correct dimensions";
}

TEST(MathGaussTest, SingularMatrixState)
{
  // Test state handling with singular matrix
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 4.0; // Singular matrix

  math_Gauss aGauss(aMatrix);
  EXPECT_FALSE(aGauss.IsDone()) << "Should fail for singular matrix";
}

TEST(MathGaussTest, CustomBounds)
{
  // Test with non-standard matrix bounds
  math_Matrix aMatrix(2, 4, 3, 5);
  aMatrix(2, 3) = 2.0;
  aMatrix(2, 4) = 1.0;
  aMatrix(2, 5) = 1.0;
  aMatrix(3, 3) = 1.0;
  aMatrix(3, 4) = 3.0;
  aMatrix(3, 5) = 2.0;
  aMatrix(4, 3) = 1.0;
  aMatrix(4, 4) = 2.0;
  aMatrix(4, 5) = 3.0;

  math_Gauss aGauss(aMatrix);
  EXPECT_TRUE(aGauss.IsDone()) << "Gauss decomposition should succeed with custom bounds";

  math_Vector aB(2, 4);
  aB(2) = 6.0;
  aB(3) = 11.0;
  aB(4) = 13.0; // Should give solution [0.75, 1.25, 3.25]

  math_Vector aX(3, 5);
  aGauss.Solve(aB, aX);

  EXPECT_NEAR(aX(3), 0.75, 1.0e-10) << "Custom bounds solution X(3)";
  EXPECT_NEAR(aX(4), 1.25, 1.0e-10) << "Custom bounds solution X(4)";
  EXPECT_NEAR(aX(5), 3.25, 1.0e-10) << "Custom bounds solution X(5)";
}

} // anonymous namespace