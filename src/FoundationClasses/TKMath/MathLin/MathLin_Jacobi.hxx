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

#ifndef _MathLin_Jacobi_HeaderFile
#define _MathLin_Jacobi_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <math_Recipes.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <algorithm>

namespace MathLin
{
using namespace MathUtils;

//! Compute eigenvalues and eigenvectors of a symmetric matrix
//! using the Jacobi iterative method.
//!
//! The Jacobi method applies a sequence of plane rotations (Givens rotations)
//! to diagonalize the symmetric matrix A:
//!   A' = R^T * A * R
//! where R is a rotation that zeroes one off-diagonal element.
//!
//! After convergence, A is diagonal with eigenvalues on the diagonal,
//! and the accumulated rotations form the eigenvector matrix.
//!
//! Properties:
//! - Only works for symmetric matrices
//! - Eigenvalues are always real for symmetric matrices
//! - Eigenvectors are orthonormal
//! - Numerically stable
//!
//! Complexity: O(n^3) per sweep, typically needs 5-10 sweeps.
//!
//! @param theA input symmetric matrix (n x n)
//! @param theSortDescending if true, eigenvalues are sorted in descending order
//! @return eigenvalue result
inline EigenResult Jacobi(const math_Matrix& theA, bool theSortDescending = true)
{
  EigenResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aN        = aRowUpper - aRowLower + 1;

  // Check for square matrix
  if (aColUpper - aColLower + 1 != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Create 1-based working copies as required by the Jacobi function
  math_Matrix aWorkA(1, aN, 1, aN);
  math_Vector aEigenVals(1, aN);
  math_Matrix aEigenVecs(1, aN, 1, aN);

  // Copy input to working matrix
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aWorkA(i - aRowLower + 1, j - aColLower + 1) = theA(i, j);
    }
  }

  // Call the Jacobi function from math_Recipes
  int aNbRotations = 0;
  if (::Jacobi(aWorkA, aEigenVals, aEigenVecs, aNbRotations) != 0)
  {
    aResult.Status = Status::NotConverged;
    return aResult;
  }

  aResult.NbIterations = static_cast<size_t>(aNbRotations);

  // Sort eigenvalues and eigenvectors if requested
  if (theSortDescending && aN > 1)
  {
    // Simple selection sort
    for (int i = 1; i < aN; ++i)
    {
      int    aMaxIdx = i;
      double aMaxVal = aEigenVals(i);

      for (int j = i + 1; j <= aN; ++j)
      {
        if (aEigenVals(j) > aMaxVal)
        {
          aMaxVal = aEigenVals(j);
          aMaxIdx = j;
        }
      }

      if (aMaxIdx != i)
      {
        // Swap eigenvalues
        std::swap(aEigenVals(i), aEigenVals(aMaxIdx));

        // Swap corresponding eigenvector columns
        for (int k = 1; k <= aN; ++k)
        {
          std::swap(aEigenVecs(k, i), aEigenVecs(k, aMaxIdx));
        }
      }
    }
  }

  // Copy results with original indexing
  aResult.EigenValues = math_Vector(aRowLower, aRowUpper);
  for (int i = 1; i <= aN; ++i)
  {
    (*aResult.EigenValues)(aRowLower + i - 1) = aEigenVals(i);
  }

  aResult.EigenVectors = math_Matrix(aRowLower, aRowUpper, aColLower, aColUpper);
  for (int i = 1; i <= aN; ++i)
  {
    for (int j = 1; j <= aN; ++j)
    {
      (*aResult.EigenVectors)(aRowLower + i - 1, aColLower + j - 1) = aEigenVecs(i, j);
    }
  }

  aResult.Status = Status::OK;
  return aResult;
}

//! Compute only eigenvalues of a symmetric matrix (faster).
//!
//! Uses the same Jacobi method but may be optimized to not store
//! eigenvectors if not needed.
//!
//! @param theA input symmetric matrix (n x n)
//! @param theSortDescending if true, eigenvalues are sorted in descending order
//! @return eigenvalue result (only EigenValues is set)
inline EigenResult EigenValues(const math_Matrix& theA, bool theSortDescending = true)
{
  // Currently delegates to full Jacobi, but eigenvectors are computed
  // A future optimization could avoid eigenvector computation
  return Jacobi(theA, theSortDescending);
}

//! Compute spectral decomposition A = V * D * V^T.
//!
//! For symmetric matrix A, decomposes into:
//! - V: orthogonal matrix of eigenvectors (columns)
//! - D: diagonal matrix of eigenvalues
//!
//! Such that A = V * D * V^T
//!
//! @param theA input symmetric matrix (n x n)
//! @return eigenvalue result with EigenValues (diagonal of D) and EigenVectors (V)
inline EigenResult SpectralDecomposition(const math_Matrix& theA)
{
  return Jacobi(theA, false);
}

//! Compute matrix power A^p for symmetric positive semi-definite matrix.
//!
//! Uses spectral decomposition: A^p = V * D^p * V^T
//! where D^p is the diagonal matrix with eigenvalues raised to power p.
//!
//! @param theA input symmetric positive semi-definite matrix
//! @param thePower exponent (can be fractional, e.g., 0.5 for sqrt)
//! @return A^p matrix
inline std::optional<math_Matrix> MatrixPower(const math_Matrix& theA, double thePower)
{
  EigenResult aEigen = Jacobi(theA, false);
  if (!aEigen.IsDone())
  {
    return std::nullopt;
  }

  const math_Vector& aD = *aEigen.EigenValues;
  const math_Matrix& aV = *aEigen.EigenVectors;

  const int aLower = aD.Lower();
  const int aUpper = aD.Upper();
  const int aN     = aUpper - aLower + 1;

  // Compute D^p
  math_Vector aDp(aLower, aUpper);
  for (int i = aLower; i <= aUpper; ++i)
  {
    if (aD(i) < 0.0 && thePower != std::floor(thePower))
    {
      // Can't take fractional power of negative eigenvalue
      return std::nullopt;
    }
    aDp(i) = (aD(i) >= 0.0) ? std::pow(aD(i), thePower) : std::pow(-aD(i), thePower);
    if (aD(i) < 0.0 && static_cast<int>(thePower) % 2 != 0)
    {
      aDp(i) = -aDp(i);
    }
  }

  // Compute V * D^p * V^T
  math_Matrix aResult(aLower, aUpper, aLower, aUpper, 0.0);
  for (int i = aLower; i <= aUpper; ++i)
  {
    for (int j = aLower; j <= aUpper; ++j)
    {
      double aSum = 0.0;
      for (int k = aLower; k <= aUpper; ++k)
      {
        aSum += aV(i, k) * aDp(k) * aV(j, k);
      }
      aResult(i, j) = aSum;
    }
  }

  return aResult;
}

//! Compute matrix square root of symmetric positive semi-definite matrix.
//!
//! @param theA input symmetric positive semi-definite matrix
//! @return sqrt(A) such that sqrt(A) * sqrt(A) = A
inline std::optional<math_Matrix> MatrixSqrt(const math_Matrix& theA)
{
  return MatrixPower(theA, 0.5);
}

//! Compute matrix inverse square root of symmetric positive definite matrix.
//!
//! @param theA input symmetric positive definite matrix
//! @return A^(-1/2) such that A^(-1/2) * A * A^(-1/2) = I
inline std::optional<math_Matrix> MatrixInvSqrt(const math_Matrix& theA)
{
  return MatrixPower(theA, -0.5);
}

} // namespace MathLin

#endif // _MathLin_Jacobi_HeaderFile
