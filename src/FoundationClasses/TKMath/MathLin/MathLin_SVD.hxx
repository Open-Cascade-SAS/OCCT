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

#ifndef _MathLin_SVD_HeaderFile
#define _MathLin_SVD_HeaderFile

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

//! Result for SVD decomposition.
struct SVDResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Matrix> U;                   //!< Left singular vectors (m x n)
  std::optional<math_Vector> SingularValues;      //!< Singular values (n elements)
  std::optional<math_Matrix> V;                   //!< Right singular vectors (n x n)
  size_t                     Rank            = 0; //!< Numerical rank
  double                     ConditionNumber = std::numeric_limits<double>::infinity();

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Singular Value Decomposition: A = U * diag(S) * V^T.
//!
//! Decomposes an m x n matrix A into:
//! - U: m x n matrix of left singular vectors; columns for nonzero singular values are orthonormal
//! - S: n singular values in descending order
//! - V: n x n matrix of right singular vectors (orthonormal)
//!
//! Properties:
//! - Works for any m x n matrix (m can be less, equal, or greater than n)
//! - For m < n, the n-m columns associated with zero singular values are padding and are not
//!   guaranteed to be orthonormal after U is truncated to m rows
//! - Singular values are always non-negative
//! - Provides the best low-rank approximation of a matrix
//! - Useful for solving ill-conditioned linear systems
//!
//! @param theA input matrix A (m x n)
//! @param theTolerance for rank determination (relative to largest singular value)
//! @return SVD decomposition result
inline SVDResult SVD(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  SVDResult aResult;

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  const double aRelTol = Utils::RelativeTolerance(theTolerance, std::max(aM, aN));
  if (aM == 0 || aN == 0 || aRelTol < 0.0 || !Utils::IsFinite(theA))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // U matrix needs to be at least n x n for the algorithm
  const size_t aURows = std::max(aM, aN);

  math_Matrix aUStorage(aURows, aN, 0.0);
  math_Vector aWStorage(aN);
  math_Matrix aVStorage(aN, aN);

  // Scaling keeps the legacy bidiagonalization away from overflow and underflow.
  double aScale = 0.0;
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aScale = std::max(aScale, std::abs(theA.At(i, j)));
    }
  }
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aUStorage.ChangeAt(i, j) = aScale == 0.0 ? 0.0 : theA.At(i, j) / aScale;
    }
  }

  // Perform SVD decomposition
  const int   aRecipeM = static_cast<int>(aURows);
  const int   aRecipeN = static_cast<int>(aN);
  math_Matrix aU(&aUStorage.ChangeAt(0, 0), 1, aRecipeM, 1, aRecipeN);
  math_Vector aW(&aWStorage[0], 1, aRecipeN);
  math_Matrix aV(&aVStorage.ChangeAt(0, 0), 1, aRecipeN, 1, aRecipeN);
  if (SVD_Decompose(aU, aW, aV) != 0)
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  for (size_t i = 0; i < aN; ++i)
  {
    aWStorage[i] *= aScale;
  }

  // SVD_Decompose does not promise ordering; move both singular-vector columns with each value.
  for (size_t i = 0; i + 1 < aN; ++i)
  {
    size_t aMaxIndex = i;
    for (size_t j = i + 1; j < aN; ++j)
    {
      if (aWStorage[j] > aWStorage[aMaxIndex])
      {
        aMaxIndex = j;
      }
    }
    if (aMaxIndex != i)
    {
      std::swap(aWStorage[i], aWStorage[aMaxIndex]);
      for (size_t j = 0; j < aURows; ++j)
      {
        std::swap(aUStorage.ChangeAt(j, i), aUStorage.ChangeAt(j, aMaxIndex));
      }
      for (size_t j = 0; j < aN; ++j)
      {
        std::swap(aVStorage.ChangeAt(j, i), aVStorage.ChangeAt(j, aMaxIndex));
      }
    }
  }

  // Determine numerical rank
  double aMaxSV = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    aMaxSV = std::max(aMaxSV, aWStorage[i]);
  }

  const double aThreshold = aRelTol * aMaxSV;
  aResult.Rank            = 0;
  for (size_t i = 0; i < aN; ++i)
  {
    if (aWStorage[i] > aThreshold)
    {
      ++aResult.Rank;
    }
  }

  const size_t aFullRank = std::min(aM, aN);
  if (aResult.Rank == aFullRank && aFullRank > 0)
  {
    aResult.ConditionNumber = aWStorage[0] / aWStorage[aFullRank - 1];
  }

  aResult.U = math_Matrix(aM, aN);
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aResult.U->ChangeAt(i, j) = aUStorage.At(i, j);
    }
  }

  aResult.SingularValues = math_Vector(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    (*aResult.SingularValues)[i] = aWStorage[i];
  }

  aResult.V = math_Matrix(aN, aN);
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aResult.V->ChangeAt(i, j) = aVStorage.At(i, j);
    }
  }

  aResult.Status = Utils::IsFinite(*aResult.U) && Utils::IsFinite(*aResult.SingularValues)
                       && Utils::IsFinite(*aResult.V)
                     ? Status::OK
                     : Status::NumericalError;
  return aResult;
}

//! Solve linear system Ax = b using SVD decomposition.
//! This is particularly useful for ill-conditioned or singular systems.
//!
//! For overdetermined systems (m > n), finds the least squares solution.
//! For underdetermined systems (m < n), finds the minimum norm solution.
//!
//! @param theA coefficient matrix (m x n)
//! @param theB right-hand side vector (length m)
//! @param theTolerance for singular value threshold
//! @return result containing solution vector
inline LinearResult SolveSVD(const math_Matrix& theA,
                             const math_Vector& theB,
                             double             theTolerance = 1.0e-6)
{
  LinearResult aResult;

  // Perform SVD
  SVDResult aSVD = SVD(theA, theTolerance);
  if (!aSVD.IsDone())
  {
    aResult.Status = aSVD.Status;
    return aResult;
  }

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  // Check dimensions
  if (theB.Size() != aM || !Utils::IsFinite(theB))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix& aU = *aSVD.U;
  const math_Vector& aW = *aSVD.SingularValues;
  const math_Matrix& aV = *aSVD.V;

  // Compute threshold for singular values
  double aMaxSV = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    aMaxSV = std::max(aMaxSV, aW[i]);
  }
  const double aRelTol = Utils::RelativeTolerance(theTolerance, std::max(aM, aN));
  const double aWMin   = aRelTol * aMaxSV;

  // Solve: x = V * diag(1/w) * U^T * b
  // First compute tmp = U^T * b
  math_Vector aTmp(aN, 0.0);
  for (size_t j = 0; j < aN; ++j)
  {
    double aSum = 0.0;
    for (size_t i = 0; i < aM; ++i)
    {
      aSum += aU.At(i, j) * theB.At(i);
    }
    // Divide by singular value if above threshold
    if (aW[j] > aWMin)
    {
      aTmp[j] = aSum / aW[j];
    }
    // else aTmp(j) remains 0 (regularization)
  }

  // Compute x = V * tmp
  aResult.Solution = math_Vector(aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    double aSum = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aSum += aV.At(i, j) * aTmp[j];
    }
    (*aResult.Solution)[i] = aSum;
  }

  aResult.Status = Utils::IsFinite(*aResult.Solution) ? Status::OK : Status::NumericalError;
  return aResult;
}

//! Compute pseudo-inverse (Moore-Penrose inverse) of matrix A.
//! A^+ = V * diag(1/w) * U^T where singular values below threshold are set to 0.
//!
//! Properties:
//! - A * A^+ * A = A
//! - A^+ * A * A^+ = A^+
//! - (A * A^+)^T = A * A^+
//! - (A^+ * A)^T = A^+ * A
//!
//! @param theA input matrix (m x n)
//! @param theTolerance for singular value threshold
//! @return result containing pseudo-inverse matrix (n x m)
inline InverseResult PseudoInverse(const math_Matrix& theA, double theTolerance = 1.0e-6)
{
  InverseResult aResult;

  // Perform SVD
  SVDResult aSVD = SVD(theA, theTolerance);
  if (!aSVD.IsDone())
  {
    aResult.Status = aSVD.Status;
    return aResult;
  }

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  const math_Matrix& aU = *aSVD.U;
  const math_Vector& aW = *aSVD.SingularValues;
  const math_Matrix& aV = *aSVD.V;

  // Compute threshold
  double aMaxSV = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    aMaxSV = std::max(aMaxSV, aW[i]);
  }
  const double aRelTol = Utils::RelativeTolerance(theTolerance, std::max(aM, aN));
  const double aWMin   = aRelTol * aMaxSV;

  // Compute A^+ = V * diag(1/w) * U^T
  // Result is n x m
  aResult.Inverse = math_Matrix(aN, aM, 0.0);

  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aM; ++j)
    {
      double aSum = 0.0;
      for (size_t k = 0; k < aN; ++k)
      {
        if (aW[k] > aWMin)
        {
          aSum += aV.At(i, k) * aU.At(j, k) / aW[k];
        }
      }
      aResult.Inverse->ChangeAt(i, j) = aSum;
    }
  }

  aResult.Status = Utils::IsFinite(*aResult.Inverse) ? Status::OK : Status::NumericalError;
  return aResult;
}

//! Compute condition number of matrix using SVD.
//! Condition number = sigma_max / sigma_min (ratio of largest to smallest singular value).
//!
//! High condition number (> 1e10) indicates ill-conditioned matrix.
//!
//! @param theA input matrix
//! @return condition number (infinity if matrix is singular)
inline double ConditionNumber(const math_Matrix& theA)
{
  SVDResult aSVD = SVD(theA);
  if (!aSVD.IsDone())
  {
    return std::numeric_limits<double>::infinity();
  }

  return aSVD.ConditionNumber;
}

//! Compute numerical rank of matrix using SVD.
//! Rank is the number of singular values above the threshold.
//!
//! @param theA input matrix
//! @param theTolerance relative tolerance for singular values
//! @return numerical rank
inline size_t NumericalRank(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  SVDResult aSVD = SVD(theA, theTolerance);
  return aSVD.IsDone() ? aSVD.Rank : 0;
}

} // namespace MathLin

#endif // _MathLin_SVD_HeaderFile
