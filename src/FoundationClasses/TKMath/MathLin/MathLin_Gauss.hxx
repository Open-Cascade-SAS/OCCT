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

#ifndef _MathLin_Gauss_HeaderFile
#define _MathLin_Gauss_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include "MathLin_Utils.hxx"
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <math_IntegerVector.hxx>

#include <cmath>

namespace MathLin
{
using namespace MathUtils;

//! Result for LU decomposition.
struct LUResult
{
  MathUtils::Status                 Status = MathUtils::Status::NotConverged;
  std::optional<math_Matrix>        LU;    //!< Combined L and U matrices
  std::optional<math_IntegerVector> Pivot; //!< Pivot indices
  std::optional<double>             Determinant;
  int                               Sign = 1; //!< Sign from row interchanges
  size_t                            RowLower = 0;
  size_t                            ColLower = 0;
  size_t                            Dimension = 0;
  double                            MinAbsPivot = 0.0;
  double                            MaxAbsPivot = 0.0;
  double                            ConditionEstimate = std::numeric_limits<double>::infinity();

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Perform LU decomposition of matrix A with partial pivoting.
//! Decomposes A into L*U where L is lower triangular with unit diagonal
//! and U is upper triangular. The result stores L and U in a combined matrix.
//!
//! @param theA input square matrix
//! @param theTolerance relative pivot tolerance
//! @return LU decomposition result
inline LUResult LU(const math_Matrix& theA, double theTolerance = 1.0e-15)
{
  LUResult aResult;

  const size_t aRowCount = theA.RowSize();
  const size_t aColCount = theA.ColSize();

  const double aRelTol = Utils::RelativeTolerance(theTolerance, aRowCount);
  if (aRowCount != aColCount || aRowCount == 0 || aRelTol < 0.0 || !Utils::IsFinite(theA))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  aResult.LU        = math_Matrix(aRowCount, aRowCount);
  aResult.Pivot     = math_IntegerVector(aRowCount);
  aResult.Sign      = 1;
  aResult.RowLower  = 0;
  aResult.ColLower  = 0;
  aResult.Dimension = aRowCount;

  math_Matrix&        aLU    = *aResult.LU;
  math_IntegerVector& aPivot = *aResult.Pivot;

  // Scaling factors for implicit pivoting
  math_Vector aScale(aRowCount);
  double      aMatrixScale = 0.0;
  for (size_t i = 0; i < aRowCount; ++i)
  {
    double aMax = 0.0;
    for (size_t j = 0; j < aRowCount; ++j)
    {
      aLU.ChangeAt(i, j) = theA.At(i, j);
      const double aAbs = std::abs(aLU.At(i, j));
      if (aAbs > aMax)
      {
        aMax = aAbs;
      }
    }
    if (aMax == 0.0)
    {
      aResult.Status = Status::NumericalError; // Singular matrix
      return aResult;
    }
    aScale[i] = 1.0 / aMax;
    aMatrixScale = std::max(aMatrixScale, aMax);
  }

  // Crout's algorithm with partial pivoting
  for (size_t k = 0; k < aRowCount; ++k)
  {
    // Find pivot
    double aMaxScaled = 0.0;
    size_t aPivotRow  = k;

    for (size_t i = k; i < aRowCount; ++i)
    {
      const double aScaled = aScale[i] * std::abs(aLU.At(i, k));
      if (aScaled > aMaxScaled)
      {
        aMaxScaled = aScaled;
        aPivotRow  = i;
      }
    }

    // Swap rows if needed
    if (aPivotRow != k)
    {
      for (size_t j = 0; j < aRowCount; ++j)
      {
        std::swap(aLU.ChangeAt(aPivotRow, j), aLU.ChangeAt(k, j));
      }
      aResult.Sign = -aResult.Sign;
      std::swap(aScale[aPivotRow], aScale[k]);
    }
    aPivot[k] = static_cast<int>(aPivotRow);

    // Check for singular matrix
    const double anAbsPivot = std::abs(aLU.At(k, k));
    if (!std::isfinite(anAbsPivot) || anAbsPivot <= aRelTol * aMatrixScale)
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    // Eliminate below diagonal
    aResult.MaxAbsPivot = std::max(aResult.MaxAbsPivot, anAbsPivot);
    aResult.MinAbsPivot = k == 0 ? anAbsPivot : std::min(aResult.MinAbsPivot, anAbsPivot);
    for (size_t i = k + 1; i < aRowCount; ++i)
    {
      aLU.ChangeAt(i, k) /= aLU.At(k, k);
      const double aFactor = aLU.At(i, k);
      for (size_t j = k + 1; j < aRowCount; ++j)
      {
        aLU.ChangeAt(i, j) -= aFactor * aLU.At(k, j);
      }
    }
  }

  // Compute determinant
  aResult.Determinant = static_cast<double>(aResult.Sign);
  for (size_t i = 0; i < aRowCount; ++i)
  {
    *aResult.Determinant *= aLU.At(i, i);
    if (!std::isfinite(*aResult.Determinant))
    {
      aResult.Determinant.reset();
      aResult.Status = Status::NumericalError;
      return aResult;
    }
  }

  aResult.ConditionEstimate = aResult.MaxAbsPivot / aResult.MinAbsPivot;

  aResult.Status = Status::OK;
  return aResult;
}

//! Solve linear system AX = B using LU decomposition.
//!
//! @param theA coefficient matrix (square)
//! @param theB right-hand side vector
//! @param theMinPivot minimum pivot value
//! @return result containing solution vector
inline LinearResult Solve(const math_Matrix& theA,
                          const math_Vector& theB,
                          double             theMinPivot = 1.0e-15)
{
  LinearResult aResult;

  // Perform LU decomposition
  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  const size_t aN = theA.RowSize();
  if (theB.Size() != aN || !Utils::IsFinite(theB))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  math_Vector aX(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aX[i] = theB.At(i);
  }

  // Forward substitution with in-place permutation (matches legacy LU_Solve)
  for (size_t i = 0; i < aN; ++i)
  {
    const size_t aPivotIdx = static_cast<size_t>(aPivot[i]);
    double       aSum      = aX[aPivotIdx];
    aX[aPivotIdx]          = aX[i]; // Swap elements in-place

    for (size_t j = 0; j < i; ++j)
    {
      aSum -= aLU.At(i, j) * aX[j];
    }
    aX[i] = aSum;
  }

  // Back substitution (solve Ux = y)
  for (size_t i = aN; i-- > 0;)
  {
    double aSum = aX[i];
    for (size_t j = i + 1; j < aN; ++j)
    {
      aSum -= aLU.At(i, j) * aX[j];
    }
    aX[i] = aSum / aLU.At(i, i);
  }

  aResult.Solution = math_Vector(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    (*aResult.Solution)[i] = aX[i];
  }
  aResult.Status      = Utils::IsFinite(*aResult.Solution) ? Status::OK : Status::NumericalError;
  aResult.Determinant = aLURes.Determinant;
  return aResult;
}

//! Solve multiple linear systems AX = B where B is a matrix.
//! Each column of B is a separate right-hand side.
//!
//! @param theA coefficient matrix (square)
//! @param theB right-hand side matrix
//! @param theMinPivot minimum pivot value
//! @return result containing solution matrix
inline LinearMultipleResult SolveMultiple(const math_Matrix& theA,
                                           const math_Matrix& theB,
                                           double             theMinPivot = 1.0e-15)
{
  LinearMultipleResult aResult;

  // Perform LU decomposition
  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  const size_t aN = theA.RowSize();
  if (theB.RowSize() != aN || !Utils::IsFinite(theB))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  // Solve for each column of B
  math_Matrix aX(theA.ColSize(), theB.ColSize());

  for (size_t aCol = 0; aCol < theB.ColSize(); ++aCol)
  {
    // Extract column as working vector
    math_Vector aWork(aN);
    for (size_t i = 0; i < aN; ++i)
    {
      aWork[i] = theB.At(i, aCol);
    }

    // Forward substitution with in-place permutation
    for (size_t i = 0; i < aN; ++i)
    {
      const size_t aPivotIdx = static_cast<size_t>(aPivot[i]);
      double       aSum      = aWork[aPivotIdx];
      aWork[aPivotIdx]       = aWork[i]; // Swap elements in-place

      for (size_t j = 0; j < i; ++j)
      {
        aSum -= aLU.At(i, j) * aWork[j];
      }
      aWork[i] = aSum;
    }

    // Back substitution
    for (size_t i = aN; i-- > 0;)
    {
      double aSum = aWork[i];
      for (size_t j = i + 1; j < aN; ++j)
      {
        aSum -= aLU.At(i, j) * aWork[j];
      }
      aWork[i] = aSum / aLU.At(i, i);
    }

    // Copy result to output matrix
    for (size_t i = 0; i < aN; ++i)
    {
      aX.ChangeAt(i, aCol) = aWork[i];
    }
  }

  aResult.Status      = Utils::IsFinite(aX) ? Status::OK : Status::NumericalError;
  aResult.Determinant = aLURes.Determinant;
  aResult.Solutions   = aX;
  return aResult;
}

//! Compute determinant of matrix A.
//!
//! @param theA input square matrix
//! @param theMinPivot minimum pivot value
//! @return result containing determinant value
inline LinearResult Determinant(const math_Matrix& theA, double theMinPivot = 1.0e-15)
{
  LinearResult aResult;

  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  aResult.Status      = Status::OK;
  aResult.Determinant = aLURes.Determinant;
  return aResult;
}

//! Compute inverse of matrix A.
//!
//! @param theA input square matrix
//! @param theMinPivot minimum pivot value
//! @return result containing inverse matrix
inline InverseResult Invert(const math_Matrix& theA, double theMinPivot = 1.0e-15)
{
  InverseResult aResult;

  // Perform LU decomposition
  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  // Compute inverse column by column using LU_Solve approach
  const size_t aN = theA.RowSize();
  math_Matrix  aInv(theA.ColSize(), theA.RowSize(), 0.0);
  math_Vector  aCol(aN);

  for (size_t aColIdx = 0; aColIdx < aN; ++aColIdx)
  {
    // Initialize column as unit vector
    for (size_t i = 0; i < aN; ++i)
    {
      aCol[i] = 0.0;
    }
    aCol[aColIdx] = 1.0;

    // Forward substitution with in-place permutation
    for (size_t i = 0; i < aN; ++i)
    {
      const size_t aPivotIdx = static_cast<size_t>(aPivot[i]);
      double       aSum      = aCol[aPivotIdx];
      aCol[aPivotIdx]        = aCol[i]; // Swap elements in-place

      for (size_t j = 0; j < i; ++j)
      {
        aSum -= aLU.At(i, j) * aCol[j];
      }
      aCol[i] = aSum;
    }

    // Back substitution
    for (size_t i = aN; i-- > 0;)
    {
      double aSum = aCol[i];
      for (size_t j = i + 1; j < aN; ++j)
      {
        aSum -= aLU.At(i, j) * aCol[j];
      }
      aCol[i] = aSum / aLU.At(i, i);
    }

    // Copy result to inverse matrix
    for (size_t i = 0; i < aN; ++i)
    {
      aInv.ChangeAt(i, aColIdx) = aCol[i];
    }
  }

  aResult.Status      = Utils::IsFinite(aInv) ? Status::OK : Status::NumericalError;
  aResult.Inverse     = aInv;
  aResult.Determinant = aLURes.Determinant;
  return aResult;
}

} // namespace MathLin

#endif // _MathLin_Gauss_HeaderFile
