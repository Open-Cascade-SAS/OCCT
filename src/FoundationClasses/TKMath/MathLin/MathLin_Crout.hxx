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
  std::optional<math_Matrix> Inverse;     //!< Inverse matrix (lower triangle only)
  std::optional<double>      Determinant; //!< Matrix determinant

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
//! - Requires positive definiteness for stability
//!
//! @param theA input symmetric matrix (only lower triangle used)
//! @param theMinPivot minimum pivot value (smaller treated as singular)
//! @return Crout decomposition result
inline CroutResult Crout(const math_Matrix& theA, double theMinPivot = 1.0e-20)
{
  CroutResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aN        = aRowUpper - aRowLower + 1;

  // Check for square matrix
  if (aN != aColUpper - aColLower + 1)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Working matrices with 1-based indexing
  math_Matrix aL(1, aN, 1, aN, 0.0);
  math_Vector aDiag(1, aN);

  double aDet = 1.0;

  // Crout decomposition: A = L * D * L^T
  for (int i = 1; i <= aN; ++i)
  {
    // Compute L(i,j) for j < i
    for (int j = 1; j <= i - 1; ++j)
    {
      double aScale = 0.0;
      for (int k = 1; k <= j - 1; ++k)
      {
        aScale += aL(i, k) * aL(j, k) * aDiag(k);
      }
      aL(i, j) = (theA(i + aRowLower - 1, j + aColLower - 1) - aScale) / aDiag(j);
    }

    // Compute D(i)
    double aScale = 0.0;
    for (int k = 1; k <= i - 1; ++k)
    {
      aScale += aL(i, k) * aL(i, k) * aDiag(k);
    }
    aDiag(i) = theA(i + aRowLower - 1, i + aColLower - 1) - aScale;
    aDet *= aDiag(i);

    // Check for singularity
    if (std::abs(aDiag(i)) <= theMinPivot)
    {
      aResult.Status = Status::Singular;
      return aResult;
    }

    aL(i, i) = 1.0;
  }

  // Compute inverse of L
  aL(1, 1) = 1.0 / aL(1, 1);
  for (int i = 2; i <= aN; ++i)
  {
    for (int k = 1; k <= i - 1; ++k)
    {
      double aScale = 0.0;
      for (int j = k; j <= i - 1; ++j)
      {
        aScale += aL(i, j) * aL(j, k);
      }
      aL(i, k) = -aScale / aL(i, i);
    }
    aL(i, i) = 1.0 / aL(i, i);
  }

  // Compute inverse of A (lower triangle only)
  math_Matrix aInv(1, aN, 1, aN, 0.0);
  for (int j = 1; j <= aN; ++j)
  {
    double aScale = aL(j, j) * aL(j, j) / aDiag(j);
    for (int k = j + 1; k <= aN; ++k)
    {
      aScale += aL(k, j) * aL(k, j) / aDiag(k);
    }
    aInv(j, j) = aScale;

    for (int i = j + 1; i <= aN; ++i)
    {
      aScale = aL(i, j) * aL(i, i) / aDiag(i);
      for (int k = i + 1; k <= aN; ++k)
      {
        aScale += aL(k, j) * aL(k, i) / aDiag(k);
      }
      aInv(i, j) = aScale;
    }
  }

  // Copy results with original indexing
  aResult.L = math_Matrix(aRowLower, aRowUpper, aColLower, aColUpper, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    for (int j = 1; j <= i; ++j)
    {
      (*aResult.L)(i + aRowLower - 1, j + aColLower - 1) = aL(i, j);
    }
  }

  aResult.D = math_Vector(aRowLower, aRowUpper);
  for (int i = 1; i <= aN; ++i)
  {
    (*aResult.D)(i + aRowLower - 1) = aDiag(i);
  }

  aResult.Inverse = math_Matrix(aRowLower, aRowUpper, aColLower, aColUpper, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    for (int j = 1; j <= i; ++j)
    {
      (*aResult.Inverse)(i + aRowLower - 1, j + aColLower - 1) = aInv(i, j);
    }
    // Mirror to upper triangle for full symmetric matrix
    for (int j = i + 1; j <= aN; ++j)
    {
      (*aResult.Inverse)(i + aRowLower - 1, j + aColLower - 1) = aInv(j, i);
    }
  }

  aResult.Determinant = aDet;
  aResult.Status      = Status::OK;
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
                               double             theMinPivot = 1.0e-20)
{
  LinearResult aResult;

  // Perform Crout decomposition
  CroutResult aCrout = Crout(theA, theMinPivot);
  if (!aCrout.IsDone())
  {
    aResult.Status = aCrout.Status;
    return aResult;
  }

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aN        = aRowUpper - aRowLower + 1;

  // Check dimensions
  if (theB.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const math_Matrix& aInv    = *aCrout.Inverse;
  const int          aBLower = theB.Lower();

  // Compute X = Inverse * B using symmetry
  aResult.Solution = math_Vector(aRowLower, aRowUpper, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    double aSum = aInv(i + aRowLower - 1, 1 + aRowLower - 1) * theB(1 + aBLower - 1);
    for (int j = 2; j <= i; ++j)
    {
      aSum += aInv(i + aRowLower - 1, j + aRowLower - 1) * theB(j + aBLower - 1);
    }
    for (int j = i + 1; j <= aN; ++j)
    {
      aSum += aInv(j + aRowLower - 1, i + aRowLower - 1) * theB(j + aBLower - 1);
    }
    (*aResult.Solution)(i + aRowLower - 1) = aSum;
  }

  aResult.Determinant = aCrout.Determinant;
  aResult.Status      = Status::OK;
  return aResult;
}

//! Compute inverse of symmetric matrix using Crout decomposition.
//!
//! @param theA input symmetric matrix
//! @param theMinPivot minimum pivot value
//! @return result containing full symmetric inverse matrix
inline InverseResult InvertCrout(const math_Matrix& theA, double theMinPivot = 1.0e-20)
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
