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

#ifndef _MathOpt_BFGS_HeaderFile
#define _MathOpt_BFGS_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_LineSearch.hxx>
#include <MathUtils_Deriv.hxx>
#include "MathOpt_Utils.hxx"

#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

//! BFGS (Broyden-Fletcher-Goldfarb-Shanno) quasi-Newton method.
//! One of the most effective algorithms for smooth unconstrained optimization.
//!
//! Algorithm:
//! 1. Start with initial Hessian approximation H (usually identity)
//! 2. Compute search direction p = -H * gradient
//! 3. Perform an Armijo backtracking line search to find a decreasing step
//! 4. Update x = x + alpha * p
//! 5. Update Hessian approximation using BFGS formula
//! 6. Repeat until convergence
//!
//! The inverse Hessian update is applied only when the measured curvature
//! s^T*y is positive; otherwise the current approximation is retained.
//!
//! Advantages:
//! - Superlinear convergence near minimum
//! - Self-correcting Hessian approximation
//! - No need to compute actual Hessian
//!
//! @tparam Function type with:
//!   - Value(const math_Vector&, double&) for function value
//!   - Gradient(const math_Vector&, math_Vector&) for gradient
//! @param theFunc function object with value and gradient
//! @param theStartingPoint initial guess
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult BFGS(Function&          theFunc,
                  const math_Vector& theStartingPoint,
                  const Config&      theConfig = Config())
{
  VectorResult aResult;

  const size_t aN = theStartingPoint.Size();

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Current point and function value
  math_Vector aX(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aX.ChangeAt(i) = theStartingPoint.At(i);
  }

  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  // Gradient at current point
  math_Vector  aGrad(aN);
  const Status aGradientStatus = Utils::GradientStatus(theFunc, aX, aGrad);
  if (aGradientStatus != Status::OK)
  {
    aResult.Status = aGradientStatus;
    return aResult;
  }

  // Check if already at minimum (gradient near zero)
  double aGradNorm = Utils::Norm(aGrad);

  if (aGradNorm < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Initialize inverse Hessian approximation to identity
  math_Matrix aH(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    aH.ChangeAt(i, i) = 1.0;
  }

  // Working vectors
  math_Vector aDir(aN);     // Search direction
  math_Vector aXNew(aN);    // New point
  math_Vector aGradNew(aN); // New gradient
  math_Vector aS(aN);       // Step: x_new - x
  math_Vector aY(aN);       // Gradient difference: grad_new - grad

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute search direction: p = -H * grad
    for (size_t i = 0; i < aN; ++i)
    {
      double aSum = 0.0;
      for (size_t j = 0; j < aN; ++j)
      {
        aSum += aH.At(i, j) * aGrad.At(j);
      }
      aDir.ChangeAt(i) = -aSum;
    }

    // Armijo backtracking line search
    MathUtils::LineSearchResult aLineResult =
      Utils::Backtrack(theFunc, aX, aDir, aGrad, aFx, 1.0, theConfig.StepMin);

    if (!aLineResult.IsValid)
    {
      const MathUtils::LineSearchResult anInitialLineResult = aLineResult;
      // Line search failed, try steepest descent direction
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGrad.At(i);
      }
      aLineResult = Utils::Backtrack(theFunc, aX, aDir, aGrad, aFx, 1.0, theConfig.StepMin);

      if (!aLineResult.IsValid)
      {
        // Both BFGS and steepest descent failed
        aResult.Status   = Utils::LineSearchFailureStatus(anInitialLineResult, aLineResult);
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }

      // Reset Hessian to identity after steepest descent step
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aH.ChangeAt(i, j) = (i == j) ? 1.0 : 0.0;
        }
      }
    }

    // Compute new point
    for (size_t i = 0; i < aN; ++i)
    {
      aXNew.ChangeAt(i) = aX.At(i) + aLineResult.Alpha * aDir.At(i);
    }

    // Compute s = x_new - x
    for (size_t i = 0; i < aN; ++i)
    {
      aS.ChangeAt(i) = aXNew.At(i) - aX.At(i);
    }

    // Evaluate gradient at new point
    const Status aNewGradientStatus = Utils::GradientStatus(theFunc, aXNew, aGradNew);
    if (aNewGradientStatus != Status::OK)
    {
      aResult.Status   = aNewGradientStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Check gradient convergence
    aGradNorm = Utils::Norm(aGradNew);

    if (aGradNorm < theConfig.FTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew.At(i) - aX.At(i)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::NotConverged;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Compute y = grad_new - grad
    for (size_t i = 0; i < aN; ++i)
    {
      aY.ChangeAt(i) = aGradNew.At(i) - aGrad.At(i);
    }

    // Compute s^T * y (curvature condition)
    double aSY = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aSY += aS.At(i) * aY.At(i);
    }

    // Skip update if curvature condition is not satisfied
    if (aSY > MathUtils::THE_ZERO_TOL)
    {
      // BFGS update: H_new = (I - rho*sy^T) H (I - rho*ys^T) + rho*ss^T
      // where rho = 1 / (s^T y)
      const double aRho = 1.0 / aSY;

      // Compute H * y
      math_Vector aHy(aN, 0.0);
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aHy.ChangeAt(i) += aH.At(i, j) * aY.At(j);
        }
      }

      // Compute y^T * H * y
      double aYHy = 0.0;
      for (size_t i = 0; i < aN; ++i)
      {
        aYHy += aY.At(i) * aHy.At(i);
      }

      // Update H using the formula:
      // H_new = H - (Hy*s^T + s*y^T*H)/(s^T*y) + (1 + y^T*H*y/(s^T*y)) * s*s^T/(s^T*y)
      const double aFactor = 1.0 + aRho * aYHy;

      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aH.ChangeAt(i, j) = aH.At(i, j) - aRho * (aHy.At(i) * aS.At(j) + aS.At(i) * aHy.At(j))
                              + aFactor * aRho * aS.At(i) * aS.At(j);
        }
      }
    }

    // Update for next iteration
    aX    = aXNew;
    aGrad = aGradNew;
    aFx   = aLineResult.FNew;
  }

  // Maximum iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  aResult.Gradient = aGrad;
  return aResult;
}

//! BFGS with numerical gradient.
//! Uses central differences to approximate gradient when analytical
//! gradient is not available.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method only
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theGradStep step size for numerical gradient (default 1e-8)
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult BFGSNumerical(Function&          theFunc,
                           const math_Vector& theStartingPoint,
                           double             theGradStep = 1.0e-8,
                           const Config&      theConfig   = Config())
{
  if (!std::isfinite(theGradStep) || theGradStep <= 0.0)
  {
    VectorResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Wrapper that adds numerical gradient
  class FuncWithGradient
  {
  public:
    FuncWithGradient(Function& theF, double theStep)
        : myFunc(theF),
          myStep(theStep)
    {
    }

    bool Value(const math_Vector& theX, double& theF) { return myFunc.Value(theX, theF); }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      math_Vector aXMod = theX; // Make mutable copy
      return MathUtils::NumericalGradientAdaptive(myFunc, aXMod, theGrad, myStep);
    }

  private:
    Function& myFunc;
    double    myStep;
  };

  FuncWithGradient aWrapper(theFunc, theGradStep);
  return BFGS(aWrapper, theStartingPoint, theConfig);
}

//! L-BFGS (Limited-memory BFGS) for large-scale optimization.
//! Uses only the m most recent {s, y} pairs instead of full Hessian.
//! Memory: O(m*n) instead of O(n^2).
//!
//! @tparam Function type with Value and Gradient methods
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theMemorySize number of gradient pairs to store (default 10)
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult LBFGS(Function&          theFunc,
                   const math_Vector& theStartingPoint,
                   size_t             theMemorySize = 10,
                   const Config&      theConfig     = Config())
{
  VectorResult aResult;

  const size_t aN = theStartingPoint.Size();
  const size_t aM = theMemorySize;

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint) || aM == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  math_Vector aX(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aX.ChangeAt(i) = theStartingPoint.At(i);
  }

  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  math_Vector  aGrad(aN);
  const Status aGradientStatus = Utils::GradientStatus(theFunc, aX, aGrad);
  if (aGradientStatus != Status::OK)
  {
    aResult.Status = aGradientStatus;
    return aResult;
  }

  // Storage for {s, y} pairs (circular buffer)
  NCollection_DynamicArray<math_Vector> aSVec(std::min<size_t>(aM, 8));
  NCollection_DynamicArray<math_Vector> aYVec(std::min<size_t>(aM, 8));
  NCollection_LinearVector<double>      aRhoVec;
  for (size_t i = 0; i < aM; ++i)
  {
    aSVec.Append(math_Vector(aN));
    aYVec.Append(math_Vector(aN));
    aRhoVec.Append(0.0);
  }
  size_t aHead  = 0; // Index of oldest entry
  size_t aCount = 0; // Number of stored pairs

  math_Vector                      aDir(aN);
  math_Vector                      aXNew(aN);
  math_Vector                      aGradNew(aN);
  math_Vector                      aQ(aN);
  NCollection_LinearVector<double> aAlphaVec;
  for (size_t i = 0; i < aM; ++i)
  {
    aAlphaVec.Append(0.0);
  }

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check gradient convergence
    if (Utils::Norm(aGrad) < theConfig.FTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // L-BFGS two-loop recursion to compute search direction
    // q = gradient
    for (size_t i = 0; i < aN; ++i)
    {
      aQ.ChangeAt(i) = aGrad.At(i);
    }

    // First loop (backward)
    for (size_t k = aCount; k-- > 0;)
    {
      const size_t aIdx = (aHead + k) % aM;
      aAlphaVec.ChangeValue(aIdx) =
        aRhoVec.Value(aIdx) * MathUtils::DotProduct(aSVec.Value(aIdx), aQ);
      for (size_t i = 0; i < aN; ++i)
      {
        aQ.ChangeAt(i) -= aAlphaVec.Value(aIdx) * aYVec.Value(aIdx).At(i);
      }
    }

    // Initial Hessian: H0 = gamma*I where gamma = s^T y / y^T y
    double aGamma = 1.0;
    if (aCount > 0)
    {
      const size_t aLastIdx = (aHead + aCount - 1) % aM;
      const double aYY      = MathUtils::DotProduct(aYVec.Value(aLastIdx), aYVec.Value(aLastIdx));
      if (aYY > MathUtils::THE_ZERO_TOL)
      {
        aGamma = 1.0 / (aRhoVec.Value(aLastIdx) * aYY);
      }
    }

    // r = H0 * q = gamma * q
    math_Vector aR(aN);
    for (size_t i = 0; i < aN; ++i)
    {
      aR.ChangeAt(i) = aGamma * aQ.At(i);
    }

    // Second loop (forward)
    for (size_t k = 0; k < aCount; ++k)
    {
      const size_t aIdx  = (aHead + k) % aM;
      const double aBeta = aRhoVec.Value(aIdx) * MathUtils::DotProduct(aYVec.Value(aIdx), aR);
      for (size_t i = 0; i < aN; ++i)
      {
        aR.ChangeAt(i) += (aAlphaVec.Value(aIdx) - aBeta) * aSVec.Value(aIdx).At(i);
      }
    }

    // Search direction: p = -r
    for (size_t i = 0; i < aN; ++i)
    {
      aDir.ChangeAt(i) = -aR.At(i);
    }

    // Line search
    MathUtils::LineSearchResult aLineResult =
      Utils::Backtrack(theFunc, aX, aDir, aGrad, aFx, 1.0, theConfig.StepMin);

    if (!aLineResult.IsValid)
    {
      const MathUtils::LineSearchResult anInitialLineResult = aLineResult;
      // Fall back to steepest descent
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGrad.At(i);
      }
      aLineResult = Utils::Backtrack(theFunc, aX, aDir, aGrad, aFx, 1.0, theConfig.StepMin);

      if (!aLineResult.IsValid)
      {
        aResult.Status   = Utils::LineSearchFailureStatus(anInitialLineResult, aLineResult);
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }
      // Reset history after steepest descent
      aCount = 0;
    }

    // Compute new point
    for (size_t i = 0; i < aN; ++i)
    {
      aXNew.ChangeAt(i) = aX.At(i) + aLineResult.Alpha * aDir.At(i);
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew.At(i) - aX.At(i)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      const Status aNewGradientStatus = Utils::GradientStatus(theFunc, aXNew, aGradNew);
      if (aNewGradientStatus != Status::OK)
      {
        aResult.Status   = aNewGradientStatus;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }
      aResult.Status =
        Utils::Norm(aGradNew) < theConfig.FTolerance ? Status::OK : Status::NotConverged;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Evaluate gradient at new point
    const Status aNewGradientStatus = Utils::GradientStatus(theFunc, aXNew, aGradNew);
    if (aNewGradientStatus != Status::OK)
    {
      aResult.Status   = aNewGradientStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // Store new {s, y} pair
    const size_t aNewIdx = (aHead + aCount) % aM;
    for (size_t i = 0; i < aN; ++i)
    {
      aSVec.ChangeValue(aNewIdx).ChangeAt(i) = aXNew.At(i) - aX.At(i);
      aYVec.ChangeValue(aNewIdx).ChangeAt(i) = aGradNew.At(i) - aGrad.At(i);
    }

    double aSY = MathUtils::DotProduct(aSVec.Value(aNewIdx), aYVec.Value(aNewIdx));
    if (aSY > MathUtils::THE_ZERO_TOL)
    {
      aRhoVec.ChangeValue(aNewIdx) = 1.0 / aSY;
      if (aCount < aM)
      {
        ++aCount;
      }
      else
      {
        aHead = (aHead + 1) % aM;
      }
    }

    // Update for next iteration
    aX    = aXNew;
    aGrad = aGradNew;
    aFx   = aLineResult.FNew;
  }

  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  aResult.Gradient = aGrad;
  return aResult;
}

//! BFGS with bound constraints (box constraints).
//!
//! Minimizes f(x) subject to theLowerBounds <= x <= theUpperBounds.
//! Uses projected gradient approach:
//! - After each step, clamp x to bounds
//! - Zero gradient components at bounds where they point outward
//!
//! @tparam Function type with Value and Gradient methods
//! @param theFunc function object
//! @param theStartingPoint initial guess
//! @param theLowerBounds lower bounds for each variable
//! @param theUpperBounds upper bounds for each variable
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult BFGSBounded(Function&          theFunc,
                         const math_Vector& theStartingPoint,
                         const math_Vector& theLowerBounds,
                         const math_Vector& theUpperBounds,
                         const Config&      theConfig = Config())
{
  VectorResult aResult;

  const size_t aN = theStartingPoint.Size();

  // Check dimensions
  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint)
      || theLowerBounds.Size() != aN || !Utils::IsValidBounds(theLowerBounds, theUpperBounds))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Lambda to clamp a point to bounds
  auto ClampToBounds = [&](math_Vector& theX) {
    for (size_t i = 0; i < aN; ++i)
    {
      if (theX.At(i) < theLowerBounds.At(i))
      {
        theX.ChangeAt(i) = theLowerBounds.At(i);
      }
      if (theX.At(i) > theUpperBounds.At(i))
      {
        theX.ChangeAt(i) = theUpperBounds.At(i);
      }
    }
  };

  // Lambda to project gradient (zero components at active bounds)
  auto ProjectGradient = [&](const math_Vector& theX, math_Vector& theGrad) {
    for (size_t i = 0; i < aN; ++i)
    {
      const double aTol = MathUtils::THE_EPSILON * std::max(1.0, std::abs(theX.At(i)));

      // At lower bound and gradient points downward -> zero gradient
      if (theX.At(i) - theLowerBounds.At(i) < aTol && theGrad.At(i) > 0.0)
      {
        theGrad.ChangeAt(i) = 0.0;
      }
      // At upper bound and gradient points upward -> zero gradient
      if (theUpperBounds.At(i) - theX.At(i) < aTol && theGrad.At(i) < 0.0)
      {
        theGrad.ChangeAt(i) = 0.0;
      }
    }
  };

  // Remove Hessian-coupled direction components that point out of an active bound.
  auto ProjectDirection = [&](const math_Vector& theX, math_Vector& theDir) {
    for (size_t i = 0; i < aN; ++i)
    {
      const double aTol = MathUtils::THE_EPSILON * std::max(1.0, std::abs(theX.At(i)));
      if ((theX.At(i) - theLowerBounds.At(i) < aTol && theDir.At(i) < 0.0)
          || (theUpperBounds.At(i) - theX.At(i) < aTol && theDir.At(i) > 0.0))
      {
        theDir.ChangeAt(i) = 0.0;
      }
    }
  };

  Utils::BoundedFunction<Function> aBoundedFunc(theFunc, theLowerBounds, theUpperBounds);

  // Current point and function value
  math_Vector aX(aN);
  for (size_t i = 0; i < aN; ++i)
  {
    aX.ChangeAt(i) = theStartingPoint.At(i);
  }
  ClampToBounds(aX);

  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  // Gradient at current point
  math_Vector  aGrad(aN);
  const Status aGradientStatus = Utils::GradientStatus(theFunc, aX, aGrad);
  if (aGradientStatus != Status::OK)
  {
    aResult.Status = aGradientStatus;
    return aResult;
  }
  ProjectGradient(aX, aGrad);

  // Check if already at minimum
  double aGradNorm = Utils::Norm(aGrad);

  if (aGradNorm < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Initialize inverse Hessian approximation to identity
  math_Matrix aH(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    aH.ChangeAt(i, i) = 1.0;
  }

  // Working vectors
  math_Vector aDir(aN);
  math_Vector aXNew(aN);
  math_Vector aGradNew(aN);
  math_Vector aS(aN);
  math_Vector aY(aN);

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute search direction: p = -H * grad
    for (size_t i = 0; i < aN; ++i)
    {
      double aSum = 0.0;
      for (size_t j = 0; j < aN; ++j)
      {
        aSum += aH.At(i, j) * aGrad.At(j);
      }
      aDir.ChangeAt(i) = -aSum;
    }
    ProjectDirection(aX, aDir);

    // Line search with bounds-aware step
    double aAlphaMax = 1.0;
    for (size_t i = 0; i < aN; ++i)
    {
      if (aDir.At(i) < -MathUtils::THE_EPSILON)
      {
        // Moving toward lower bound
        const double aMaxStep = (theLowerBounds.At(i) - aX.At(i)) / aDir.At(i);
        aAlphaMax             = std::min(aAlphaMax, aMaxStep);
      }
      else if (aDir.At(i) > MathUtils::THE_EPSILON)
      {
        // Moving toward upper bound
        const double aMaxStep = (theUpperBounds.At(i) - aX.At(i)) / aDir.At(i);
        aAlphaMax             = std::min(aAlphaMax, aMaxStep);
      }
    }
    MathUtils::LineSearchResult aLineResult;
    if (aAlphaMax > 0.0)
    {
      aLineResult =
        Utils::Backtrack(aBoundedFunc, aX, aDir, aGrad, aFx, aAlphaMax, theConfig.StepMin);
    }

    if (!aLineResult.IsValid)
    {
      const MathUtils::LineSearchResult anInitialLineResult = aLineResult;
      // Fall back to projected steepest descent
      for (size_t i = 0; i < aN; ++i)
      {
        aDir.ChangeAt(i) = -aGrad.At(i);
      }
      ProjectDirection(aX, aDir);
      // Recompute alpha max for steepest descent
      aAlphaMax = 1.0;
      for (size_t i = 0; i < aN; ++i)
      {
        if (aDir.At(i) < -MathUtils::THE_EPSILON)
        {
          aAlphaMax = std::min(aAlphaMax, (theLowerBounds.At(i) - aX.At(i)) / aDir.At(i));
        }
        else if (aDir.At(i) > MathUtils::THE_EPSILON)
        {
          aAlphaMax = std::min(aAlphaMax, (theUpperBounds.At(i) - aX.At(i)) / aDir.At(i));
        }
      }
      if (aAlphaMax > 0.0)
      {
        aLineResult =
          Utils::Backtrack(aBoundedFunc, aX, aDir, aGrad, aFx, aAlphaMax, theConfig.StepMin);
      }

      if (!aLineResult.IsValid)
      {
        aResult.Status   = Utils::LineSearchFailureStatus(anInitialLineResult, aLineResult);
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }

      // Reset Hessian
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aH.ChangeAt(i, j) = (i == j) ? 1.0 : 0.0;
        }
      }
    }

    // Compute and clamp new point
    for (size_t i = 0; i < aN; ++i)
    {
      aXNew.ChangeAt(i) = aX.At(i) + aLineResult.Alpha * aDir.At(i);
    }
    ClampToBounds(aXNew);

    // Compute s = x_new - x
    for (size_t i = 0; i < aN; ++i)
    {
      aS.ChangeAt(i) = aXNew.At(i) - aX.At(i);
    }

    // Evaluate at new point
    double       aFxNew          = 0.0;
    const Status aNewValueStatus = Utils::ValueStatus(theFunc, aXNew, aFxNew);
    if (aNewValueStatus != Status::OK)
    {
      aResult.Status   = aNewValueStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    const Status aNewGradientStatus = Utils::GradientStatus(theFunc, aXNew, aGradNew);
    if (aNewGradientStatus != Status::OK)
    {
      aResult.Status   = aNewGradientStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }
    ProjectGradient(aXNew, aGradNew);

    // Check gradient convergence
    aGradNorm = Utils::Norm(aGradNew);

    if (aGradNorm < theConfig.FTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aFxNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew.At(i) - aX.At(i)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::NotConverged;
      aResult.Solution = aXNew;
      aResult.Value    = aFxNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Compute y = grad_new - grad
    for (size_t i = 0; i < aN; ++i)
    {
      aY.ChangeAt(i) = aGradNew.At(i) - aGrad.At(i);
    }

    // Curvature condition
    double aSY = 0.0;
    for (size_t i = 0; i < aN; ++i)
    {
      aSY += aS.At(i) * aY.At(i);
    }

    if (aSY > MathUtils::THE_ZERO_TOL)
    {
      const double aRho = 1.0 / aSY;

      math_Vector aHy(aN, 0.0);
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aHy.ChangeAt(i) += aH.At(i, j) * aY.At(j);
        }
      }

      double aYHy = 0.0;
      for (size_t i = 0; i < aN; ++i)
      {
        aYHy += aY.At(i) * aHy.At(i);
      }

      const double aFactor = 1.0 + aRho * aYHy;
      for (size_t i = 0; i < aN; ++i)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aH.ChangeAt(i, j) = aH.At(i, j) - aRho * (aHy.At(i) * aS.At(j) + aS.At(i) * aHy.At(j))
                              + aFactor * aRho * aS.At(i) * aS.At(j);
        }
      }
    }

    aX    = aXNew;
    aGrad = aGradNew;
    aFx   = aFxNew;
  }

  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  aResult.Gradient = aGrad;
  return aResult;
}

} // namespace MathOpt

#endif // _MathOpt_BFGS_HeaderFile
