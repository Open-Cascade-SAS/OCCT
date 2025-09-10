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

#include <math_EigenValuesSearcher.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Failure.hxx>
#include <Precision.hxx>

#include <cmath>
#include <algorithm>

namespace
{
// Helper function to create a tridiagonal matrix from arrays
void createTridiagonalMatrix(const TColStd_Array1OfReal& theDiagonal,
                             const TColStd_Array1OfReal& theSubdiagonal,
                             math_Matrix&                theMatrix)
{
  const Standard_Integer aN = theDiagonal.Length();
  theMatrix.Init(0.0);

  // Set diagonal elements
  for (Standard_Integer i = 1; i <= aN; i++)
    theMatrix(i, i) = theDiagonal(i);

  // Set sub and super diagonal elements
  for (Standard_Integer i = 2; i <= aN; i++)
  {
    theMatrix(i, i - 1) = theSubdiagonal(i);
    theMatrix(i - 1, i) = theSubdiagonal(i);
  }
}

// Helper function to verify eigenvalue-eigenvector relationship: A*v = lambda*v
Standard_Boolean verifyEigenPair(const math_Matrix&  theMatrix,
                                 const Standard_Real theEigenValue,
                                 const math_Vector&  theEigenVector,
                                 const Standard_Real theTolerance = 1e-12)
{
  const Standard_Integer aN = theMatrix.RowNumber();
  math_Vector            aResult(1, aN);

  // Compute A*v
  for (Standard_Integer i = 1; i <= aN; i++)
  {
    Standard_Real aSum = 0.0;
    for (Standard_Integer j = 1; j <= aN; j++)
      aSum += theMatrix(i, j) * theEigenVector(j);
    aResult(i) = aSum;
  }

  // Check if A*v ~= lambda*v
  for (Standard_Integer i = 1; i <= aN; i++)
  {
    const Standard_Real aExpected = theEigenValue * theEigenVector(i);
    if (Abs(aResult(i) - aExpected) > theTolerance)
      return Standard_False;
  }

  return Standard_True;
}

// Helper function to check if eigenvectors are orthogonal
Standard_Boolean areOrthogonal(const math_Vector&  theVec1,
                               const math_Vector&  theVec2,
                               const Standard_Real theTolerance = 1e-12)
{
  Standard_Real aDotProduct = 0.0;
  for (Standard_Integer i = 1; i <= theVec1.Length(); i++)
    aDotProduct += theVec1(i) * theVec2(i);

  return Abs(aDotProduct) < theTolerance;
}

// Helper function to compute vector norm
Standard_Real vectorNorm(const math_Vector& theVector)
{
  Standard_Real aNorm = 0.0;
  for (Standard_Integer i = 1; i <= theVector.Length(); i++)
    aNorm += theVector(i) * theVector(i);
  return Sqrt(aNorm);
}
} // namespace

// Test constructor with dimension mismatch
TEST(math_EigenValuesSearcherTest, ConstructorDimensionMismatch)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 2); // Wrong size

  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 2.0);
  aDiagonal.SetValue(3, 3.0);

  aSubdiagonal.SetValue(1, 0.5);
  aSubdiagonal.SetValue(2, 1.5);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  // Should not be done due to dimension mismatch
  EXPECT_FALSE(searcher.IsDone());
}

// Test 1x1 matrix (trivial case)
TEST(math_EigenValuesSearcherTest, OneByOneMatrix)
{
  TColStd_Array1OfReal aDiagonal(1, 1);
  TColStd_Array1OfReal aSubdiagonal(1, 1);

  aDiagonal.SetValue(1, 5.0);
  aSubdiagonal.SetValue(1, 0.0); // Not used for 1x1 matrix

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 1);
  EXPECT_NEAR(searcher.EigenValue(1), 5.0, Precision::Confusion());

  math_Vector eigenvec = searcher.EigenVector(1);
  EXPECT_EQ(eigenvec.Length(), 1);
  EXPECT_NEAR(eigenvec(1), 1.0, Precision::Confusion());
}

// Test 2x2 symmetric tridiagonal matrix
TEST(math_EigenValuesSearcherTest, TwoByTwoMatrix)
{
  TColStd_Array1OfReal aDiagonal(1, 2);
  TColStd_Array1OfReal aSubdiagonal(1, 2);

  // Matrix: [2 1]
  //         [1 3]
  aDiagonal.SetValue(1, 2.0);
  aDiagonal.SetValue(2, 3.0);
  aSubdiagonal.SetValue(1, 0.0); // Not used
  aSubdiagonal.SetValue(2, 1.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 2);

  // Verify eigenvalues are approximately 0.382 and 4.618
  std::vector<Standard_Real> eigenvals = {searcher.EigenValue(1), searcher.EigenValue(2)};
  std::sort(eigenvals.begin(), eigenvals.end());

  const Standard_Real lambda1 = 2.5 - 0.5 * Sqrt(5.0); // ~= 0.382
  const Standard_Real lambda2 = 2.5 + 0.5 * Sqrt(5.0); // ~= 4.618

  EXPECT_NEAR(eigenvals[0], lambda1, 1e-10);
  EXPECT_NEAR(eigenvals[1], lambda2, 1e-10);

  // Create original matrix for verification
  math_Matrix originalMatrix(1, 2, 1, 2);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  // Verify eigenvalue-eigenvector relationships
  for (Standard_Integer i = 1; i <= 2; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-10));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-10); // Should be normalized
  }

  // Verify eigenvectors are orthogonal
  const math_Vector vec1 = searcher.EigenVector(1);
  const math_Vector vec2 = searcher.EigenVector(2);
  EXPECT_TRUE(areOrthogonal(vec1, vec2, 1e-10));
}

// Test 3x3 symmetric tridiagonal matrix
TEST(math_EigenValuesSearcherTest, ThreeByThreeMatrix)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  // Matrix: [4 1 0]
  //         [1 4 1]
  //         [0 1 4]
  aDiagonal.SetValue(1, 4.0);
  aDiagonal.SetValue(2, 4.0);
  aDiagonal.SetValue(3, 4.0);
  aSubdiagonal.SetValue(1, 0.0); // Not used
  aSubdiagonal.SetValue(2, 1.0);
  aSubdiagonal.SetValue(3, 1.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 3);

  // Create original matrix for verification
  math_Matrix originalMatrix(1, 3, 1, 3);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  // Verify all eigenvalue-eigenvector pairs
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-10));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-10);
  }

  // Verify orthogonality of eigenvectors
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    for (Standard_Integer j = i + 1; j <= 3; j++)
    {
      const math_Vector vec_i = searcher.EigenVector(i);
      const math_Vector vec_j = searcher.EigenVector(j);
      EXPECT_TRUE(areOrthogonal(vec_i, vec_j, 1e-10));
    }
  }
}

// Test diagonal matrix (eigenvalues should be diagonal elements)
TEST(math_EigenValuesSearcherTest, DiagonalMatrix)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 2.0);
  aDiagonal.SetValue(3, 3.0);
  aDiagonal.SetValue(4, 4.0);

  // All subdiagonal elements are zero
  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 0.0);
  aSubdiagonal.SetValue(3, 0.0);
  aSubdiagonal.SetValue(4, 0.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 4);

  // For a diagonal matrix, eigenvalues should be the diagonal elements
  std::vector<Standard_Real> eigenvals;
  for (Standard_Integer i = 1; i <= 4; i++)
    eigenvals.push_back(searcher.EigenValue(i));

  std::sort(eigenvals.begin(), eigenvals.end());

  EXPECT_NEAR(eigenvals[0], 1.0, Precision::Confusion());
  EXPECT_NEAR(eigenvals[1], 2.0, Precision::Confusion());
  EXPECT_NEAR(eigenvals[2], 3.0, Precision::Confusion());
  EXPECT_NEAR(eigenvals[3], 4.0, Precision::Confusion());
}

// Test identity matrix
TEST(math_EigenValuesSearcherTest, IdentityMatrix)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  // Identity matrix
  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 1.0);
  aDiagonal.SetValue(3, 1.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 0.0);
  aSubdiagonal.SetValue(3, 0.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 3);

  // All eigenvalues should be 1.0
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    EXPECT_NEAR(searcher.EigenValue(i), 1.0, Precision::Confusion());
  }
}

// Test matrix with negative eigenvalues
TEST(math_EigenValuesSearcherTest, NegativeEigenvalues)
{
  TColStd_Array1OfReal aDiagonal(1, 2);
  TColStd_Array1OfReal aSubdiagonal(1, 2);

  // Matrix: [-1  2]
  //         [ 2 -1]
  aDiagonal.SetValue(1, -1.0);
  aDiagonal.SetValue(2, -1.0);
  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 2.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Eigenvalues should be -3 and 1
  std::vector<Standard_Real> eigenvals = {searcher.EigenValue(1), searcher.EigenValue(2)};
  std::sort(eigenvals.begin(), eigenvals.end());

  EXPECT_NEAR(eigenvals[0], -3.0, 1e-10);
  EXPECT_NEAR(eigenvals[1], 1.0, 1e-10);

  // Create original matrix for verification
  math_Matrix originalMatrix(1, 2, 1, 2);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 2; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);
    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-10));
  }
}

// Test larger matrix (5x5)
TEST(math_EigenValuesSearcherTest, FiveByFiveMatrix)
{
  TColStd_Array1OfReal aDiagonal(1, 5);
  TColStd_Array1OfReal aSubdiagonal(1, 5);

  // Create a symmetric tridiagonal matrix with pattern
  for (Standard_Integer i = 1; i <= 5; i++)
    aDiagonal.SetValue(i, 2.0);

  aSubdiagonal.SetValue(1, 0.0); // Not used
  for (Standard_Integer i = 2; i <= 5; i++)
    aSubdiagonal.SetValue(i, -1.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 5);

  // Create original matrix for verification
  math_Matrix originalMatrix(1, 5, 1, 5);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  // Verify all eigenvalue-eigenvector pairs
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-9);
  }

  // Check that all eigenvalues are reasonable
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    EXPECT_GE(eigenval, 0.0); // All eigenvalues should be non-negative for this matrix
    EXPECT_LE(eigenval, 4.0); // Should be bounded
  }
}

// Test with very small subdiagonal elements (near singular)
TEST(math_EigenValuesSearcherTest, SmallSubdiagonalElements)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 2.0);
  aDiagonal.SetValue(3, 3.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e-14);
  aSubdiagonal.SetValue(3, 1e-14);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Should converge to approximately diagonal values
  std::vector<Standard_Real> eigenvals;
  for (Standard_Integer i = 1; i <= 3; i++)
    eigenvals.push_back(searcher.EigenValue(i));

  std::sort(eigenvals.begin(), eigenvals.end());

  EXPECT_NEAR(eigenvals[0], 1.0, 1e-10);
  EXPECT_NEAR(eigenvals[1], 2.0, 1e-10);
  EXPECT_NEAR(eigenvals[2], 3.0, 1e-10);
}

// Test with zero diagonal elements
TEST(math_EigenValuesSearcherTest, ZeroDiagonalElements)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  // Matrix: [0 1 0]
  //         [1 0 1]
  //         [0 1 0]
  aDiagonal.SetValue(1, 0.0);
  aDiagonal.SetValue(2, 0.0);
  aDiagonal.SetValue(3, 0.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1.0);
  aSubdiagonal.SetValue(3, 1.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());
  EXPECT_EQ(searcher.Dimension(), 3);

  // Eigenvalues should be -sqrt(2), 0, sqrt(2)
  std::vector<Standard_Real> eigenvals;
  for (Standard_Integer i = 1; i <= 3; i++)
    eigenvals.push_back(searcher.EigenValue(i));

  std::sort(eigenvals.begin(), eigenvals.end());

  EXPECT_NEAR(eigenvals[0], -Sqrt(2.0), 1e-10);
  EXPECT_NEAR(eigenvals[1], 0.0, 1e-10);
  EXPECT_NEAR(eigenvals[2], Sqrt(2.0), 1e-10);
}

// Test with large diagonal elements (numerical stability)
TEST(math_EigenValuesSearcherTest, LargeDiagonalElements)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  aDiagonal.SetValue(1, 1e6);
  aDiagonal.SetValue(2, 2e6);
  aDiagonal.SetValue(3, 3e6);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e3);
  aSubdiagonal.SetValue(3, 2e3);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Create original matrix for verification
  math_Matrix originalMatrix(1, 3, 1, 3);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-6));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-10);
  }
}

// Test with alternating pattern
TEST(math_EigenValuesSearcherTest, AlternatingPattern)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  // Matrix with alternating diagonal pattern
  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, -1.0);
  aDiagonal.SetValue(3, 1.0);
  aDiagonal.SetValue(4, -1.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 0.5);
  aSubdiagonal.SetValue(3, -0.5);
  aSubdiagonal.SetValue(4, 0.5);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Verify eigenvalue-eigenvector relationships
  math_Matrix originalMatrix(1, 4, 1, 4);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 4; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-10));
  }
}

// Test repeated eigenvalues (multiple eigenvalues)
TEST(math_EigenValuesSearcherTest, RepeatedEigenvalues)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  // Matrix designed to have repeated eigenvalues
  aDiagonal.SetValue(1, 2.0);
  aDiagonal.SetValue(2, 2.0);
  aDiagonal.SetValue(3, 2.0);
  aDiagonal.SetValue(4, 2.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 0.0); // Block diagonal structure
  aSubdiagonal.SetValue(3, 0.0);
  aSubdiagonal.SetValue(4, 0.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // All eigenvalues should be 2.0
  for (Standard_Integer i = 1; i <= 4; i++)
  {
    EXPECT_NEAR(searcher.EigenValue(i), 2.0, Precision::Confusion());
  }
}

// Test with very small matrix elements (precision test)
TEST(math_EigenValuesSearcherTest, VerySmallElements)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  aDiagonal.SetValue(1, 1e-10);
  aDiagonal.SetValue(2, 2e-10);
  aDiagonal.SetValue(3, 3e-10);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e-11);
  aSubdiagonal.SetValue(3, 2e-11);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 3, 1, 3);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-18));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-12);
  }
}

// Test antisymmetric subdiagonal pattern
TEST(math_EigenValuesSearcherTest, AntisymmetricSubdiagonal)
{
  TColStd_Array1OfReal aDiagonal(1, 5);
  TColStd_Array1OfReal aSubdiagonal(1, 5);

  // Symmetric tridiagonal with specific pattern
  for (Standard_Integer i = 1; i <= 5; i++)
    aDiagonal.SetValue(i, static_cast<Standard_Real>(i));

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1.0);
  aSubdiagonal.SetValue(3, -2.0);
  aSubdiagonal.SetValue(4, 1.0);
  aSubdiagonal.SetValue(5, -2.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 5, 1, 5);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  // Verify all eigenvalue-eigenvector pairs
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
  }

  // Verify orthogonality
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    for (Standard_Integer j = i + 1; j <= 5; j++)
    {
      const math_Vector vec_i = searcher.EigenVector(i);
      const math_Vector vec_j = searcher.EigenVector(j);
      EXPECT_TRUE(areOrthogonal(vec_i, vec_j, 1e-9));
    }
  }
}

// Test Wilkinson matrix (known challenging case)
TEST(math_EigenValuesSearcherTest, WilkinsonMatrix)
{
  const Standard_Integer n = 5;
  TColStd_Array1OfReal   aDiagonal(1, n);
  TColStd_Array1OfReal   aSubdiagonal(1, n);

  // Wilkinson matrix W_n pattern
  const Standard_Integer m = (n - 1) / 2;
  for (Standard_Integer i = 1; i <= n; i++)
  {
    aDiagonal.SetValue(i, static_cast<Standard_Real>(Abs(i - 1 - m)));
  }

  aSubdiagonal.SetValue(1, 0.0);
  for (Standard_Integer i = 2; i <= n; i++)
    aSubdiagonal.SetValue(i, 1.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, n, 1, n);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= n; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-9);
  }
}

// Test with mixed positive/negative diagonal
TEST(math_EigenValuesSearcherTest, MixedSignDiagonal)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  aDiagonal.SetValue(1, -2.0);
  aDiagonal.SetValue(2, 1.0);
  aDiagonal.SetValue(3, -1.0);
  aDiagonal.SetValue(4, 3.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1.5);
  aSubdiagonal.SetValue(3, 0.8);
  aSubdiagonal.SetValue(4, -0.6);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 4, 1, 4);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  std::vector<Standard_Real> eigenvals;
  for (Standard_Integer i = 1; i <= 4; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    eigenvals.push_back(eigenval);
    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
  }

  // Check trace preservation (sum of eigenvalues = sum of diagonal)
  Standard_Real eigenSum = 0.0, diagSum = 0.0;
  for (Standard_Integer i = 1; i <= 4; i++)
  {
    eigenSum += eigenvals[i - 1];
    diagSum += aDiagonal(i);
  }
  EXPECT_NEAR(eigenSum, diagSum, 1e-9);
}

// Test maximum size matrix (stress test)
TEST(math_EigenValuesSearcherTest, LargerMatrix)
{
  const Standard_Integer n = 8;
  TColStd_Array1OfReal   aDiagonal(1, n);
  TColStd_Array1OfReal   aSubdiagonal(1, n);

  // Create a more complex pattern
  for (Standard_Integer i = 1; i <= n; i++)
  {
    aDiagonal.SetValue(i, static_cast<Standard_Real>(i * i));
  }

  aSubdiagonal.SetValue(1, 0.0);
  for (Standard_Integer i = 2; i <= n; i++)
  {
    Standard_Real val = static_cast<Standard_Real>(i % 3 == 0 ? -1.0 : 1.0);
    aSubdiagonal.SetValue(i, val * Sqrt(static_cast<Standard_Real>(i)));
  }

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, n, 1, n);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  // Verify all eigenvalue-eigenvector pairs
  for (Standard_Integer i = 1; i <= n; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-8));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-8);
  }

  // Test orthogonality for larger matrix
  for (Standard_Integer i = 1; i <= n; i++)
  {
    for (Standard_Integer j = i + 1; j <= n; j++)
    {
      const math_Vector vec_i = searcher.EigenVector(i);
      const math_Vector vec_j = searcher.EigenVector(j);
      EXPECT_TRUE(areOrthogonal(vec_i, vec_j, 1e-8));
    }
  }
}

// Test with rational number patterns
TEST(math_EigenValuesSearcherTest, RationalNumberPattern)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  aDiagonal.SetValue(1, 1.0 / 3.0);
  aDiagonal.SetValue(2, 2.0 / 3.0);
  aDiagonal.SetValue(3, 1.0);
  aDiagonal.SetValue(4, 4.0 / 3.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1.0 / 6.0);
  aSubdiagonal.SetValue(3, 1.0 / 4.0);
  aSubdiagonal.SetValue(4, 1.0 / 5.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 4, 1, 4);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 4; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-12));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-12);
  }
}

// Test near-degenerate case (eigenvalues very close)
TEST(math_EigenValuesSearcherTest, NearDegenerateEigenvalues)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 1.0 + 1e-8);
  aDiagonal.SetValue(3, 1.0 + 2e-8);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e-10);
  aSubdiagonal.SetValue(3, 1e-10);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Should still find distinct eigenvalues despite near-degeneracy
  std::vector<Standard_Real> eigenvals;
  for (Standard_Integer i = 1; i <= 3; i++)
    eigenvals.push_back(searcher.EigenValue(i));

  std::sort(eigenvals.begin(), eigenvals.end());

  // Eigenvalues should be close to but distinct from diagonal values
  EXPECT_NEAR(eigenvals[0], 1.0, 1e-7);
  EXPECT_NEAR(eigenvals[1], 1.0 + 1e-8, 1e-7);
  EXPECT_NEAR(eigenvals[2], 1.0 + 2e-8, 1e-7);

  math_Matrix originalMatrix(1, 3, 1, 3);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
  }
}
