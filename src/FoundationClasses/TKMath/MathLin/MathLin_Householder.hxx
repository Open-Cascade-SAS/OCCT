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
#include "MathLin_Utils.hxx"

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
  size_t                     Rank = 0; //!< Numerical rank
  double                     ConditionEstimate = std::numeric_limits<double>::infinity();
  double                     DiagonalThreshold = 0.0;

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
inline QRResult QR(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  QRResult aResult;

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  const double aRelTol = Utils::RelativeTolerance(theTolerance, std::max(aM, aN));
  if (aM < aN || aN == 0 || aRelTol < 0.0 || !Utils::IsFinite(theA))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Working copy of A that will become R
  math_Matrix aR(aM, aN);
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      aR.ChangeAt(i, j) = theA.At(i, j);
    }
  }

  // Q starts as identity
  math_Matrix aQ(aM, aM, 0.0);
  for (size_t i = 0; i < aM; ++i)
  {
    aQ.ChangeAt(i, i) = 1.0;
  }

  // Householder vector storage
  math_Vector aV(aM);

  // Apply Householder reflections to each column
  for (size_t j = 0; j < aN; ++j)
  {
    // Compute norm of column below diagonal
    double aNormScale = 0.0;
    double aNormSumSq = 1.0;
    for (size_t i = j; i < aM; ++i)
    {
      if (!Utils::AccumulateNorm(aR.At(i, j), aNormScale, aNormSumSq))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }
    const std::optional<double> aNormValue = Utils::Norm(aNormScale, aNormSumSq);
    if (!aNormValue.has_value())
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    const double aNorm = *aNormValue;

    if (aNorm == 0.0)
    {
      // Column is essentially zero, skip
      continue;
    }

    // Compute Householder vector v
    double aAlpha = aR.At(j, j);
    double aBeta  = (aAlpha >= 0.0) ? -aNorm : aNorm;

    // v = [0,...,0, x_j - beta, x_{j+1}, ..., x_m]
    for (size_t i = 0; i < j; ++i)
    {
      aV[i] = 0.0;
    }
    aV[j] = aAlpha - aBeta;
    for (size_t i = j + 1; i < aM; ++i)
    {
      aV[i] = aR.At(i, j);
    }

    // Compute tau = 2 / (v^T v)
    double aVScale = 0.0;
    for (size_t i = j; i < aM; ++i)
    {
      aVScale = std::max(aVScale, std::abs(aV[i]));
    }
    if (aVScale == 0.0)
    {
      continue;
    }
    double aVNormSq = 0.0;
    for (size_t i = j; i < aM; ++i)
    {
      aV[i] /= aVScale;
      aVNormSq += aV[i] * aV[i];
    }
    const double aTau = 2.0 / aVNormSq;

    // Apply H = I - tau * v * v^T to remaining columns of R
    // R := H * R
    for (size_t k = j; k < aN; ++k)
    {
      // Compute v^T * R(:,k)
      double aVdotRk = 0.0;
      for (size_t i = j; i < aM; ++i)
      {
        aVdotRk += aV[i] * aR.At(i, k);
      }

      // R(:,k) := R(:,k) - tau * (v^T * R(:,k)) * v
      for (size_t i = j; i < aM; ++i)
      {
        aR.ChangeAt(i, k) -= aTau * aVdotRk * aV[i];
      }
    }

    // Apply H to Q: Q := Q * H = Q - tau * Q * v * v^T
    for (size_t k = 0; k < aM; ++k)
    {
      // Compute Q(k,:) * v
      double aQkV = 0.0;
      for (size_t i = j; i < aM; ++i)
      {
        aQkV += aQ.At(k, i) * aV[i];
      }

      // Q(k,:) := Q(k,:) - tau * (Q(k,:) * v) * v^T
      for (size_t i = j; i < aM; ++i)
      {
        aQ.ChangeAt(k, i) -= aTau * aQkV * aV[i];
      }
    }
  }

  double aMaxDiag = 0.0;
  double aMinDiag = std::numeric_limits<double>::infinity();
  for (size_t j = 0; j < aN; ++j)
  {
    aMaxDiag = std::max(aMaxDiag, std::abs(aR.At(j, j)));
  }
  aResult.DiagonalThreshold = aRelTol * aMaxDiag;
  for (size_t j = 0; j < aN; ++j)
  {
    const double aDiag = std::abs(aR.At(j, j));
    if (aDiag > aResult.DiagonalThreshold)
    {
      ++aResult.Rank;
      aMinDiag = std::min(aMinDiag, aDiag);
    }
  }
  if (aResult.Rank == aN)
  {
    aResult.ConditionEstimate = aMaxDiag / aMinDiag;
  }

  // Zero out below-diagonal elements of R for cleanliness
  for (size_t i = 0; i < aM; ++i)
  {
    for (size_t j = 0; j < i && j < aN; ++j)
    {
      aR.ChangeAt(i, j) = 0.0;
    }
  }

  aResult.Q      = aQ;
  aResult.R      = aR;
  aResult.Status = Utils::IsFinite(aQ) && Utils::IsFinite(aR) ? Status::OK
                                                                    : Status::NumericalError;
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
                            double             theTolerance = 1.0e-15)
{
  LinearResult aResult;

  const size_t aM = theA.RowSize();

  // Check dimensions
  if (theB.Size() != aM || !Utils::IsFinite(theB))
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
  if (aQR.Rank != theA.ColSize())
  {
    aResult.Status = Status::Singular;
    return aResult;
  }

  // Compute c = Q^T * b
  math_Vector aC(aM, 0.0);
  for (size_t i = 0; i < aM; ++i)
  {
    double aSum = 0.0;
    for (size_t k = 0; k < aM; ++k)
    {
      aSum += aQ.At(k, i) * theB.At(k);
    }
    aC[i] = aSum;
  }

  // Back substitution: R[0:n, 0:n] * x = c[0:n]
  const size_t aN = theA.ColSize();
  aResult.Solution = math_Vector(aN, 0.0);

  for (size_t i = aN; i-- > 0;)
  {
    const double aDiag = aR.At(i, i);

    if (std::abs(aDiag) <= aQR.DiagonalThreshold)
    {
      aResult.Status = Status::Singular;
      return aResult;
    }

    double aSum = aC[i];
    for (size_t k = i + 1; k < aN; ++k)
    {
      aSum -= aR.At(i, k) * (*aResult.Solution)[k];
    }
    (*aResult.Solution)[i] = aSum / aDiag;
  }

  aResult.Status = Utils::IsFinite(*aResult.Solution) ? Status::OK : Status::NumericalError;
  return aResult;
}

//! Solve multiple right-hand sides using QR decomposition.
//!
//! @param theA coefficient matrix (m x n, m >= n)
//! @param theB right-hand side matrix (m x p)
//! @param theTolerance for singularity detection
//! @return result containing solution matrix (n x p)
inline LinearMultipleResult SolveQRMultiple(const math_Matrix& theA,
                                            const math_Matrix& theB,
                                            double             theTolerance = 1.0e-15)
{
  LinearMultipleResult aResult;

  // Check dimensions
  if (theB.RowSize() != theA.RowSize() || !Utils::IsFinite(theB))
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
  if (aQR.Rank != theA.ColSize())
  {
    aResult.Status = Status::Singular;
    return aResult;
  }

  // Solve for each column of B
  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();
  math_Matrix aX(aN, theB.ColSize(), 0.0);

  for (size_t j = 0; j < theB.ColSize(); ++j)
  {
    // Compute c = Q^T * b_j
    math_Vector aC(aM, 0.0);
    for (size_t i = 0; i < aM; ++i)
    {
      double aSum = 0.0;
      for (size_t k = 0; k < aM; ++k)
      {
        aSum += aQ.At(k, i) * theB.At(k, j);
      }
      aC[i] = aSum;
    }

    // Back substitution: R[0:n,0:n] * x_j = c[0:n]
    for (size_t i = aN; i-- > 0;)
    {
      const double aDiag = aR.At(i, i);

      if (std::abs(aDiag) <= aQR.DiagonalThreshold)
      {
        aResult.Status = Status::Singular;
        return aResult;
      }

      double aSum = aC[i];
      for (size_t k = i + 1; k < aN; ++k)
      {
        aSum -= aR.At(i, k) * aX.At(k, j);
      }
      aX.ChangeAt(i, j) = aSum / aDiag;
    }
  }

  aResult.Solutions = aX;
  aResult.Status    = Utils::IsFinite(aX) ? Status::OK : Status::NumericalError;
  return aResult;
}

} // namespace MathLin

#endif // _MathLin_Householder_HeaderFile
