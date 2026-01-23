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

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Perform LU decomposition of matrix A with partial pivoting.
//! Decomposes A into L*U where L is lower triangular with unit diagonal
//! and U is upper triangular. The result stores L and U in a combined matrix.
//!
//! @param theA input square matrix
//! @param theMinPivot minimum pivot value (smaller treated as singular)
//! @return LU decomposition result
inline LUResult LU(const math_Matrix& theA, double theMinPivot = 1.0e-20)
{
  LUResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aRowCount = aRowUpper - aRowLower + 1;
  const int aColCount = aColUpper - aColLower + 1;

  // Check for square matrix
  if (aRowCount != aColCount)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Initialize LU matrix as copy of A
  aResult.LU    = theA;
  aResult.Pivot = math_IntegerVector(aRowLower, aRowUpper);
  aResult.Sign  = 1;

  math_Matrix&        aLU    = *aResult.LU;
  math_IntegerVector& aPivot = *aResult.Pivot;

  // Scaling factors for implicit pivoting
  math_Vector aScale(aRowLower, aRowUpper);
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    double aMax = 0.0;
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      const double aAbs = std::abs(aLU(i, j));
      if (aAbs > aMax)
      {
        aMax = aAbs;
      }
    }
    if (aMax < theMinPivot)
    {
      aResult.Status = Status::NumericalError; // Singular matrix
      return aResult;
    }
    aScale(i) = 1.0 / aMax;
  }

  // Crout's algorithm with partial pivoting
  for (int k = aRowLower; k <= aRowUpper; ++k)
  {
    // Find pivot
    double aMaxScaled = 0.0;
    int    aPivotRow  = k;

    for (int i = k; i <= aRowUpper; ++i)
    {
      const double aScaled = aScale(i) * std::abs(aLU(i, k));
      if (aScaled > aMaxScaled)
      {
        aMaxScaled = aScaled;
        aPivotRow  = i;
      }
    }

    // Swap rows if needed
    if (aPivotRow != k)
    {
      for (int j = aColLower; j <= aColUpper; ++j)
      {
        std::swap(aLU(aPivotRow, j), aLU(k, j));
      }
      aResult.Sign = -aResult.Sign;
      std::swap(aScale(aPivotRow), aScale(k));
    }
    aPivot(k) = aPivotRow;

    // Check for singular matrix
    if (std::abs(aLU(k, k)) < theMinPivot)
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    // Eliminate below diagonal
    for (int i = k + 1; i <= aRowUpper; ++i)
    {
      aLU(i, k) /= aLU(k, k);
      const double aFactor = aLU(i, k);
      for (int j = k + 1; j <= aColUpper; ++j)
      {
        aLU(i, j) -= aFactor * aLU(k, j);
      }
    }
  }

  // Compute determinant
  aResult.Determinant = static_cast<double>(aResult.Sign);
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    *aResult.Determinant *= aLU(i, i);
  }

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
                          double             theMinPivot = 1.0e-20)
{
  LinearResult aResult;

  // Perform LU decomposition
  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();

  // Check dimensions
  if (theB.Lower() != aRowLower || theB.Upper() != aRowUpper)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  // Copy B to working vector (we modify it in-place like the legacy algorithm)
  math_Vector aX = theB;

  // Forward substitution with in-place permutation (matches legacy LU_Solve)
  int aFirstNonZero = 0;
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    const int aPivotIdx = aPivot(i);
    double    aSum      = aX(aPivotIdx);
    aX(aPivotIdx)       = aX(i); // Swap elements in-place

    if (aFirstNonZero != 0)
    {
      for (int j = aFirstNonZero; j < i; ++j)
      {
        aSum -= aLU(i, j) * aX(j);
      }
    }
    else if (!MathUtils::IsZero(aSum))
    {
      aFirstNonZero = i;
    }
    aX(i) = aSum;
  }

  // Back substitution (solve Ux = y)
  for (int i = aRowUpper; i >= aRowLower; --i)
  {
    double aSum = aX(i);
    for (int j = i + 1; j <= aRowUpper; ++j)
    {
      aSum -= aLU(i, j) * aX(j);
    }
    aX(i) = aSum / aLU(i, i);
  }

  aResult.Status      = Status::OK;
  aResult.Solution    = aX;
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
inline LinearResult SolveMultiple(const math_Matrix& theA,
                                  const math_Matrix& theB,
                                  double             theMinPivot = 1.0e-20)
{
  LinearResult aResult;

  // Perform LU decomposition
  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();

  // Check dimensions
  if (theB.LowerRow() != aRowLower || theB.UpperRow() != aRowUpper)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  // Solve for each column of B
  math_Matrix aX(theB.LowerRow(), theB.UpperRow(), theB.LowerCol(), theB.UpperCol());

  for (int col = theB.LowerCol(); col <= theB.UpperCol(); ++col)
  {
    // Extract column as working vector
    math_Vector aWork(aRowLower, aRowUpper);
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      aWork(i) = theB(i, col);
    }

    // Forward substitution with in-place permutation
    int aFirstNonZero = 0;
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      const int aPivotIdx = aPivot(i);
      double    aSum      = aWork(aPivotIdx);
      aWork(aPivotIdx)    = aWork(i); // Swap elements in-place

      if (aFirstNonZero != 0)
      {
        for (int j = aFirstNonZero; j < i; ++j)
        {
          aSum -= aLU(i, j) * aWork(j);
        }
      }
      else if (!MathUtils::IsZero(aSum))
      {
        aFirstNonZero = i;
      }
      aWork(i) = aSum;
    }

    // Back substitution
    for (int i = aRowUpper; i >= aRowLower; --i)
    {
      double aSum = aWork(i);
      for (int j = i + 1; j <= aRowUpper; ++j)
      {
        aSum -= aLU(i, j) * aWork(j);
      }
      aWork(i) = aSum / aLU(i, i);
    }

    // Copy result to output matrix
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      aX(i, col) = aWork(i);
    }
  }

  aResult.Status      = Status::OK;
  aResult.Determinant = aLURes.Determinant;
  // Store first column as solution vector for compatibility
  math_Vector aSol(aRowLower, aRowUpper);
  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    aSol(i) = aX(i, theB.LowerCol());
  }
  aResult.Solution = aSol;
  return aResult;
}

//! Compute determinant of matrix A.
//!
//! @param theA input square matrix
//! @param theMinPivot minimum pivot value
//! @return result containing determinant value
inline LinearResult Determinant(const math_Matrix& theA, double theMinPivot = 1.0e-20)
{
  LinearResult aResult;

  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status      = aLURes.Status;
    aResult.Determinant = 0.0;
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
inline InverseResult Invert(const math_Matrix& theA, double theMinPivot = 1.0e-20)
{
  InverseResult aResult;

  // Perform LU decomposition
  LUResult aLURes = LU(theA, theMinPivot);
  if (!aLURes.IsDone())
  {
    aResult.Status = aLURes.Status;
    return aResult;
  }

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  // Compute inverse column by column using LU_Solve approach
  math_Matrix aInv(aRowLower, aRowUpper, aColLower, aColUpper, 0.0);
  math_Vector aCol(aRowLower, aRowUpper);

  for (int col = aColLower; col <= aColUpper; ++col)
  {
    // Initialize column as unit vector
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      aCol(i) = 0.0;
    }
    aCol(col - aColLower + aRowLower) = 1.0;

    // Forward substitution with in-place permutation
    int aFirstNonZero = 0;
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      const int aPivotIdx = aPivot(i);
      double    aSum      = aCol(aPivotIdx);
      aCol(aPivotIdx)     = aCol(i); // Swap elements in-place

      if (aFirstNonZero != 0)
      {
        for (int j = aFirstNonZero; j < i; ++j)
        {
          aSum -= aLU(i, j) * aCol(j);
        }
      }
      else if (!MathUtils::IsZero(aSum))
      {
        aFirstNonZero = i;
      }
      aCol(i) = aSum;
    }

    // Back substitution
    for (int i = aRowUpper; i >= aRowLower; --i)
    {
      double aSum = aCol(i);
      for (int j = i + 1; j <= aRowUpper; ++j)
      {
        aSum -= aLU(i, j) * aCol(j);
      }
      aCol(i) = aSum / aLU(i, i);
    }

    // Copy result to inverse matrix
    for (int i = aRowLower; i <= aRowUpper; ++i)
    {
      aInv(i, col) = aCol(i);
    }
  }

  aResult.Status      = Status::OK;
  aResult.Inverse     = aInv;
  aResult.Determinant = aLURes.Determinant;
  return aResult;
}

} // namespace MathLin

#endif // _MathLin_Gauss_HeaderFile
