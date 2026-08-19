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
#include "MathLin_Utils.hxx"

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
inline MathUtils::EigenResult Jacobi(const math_Matrix& theA, bool theSortDescending = true)
{
  MathUtils::EigenResult aResult;

  const size_t aN = theA.RowSize();

  // Check for square matrix
  if (theA.ColSize() != aN || aN == 0 || !Utils::IsFinite(theA))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (aN == 1)
  {
    aResult.EigenValues  = math_Vector(size_t{1}, theA.At(0, 0));
    aResult.EigenVectors = math_Matrix(size_t{1}, size_t{1}, 1.0);
    aResult.Status       = Status::OK;
    return aResult;
  }

  double aScale = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      const double anAij = theA.At(i, j);
      const double anAji = theA.At(j, i);
      aScale             = std::max(aScale, std::abs(anAij));
      if (std::abs(anAij - anAji) > std::numeric_limits<double>::epsilon() * static_cast<double>(aN)
                                      * std::max(std::abs(anAij), std::abs(anAji)))
      {
        aResult.Status = Status::InvalidInput;
        return aResult;
      }
    }
  }
  math_Matrix aWorkAStorage(aN, aN);
  math_Vector aEigenValsStorage(aN);
  math_Matrix aEigenVecsStorage(aN, aN);

  // Copy input to working matrix
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aWorkAStorage.ChangeAt(i, j) = aScale == 0.0 ? 0.0 : theA.At(i, j) / aScale;
    }
  }

  // Call the Jacobi function from math_Recipes
  const int   aRecipeN = static_cast<int>(aN);
  math_Matrix aWorkA(&aWorkAStorage.ChangeAt(0, 0), 1, aRecipeN, 1, aRecipeN);
  math_Vector aEigenVals(&aEigenValsStorage[0], 1, aRecipeN);
  math_Matrix aEigenVecs(&aEigenVecsStorage.ChangeAt(0, 0), 1, aRecipeN, 1, aRecipeN);
  int         aNbRotations = 0;
  if (::Jacobi(aWorkA, aEigenVals, aEigenVecs, aNbRotations) != 0)
  {
    aResult.Status = Status::NotConverged;
    return aResult;
  }

  for (size_t i = 0; i < aN; ++i)
  {
    aEigenValsStorage[i] *= aScale;
  }

  aResult.NbIterations = static_cast<uint32_t>(aNbRotations);

  // Sort eigenvalues and eigenvectors if requested
  if (theSortDescending && aN > 1)
  {
    // Simple selection sort
    for (size_t i = 0; i + 1 < aN; ++i)
    {
      size_t aMaxIdx = i;
      double aMaxVal = aEigenValsStorage[i];

      for (size_t j = i + 1; j < aN; ++j)
      {
        if (aEigenValsStorage[j] > aMaxVal)
        {
          aMaxVal = aEigenValsStorage[j];
          aMaxIdx = j;
        }
      }

      if (aMaxIdx != i)
      {
        // Swap eigenvalues
        std::swap(aEigenValsStorage[i], aEigenValsStorage[aMaxIdx]);

        // Swap corresponding eigenvector columns
        for (size_t k = 0; k < aN; ++k)
        {
          std::swap(aEigenVecsStorage.ChangeAt(k, i), aEigenVecsStorage.ChangeAt(k, aMaxIdx));
        }
      }
    }
  }

  aResult.EigenValues = math_Vector(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    (*aResult.EigenValues)[i] = aEigenValsStorage[i];
  }

  aResult.EigenVectors = math_Matrix(aN, aN);
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aResult.EigenVectors->ChangeAt(i, j) = aEigenVecsStorage.At(i, j);
    }
  }

  aResult.Status = Utils::IsFinite(*aResult.EigenValues) && Utils::IsFinite(*aResult.EigenVectors)
                     ? Status::OK
                     : Status::NumericalError;
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
inline MathUtils::EigenResult EigenValues(const math_Matrix& theA, bool theSortDescending = true)
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
inline MathUtils::EigenResult SpectralDecomposition(const math_Matrix& theA)
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
  if (!std::isfinite(thePower))
  {
    return std::nullopt;
  }

  MathUtils::EigenResult aEigen = Jacobi(theA, false);
  if (!aEigen.IsDone())
  {
    return std::nullopt;
  }

  const math_Vector& aD = *aEigen.EigenValues;
  const math_Matrix& aV = *aEigen.EigenVectors;

  const size_t aN = aD.Size();

  // Compute D^p
  math_Vector aDp(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    if (aD[i] < 0.0 && thePower != std::floor(thePower))
    {
      // Can't take fractional power of negative eigenvalue
      return std::nullopt;
    }
    aDp[i] = (aD[i] >= 0.0) ? std::pow(aD[i], thePower) : std::pow(-aD[i], thePower);
    if (aD[i] < 0.0 && std::fmod(std::abs(thePower), 2.0) == 1.0)
    {
      aDp[i] = -aDp[i];
    }
    if (!std::isfinite(aDp[i]))
    {
      return std::nullopt;
    }
  }

  // Compute V * D^p * V^T
  math_Matrix aResult(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      double aSum = 0.0;
      for (size_t k = 0; k < aN; ++k)
      {
        aSum += aV.At(i, k) * aDp[k] * aV.At(j, k);
      }
      if (!std::isfinite(aSum))
      {
        return std::nullopt;
      }
      aResult.ChangeAt(i, j) = aSum;
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
