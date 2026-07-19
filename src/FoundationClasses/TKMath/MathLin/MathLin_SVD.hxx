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

#include <cmath>
#include <algorithm>

namespace MathLin
{
using namespace MathUtils;

//! Result for SVD decomposition.
struct SVDResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Matrix> U;              //!< Left singular vectors (m x n)
  std::optional<math_Vector> SingularValues; //!< Singular values (n elements)
  std::optional<math_Matrix> V;              //!< Right singular vectors (n x n)
  int                        Rank = 0;       //!< Numerical rank

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Singular Value Decomposition: A = U * diag(S) * V^T.
//!
//! Decomposes an m x n matrix A into:
//! - U: m x n matrix of left singular vectors (orthonormal columns)
//! - S: n singular values in descending order
//! - V: n x n matrix of right singular vectors (orthonormal)
//!
//! Properties:
//! - Works for any m x n matrix (m can be less, equal, or greater than n)
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

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aM        = aRowUpper - aRowLower + 1; // Number of rows
  const int aN        = aColUpper - aColLower + 1; // Number of columns

  // U matrix needs to be at least n x n for the algorithm
  const int aURows = std::max(aM, aN);

  // Create working matrices with 1-based indexing as required by SVD_Decompose
  math_Matrix aU(1, aURows, 1, aN, 0.0);
  math_Vector aW(1, aN);
  math_Matrix aV(1, aN, 1, aN);

  // Copy input matrix to U
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aU(i - aRowLower + 1, j - aColLower + 1) = theA(i, j);
    }
  }

  // Perform SVD decomposition
  if (SVD_Decompose(aU, aW, aV) != 0)
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Determine numerical rank
  double aMaxSV = 0.0;
  for (int i = 1; i <= aN; ++i)
  {
    aMaxSV = std::max(aMaxSV, aW(i));
  }

  double aThreshold = theTolerance * aMaxSV;
  aResult.Rank      = 0;
  for (int i = 1; i <= aN; ++i)
  {
    if (aW(i) > aThreshold)
    {
      ++aResult.Rank;
    }
  }

  // Copy results back with original indexing
  aResult.U = math_Matrix(aRowLower, aRowLower + aM - 1, aColLower, aColLower + aN - 1);
  for (int i = 1; i <= aM; ++i)
  {
    for (int j = 1; j <= aN; ++j)
    {
      (*aResult.U)(aRowLower + i - 1, aColLower + j - 1) = aU(i, j);
    }
  }

  aResult.SingularValues = math_Vector(aColLower, aColLower + aN - 1);
  for (int i = 1; i <= aN; ++i)
  {
    (*aResult.SingularValues)(aColLower + i - 1) = aW(i);
  }

  aResult.V = math_Matrix(aColLower, aColLower + aN - 1, aColLower, aColLower + aN - 1);
  for (int i = 1; i <= aN; ++i)
  {
    for (int j = 1; j <= aN; ++j)
    {
      (*aResult.V)(aColLower + i - 1, aColLower + j - 1) = aV(i, j);
    }
  }

  aResult.Status = Status::OK;
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

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aM        = aRowUpper - aRowLower + 1;

  // Check dimensions
  if (theB.Length() != aM)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aBLower = theB.Lower(); // B vector may have different indexing

  const math_Matrix& aU = *aSVD.U;
  const math_Vector& aW = *aSVD.SingularValues;
  const math_Matrix& aV = *aSVD.V;

  // Compute threshold for singular values
  double aMaxSV = 0.0;
  for (int i = aColLower; i <= aColUpper; ++i)
  {
    aMaxSV = std::max(aMaxSV, aW(i));
  }
  double aWMin = theTolerance * aMaxSV;

  // Solve: x = V * diag(1/w) * U^T * b
  // First compute tmp = U^T * b
  math_Vector aTmp(aColLower, aColUpper, 0.0);
  for (int j = aColLower; j <= aColUpper; ++j)
  {
    double aSum = 0.0;
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      // Map i to B's index space: B[aBLower + (i - aRowLower)]
      aSum += aU(i, j) * theB(aBLower + (i - aRowLower));
    }
    // Divide by singular value if above threshold
    if (aW(j) > aWMin)
    {
      aTmp(j) = aSum / aW(j);
    }
    // else aTmp(j) remains 0 (regularization)
  }

  // Compute x = V * tmp
  aResult.Solution = math_Vector(aColLower, aColUpper, 0.0);
  for (int i = aColLower; i <= aColUpper; ++i)
  {
    double aSum = 0.0;
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aSum += aV(i, j) * aTmp(j);
    }
    (*aResult.Solution)(i) = aSum;
  }

  aResult.Status = Status::OK;
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

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();

  const math_Matrix& aU = *aSVD.U;
  const math_Vector& aW = *aSVD.SingularValues;
  const math_Matrix& aV = *aSVD.V;

  // Compute threshold
  double aMaxSV = 0.0;
  for (int i = aColLower; i <= aColUpper; ++i)
  {
    aMaxSV = std::max(aMaxSV, aW(i));
  }
  double aWMin = theTolerance * aMaxSV;

  // Compute A^+ = V * diag(1/w) * U^T
  // Result is n x m
  aResult.Inverse = math_Matrix(aColLower, aColUpper, aRowLower, aRowUpper, 0.0);

  for (int i = aColLower; i <= aColUpper; ++i)
  {
    for (int j = aRowLower; j <= aRowUpper; ++j)
    {
      double aSum = 0.0;
      for (int k = aColLower; k <= aColUpper; ++k)
      {
        if (aW(k) > aWMin)
        {
          aSum += aV(i, k) * aU(j, k) / aW(k);
        }
      }
      (*aResult.Inverse)(i, j) = aSum;
    }
  }

  aResult.Status = Status::OK;
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

  const math_Vector& aW     = *aSVD.SingularValues;
  const int          aLower = aW.Lower();
  const int          aUpper = aW.Upper();

  double aMaxSV = 0.0;
  double aMinSV = std::numeric_limits<double>::max();

  for (int i = aLower; i <= aUpper; ++i)
  {
    if (aW(i) > 0.0)
    {
      aMaxSV = std::max(aMaxSV, aW(i));
      aMinSV = std::min(aMinSV, aW(i));
    }
  }

  if (aMinSV <= 0.0 || aMaxSV <= 0.0)
  {
    return std::numeric_limits<double>::infinity();
  }

  return aMaxSV / aMinSV;
}

//! Compute numerical rank of matrix using SVD.
//! Rank is the number of singular values above the threshold.
//!
//! @param theA input matrix
//! @param theTolerance relative tolerance for singular values
//! @return numerical rank
inline int NumericalRank(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  SVDResult aSVD = SVD(theA, theTolerance);
  return aSVD.IsDone() ? aSVD.Rank : 0;
}

} // namespace MathLin

#endif // _MathLin_SVD_HeaderFile
