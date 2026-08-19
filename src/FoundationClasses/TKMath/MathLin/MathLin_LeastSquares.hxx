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
#include "MathLin_Utils.hxx"

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
  size_t                     Rank = 0;   //!< Numerical rank of A (for SVD)
  std::optional<double>      ConditionNumber; //!< Decomposition-based conditioning diagnostic

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

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  // Check dimensions
  if (aM < aN || theB.Size() != aM || !Utils::IsFinite(theA)
      || !Utils::IsFinite(theB))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  LinearResult aLinResult;

  switch (theMethod)
  {
    case LeastSquaresMethod::NormalEquations: {
      // Form normal equations: A^T * A * x = A^T * b
      math_Matrix aAtA(aN, aN, 0.0);
      math_Vector aAtb(aN, 0.0);

      // Compute A^T * A
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          double aSum = 0.0;
          for (size_t k = 0; k < aM; ++k)
          {
            aSum += theA.At(k, i) * theA.At(k, j);
          }
          aAtA.ChangeAt(i, j) = aSum;
        }
      }

      // Compute A^T * b
      for (size_t i = 0; i < aN; ++i)
      {
        double aSum = 0.0;
        for (size_t k = 0; k < aM; ++k)
        {
          aSum += theA.At(k, i) * theB.At(k);
        }
        aAtb[i] = aSum;
      }

      // Solve the normal equations
      aLinResult   = Solve(aAtA, aAtb, theTolerance);
      aResult.Rank = (aLinResult.IsDone()) ? aN : 0;
    }
    break;

    case LeastSquaresMethod::QR: {
      const QRResult aQR = QR(theA, theTolerance);
      if (!aQR.IsDone() || aQR.Rank != aN)
      {
        aResult.Status = aQR.IsDone() ? Status::Singular : aQR.Status;
        return aResult;
      }
      aResult.Rank            = aQR.Rank;
      aResult.ConditionNumber = aQR.ConditionEstimate;
      aLinResult              = SolveQR(theA, theB, theTolerance);
    }
    break;

    case LeastSquaresMethod::SVD: {
      aLinResult = SolveSVD(theA, theB, theTolerance);
      if (aLinResult.IsDone())
      {
        // Get rank from SVD
        SVDResult aSVD = SVD(theA, theTolerance);
        aResult.Rank   = aSVD.IsDone() ? aSVD.Rank : 0;
        if (aSVD.IsDone())
        {
          aResult.ConditionNumber = aSVD.ConditionNumber;
        }
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
  double             aResidualScale = 0.0;
  double             aResidualSumSq = 1.0;
  bool               isResidualValid = true;

  for (size_t i = 0; i < aM; ++i)
  {
    double aAxi = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aAxi += theA.At(i, j) * aX[j];
    }
    const double aRi = aAxi - theB.At(i);
    isResidualValid = Utils::AccumulateNorm(aRi, aResidualScale, aResidualSumSq)
                      && isResidualValid;
  }

  const std::optional<double> aResidual = isResidualValid
                                            ? Utils::Norm(aResidualScale, aResidualSumSq)
                                            : std::nullopt;
  if (!aResidual.has_value())
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  const double aMaxNormWithFiniteSquare = std::sqrt(std::numeric_limits<double>::max());
  if (*aResidual <= aMaxNormWithFiniteSquare)
  {
    aResult.ResidualSq = *aResidual * *aResidual;
  }
  aResult.Residual = aResidual;
  aResult.Status   = Status::OK;
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

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  // Check dimensions
  if (theB.Size() != aM || theW.Size() != aM || !Utils::IsFinite(theW))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Check weights are positive
  for (size_t i = 0; i < theW.Size(); ++i)
  {
    if (theW[i] <= 0.0)
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
  }

  // Apply weights: A' = W^{1/2} * A, b' = W^{1/2} * b
  math_Matrix aWA(aM, aN);
  math_Vector aWB(aM);

  for (size_t i = 0; i < aM; ++i)
  {
    const double aSqrtW = std::sqrt(theW.At(i));
    for (size_t j = 0; j < aN; ++j)
    {
      aWA.ChangeAt(i, j) = aSqrtW * theA.At(i, j);
    }
    aWB[i] = aSqrtW * theB.At(i);
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

  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();

  // Check dimensions
  if (theB.Size() != aM || !Utils::IsFinite(theA) || !Utils::IsFinite(theB))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  if (!std::isfinite(theLambda) || theLambda < 0.0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const SVDResult aSVD = SVD(theA, theTolerance);
  if (!aSVD.IsDone())
  {
    aResult.Status = aSVD.Status;
    return aResult;
  }
  const math_Matrix& aU = *aSVD.U;
  const math_Vector& aS = *aSVD.SingularValues;
  const math_Matrix& aV = *aSVD.V;
  math_Vector aFiltered(aN, 0.0);
  for (size_t j = 0; j < aN; ++j)
  {
    double aProjection = 0.0;
    for (size_t i = 0; i < aM; ++i)
    {
      aProjection += aU.At(i, j) * theB.At(i);
    }
    const double aSigma = aS[j];
    const double aFilter = j >= aSVD.Rank
                             ? 0.0
                             : 1.0 / (aSigma + theLambda / aSigma);
    aFiltered[j] = aFilter * aProjection;
  }
  aResult.Solution = math_Vector(aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    for (size_t j = 0; j < aN; ++j)
    {
      (*aResult.Solution)[i] += aV.At(i, j) * aFiltered[j];
    }
  }
  aResult.Rank            = aSVD.Rank;
  aResult.ConditionNumber = aSVD.ConditionNumber;

  // Compute residual
  const math_Vector& aX          = *aResult.Solution;
  double             aResidualScale = 0.0;
  double             aResidualSumSq = 1.0;
  bool               isResidualValid = true;

  for (size_t i = 0; i < aM; ++i)
  {
    double aAxi = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aAxi += theA.At(i, j) * aX[j];
    }
    const double aRi = aAxi - theB.At(i);
    isResidualValid = Utils::AccumulateNorm(aRi, aResidualScale, aResidualSumSq)
                      && isResidualValid;
  }

  const std::optional<double> aResidual = isResidualValid
                                            ? Utils::Norm(aResidualScale, aResidualSumSq)
                                            : std::nullopt;
  if (!aResidual.has_value() || !Utils::IsFinite(*aResult.Solution))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  const double aMaxNormWithFiniteSquare = std::sqrt(std::numeric_limits<double>::max());
  if (*aResidual <= aMaxNormWithFiniteSquare)
  {
    aResult.ResidualSq = *aResidual * *aResidual;
  }
  aResult.Residual = aResidual;
  aResult.Status   = Status::OK;
  return aResult;
}

//! Compute a regularization parameter using generalized cross-validation (GCV).
//!
//! Minimizes ||(I-H_lambda)b||_2^2 / (m-tr(H_lambda))^2 on a logarithmic grid,
//! where H_lambda is the ridge-regression influence matrix. GCV is a rotation-invariant
//! approximation to leave-one-out cross-validation and does not minimize training error.
//!
//! @param theA coefficient matrix
//! @param theB right-hand side vector
//! @param theLambdaMin minimum lambda to consider
//! @param theLambdaMax maximum lambda to consider
//! @param theNbPoints number of lambda values to try
//! @return optimal regularization parameter, or no value if no finite score can be evaluated
inline std::optional<double> OptimalRegularization(const math_Matrix& theA,
                                                   const math_Vector& theB,
                                                   double             theLambdaMin = 1.0e-10,
                                                   double             theLambdaMax = 1.0e2,
                                                   size_t             theNbPoints  = 20)
{
  if (theA.RowSize() < theA.ColSize() || theB.Size() != theA.RowSize()
      || !Utils::IsFinite(theA) || !Utils::IsFinite(theB)
      || !std::isfinite(theLambdaMin) || !std::isfinite(theLambdaMax)
      || theLambdaMin <= 0.0 || theLambdaMax < theLambdaMin || theNbPoints < 2)
  {
    return std::nullopt;
  }

  const SVDResult aSVD = SVD(theA);
  if (!aSVD.IsDone())
  {
    return std::nullopt;
  }

  const math_Matrix& aU = *aSVD.U;
  const math_Vector& aS = *aSVD.SingularValues;
  const size_t aM = theA.RowSize();
  const size_t aN = theA.ColSize();
  math_Vector aProjection(aS.Size(), 0.0);
  double aBScale = 0.0;
  double aBSumSq = 1.0;
  for (size_t i = 0; i < aM; ++i)
  {
    if (!Utils::AccumulateNorm(theB.At(i), aBScale, aBSumSq))
    {
      return std::nullopt;
    }
  }
  if (aBScale != 0.0)
  {
    for (size_t i = 0; i < aM; ++i)
    {
      const double aNormalizedB = theB.At(i) / aBScale;
      for (size_t j = 0; j < aN; ++j)
      {
        aProjection[j] += aU.At(i, j) * aNormalizedB;
      }
    }
  }
  double aOrthogonalResidualSq = aBScale == 0.0 ? 0.0 : aBSumSq;
  for (size_t j = 0; j < aProjection.Size(); ++j)
  {
    aOrthogonalResidualSq -= aProjection[j] * aProjection[j];
  }
  aOrthogonalResidualSq = std::max(0.0, aOrthogonalResidualSq);

  std::optional<double> aBestLambda;
  double aBestScore  = std::numeric_limits<double>::infinity();

  // Logarithmic grid search
  const double aLogMin  = std::log10(theLambdaMin);
  const double aLogMax  = std::log10(theLambdaMax);
  const double aLogStep = (aLogMax - aLogMin) / (theNbPoints - 1);

  for (size_t k = 0; k < theNbPoints; ++k)
  {
    const double aLambda = std::pow(10.0, aLogMin + k * aLogStep);
    double aResidualSq = aOrthogonalResidualSq;
    double aTrace = 0.0;
    for (size_t j = 0; j < aS.Size(); ++j)
    {
      const double aSigma = aS[j];
      const double aFilter = aSigma == 0.0
                               ? 0.0
                               : 1.0 / (1.0 + (aLambda / aSigma) / aSigma);
      const double aResidualProjection = (1.0 - aFilter) * aProjection[j];
      aResidualSq += aResidualProjection * aResidualProjection;
      aTrace += aFilter;
    }
    const double aDenominator = static_cast<double>(aM) - aTrace;
    const double aScore = aDenominator > 0.0
                            ? aResidualSq / (aDenominator * aDenominator)
                            : std::numeric_limits<double>::infinity();
    if (std::isfinite(aScore) && aScore < aBestScore)
    {
      aBestScore  = aScore;
      aBestLambda = aLambda;
    }
  }

  return aBestLambda;
}

} // namespace MathLin

#endif // _MathLin_LeastSquares_HeaderFile
