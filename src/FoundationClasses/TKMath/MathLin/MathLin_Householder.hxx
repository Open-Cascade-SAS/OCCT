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

#ifndef _MathLin_Householder_HeaderFile
#define _MathLin_Householder_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <algorithm>

namespace MathLin
{
using namespace MathUtils;

//! Result for QR decomposition using Householder reflections.
struct QRResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Matrix> Q;        //!< Orthogonal matrix Q (m x m)
  std::optional<math_Matrix> R;        //!< Upper triangular matrix R (m x n)
  int                        Rank = 0; //!< Numerical rank

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! QR decomposition using Householder reflections: A = Q * R.
//!
//! Decomposes an m x n matrix A (m >= n) into:
//! - Q: m x m orthogonal matrix (Q^T * Q = I)
//! - R: m x n upper triangular matrix
//!
//! The Householder method applies orthogonal transformations
//! to reduce A to upper triangular form. It is more numerically
//! stable than Gram-Schmidt orthogonalization.
//!
//! Uses: Least squares problems, orthogonalization, computing
//! determinant sign.
//!
//! @param theA input matrix A (m x n, m >= n)
//! @param theTolerance for rank determination
//! @return QR decomposition result
inline QRResult QR(const math_Matrix& theA, double theTolerance = 1.0e-20)
{
  QRResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aM        = aRowUpper - aRowLower + 1; // Number of rows
  const int aN        = aColUpper - aColLower + 1; // Number of columns

  if (aM < aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Working copy of A that will become R
  math_Matrix aR(aRowLower, aRowUpper, aColLower, aColUpper);
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aR(i, j) = theA(i, j);
    }
  }

  // Q starts as identity
  math_Matrix aQ(aRowLower, aRowUpper, aRowLower, aRowUpper, 0.0);
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    aQ(i, i) = 1.0;
  }

  // Householder vector storage
  math_Vector aV(aRowLower, aRowUpper);

  aResult.Rank = 0;

  // Apply Householder reflections to each column
  for (int j = aColLower; j <= aColUpper; ++j)
  {
    const int aJOffset = j - aColLower;

    // Compute norm of column below diagonal
    double aNorm = 0.0;
    for (int i = aRowLower + aJOffset; i <= aRowUpper; ++i)
    {
      aNorm += MathUtils::Sqr(aR(i, j));
    }
    aNorm = std::sqrt(aNorm);

    if (aNorm < theTolerance)
    {
      // Column is essentially zero, skip
      continue;
    }

    ++aResult.Rank;

    // Compute Householder vector v
    double aAlpha = aR(aRowLower + aJOffset, j);
    double aBeta  = (aAlpha >= 0.0) ? -aNorm : aNorm;

    // v = [0,...,0, x_j - beta, x_{j+1}, ..., x_m]
    for (int i = aRowLower; i < aRowLower + aJOffset; ++i)
    {
      aV(i) = 0.0;
    }
    aV(aRowLower + aJOffset) = aAlpha - aBeta;
    for (int i = aRowLower + aJOffset + 1; i <= aRowUpper; ++i)
    {
      aV(i) = aR(i, j);
    }

    // Compute tau = 2 / (v^T v)
    double aVNormSq = 0.0;
    for (int i = aRowLower + aJOffset; i <= aRowUpper; ++i)
    {
      aVNormSq += MathUtils::Sqr(aV(i));
    }

    if (aVNormSq < MathUtils::THE_ZERO_TOL)
    {
      continue;
    }

    double aTau = 2.0 / aVNormSq;

    // Apply H = I - tau * v * v^T to remaining columns of R
    // R := H * R
    for (int k = j; k <= aColUpper; ++k)
    {
      // Compute v^T * R(:,k)
      double aVdotRk = 0.0;
      for (int i = aRowLower + aJOffset; i <= aRowUpper; ++i)
      {
        aVdotRk += aV(i) * aR(i, k);
      }

      // R(:,k) := R(:,k) - tau * (v^T * R(:,k)) * v
      for (int i = aRowLower + aJOffset; i <= aRowUpper; ++i)
      {
        aR(i, k) -= aTau * aVdotRk * aV(i);
      }
    }

    // Apply H to Q: Q := Q * H = Q - tau * Q * v * v^T
    for (int k = aRowLower; k <= aRowUpper; ++k)
    {
      // Compute Q(k,:) * v
      double aQkV = 0.0;
      for (int i = aRowLower + aJOffset; i <= aRowUpper; ++i)
      {
        aQkV += aQ(k, i) * aV(i);
      }

      // Q(k,:) := Q(k,:) - tau * (Q(k,:) * v) * v^T
      for (int i = aRowLower + aJOffset; i <= aRowUpper; ++i)
      {
        aQ(k, i) -= aTau * aQkV * aV(i);
      }
    }
  }

  // Zero out below-diagonal elements of R for cleanliness
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    for (int j = aColLower; j < aColLower + (i - aRowLower) && j <= aColUpper; ++j)
    {
      aR(i, j) = 0.0;
    }
  }

  aResult.Q      = aQ;
  aResult.R      = aR;
  aResult.Status = Status::OK;
  return aResult;
}

//! Solve overdetermined system Ax = b using QR decomposition (least squares).
//!
//! For m x n system with m > n, finds x that minimizes ||Ax - b||_2.
//!
//! Algorithm:
//! 1. Decompose A = Q * R
//! 2. Compute c = Q^T * b
//! 3. Solve R * x = c[1:n] (back substitution)
//!
//! @param theA coefficient matrix (m x n, m >= n)
//! @param theB right-hand side vector (length m)
//! @param theTolerance for singularity detection
//! @return result containing least squares solution
inline LinearResult SolveQR(const math_Matrix& theA,
                            const math_Vector& theB,
                            double             theTolerance = 1.0e-20)
{
  LinearResult aResult;

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

  // Perform QR decomposition
  QRResult aQR = QR(theA, theTolerance);
  if (!aQR.IsDone())
  {
    aResult.Status = aQR.Status;
    return aResult;
  }

  const math_Matrix& aQ = *aQR.Q;
  const math_Matrix& aR = *aQR.R;

  // Compute c = Q^T * b
  math_Vector aC(aRowLower, aRowUpper, 0.0);
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    double aSum = 0.0;
    for (int k = aRowLower; k <= aRowUpper; ++k)
    {
      aSum += aQ(k, i) * theB(theB.Lower() + k - aRowLower);
    }
    aC(i) = aSum;
  }

  // Back substitution: R[1:n, 1:n] * x = c[1:n]
  aResult.Solution = math_Vector(aColLower, aColUpper, 0.0);

  for (int i = aColUpper; i >= aColLower; --i)
  {
    const int aIOffset = i - aColLower;
    double    aDiag    = aR(aRowLower + aIOffset, i);

    if (std::abs(aDiag) < theTolerance)
    {
      aResult.Status = Status::Singular;
      return aResult;
    }

    double aSum = aC(aRowLower + aIOffset);
    for (int k = i + 1; k <= aColUpper; ++k)
    {
      aSum -= aR(aRowLower + aIOffset, k) * (*aResult.Solution)(k);
    }
    (*aResult.Solution)(i) = aSum / aDiag;
  }

  aResult.Status = Status::OK;
  return aResult;
}

//! Solve multiple right-hand sides using QR decomposition.
//!
//! @param theA coefficient matrix (m x n, m >= n)
//! @param theB right-hand side matrix (m x p)
//! @param theTolerance for singularity detection
//! @return result containing solution matrix (n x p)
inline LinearResult SolveQRMultiple(const math_Matrix& theA,
                                    const math_Matrix& theB,
                                    double             theTolerance = 1.0e-20)
{
  LinearResult aResult;

  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();

  // Check dimensions
  if (theB.RowNumber() != theA.RowNumber())
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Perform QR decomposition
  QRResult aQR = QR(theA, theTolerance);
  if (!aQR.IsDone())
  {
    aResult.Status = aQR.Status;
    return aResult;
  }

  // Solve for each column of B
  math_Vector aFirstSol(aColLower, aColUpper);
  bool        aFirstDone = false;

  for (int j = theB.LowerCol(); j <= theB.UpperCol(); ++j)
  {
    // Extract column j of B
    math_Vector aBj(theB.LowerRow(), theB.UpperRow());
    for (int i = theB.LowerRow(); i <= theB.UpperRow(); ++i)
    {
      aBj(i) = theB(i, j);
    }

    // Solve with QR (we should refactor to avoid re-decomposition)
    LinearResult aColResult = SolveQR(theA, aBj, theTolerance);
    if (!aColResult.IsDone())
    {
      aResult.Status = aColResult.Status;
      return aResult;
    }

    if (!aFirstDone)
    {
      aFirstSol  = *aColResult.Solution;
      aFirstDone = true;
    }
  }

  aResult.Solution = aFirstSol;
  aResult.Status   = Status::OK;
  return aResult;
}

} // namespace MathLin

#endif // _MathLin_Householder_HeaderFile
