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
    if (std::abs(aResult(i) - aExpected) > theTolerance)
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

  return std::abs(aDotProduct) < theTolerance;
}

// Helper function to compute vector norm
Standard_Real vectorNorm(const math_Vector& theVector)
{
  Standard_Real aNorm = 0.0;
  for (Standard_Integer i = 1; i <= theVector.Length(); i++)
    aNorm += theVector(i) * theVector(i);
  return std::sqrt(aNorm);
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

  const Standard_Real lambda1 = 2.5 - 0.5 * std::sqrt(5.0); // ~= 0.382
  const Standard_Real lambda2 = 2.5 + 0.5 * std::sqrt(5.0); // ~= 4.618

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

  EXPECT_NEAR(eigenvals[0], -std::sqrt(2.0), 1e-10);
  EXPECT_NEAR(eigenvals[1], 0.0, 1e-10);
  EXPECT_NEAR(eigenvals[2], std::sqrt(2.0), 1e-10);
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
    aDiagonal.SetValue(i, static_cast<Standard_Real>(std::abs(i - 1 - m)));
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
    aSubdiagonal.SetValue(i, val * std::sqrt(static_cast<Standard_Real>(i)));
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

// Test deflation condition edge case - subdiagonal element smaller than machine epsilon
TEST(math_EigenValuesSearcherTest, DeflationConditionPrecision)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  // Large diagonal elements
  aDiagonal.SetValue(1, 1e6);
  aDiagonal.SetValue(2, 2e6);
  aDiagonal.SetValue(3, 3e6);
  aDiagonal.SetValue(4, 4e6);

  // Subdiagonal elements at machine epsilon level
  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e-15); // Should trigger deflation
  aSubdiagonal.SetValue(3, 2e-15); // Should trigger deflation
  aSubdiagonal.SetValue(4, 3e-15); // Should trigger deflation

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Should converge quickly due to deflation
  math_Matrix originalMatrix(1, 4, 1, 4);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 4; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
  }

  // Eigenvalues should be close to diagonal elements due to small off-diagonal
  std::vector<Standard_Real> eigenvals;
  for (Standard_Integer i = 1; i <= 4; i++)
    eigenvals.push_back(searcher.EigenValue(i));

  std::sort(eigenvals.begin(), eigenvals.end());

  EXPECT_NEAR(eigenvals[0], 1e6, 1e3);
  EXPECT_NEAR(eigenvals[1], 2e6, 1e3);
  EXPECT_NEAR(eigenvals[2], 3e6, 1e3);
  EXPECT_NEAR(eigenvals[3], 4e6, 1e3);
}

// Test exact zero subdiagonal elements (should deflate immediately)
TEST(math_EigenValuesSearcherTest, ExactZeroSubdiagonal)
{
  TColStd_Array1OfReal aDiagonal(1, 5);
  TColStd_Array1OfReal aSubdiagonal(1, 5);

  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 4.0);
  aDiagonal.SetValue(3, 9.0);
  aDiagonal.SetValue(4, 16.0);
  aDiagonal.SetValue(5, 25.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1.0);
  aSubdiagonal.SetValue(3, 0.0); // Exact zero - should cause deflation
  aSubdiagonal.SetValue(4, 2.0);
  aSubdiagonal.SetValue(5, 0.0); // Exact zero - should cause deflation

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Should handle block structure correctly
  math_Matrix originalMatrix(1, 5, 1, 5);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 5; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-12));
  }
}

// Test convergence behavior with maximum iterations edge case
TEST(math_EigenValuesSearcherTest, ConvergenceBehavior)
{
  TColStd_Array1OfReal aDiagonal(1, 6);
  TColStd_Array1OfReal aSubdiagonal(1, 6);

  // Create a matrix that might require more iterations to converge
  for (Standard_Integer i = 1; i <= 6; i++)
    aDiagonal.SetValue(i, static_cast<Standard_Real>(i) * 0.1);

  aSubdiagonal.SetValue(1, 0.0);
  for (Standard_Integer i = 2; i <= 6; i++)
    aSubdiagonal.SetValue(i, 0.99); // Large off-diagonal elements

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Should still converge within maximum iterations
  math_Matrix originalMatrix(1, 6, 1, 6);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 6; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
  }
}

// Test underflow/overflow protection
TEST(math_EigenValuesSearcherTest, NumericalStability)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  // Mix of very large and very small values
  aDiagonal.SetValue(1, 1e-100);
  aDiagonal.SetValue(2, 1e100);
  aDiagonal.SetValue(3, 1e-50);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e-75);
  aSubdiagonal.SetValue(3, 1e75);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Should handle extreme values without overflow/underflow
  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    EXPECT_TRUE(std::isfinite(eigenval));
    EXPECT_FALSE(std::isnan(eigenval));

    const math_Vector eigenvec = searcher.EigenVector(i);
    for (Standard_Integer j = 1; j <= 3; j++)
    {
      EXPECT_TRUE(std::isfinite(eigenvec(j)));
      EXPECT_FALSE(std::isnan(eigenvec(j)));
    }
  }
}

// Test Wilkinson shift effectiveness
TEST(math_EigenValuesSearcherTest, WilkinsonShiftAccuracy)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  // Matrix where Wilkinson shift should provide fast convergence
  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 2.0);
  aDiagonal.SetValue(3, 1.0000001); // Very close to first diagonal element

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1.0);
  aSubdiagonal.SetValue(3, 0.0000001); // Very small

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 3, 1, 3);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-10));
  }
}

// Test special case when radius becomes zero in QL step
TEST(math_EigenValuesSearcherTest, ZeroRadiusHandling)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  // Configuration that might lead to zero radius in computeHypotenuseLength
  aDiagonal.SetValue(1, 0.0);
  aDiagonal.SetValue(2, 0.0);
  aDiagonal.SetValue(3, 1.0);
  aDiagonal.SetValue(4, 1.0);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(2, 1e-16); // Very small but non-zero
  aSubdiagonal.SetValue(3, 1e-16);
  aSubdiagonal.SetValue(4, 0.0);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 4, 1, 4);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 4; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-12));
  }
}

// Test pathological case with all equal elements
TEST(math_EigenValuesSearcherTest, PathologicalEqualElements)
{
  TColStd_Array1OfReal aDiagonal(1, 5);
  TColStd_Array1OfReal aSubdiagonal(1, 5);

  // All elements equal - degenerate case
  const Standard_Real value = 42.0;
  for (Standard_Integer i = 1; i <= 5; i++)
    aDiagonal.SetValue(i, value);

  aSubdiagonal.SetValue(1, 0.0);
  for (Standard_Integer i = 2; i <= 5; i++)
    aSubdiagonal.SetValue(i, value);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  // Algorithm should still work correctly
  math_Matrix originalMatrix(1, 5, 1, 5);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 5; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
  }
}

// Test matrix with systematically increasing subdiagonal elements
TEST(math_EigenValuesSearcherTest, IncreasingSubdiagonal)
{
  TColStd_Array1OfReal aDiagonal(1, 6);
  TColStd_Array1OfReal aSubdiagonal(1, 6);

  for (Standard_Integer i = 1; i <= 6; i++)
    aDiagonal.SetValue(i, 1.0);

  aSubdiagonal.SetValue(1, 0.0);
  for (Standard_Integer i = 2; i <= 6; i++)
    aSubdiagonal.SetValue(i, static_cast<Standard_Real>(i - 1) * 0.1);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);

  EXPECT_TRUE(searcher.IsDone());

  math_Matrix originalMatrix(1, 6, 1, 6);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  // Verify all pairs and orthogonality
  for (Standard_Integer i = 1; i <= 6; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-10));
    EXPECT_NEAR(vectorNorm(eigenvec), 1.0, 1e-10);
  }

  // Check orthogonality of all eigenvector pairs
  for (Standard_Integer i = 1; i <= 6; i++)
  {
    for (Standard_Integer j = i + 1; j <= 6; j++)
    {
      const math_Vector vec_i = searcher.EigenVector(i);
      const math_Vector vec_j = searcher.EigenVector(j);
      EXPECT_TRUE(areOrthogonal(vec_i, vec_j, 1e-10));
    }
  }
}

// Test to demonstrate and verify the deflation condition behavior
// This test documents the precision semantics of the deflation test:
// |e[i]| + (|d[i]| + |d[i+1]|) == |d[i]| + |d[i+1]| in floating-point arithmetic
TEST(math_EigenValuesSearcherTest, DeflationConditionSemantics)
{
  TColStd_Array1OfReal aDiagonal(1, 3);
  TColStd_Array1OfReal aSubdiagonal(1, 3);

  // Test 1: Large diagonal elements with subdiagonal at machine epsilon level
  const Standard_Real largeDiag1       = 1e8;
  const Standard_Real largeDiag2       = 2e8;
  const Standard_Real machEpsilonLevel = largeDiag1 * std::numeric_limits<Standard_Real>::epsilon();

  aDiagonal.SetValue(1, largeDiag1);
  aDiagonal.SetValue(2, largeDiag2);
  aDiagonal.SetValue(3, 3e8);

  aSubdiagonal.SetValue(1, 0.0);
  aSubdiagonal.SetValue(
    2,
    machEpsilonLevel); // Should trigger deflation due to floating-point precision
  aSubdiagonal.SetValue(3, machEpsilonLevel * 0.5); // Even smaller, should definitely deflate

  // Verify the mathematical behavior that the deflation condition tests
  const Standard_Real diagSum       = std::abs(largeDiag1) + std::abs(largeDiag2);
  const Standard_Real testCondition = machEpsilonLevel + diagSum;

  // This should be true in floating-point arithmetic due to precision limits
  EXPECT_EQ(testCondition, diagSum)
    << "Deflation condition should hold for machine-epsilon level elements";

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);
  EXPECT_TRUE(searcher.IsDone());

  // Should converge efficiently due to deflation
  math_Matrix originalMatrix(1, 3, 1, 3);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 3; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);
    // Use looser tolerance for very large eigenvalues
    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-6));
  }
}

// Test edge case where deflation condition is exactly at the boundary
TEST(math_EigenValuesSearcherTest, DeflationBoundaryCondition)
{
  TColStd_Array1OfReal aDiagonal(1, 4);
  TColStd_Array1OfReal aSubdiagonal(1, 4);

  // Create diagonal elements of different scales
  aDiagonal.SetValue(1, 1.0);
  aDiagonal.SetValue(2, 1000.0);
  aDiagonal.SetValue(3, 1e-6);
  aDiagonal.SetValue(4, 1e6);

  aSubdiagonal.SetValue(1, 0.0);

  // Set subdiagonal elements right at the deflation boundary for different scales
  const Standard_Real eps = std::numeric_limits<Standard_Real>::epsilon();

  // For first pair: should deflate
  const Standard_Real sum1 = std::abs(aDiagonal(1)) + std::abs(aDiagonal(2));
  aSubdiagonal.SetValue(2, sum1 * eps * 0.1); // Below machine epsilon relative to sum

  // For second pair: should deflate
  const Standard_Real sum2 = std::abs(aDiagonal(2)) + std::abs(aDiagonal(3));
  aSubdiagonal.SetValue(3, sum2 * eps * 0.1);

  // For third pair: should deflate
  const Standard_Real sum3 = std::abs(aDiagonal(3)) + std::abs(aDiagonal(4));
  aSubdiagonal.SetValue(4, sum3 * eps * 0.1);

  math_EigenValuesSearcher searcher(aDiagonal, aSubdiagonal);
  EXPECT_TRUE(searcher.IsDone());

  // Verify that algorithm handles the mixed scales correctly
  math_Matrix originalMatrix(1, 4, 1, 4);
  createTridiagonalMatrix(aDiagonal, aSubdiagonal, originalMatrix);

  for (Standard_Integer i = 1; i <= 4; i++)
  {
    const Standard_Real eigenval = searcher.EigenValue(i);
    const math_Vector   eigenvec = searcher.EigenVector(i);

    EXPECT_TRUE(verifyEigenPair(originalMatrix, eigenval, eigenvec, 1e-9));
    EXPECT_TRUE(std::isfinite(eigenval));
    EXPECT_FALSE(std::isnan(eigenval));
  }
}
