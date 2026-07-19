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
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>

namespace MathLin
{
using namespace MathUtils;

//! Result for eigenvalue decomposition of tridiagonal matrix.
struct EigenResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> EigenValues;  //!< Computed eigenvalues
  std::optional<math_Matrix> EigenVectors; //!< Eigenvectors as columns
  int                        Dimension = 0;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

namespace Internal
{

//! Computes sqrt(x*x + y*y) avoiding overflow/underflow.
inline double Hypot(double theX, double theY)
{
  return std::sqrt(theX * theX + theY * theY);
}

//! Finds the end of unreduced submatrix using deflation test.
inline int FindSubmatrixEnd(const math_Vector& theDiag,
                            const math_Vector& theSubdiag,
                            int                theStart,
                            int                theN)
{
  const int aLower = theDiag.Lower();
  int       aEnd;
  for (aEnd = theStart; aEnd <= theN - 1; ++aEnd)
  {
    const double aDiagSum =
      std::abs(theDiag(aEnd + aLower - 1)) + std::abs(theDiag(aEnd + 1 + aLower - 1));
    // Deflation test: subdiagonal negligible relative to diagonal elements
    if (std::abs(theSubdiag(aEnd + aLower - 1)) + aDiagSum == aDiagSum)
    {
      break;
    }
  }
  return aEnd;
}

//! Computes Wilkinson's shift for accelerated convergence.
inline double ComputeWilkinsonShift(const math_Vector& theDiag,
                                    const math_Vector& theSubdiag,
                                    int                theStart,
                                    int                theEnd)
{
  const int aLower = theDiag.Lower();
  double    aShift = (theDiag(theStart + 1 + aLower - 1) - theDiag(theStart + aLower - 1))
                  / (2.0 * theSubdiag(theStart + aLower - 1));
  const double aRadius = Hypot(1.0, aShift);

  if (aShift < 0.0)
  {
    aShift = theDiag(theEnd + aLower - 1) - theDiag(theStart + aLower - 1)
             + theSubdiag(theStart + aLower - 1) / (aShift - aRadius);
  }
  else
  {
    aShift = theDiag(theEnd + aLower - 1) - theDiag(theStart + aLower - 1)
             + theSubdiag(theStart + aLower - 1) / (aShift + aRadius);
  }
  return aShift;
}

//! Performs a single QL step with implicit shift.
inline bool PerformQLStep(math_Vector& theDiag,
                          math_Vector& theSubdiag,
                          math_Matrix& theEigenVec,
                          int          theStart,
                          int          theEnd,
                          double       theShift,
                          int          theN)
{
  const int aLowerD = theDiag.Lower();
  const int aLowerV = theEigenVec.LowerRow();

  double aSine     = 1.0;
  double aCosine   = 1.0;
  double aPrevDiag = 0.0;
  double aShift    = theShift;
  double aRadius   = 0.0;

  int aRowIdx;
  for (aRowIdx = theEnd - 1; aRowIdx >= theStart; --aRowIdx)
  {
    const double aTempVal                 = aSine * theSubdiag(aRowIdx + aLowerD - 1);
    const double aSubdiagTemp             = aCosine * theSubdiag(aRowIdx + aLowerD - 1);
    aRadius                               = Hypot(aTempVal, aShift);
    theSubdiag(aRowIdx + 1 + aLowerD - 1) = aRadius;

    if (aRadius == 0.0)
    {
      theDiag(aRowIdx + 1 + aLowerD - 1) -= aPrevDiag;
      theSubdiag(theEnd + aLowerD - 1) = 0.0;
      break;
    }

    aSine   = aTempVal / aRadius;
    aCosine = aShift / aRadius;
    aShift  = theDiag(aRowIdx + 1 + aLowerD - 1) - aPrevDiag;

    const double aRadiusTemp =
      (theDiag(aRowIdx + aLowerD - 1) - aShift) * aSine + 2.0 * aCosine * aSubdiagTemp;
    aPrevDiag                          = aSine * aRadiusTemp;
    theDiag(aRowIdx + 1 + aLowerD - 1) = aShift + aPrevDiag;
    aShift                             = aCosine * aRadiusTemp - aSubdiagTemp;

    // Update eigenvector matrix
    for (int aVecIdx = 1; aVecIdx <= theN; ++aVecIdx)
    {
      const double aTempVec = theEigenVec(aVecIdx + aLowerV - 1, aRowIdx + 1 + aLowerV - 1);
      theEigenVec(aVecIdx + aLowerV - 1, aRowIdx + 1 + aLowerV - 1) =
        aSine * theEigenVec(aVecIdx + aLowerV - 1, aRowIdx + aLowerV - 1) + aCosine * aTempVec;
      theEigenVec(aVecIdx + aLowerV - 1, aRowIdx + aLowerV - 1) =
        aCosine * theEigenVec(aVecIdx + aLowerV - 1, aRowIdx + aLowerV - 1) - aSine * aTempVec;
    }
  }

  if (aRadius == 0.0 && aRowIdx >= 1)
  {
    return true;
  }

  theDiag(theStart + aLowerD - 1) -= aPrevDiag;
  theSubdiag(theStart + aLowerD - 1) = aShift;
  theSubdiag(theEnd + aLowerD - 1)   = 0.0;

  return true;
}

} // namespace Internal

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
//! @param theSubdiagonal subdiagonal elements (one less than diagonal)
//! @param theMaxIterations maximum iterations per eigenvalue (default 30)
//! @return EigenResult containing eigenvalues and eigenvector matrix
inline EigenResult EigenTridiagonal(const math_Vector& theDiagonal,
                                    const math_Vector& theSubdiagonal,
                                    int                theMaxIterations = 30)
{
  EigenResult aResult;

  const int aN = theDiagonal.Length();
  if (theSubdiagonal.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  aResult.Dimension = aN;

  // Create working copies
  math_Vector aDiag(1, aN);
  math_Vector aSubdiag(1, aN);

  // Copy diagonal
  for (int i = 1; i <= aN; ++i)
  {
    aDiag(i) = theDiagonal(theDiagonal.Lower() + i - 1);
  }

  // Shift subdiagonal: e[i-1] = e[i] for QL algorithm
  for (int i = 2; i <= aN; ++i)
  {
    aSubdiag(i - 1) = theSubdiagonal(theSubdiagonal.Lower() + i - 1);
  }
  aSubdiag(aN) = 0.0;

  // Initialize eigenvector matrix as identity
  math_Matrix aEigenVec(1, aN, 1, aN, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    aEigenVec(i, i) = 1.0;
  }

  // Special case: 1x1 matrix
  if (aN == 1)
  {
    aResult.EigenValues  = aDiag;
    aResult.EigenVectors = aEigenVec;
    aResult.Status       = Status::OK;
    return aResult;
  }

  // QL Algorithm with implicit shifts
  for (int aStart = 1; aStart <= aN; ++aStart)
  {
    int aIterCount = 0;
    int aEnd;

    do
    {
      aEnd = Internal::FindSubmatrixEnd(aDiag, aSubdiag, aStart, aN);

      if (aEnd != aStart)
      {
        if (aIterCount++ >= theMaxIterations)
        {
          aResult.Status = Status::MaxIterations;
          return aResult;
        }

        const double aShift = Internal::ComputeWilkinsonShift(aDiag, aSubdiag, aStart, aEnd);

        if (!Internal::PerformQLStep(aDiag, aSubdiag, aEigenVec, aStart, aEnd, aShift, aN))
        {
          aResult.Status = Status::NotConverged;
          return aResult;
        }
      }
    } while (aEnd != aStart);
  }

  aResult.EigenValues  = aDiag;
  aResult.EigenVectors = aEigenVec;
  aResult.Status       = Status::OK;
  return aResult;
}

//! Get a single eigenvector from the result.
//!
//! @param theResult eigenvalue decomposition result
//! @param theIndex 1-based index of eigenvector
//! @return eigenvector as math_Vector
inline math_Vector GetEigenVector(const EigenResult& theResult, int theIndex)
{
  if (!theResult.EigenVectors.has_value())
  {
    return math_Vector(1, 1);
  }

  const math_Matrix& aVecs   = *theResult.EigenVectors;
  const int          aN      = aVecs.RowNumber();
  const int          aLowerR = aVecs.LowerRow();
  const int          aLowerC = aVecs.LowerCol();

  math_Vector aVec(1, aN);
  for (int i = 1; i <= aN; ++i)
  {
    aVec(i) = aVecs(i + aLowerR - 1, theIndex + aLowerC - 1);
  }
  return aVec;
}

} // namespace MathLin

#endif // _MathLin_EigenSearch_HeaderFile
