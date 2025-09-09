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

// Test fixture for math_EigenValuesSearcher
class EigenValuesSearcherTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Setup common test data if needed
  }

  void TearDown() override
  {
    // Cleanup if needed
  }
};

// Test constructor with dimension mismatch
TEST_F(EigenValuesSearcherTest, ConstructorDimensionMismatch)
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
TEST_F(EigenValuesSearcherTest, OneByOneMatrix)
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
TEST_F(EigenValuesSearcherTest, TwoByTwoMatrix)
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
TEST_F(EigenValuesSearcherTest, ThreeByThreeMatrix)
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
TEST_F(EigenValuesSearcherTest, DiagonalMatrix)
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
TEST_F(EigenValuesSearcherTest, IdentityMatrix)
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
TEST_F(EigenValuesSearcherTest, NegativeEigenvalues)
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
TEST_F(EigenValuesSearcherTest, FiveByFiveMatrix)
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
TEST_F(EigenValuesSearcherTest, SmallSubdiagonalElements)
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
