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

#ifndef _MathLin_Crout_HeaderFile
#define _MathLin_Crout_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include "MathLin_Utils.hxx"
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>

namespace MathLin
{
using namespace MathUtils;

//! Result for Crout LDL^T decomposition.
//! Specialized for symmetric matrices.
struct CroutResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Matrix> L;           //!< Lower triangular matrix (unit diagonal)
  std::optional<math_Vector> D;           //!< Diagonal elements
  std::optional<math_Matrix> Inverse;     //!< Full symmetric inverse matrix
  std::optional<double>      Determinant; //!< Matrix determinant
  double                     MinAbsPivot       = 0.0;
  double                     MaxAbsPivot       = 0.0;
  double                     ConditionEstimate = std::numeric_limits<double>::infinity();

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Crout decomposition for symmetric matrices: A = L * D * L^T.
//!
//! This algorithm decomposes a symmetric matrix A into:
//! - L: lower triangular matrix with unit diagonal
//! - D: diagonal matrix
//!
//! Properties:
//! - Only the lower triangle of A is used
//! - Faster than general LU for symmetric matrices
//! - Computes inverse efficiently
//! - Supports negative pivots when the unpivoted LDL^T factorization exists
//!
//! @param theA input symmetric matrix (only lower triangle used)
//! @param theMinPivot minimum pivot value (smaller treated as singular)
//! @return Crout decomposition result
inline CroutResult Crout(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  CroutResult aResult;

  const size_t aN = theA.RowSize();

  // Check for square matrix
  const double aRelTol = Utils::RelativeTolerance(theTolerance, aN);
  if (aN != theA.ColSize() || aN == 0 || aRelTol < 0.0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  math_Matrix aL(aN, aN, 0.0);
  math_Vector aDiag(aN);

  double aDet         = 1.0;
  double aMatrixScale = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j <= i; ++j)
    {
      const double aValue = theA.At(i, j);
      if (!std::isfinite(aValue))
      {
        aResult.Status = Status::InvalidInput;
        return aResult;
      }
      aMatrixScale = std::max(aMatrixScale, std::abs(aValue));
    }
  }

  // Crout decomposition: A = L * D * L^T
  for (size_t i = 0; i < aN; ++i)
  {
    // Compute L(i,j) for j < i
    for (size_t j = 0; j < i; ++j)
    {
      double aScale = 0.0;
      for (size_t k = 0; k < j; ++k)
      {
        aScale += aL.At(i, k) * aL.At(j, k) * aDiag[k];
      }
      aL.ChangeAt(i, j) = (theA.At(i, j) - aScale) / aDiag[j];
      if (!std::isfinite(aScale) || !std::isfinite(aL.At(i, j)))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }

    // Compute D(i)
    double aScale = 0.0;
    for (size_t k = 0; k < i; ++k)
    {
      aScale += aL.At(i, k) * aL.At(i, k) * aDiag[k];
    }
    aDiag[i] = theA.At(i, i) - aScale;

    // Check for singularity
    if (!std::isfinite(aScale) || !std::isfinite(aDiag[i]))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    if (std::abs(aDiag[i]) <= aRelTol * aMatrixScale)
    {
      aResult.Status = Status::Singular;
      return aResult;
    }

    aDet *= aDiag[i];
    if (!std::isfinite(aDet))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    const double anAbsPivot = std::abs(aDiag[i]);
    aResult.MaxAbsPivot     = std::max(aResult.MaxAbsPivot, anAbsPivot);
    aResult.MinAbsPivot     = i == 0 ? anAbsPivot : std::min(aResult.MinAbsPivot, anAbsPivot);

    aL.ChangeAt(i, i) = 1.0;
  }

  const math_Matrix aFactorL = aL;

  math_Vector anInvDiag(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    anInvDiag[i] = 1.0 / aDiag[i];
    if (!std::isfinite(anInvDiag[i]))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
  }

  // Compute inverse of L
  aL.ChangeAt(0, 0) = 1.0 / aL.At(0, 0);
  for (size_t i = 1; i < aN; ++i)
  {
    for (size_t k = 0; k < i; ++k)
    {
      double aScale = 0.0;
      for (size_t j = k; j < i; ++j)
      {
        aScale += aL.At(i, j) * aL.At(j, k);
      }
      aL.ChangeAt(i, k) = -aScale / aL.At(i, i);
      if (!std::isfinite(aScale) || !std::isfinite(aL.At(i, k)))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }
    aL.ChangeAt(i, i) = 1.0 / aL.At(i, i);
    if (!std::isfinite(aL.At(i, i)))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
  }

  // Compute inverse of A (lower triangle only)
  math_Matrix aInv(aN, aN, 0.0);
  for (size_t j = 0; j < aN; ++j)
  {
    double aScale = aL.At(j, j) * aL.At(j, j) * anInvDiag[j];
    for (size_t k = j + 1; k < aN; ++k)
    {
      aScale += aL.At(k, j) * aL.At(k, j) * anInvDiag[k];
    }
    if (!std::isfinite(aScale))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    aInv.ChangeAt(j, j) = aScale;

    for (size_t i = j + 1; i < aN; ++i)
    {
      aScale = aL.At(i, j) * aL.At(i, i) * anInvDiag[i];
      for (size_t k = i + 1; k < aN; ++k)
      {
        aScale += aL.At(k, j) * aL.At(k, i) * anInvDiag[k];
      }
      if (!std::isfinite(aScale))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
      aInv.ChangeAt(i, j) = aScale;
    }
  }

  if (!Utils::IsFinite(aInv) || !std::isfinite(aDet))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  aResult.L = math_Matrix(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j <= i; ++j)
    {
      aResult.L->ChangeAt(i, j) = aFactorL.At(i, j);
    }
  }

  aResult.D = math_Vector(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    (*aResult.D)[i] = aDiag[i];
  }

  aResult.Inverse = math_Matrix(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j <= i; ++j)
    {
      aResult.Inverse->ChangeAt(i, j) = aInv.At(i, j);
    }
    // Mirror to upper triangle for full symmetric matrix
    for (size_t j = i + 1; j < aN; ++j)
    {
      aResult.Inverse->ChangeAt(i, j) = aInv.At(j, i);
    }
  }

  aResult.Determinant       = aDet;
  aResult.ConditionEstimate = aResult.MaxAbsPivot / aResult.MinAbsPivot;
  aResult.Status            = Status::OK;
  return aResult;
}

//! Solve symmetric linear system Ax = b using Crout decomposition.
//!
//! Uses precomputed Crout decomposition to solve for x.
//! More efficient than LU for symmetric positive definite matrices.
//!
//! @param theA coefficient matrix (symmetric)
//! @param theB right-hand side vector
//! @param theMinPivot minimum pivot value
//! @return result containing solution vector
inline LinearResult SolveCrout(const math_Matrix& theA,
                               const math_Vector& theB,
                               double             theMinPivot = 1.0e-15)
{
  LinearResult aResult;

  // Perform Crout decomposition
  CroutResult aCrout = Crout(theA, theMinPivot);
  if (!aCrout.IsDone())
  {
    aResult.Status = aCrout.Status;
    return aResult;
  }

  const size_t aN = theA.RowSize();

  // Check dimensions
  if (theB.Size() != aN || !Utils::IsFinite(theB))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix& aInv = *aCrout.Inverse;

  // Compute X = Inverse * B using symmetry
  aResult.Solution = math_Vector(aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    double aSum = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aSum += aInv.At(i, j) * theB.At(j);
    }
    (*aResult.Solution)[i] = aSum;
  }

  aResult.Determinant = aCrout.Determinant;
  aResult.Status      = Utils::IsFinite(*aResult.Solution) ? Status::OK : Status::NumericalError;
  return aResult;
}

//! Compute inverse of symmetric matrix using Crout decomposition.
//!
//! @param theA input symmetric matrix
//! @param theMinPivot minimum pivot value
//! @return result containing full symmetric inverse matrix
inline InverseResult InvertCrout(const math_Matrix& theA, double theMinPivot = 1.0e-15)
{
  InverseResult aResult;

  CroutResult aCrout = Crout(theA, theMinPivot);
  if (!aCrout.IsDone())
  {
    aResult.Status = aCrout.Status;
    return aResult;
  }

  aResult.Status      = Status::OK;
  aResult.Inverse     = aCrout.Inverse;
  aResult.Determinant = aCrout.Determinant;
  return aResult;
}

} // namespace MathLin

#endif // _MathLin_Crout_HeaderFile
