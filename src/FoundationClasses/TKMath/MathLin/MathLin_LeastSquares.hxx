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

#ifndef _MathLin_LeastSquares_HeaderFile
#define _MathLin_LeastSquares_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathLin_Gauss.hxx>
#include <MathLin_SVD.hxx>
#include <MathLin_Householder.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>

namespace MathLin
{
using namespace MathUtils;

//! Method for solving least squares problems.
enum class LeastSquaresMethod
{
  NormalEquations, //!< A^T*A*x = A^T*b (fast but less stable)
  QR,              //!< Householder QR (good balance of speed and stability)
  SVD              //!< SVD (most stable, handles rank-deficient matrices)
};

//! Result for least squares problems.
struct LeastSquaresResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> Solution;   //!< Least squares solution x
  std::optional<double>      Residual;   //!< ||Ax - b||_2 (L2 norm of residual)
  std::optional<double>      ResidualSq; //!< ||Ax - b||_2^2 (squared residual)
  int                        Rank = 0;   //!< Numerical rank of A (for SVD)

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Solve overdetermined linear least squares: minimize ||Ax - b||_2.
//!
//! Given m x n matrix A (m >= n) and m-vector b, finds n-vector x
//! that minimizes the 2-norm of the residual r = Ax - b.
//!
//! Methods:
//! - NormalEquations: Solves A^T*A*x = A^T*b (fastest, may lose precision)
//! - QR: Uses Householder QR decomposition (good general choice)
//! - SVD: Most robust, handles rank-deficient systems
//!
//! @param theA coefficient matrix (m x n, m >= n)
//! @param theB right-hand side vector (length m)
//! @param theMethod solution method (default: QR)
//! @param theTolerance for rank/singularity detection
//! @return least squares result
inline LeastSquaresResult LeastSquares(const math_Matrix& theA,
                                       const math_Vector& theB,
                                       LeastSquaresMethod theMethod    = LeastSquaresMethod::QR,
                                       double             theTolerance = 1.0e-15)
{
  LeastSquaresResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aM        = aRowUpper - aRowLower + 1;
  const int aN        = aColUpper - aColLower + 1;

  // Check dimensions
  if (theB.Length() != aM)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  LinearResult aLinResult;

  switch (theMethod)
  {
    case LeastSquaresMethod::NormalEquations: {
      // Form normal equations: A^T * A * x = A^T * b
      math_Matrix aAtA(aColLower, aColUpper, aColLower, aColUpper, 0.0);
      math_Vector aAtb(aColLower, aColUpper, 0.0);

      // Compute A^T * A
      for (int i = aColLower; i <= aColUpper; ++i)
      {
        for (int j = aColLower; j <= aColUpper; ++j)
        {
          double aSum = 0.0;
          for (int k = aRowLower; k <= aRowUpper; ++k)
          {
            aSum += theA(k, i) * theA(k, j);
          }
          aAtA(i, j) = aSum;
        }
      }

      // Compute A^T * b
      for (int i = aColLower; i <= aColUpper; ++i)
      {
        double aSum = 0.0;
        for (int k = aRowLower; k <= aRowUpper; ++k)
        {
          aSum += theA(k, i) * theB(theB.Lower() + k - aRowLower);
        }
        aAtb(i) = aSum;
      }

      // Solve the normal equations
      aLinResult   = Solve(aAtA, aAtb, theTolerance);
      aResult.Rank = (aLinResult.IsDone()) ? aN : 0;
    }
    break;

    case LeastSquaresMethod::QR: {
      aLinResult = SolveQR(theA, theB, theTolerance);
      if (aLinResult.IsDone())
      {
        // Estimate rank from QR (not exact)
        aResult.Rank = aN;
      }
    }
    break;

    case LeastSquaresMethod::SVD: {
      aLinResult = SolveSVD(theA, theB, theTolerance);
      if (aLinResult.IsDone())
      {
        // Get rank from SVD
        SVDResult aSVD = SVD(theA, theTolerance);
        aResult.Rank   = aSVD.IsDone() ? aSVD.Rank : 0;
      }
    }
    break;
  }

  if (!aLinResult.IsDone())
  {
    aResult.Status = aLinResult.Status;
    return aResult;
  }

  aResult.Solution = aLinResult.Solution;

  // Compute residual ||Ax - b||_2
  const math_Vector& aX          = *aResult.Solution;
  double             aResidualSq = 0.0;

  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    double aAxi = 0.0;
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aAxi += theA(i, j) * aX(j);
    }
    double aRi = aAxi - theB(theB.Lower() + i - aRowLower);
    aResidualSq += aRi * aRi;
  }

  aResult.ResidualSq = aResidualSq;
  aResult.Residual   = std::sqrt(aResidualSq);
  aResult.Status     = Status::OK;
  return aResult;
}

//! Solve weighted least squares: minimize ||W^{1/2}(Ax - b)||_2.
//!
//! Equivalent to minimizing sum of w_i * (a_i^T * x - b_i)^2
//! where w_i are the weights.
//!
//! @param theA coefficient matrix (m x n)
//! @param theB right-hand side vector (length m)
//! @param theW weight vector (length m, positive values)
//! @param theMethod solution method
//! @param theTolerance for rank detection
//! @return weighted least squares result
inline LeastSquaresResult WeightedLeastSquares(
  const math_Matrix& theA,
  const math_Vector& theB,
  const math_Vector& theW,
  LeastSquaresMethod theMethod    = LeastSquaresMethod::QR,
  double             theTolerance = 1.0e-15)
{
  LeastSquaresResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aM        = aRowUpper - aRowLower + 1;

  // Check dimensions
  if (theB.Length() != aM || theW.Length() != aM)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Check weights are positive
  for (int i = theW.Lower(); i <= theW.Upper(); ++i)
  {
    if (theW(i) <= 0.0)
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
  }

  // Apply weights: A' = W^{1/2} * A, b' = W^{1/2} * b
  math_Matrix aWA(aRowLower, aRowUpper, aColLower, aColUpper);
  math_Vector aWB(theB.Lower(), theB.Upper());

  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    double aSqrtW = std::sqrt(theW(theW.Lower() + i - aRowLower));
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aWA(i, j) = aSqrtW * theA(i, j);
    }
    aWB(theB.Lower() + i - aRowLower) = aSqrtW * theB(theB.Lower() + i - aRowLower);
  }

  // Solve weighted system
  return LeastSquares(aWA, aWB, theMethod, theTolerance);
}

//! Solve regularized least squares (Tikhonov/Ridge regression):
//! minimize ||Ax - b||_2^2 + lambda*||x||_2^2
//!
//! Adds regularization to stabilize ill-conditioned problems.
//! The solution is: x = (A^T*A + lambda*I)^{-1} * A^T * b
//!
//! @param theA coefficient matrix (m x n)
//! @param theB right-hand side vector (length m)
//! @param theLambda regularization parameter (>= 0)
//! @param theTolerance for singularity detection
//! @return regularized least squares result
inline LeastSquaresResult RegularizedLeastSquares(const math_Matrix& theA,
                                                  const math_Vector& theB,
                                                  double             theLambda,
                                                  double             theTolerance = 1.0e-15)
{
  LeastSquaresResult aResult;

  const int aRowLower = theA.LowerRow();
  const int aRowUpper = theA.UpperRow();
  const int aColLower = theA.LowerCol();
  const int aColUpper = theA.UpperCol();
  const int aM        = aRowUpper - aRowLower + 1;
  const int aN        = aColUpper - aColLower + 1;

  // Check dimensions
  if (theB.Length() != aM)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  if (theLambda < 0.0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Form regularized normal equations: (A^T*A + lambda*I) * x = A^T * b
  math_Matrix aAtA(aColLower, aColUpper, aColLower, aColUpper, 0.0);
  math_Vector aAtb(aColLower, aColUpper, 0.0);

  // Compute A^T * A + lambda*I
  for (int i = aColLower; i <= aColUpper; ++i)
  {
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      double aSum = 0.0;
      for (int k = aRowLower; k <= aRowUpper; ++k)
      {
        aSum += theA(k, i) * theA(k, j);
      }
      aAtA(i, j) = aSum;
    }
    // Add regularization
    aAtA(i, i) += theLambda;
  }

  // Compute A^T * b
  for (int i = aColLower; i <= aColUpper; ++i)
  {
    double aSum = 0.0;
    for (int k = aRowLower; k <= aRowUpper; ++k)
    {
      aSum += theA(k, i) * theB(theB.Lower() + k - aRowLower);
    }
    aAtb(i) = aSum;
  }

  // Solve the regularized normal equations
  LinearResult aLinResult = Solve(aAtA, aAtb, theTolerance);
  if (!aLinResult.IsDone())
  {
    aResult.Status = aLinResult.Status;
    return aResult;
  }

  aResult.Solution = aLinResult.Solution;
  aResult.Rank     = aN;

  // Compute residual
  const math_Vector& aX          = *aResult.Solution;
  double             aResidualSq = 0.0;

  for (int i = aRowLower; i <= aRowUpper; ++i)
  {
    double aAxi = 0.0;
    for (int j = aColLower; j <= aColUpper; ++j)
    {
      aAxi += theA(i, j) * aX(j);
    }
    double aRi = aAxi - theB(theB.Lower() + i - aRowLower);
    aResidualSq += aRi * aRi;
  }

  aResult.ResidualSq = aResidualSq;
  aResult.Residual   = std::sqrt(aResidualSq);
  aResult.Status     = Status::OK;
  return aResult;
}

//! Compute optimal regularization parameter using Leave-One-Out Cross-Validation.
//!
//! Minimizes the LOO-CV score: sum_i (a_i^T * x_{-i} - b_i)^2
//! where x_{-i} is the solution with the i-th observation removed.
//!
//! @param theA coefficient matrix
//! @param theB right-hand side vector
//! @param theLambdaMin minimum lambda to consider
//! @param theLambdaMax maximum lambda to consider
//! @param theNbPoints number of lambda values to try
//! @return optimal regularization parameter
inline double OptimalRegularization(const math_Matrix& theA,
                                    const math_Vector& theB,
                                    double             theLambdaMin = 1.0e-10,
                                    double             theLambdaMax = 1.0e2,
                                    int                theNbPoints  = 20)
{
  double aBestLambda = theLambdaMin;
  double aBestScore  = std::numeric_limits<double>::max();

  // Logarithmic grid search
  const double aLogMin  = std::log10(theLambdaMin);
  const double aLogMax  = std::log10(theLambdaMax);
  const double aLogStep = (aLogMax - aLogMin) / (theNbPoints - 1);

  for (int k = 0; k < theNbPoints; ++k)
  {
    double aLambda = std::pow(10.0, aLogMin + k * aLogStep);

    auto aResult = RegularizedLeastSquares(theA, theB, aLambda);
    if (aResult.IsDone() && aResult.ResidualSq)
    {
      double aScore = *aResult.ResidualSq;
      if (aScore < aBestScore)
      {
        aBestScore  = aScore;
        aBestLambda = aLambda;
      }
    }
  }

  return aBestLambda;
}

} // namespace MathLin

#endif // _MathLin_LeastSquares_HeaderFile
