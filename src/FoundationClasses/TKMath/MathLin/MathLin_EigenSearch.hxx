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

#ifndef _MathLin_EigenSearch_HeaderFile
#define _MathLin_EigenSearch_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include "MathLin_Utils.hxx"
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

namespace MathLin
{
using namespace MathUtils;

//! Result for eigenvalue decomposition of tridiagonal matrix.
struct EigenResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> EigenValues;  //!< Computed eigenvalues
  std::optional<math_Matrix> EigenVectors; //!< Eigenvectors as columns
  size_t                     Dimension = 0;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

namespace Utils
{

//! Relative threshold used to deflate a negligible tridiagonal coupling.
constexpr double THE_TRIDIAGONAL_DEFLATION_TOLERANCE = std::numeric_limits<double>::epsilon();

//! Multiplier for the scale-normalized eigenpair residual acceptance threshold.
constexpr double THE_TRIDIAGONAL_RESIDUAL_TOLERANCE_FACTOR = 256.0;

//! Finds the end of unreduced submatrix using deflation test.
inline size_t FindSubmatrixEnd(const math_Vector& theDiag,
                               const math_Vector& theSubdiag,
                               size_t             theStart,
                               size_t             theN)
{
  size_t aEnd;
  for (aEnd = theStart; aEnd + 1 < theN; ++aEnd)
  {
    const double aDiagScale = std::max(std::abs(theDiag[aEnd]), std::abs(theDiag[aEnd + 1]));
    const double aThreshold = THE_TRIDIAGONAL_DEFLATION_TOLERANCE * aDiagScale;
    if (std::abs(theSubdiag[aEnd]) <= aThreshold)
    {
      break;
    }
  }
  return aEnd;
}

//! Computes Wilkinson's shift for accelerated convergence.
inline double ComputeWilkinsonShift(const math_Vector& theDiag,
                                    const math_Vector& theSubdiag,
                                    size_t             theStart,
                                    size_t             theEnd)
{
  double aShift = (theDiag[theStart + 1] - theDiag[theStart])
                  / (2.0 * theSubdiag[theStart]);
  const double aRadius = std::hypot(1.0, aShift);

  if (aShift < 0.0)
  {
    aShift = theDiag[theEnd] - theDiag[theStart]
             + theSubdiag[theStart] / (aShift - aRadius);
  }
  else
  {
    aShift = theDiag[theEnd] - theDiag[theStart]
             + theSubdiag[theStart] / (aShift + aRadius);
  }
  return aShift;
}

//! Performs a single QL step with implicit shift.
inline bool PerformQLStep(math_Vector& theDiag,
                          math_Vector& theSubdiag,
                          math_Matrix& theEigenVec,
                          size_t       theStart,
                          size_t       theEnd,
                          double       theShift,
                          size_t       theN)
{
  double aSine     = 1.0;
  double aCosine   = 1.0;
  double aPrevDiag = 0.0;
  double aShift    = theShift;
  double aRadius   = 0.0;

  for (size_t aRowIdx = theEnd; aRowIdx-- > theStart;)
  {
    const double aTempVal     = aSine * theSubdiag[aRowIdx];
    const double aSubdiagTemp = aCosine * theSubdiag[aRowIdx];
    aRadius                   = std::hypot(aTempVal, aShift);
    theSubdiag[aRowIdx + 1]   = aRadius;

    if (aRadius == 0.0)
    {
      theDiag[aRowIdx + 1] -= aPrevDiag;
      theSubdiag[theEnd] = 0.0;
      return true;
    }

    aSine   = aTempVal / aRadius;
    aCosine = aShift / aRadius;
    aShift  = theDiag[aRowIdx + 1] - aPrevDiag;

    const double aRadiusTemp =
      (theDiag[aRowIdx] - aShift) * aSine + 2.0 * aCosine * aSubdiagTemp;
    aPrevDiag              = aSine * aRadiusTemp;
    theDiag[aRowIdx + 1]   = aShift + aPrevDiag;
    aShift                 = aCosine * aRadiusTemp - aSubdiagTemp;

    // Update eigenvector matrix
    for (size_t aVecIdx = 0; aVecIdx < theN; ++aVecIdx)
    {
      const double aTempVec = theEigenVec.At(aVecIdx, aRowIdx + 1);
      theEigenVec.ChangeAt(aVecIdx, aRowIdx + 1) =
        aSine * theEigenVec.At(aVecIdx, aRowIdx) + aCosine * aTempVec;
      theEigenVec.ChangeAt(aVecIdx, aRowIdx) =
        aCosine * theEigenVec.At(aVecIdx, aRowIdx) - aSine * aTempVec;
    }
  }

  theDiag[theStart] -= aPrevDiag;
  theSubdiag[theStart] = aShift;
  theSubdiag[theEnd]   = 0.0;

  return true;
}

//! Checks T*v = lambda*v without using overflow-prone unscaled matrix products.
inline bool ValidateEigenpairs(const math_Vector& theOriginalDiag,
                               const math_Vector& theOriginalSubdiag,
                               const math_Vector& theEigenValues,
                               const math_Matrix& theEigenVectors)
{
  const size_t aN = theOriginalDiag.Size();
  double       aMatrixScale = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    aMatrixScale = std::max(aMatrixScale, std::abs(theOriginalDiag[i]));
    if (i + 1 < aN)
    {
      aMatrixScale = std::max(aMatrixScale, std::abs(theOriginalSubdiag[i]));
    }
  }
  if (aMatrixScale == 0.0)
  {
    return true;
  }

  const double aDimensionFactor = static_cast<double>(std::max<size_t>(1, aN));
  for (size_t j = 0; j < aN; ++j)
  {
    const double aScaledEigenValue = theEigenValues[j] / aMatrixScale;
    const double aTolerance = THE_TRIDIAGONAL_RESIDUAL_TOLERANCE_FACTOR
                              * std::numeric_limits<double>::epsilon() * aDimensionFactor
                              * std::max(1.0, std::abs(aScaledEigenValue));
    for (size_t i = 0; i < aN; ++i)
    {
      double aResidual = theOriginalDiag[i] / aMatrixScale * theEigenVectors.At(i, j)
                         - aScaledEigenValue * theEigenVectors.At(i, j);
      if (i > 0)
      {
        aResidual += theOriginalSubdiag[i - 1] / aMatrixScale
                     * theEigenVectors.At(i - 1, j);
      }
      if (i + 1 < aN)
      {
        aResidual += theOriginalSubdiag[i] / aMatrixScale
                     * theEigenVectors.At(i + 1, j);
      }
      if (!std::isfinite(aResidual) || std::abs(aResidual) > aTolerance)
      {
        return false;
      }
    }
  }
  return true;
}

} // namespace Utils

//! Eigenvalue decomposition of symmetric tridiagonal matrix using QL algorithm.
//!
//! The QL algorithm with implicit Wilkinson shifts finds all eigenvalues and
//! eigenvectors of a symmetric tridiagonal matrix T = Q * D * Q^T.
//!
//! Properties:
//! - All eigenvalues are real (matrix is symmetric)
//! - Eigenvectors are orthonormal
//! - Numerically stable with implicit shifts
//!
//! @param theDiagonal diagonal elements of the tridiagonal matrix
//! @param theSubdiagonal N-1 subdiagonal elements. An N-element legacy vector with an ignored
//! leading element is also accepted for integration with math_EigenValuesSearcher callers.
//! @param theMaxIterations maximum iterations per eigenvalue (default 30)
//! @return EigenResult containing eigenvalues and eigenvector matrix
inline EigenResult EigenTridiagonal(const math_Vector& theDiagonal,
                                    const math_Vector& theSubdiagonal,
                                     uint32_t           theMaxIterations = 30)
{
  EigenResult aResult;

  const size_t aN = theDiagonal.Size();
  const bool hasNaturalSubdiagonal = aN > 0 && theSubdiagonal.Size() == aN - 1;
  const bool hasLegacySubdiagonal  = theSubdiagonal.Size() == aN;
  if ((!hasNaturalSubdiagonal && !hasLegacySubdiagonal) || aN == 0 || theMaxIterations == 0
      || !Utils::IsFinite(theDiagonal) || !Utils::IsFinite(theSubdiagonal))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  aResult.Dimension = aN;

  // Create working copies
  math_Vector aDiag(aN);
  math_Vector aSubdiag(aN);

  // Copy diagonal
  for (size_t i = 0; i < aN; ++i)
  {
    aDiag[i] = theDiagonal.At(i);
  }

  for (size_t i = 0; i + 1 < aN; ++i)
  {
    aSubdiag[i] = theSubdiagonal.At(hasNaturalSubdiagonal ? i : i + 1);
  }
  aSubdiag[aN - 1] = 0.0;
  const math_Vector anOriginalDiag    = aDiag;
  const math_Vector anOriginalSubdiag = aSubdiag;

  // Initialize eigenvector matrix as identity
  math_Matrix aEigenVec(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    aEigenVec.ChangeAt(i, i) = 1.0;
  }

  // Special case: 1x1 matrix
  if (aN == 1)
  {
    aResult.EigenValues = math_Vector(size_t{1});
    (*aResult.EigenValues)[0] = aDiag[0];
    aResult.EigenVectors = math_Matrix(size_t{1}, size_t{1}, 1.0);
    aResult.Status       = Status::OK;
    return aResult;
  }

  // QL Algorithm with implicit shifts
  for (size_t aStart = 0; aStart < aN; ++aStart)
  {
    uint32_t aIterCount = 0;
    size_t aEnd;

    do
    {
      aEnd = Utils::FindSubmatrixEnd(aDiag, aSubdiag, aStart, aN);

      if (aEnd != aStart)
      {
        if (aIterCount++ >= theMaxIterations)
        {
          aResult.Status = Status::MaxIterations;
          return aResult;
        }

        const double aShift = Utils::ComputeWilkinsonShift(aDiag, aSubdiag, aStart, aEnd);

        if (!std::isfinite(aShift)
            || !Utils::PerformQLStep(aDiag, aSubdiag, aEigenVec, aStart, aEnd, aShift, aN)
            || !Utils::IsFinite(aDiag) || !Utils::IsFinite(aSubdiag)
            || !Utils::IsFinite(aEigenVec))
        {
          aResult.Status = Status::NumericalError;
          return aResult;
        }
      }
    } while (aEnd != aStart);
  }

  aResult.EigenValues = math_Vector(aN);
  aResult.EigenVectors = math_Matrix(aN, aN);
  for (size_t i = 0; i < aN; ++i)
  {
    (*aResult.EigenValues)[i] = aDiag[i];
    for (size_t j = 0; j < aN; ++j)
    {
      aResult.EigenVectors->ChangeAt(i, j) = aEigenVec.At(i, j);
    }
  }
  aResult.Status = Utils::IsFinite(*aResult.EigenValues) && Utils::IsFinite(*aResult.EigenVectors)
                           && Utils::ValidateEigenpairs(anOriginalDiag,
                                                        anOriginalSubdiag,
                                                        *aResult.EigenValues,
                                                        *aResult.EigenVectors)
                     ? Status::OK
                     : Status::NumericalError;
  return aResult;
}

//! Get a single eigenvector from the result.
//!
//! @param theResult eigenvalue decomposition result
//! @param theIndex zero-based index of eigenvector
//! @return eigenvector as math_Vector
inline math_Vector GetEigenVector(const EigenResult& theResult, size_t theIndex)
{
  if (!theResult.EigenVectors.has_value())
  {
    return math_Vector(size_t{1});
  }

  const math_Matrix& aVecs = *theResult.EigenVectors;
  const size_t       aN    = aVecs.RowSize();

  math_Vector aVec(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aVec[i] = aVecs.At(i, theIndex);
  }
  return aVec;
}

} // namespace MathLin

#endif // _MathLin_EigenSearch_HeaderFile
