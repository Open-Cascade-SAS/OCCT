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
#include <MathLin_Utils.hxx>

#include <cmath>
#include <algorithm>
#include <utility>

namespace MathLin
{
using namespace MathUtils;

//! Result for QR decomposition using Householder reflections.
struct QRResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Matrix> Q;                 //!< Orthogonal matrix Q (m x m)
  std::optional<math_Matrix> R;                 //!< Upper triangular matrix R (m x n)
  size_t                     Rank = 0;          //!< Diagonal rank estimate (no pivoting)
  std::optional<double>      ConditionEstimate; //!< Full-rank diagonal condition estimate
  double                     DiagonalThreshold = 0.0;

  //! Returns true when the computation completed successfully.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Returns the computation success status.
  explicit operator bool() const { return IsDone(); }
};

namespace HouseholderDetail
{
//! Apply a reflector to a column without squaring or doubling large input values.
inline bool ApplyHouseholder(const math_Vector& theV,
                             const size_t       theFirst,
                             const double       theTau,
                             math_Matrix&       theMatrix,
                             const size_t       theColumn)
{
  double aScale = 0.0;
  for (size_t i = theFirst; i < theMatrix.RowSize(); ++i)
  {
    if (theV[i] != 0.0)
    {
      aScale = std::max(aScale, std::abs(theMatrix.At(i, theColumn)));
    }
  }
  if (aScale == 0.0)
  {
    return true;
  }
  double aProjection = 0.0;
  for (size_t i = theFirst; i < theMatrix.RowSize(); ++i)
  {
    if (theV[i] != 0.0)
    {
      aProjection += theV[i] * (theMatrix.At(i, theColumn) / aScale);
    }
  }
  aProjection *= theTau;
  for (size_t i = theFirst; i < theMatrix.RowSize(); ++i)
  {
    // Keep the original entry: scaling it down and back can erase a small
    // component. FMA also avoids overflow in a correction that cancels the entry.
    const double aValue = std::fma(-aProjection * theV[i], aScale, theMatrix.At(i, theColumn));
    if (!std::isfinite(aValue))
    {
      return false;
    }
    theMatrix.ChangeAt(i, theColumn) = aValue;
  }
  return true;
}

//! Shared QR kernel. Applies Q^T directly to the supplied matrix.
//! Passing an identity matrix constructs Q^T; solving needs only the RHS columns.
//! For each column j, a reflector H_j zeros the entries below the diagonal:
//!   R <- H_j R,  transformed <- H_j transformed.
//! The final transformed matrix is Q^T times the original RHS (or identity).
//! Reflectors use scaled norms and normalized vectors to avoid intermediate
//! overflow/underflow. No column pivoting or rank-deficient solution is provided.
inline QRResult TransformQR(const math_Matrix& theA,
                            math_Matrix&       theTransformed,
                            const double       theTolerance)
{
  QRResult     aResult;
  const size_t aM      = theA.RowSize();
  const size_t aN      = theA.ColSize();
  const double aRelTol = Utils::RelativeTolerance(theTolerance, std::max(aM, aN));
  if (aM < aN || aN == 0 || aRelTol < 0.0 || theTransformed.RowSize() != aM
      || !Utils::IsFinite(theA) || !Utils::IsFinite(theTransformed))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  math_Matrix aR(aM, aN);
  aR = theA;
  math_Vector aV(aM);
  for (size_t j = 0; j < aN; ++j)
  {
    double aScale = 0.0;
    double aSumSq = 1.0;
    for (size_t i = j; i < aM; ++i)
    {
      if (!Utils::AccumulateNorm(aR.At(i, j), aScale, aSumSq))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }
    const auto aNorm = Utils::Norm(aScale, aSumSq);
    if (!aNorm.has_value())
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    if (*aNorm == 0.0)
    {
      continue;
    }
    const double aBeta = -std::copysign(*aNorm, aR.At(j, j));
    // Normalize before subtracting beta: x_j - beta can overflow even when
    // the matrix, its norm and the resulting factorization are all finite.
    aV[j]           = aR.At(j, j) / *aNorm - std::copysign(1.0, aBeta);
    double aVNormSq = aV[j] * aV[j];
    for (size_t i = j + 1; i < aM; ++i)
    {
      aV[i] = aR.At(i, j) / *aNorm;
      aVNormSq += aV[i] * aV[i];
    }
    const double aTau = 2.0 / aVNormSq;
    aR.ChangeAt(j, j) = aBeta;
    for (size_t i = j + 1; i < aM; ++i)
    {
      aR.ChangeAt(i, j) = 0.0;
    }
    for (size_t k = j + 1; k < aN; ++k)
    {
      if (!ApplyHouseholder(aV, j, aTau, aR, k))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }
    for (size_t k = 0; k < theTransformed.ColSize(); ++k)
    {
      if (!ApplyHouseholder(aV, j, aTau, theTransformed, k))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }
  }

  double aMaxDiag = 0.0;
  for (size_t j = 0; j < aN; ++j)
  {
    aMaxDiag = std::max(aMaxDiag, std::abs(aR.At(j, j)));
  }
  double aMinDiag           = aMaxDiag;
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
    const double aCondition = aMaxDiag / aMinDiag;
    if (std::isfinite(aCondition))
    {
      aResult.ConditionEstimate = aCondition;
    }
  }
  aResult.R      = std::move(aR);
  aResult.Status = Status::OK;
  return aResult;
}

//! Back substitution for an already transformed set of right-hand sides.
inline LinearMultipleResult SolveTransformedQR(const QRResult&    theQR,
                                               const math_Matrix& theTransformed)
{
  LinearMultipleResult aResult;
  if (!theQR.IsDone())
  {
    aResult.Status = theQR.Status;
    return aResult;
  }
  if (!theQR.R.has_value())
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  const math_Matrix& aR = *theQR.R;
  const size_t       aN = aR.ColSize();
  if (aN == 0 || aR.RowSize() < aN || theTransformed.RowSize() != aR.RowSize() || theQR.Rank > aN
      || !Utils::IsFinite(theTransformed))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (!Utils::IsFinite(aR) || !std::isfinite(theQR.DiagonalThreshold)
      || theQR.DiagonalThreshold < 0.0)
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  if (theQR.Rank != aN)
  {
    aResult.Status = Status::Singular;
    return aResult;
  }

  math_Matrix aX(aN, theTransformed.ColSize());
  for (size_t j = 0; j < theTransformed.ColSize(); ++j)
  {
    for (size_t i = aN; i-- > 0;)
    {
      const double aDiag = aR.At(i, i);
      if (std::abs(aDiag) <= theQR.DiagonalThreshold)
      {
        aResult.Status = Status::Singular;
        return aResult;
      }

      double aValue = theTransformed.At(i, j);
      for (size_t k = i + 1; k < aN; ++k)
      {
        aValue -= aR.At(i, k) * aX.At(k, j);
      }
      aX.ChangeAt(i, j) = aValue / aDiag;
    }
  }
  if (!Utils::IsFinite(aX))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  aResult.Solutions = std::move(aX);
  aResult.Status    = Status::OK;
  return aResult;
}

} // namespace HouseholderDetail

//! QR decomposition A = Q * R for an m x n matrix, m >= n.
//! Returns the full m x m orthogonal Q and m x n upper-triangular R.
//! theTolerance is relative to the largest diagonal of R, with a machine-precision floor.
//! SolveQR and SolveQRMultiple avoid forming the full Q when only a solution is needed.
//! @param theA coefficient matrix with m rows and n columns, m >= n
//! @param theTolerance relative diagonal rank threshold
//! @return orthogonal Q, triangular R, rank estimate and computation status
inline QRResult QR(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  if (theA.RowSize() < theA.ColSize() || theA.ColSize() == 0
      || Utils::RelativeTolerance(theTolerance, theA.RowSize()) < 0.0 || !Utils::IsFinite(theA))
  {
    QRResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  const size_t aM = theA.RowSize();
  math_Matrix  aQT(aM, aM, 0.0);
  for (size_t i = 0; i < aM; ++i)
  {
    aQT.ChangeAt(i, i) = 1.0;
  }
  QRResult aResult = HouseholderDetail::TransformQR(theA, aQT, theTolerance);
  if (aResult.IsDone())
  {
    aResult.Q = aQT.Transposed();
  }
  return aResult;
}

//! Solve AX = B by Householder QR, reusing each reflector for all RHS columns.
//! A has m rows and n columns; B has m rows and p right-hand sides. For m > n,
//! each solution minimizes the Euclidean norm of its residual independently.
//! Invalid dimensions, non-finite inputs or an invalid tolerance return InvalidInput;
//! deficient diagonal rank returns Singular; non-finite arithmetic returns NumericalError.
//! Solutions are available only when IsDone() is true.
//! Requires m >= n and full column rank. Uses O(m*(n+p)) storage, without a dense Q.
//! Input matrix bounds may differ; output rows and columns use the math_Matrix default bounds.
//! @param theA coefficient matrix with m rows and n columns
//! @param theB right-hand side matrix with m rows and p columns
//! @param theTolerance relative rank threshold, with a machine-precision floor
//! @return solution matrix and computation status
inline LinearMultipleResult SolveQRMultiple(const math_Matrix& theA,
                                            const math_Matrix& theB,
                                            double             theTolerance = 1.0e-15)
{
  LinearMultipleResult aResult;
  math_Matrix          aTransformed(theB);
  const QRResult       aQR = HouseholderDetail::TransformQR(theA, aTransformed, theTolerance);
  if (!aQR.IsDone())
  {
    aResult.Status = aQR.Status;
    return aResult;
  }
  return HouseholderDetail::SolveTransformedQR(aQR, aTransformed);
}

//! Solve Ax = b by QR (least squares when m > n), without constructing Q.
//! @param theA coefficient matrix (m rows, n columns, m >= n)
//! @param theB right-hand side vector with m entries
//! @param theTolerance relative rank threshold, with a machine-precision floor
//! @return solution and status; the solution is available only on success
inline LinearResult SolveQR(const math_Matrix& theA,
                            const math_Vector& theB,
                            double             theTolerance = 1.0e-15)
{
  LinearResult aResult;
  if (theB.Size() == 0 || theA.RowSize() != theB.Size())
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  // The matrix view borrows an owned copy; QR may modify it without changing B.
  math_Vector    aRhs(theB);
  math_Matrix    aTransformed(&aRhs.ChangeAt(0), 0, aRhs.Length() - 1, 0, 0);
  const QRResult aQR = HouseholderDetail::TransformQR(theA, aTransformed, theTolerance);
  if (!aQR.IsDone())
  {
    aResult.Status = aQR.Status;
    return aResult;
  }
  const auto aFit = HouseholderDetail::SolveTransformedQR(aQR, aTransformed);
  aResult.Status  = aFit.Status;
  if (aFit.IsDone())
  {
    // Copy from the column view so the returned vector owns its storage.
    const math_Vector aSolution(&aFit.Solutions->At(0, 0), 0, theA.ColNumber() - 1);
    aResult.Solution = aSolution;
  }
  return aResult;
}

} // namespace MathLin

#endif // _MathLin_Householder_HeaderFile
