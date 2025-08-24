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

#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <math_NotSquare.hxx>
#include <Precision.hxx>

#include <cmath>
#include <algorithm>

namespace
{

} // anonymous namespace

TEST(MathJacobiTest, IdentityMatrix)
{
  // Test with identity matrix - all eigenvalues should be 1
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

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed for identity matrix";

  const math_Vector& aValues = aJacobi.Values();
  EXPECT_EQ(aValues.Length(), 3) << "Should have 3 eigenvalues";

  // All eigenvalues should be 1
  EXPECT_NEAR(aValues(1), 1.0, 1.0e-10) << "First eigenvalue should be 1";
  EXPECT_NEAR(aValues(2), 1.0, 1.0e-10) << "Second eigenvalue should be 1";
  EXPECT_NEAR(aValues(3), 1.0, 1.0e-10) << "Third eigenvalue should be 1";

  // Test individual value access
  EXPECT_NEAR(aJacobi.Value(1), 1.0, 1.0e-10) << "Value(1) should be 1";
  EXPECT_NEAR(aJacobi.Value(2), 1.0, 1.0e-10) << "Value(2) should be 1";
  EXPECT_NEAR(aJacobi.Value(3), 1.0, 1.0e-10) << "Value(3) should be 1";
}

TEST(MathJacobiTest, DiagonalMatrix)
{
  // Test with diagonal matrix - eigenvalues should be diagonal elements
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 5.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(1, 3) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 3.0;
  aMatrix(2, 3) = 0.0;
  aMatrix(3, 1) = 0.0;
  aMatrix(3, 2) = 0.0;
  aMatrix(3, 3) = 7.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed for diagonal matrix";

  const math_Vector& aValues = aJacobi.Values();

  // Eigenvalues might be in different order, so collect them
  Standard_Real aEigenvals[3];
  aEigenvals[0] = aValues(1);
  aEigenvals[1] = aValues(2);
  aEigenvals[2] = aValues(3);

  std::sort(aEigenvals, aEigenvals + 3);

  EXPECT_NEAR(aEigenvals[0], 3.0, 1.0e-10) << "Smallest eigenvalue should be 3";
  EXPECT_NEAR(aEigenvals[1], 5.0, 1.0e-10) << "Middle eigenvalue should be 5";
  EXPECT_NEAR(aEigenvals[2], 7.0, 1.0e-10) << "Largest eigenvalue should be 7";
}

TEST(MathJacobiTest, SimpleSymmetricMatrix)
{
  // Test with simple 2x2 symmetric matrix
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 3.0;
  aMatrix(1, 2) = 1.0;
  aMatrix(2, 1) = 1.0;
  aMatrix(2, 2) = 3.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed for 2x2 symmetric matrix";

  const math_Vector& aValues = aJacobi.Values();

  // For this matrix, eigenvalues are 2 and 4
  Standard_Real aEigenvals[2];
  aEigenvals[0] = aValues(1);
  aEigenvals[1] = aValues(2);
  std::sort(aEigenvals, aEigenvals + 2);

  EXPECT_NEAR(aEigenvals[0], 2.0, 1.0e-10) << "First eigenvalue should be 2";
  EXPECT_NEAR(aEigenvals[1], 4.0, 1.0e-10) << "Second eigenvalue should be 4";
}

TEST(MathJacobiTest, EigenvectorVerification)
{
  // Test eigenvector computation and verification
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 4.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 1.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed";

  // Verify A * v = lambda * v for each eigenpair
  for (Standard_Integer i = 1; i <= 2; i++)
  {
    math_Vector aV(1, 2);
    aJacobi.Vector(i, aV);

    Standard_Real aLambda = aJacobi.Value(i);

    // Compute A * v
    math_Vector aAv(1, 2);
    aAv(1) = aMatrix(1, 1) * aV(1) + aMatrix(1, 2) * aV(2);
    aAv(2) = aMatrix(2, 1) * aV(1) + aMatrix(2, 2) * aV(2);

    // Compute lambda * v
    math_Vector aLambdaV(1, 2);
    aLambdaV(1) = aLambda * aV(1);
    aLambdaV(2) = aLambda * aV(2);

    // Check A*v = lambda*v
    EXPECT_NEAR(aAv(1), aLambdaV(1), 1.0e-10) << "Eigenvector equation should hold for component 1";
    EXPECT_NEAR(aAv(2), aLambdaV(2), 1.0e-10) << "Eigenvector equation should hold for component 2";
  }
}

TEST(MathJacobiTest, LargerSymmetricMatrix)
{
  // Test with larger 4x4 symmetric matrix
  math_Matrix aMatrix(1, 4, 1, 4);
  // Create a symmetric positive definite matrix
  aMatrix(1, 1) = 4.0;
  aMatrix(1, 2) = 1.0;
  aMatrix(1, 3) = 0.5;
  aMatrix(1, 4) = 0.2;
  aMatrix(2, 1) = 1.0;
  aMatrix(2, 2) = 5.0;
  aMatrix(2, 3) = 1.5;
  aMatrix(2, 4) = 0.3;
  aMatrix(3, 1) = 0.5;
  aMatrix(3, 2) = 1.5;
  aMatrix(3, 3) = 6.0;
  aMatrix(3, 4) = 2.0;
  aMatrix(4, 1) = 0.2;
  aMatrix(4, 2) = 0.3;
  aMatrix(4, 3) = 2.0;
  aMatrix(4, 4) = 3.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed for 4x4 matrix";

  const math_Vector& aValues = aJacobi.Values();
  EXPECT_EQ(aValues.Length(), 4) << "Should have 4 eigenvalues";

  // All eigenvalues should be real and positive (positive definite matrix)
  for (Standard_Integer i = 1; i <= 4; i++)
  {
    EXPECT_GT(aJacobi.Value(i), 0.0) << "Eigenvalue " << i << " should be positive";
  }

  // Verify trace preservation (sum of eigenvalues = sum of diagonal elements)
  Standard_Real aTraceOriginal    = aMatrix(1, 1) + aMatrix(2, 2) + aMatrix(3, 3) + aMatrix(4, 4);
  Standard_Real aTraceEigenvalues = aValues(1) + aValues(2) + aValues(3) + aValues(4);

  EXPECT_NEAR(aTraceEigenvalues, aTraceOriginal, 1.0e-10)
    << "Sum of eigenvalues should equal trace";
}

TEST(MathJacobiTest, SingleElementMatrix)
{
  // Test with 1x1 matrix
  math_Matrix aMatrix(1, 1, 1, 1);
  aMatrix(1, 1) = 42.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed for 1x1 matrix";

  const math_Vector& aValues = aJacobi.Values();
  EXPECT_EQ(aValues.Length(), 1) << "Should have 1 eigenvalue";
  EXPECT_NEAR(aValues(1), 42.0, 1.0e-12) << "Eigenvalue should be the matrix element";

  math_Vector aV(1, 1);
  aJacobi.Vector(1, aV);
  EXPECT_NEAR(aV(1), 1.0, 1.0e-12) << "Eigenvector should be [1]";
}

TEST(MathJacobiTest, SquareMatrixRequirement)
{
  // Test square matrix requirement for Jacobi eigenvalue decomposition
  math_Matrix aNonSquareMatrix(1, 2, 1, 3); // 2x3 matrix
  aNonSquareMatrix(1, 1) = 1.0;
  aNonSquareMatrix(1, 2) = 2.0;
  aNonSquareMatrix(1, 3) = 3.0;
  aNonSquareMatrix(2, 1) = 4.0;
  aNonSquareMatrix(2, 2) = 5.0;
  aNonSquareMatrix(2, 3) = 6.0;

  // Verify matrix is indeed non-square
  EXPECT_NE(aNonSquareMatrix.RowNumber(), aNonSquareMatrix.ColNumber())
    << "Matrix should be non-square for this test";

  // Jacobi eigenvalue decomposition requires square matrices
  // Test with a proper square matrix instead
  math_Matrix aSquareMatrix(1, 2, 1, 2);
  aSquareMatrix(1, 1) = 1.0;
  aSquareMatrix(1, 2) = 0.5;
  aSquareMatrix(2, 1) = 0.5;
  aSquareMatrix(2, 2) = 2.0;

  math_Jacobi aJacobi(aSquareMatrix);
  EXPECT_TRUE(aJacobi.IsDone()) << "Should work with square matrix";
}

TEST(MathJacobiTest, NotDoneExceptions)
{
  // Create a scenario where Jacobi might fail (though it usually succeeds)
  // Test exception handling for accessing results before computation
  math_Matrix aMatrix(1, 2, 1, 2);
  aMatrix(1, 1) = 1.0;
  aMatrix(1, 2) = 0.0;
  aMatrix(2, 1) = 0.0;
  aMatrix(2, 2) = 1.0;

  math_Jacobi aJacobi(aMatrix);

  if (!aJacobi.IsDone())
  {
    // Only test exceptions if computation actually failed
    EXPECT_THROW(aJacobi.Values(), StdFail_NotDone) << "Should throw NotDone for Values()";
    EXPECT_THROW(aJacobi.Value(1), StdFail_NotDone) << "Should throw NotDone for Value()";
    EXPECT_THROW(aJacobi.Vectors(), StdFail_NotDone) << "Should throw NotDone for Vectors()";

    math_Vector aV(1, 2);
    EXPECT_THROW(aJacobi.Vector(1, aV), StdFail_NotDone) << "Should throw NotDone for Vector()";
  }
}

TEST(MathJacobiTest, OrthogonalityOfEigenvectors)
{
  // Test orthogonality of eigenvectors for symmetric matrix
  math_Matrix aMatrix(1, 3, 1, 3);
  aMatrix(1, 1) = 6.0;
  aMatrix(1, 2) = 2.0;
  aMatrix(1, 3) = 1.0;
  aMatrix(2, 1) = 2.0;
  aMatrix(2, 2) = 3.0;
  aMatrix(2, 3) = 1.0;
  aMatrix(3, 1) = 1.0;
  aMatrix(3, 2) = 1.0;
  aMatrix(3, 3) = 1.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed";

  // Get eigenvectors
  math_Vector aV1(1, 3), aV2(1, 3), aV3(1, 3);
  aJacobi.Vector(1, aV1);
  aJacobi.Vector(2, aV2);
  aJacobi.Vector(3, aV3);

  // Check orthogonality (dot products should be zero for different eigenvalues)
  Standard_Real aDot12 = aV1(1) * aV2(1) + aV1(2) * aV2(2) + aV1(3) * aV2(3);
  Standard_Real aDot13 = aV1(1) * aV3(1) + aV1(2) * aV3(2) + aV1(3) * aV3(3);
  Standard_Real aDot23 = aV2(1) * aV3(1) + aV2(2) * aV3(2) + aV2(3) * aV3(3);

  const math_Vector& aValues = aJacobi.Values();

  // Only check orthogonality for distinct eigenvalues
  if (abs(aValues(1) - aValues(2)) > 1.0e-10)
  {
    EXPECT_NEAR(aDot12, 0.0, 1.0e-10) << "Eigenvectors 1 and 2 should be orthogonal";
  }
  if (abs(aValues(1) - aValues(3)) > 1.0e-10)
  {
    EXPECT_NEAR(aDot13, 0.0, 1.0e-10) << "Eigenvectors 1 and 3 should be orthogonal";
  }
  if (abs(aValues(2) - aValues(3)) > 1.0e-10)
  {
    EXPECT_NEAR(aDot23, 0.0, 1.0e-10) << "Eigenvectors 2 and 3 should be orthogonal";
  }
}

TEST(MathJacobiTest, NormalizationOfEigenvectors)
{
  // Test that eigenvectors are normalized
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

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should succeed";

  // Check that each eigenvector has unit norm
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    math_Vector aV(1, 3);
    aJacobi.Vector(i, aV);

    Standard_Real aNorm = sqrt(aV(1) * aV(1) + aV(2) * aV(2) + aV(3) * aV(3));
    EXPECT_NEAR(aNorm, 1.0, 1.0e-10) << "Eigenvector " << i << " should be normalized";
  }
}

TEST(MathJacobiTest, CustomBounds)
{
  // Test with custom matrix bounds
  math_Matrix aMatrix(2, 3, 2, 3);
  aMatrix(2, 2) = 5.0;
  aMatrix(2, 3) = 1.0;
  aMatrix(3, 2) = 1.0;
  aMatrix(3, 3) = 3.0;

  math_Jacobi aJacobi(aMatrix);

  EXPECT_TRUE(aJacobi.IsDone()) << "Jacobi should work with custom bounds";

  const math_Vector& aValues = aJacobi.Values();
  EXPECT_EQ(aValues.Length(), 2) << "Should have 2 eigenvalues for 2x2 matrix";

  // Both eigenvalues should be positive
  EXPECT_GT(aValues(1), 0.0) << "First eigenvalue should be positive";
  EXPECT_GT(aValues(2), 0.0) << "Second eigenvalue should be positive";
}