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

#include <NCollection_Vector.hxx>

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
//! 3. Perform line search to find step size alpha satisfying Wolfe conditions
//! 4. Update x = x + alpha * p
//! 5. Update Hessian approximation using BFGS formula
//! 6. Repeat until convergence
//!
//! The BFGS update maintains positive definiteness of H if started with
//! a positive definite matrix and using proper line search.
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

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();
  const int aN     = aUpper - aLower + 1;

  // Current point and function value
  math_Vector aX(aLower, aUpper);
  aX = theStartingPoint;

  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Gradient at current point
  math_Vector aGrad(aLower, aUpper);
  if (!theFunc.Gradient(aX, aGrad))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Check if already at minimum (gradient near zero)
  double aGradNorm = 0.0;
  for (int i = aLower; i <= aUpper; ++i)
  {
    aGradNorm += MathUtils::Sqr(aGrad(i));
  }
  aGradNorm = std::sqrt(aGradNorm);

  if (aGradNorm < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Initialize inverse Hessian approximation to identity
  math_Matrix aH(1, aN, 1, aN, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    aH(i, i) = 1.0;
  }

  // Working vectors
  math_Vector aDir(aLower, aUpper);     // Search direction
  math_Vector aXNew(aLower, aUpper);    // New point
  math_Vector aGradNew(aLower, aUpper); // New gradient
  math_Vector aS(1, aN);                // Step: x_new - x
  math_Vector aY(1, aN);                // Gradient difference: grad_new - grad

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute search direction: p = -H * grad
    for (int i = 1; i <= aN; ++i)
    {
      double aSum = 0.0;
      for (int j = 1; j <= aN; ++j)
      {
        aSum += aH(i, j) * aGrad(aLower + j - 1);
      }
      aDir(aLower + i - 1) = -aSum;
    }

    // Line search with Wolfe conditions
    MathUtils::LineSearchResult aLineResult =
      MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

    if (!aLineResult.IsValid || aLineResult.Alpha < MathUtils::THE_EPSILON)
    {
      // Line search failed, try steepest descent direction
      for (int i = aLower; i <= aUpper; ++i)
      {
        aDir(i) = -aGrad(i);
      }
      aLineResult = MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

      if (!aLineResult.IsValid)
      {
        // Both BFGS and steepest descent failed
        aResult.Status   = Status::NotConverged;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }

      // Reset Hessian to identity after steepest descent step
      for (int i = 1; i <= aN; ++i)
      {
        for (int j = 1; j <= aN; ++j)
        {
          aH(i, j) = (i == j) ? 1.0 : 0.0;
        }
      }
    }

    // Compute new point
    for (int i = aLower; i <= aUpper; ++i)
    {
      aXNew(i) = aX(i) + aLineResult.Alpha * aDir(i);
    }

    // Compute s = x_new - x
    for (int i = 1; i <= aN; ++i)
    {
      aS(i) = aXNew(aLower + i - 1) - aX(aLower + i - 1);
    }

    // Evaluate gradient at new point
    if (!theFunc.Gradient(aXNew, aGradNew))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Check gradient convergence
    aGradNorm = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aGradNorm += MathUtils::Sqr(aGradNew(i));
    }
    aGradNorm = std::sqrt(aGradNorm);

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
    for (int i = aLower; i <= aUpper; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew(i) - aX(i)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Compute y = grad_new - grad
    for (int i = 1; i <= aN; ++i)
    {
      aY(i) = aGradNew(aLower + i - 1) - aGrad(aLower + i - 1);
    }

    // Compute s^T * y (curvature condition)
    double aSY = 0.0;
    for (int i = 1; i <= aN; ++i)
    {
      aSY += aS(i) * aY(i);
    }

    // Skip update if curvature condition is not satisfied
    if (aSY > MathUtils::THE_ZERO_TOL)
    {
      // BFGS update: H_new = (I - rho*sy^T) H (I - rho*ys^T) + rho*ss^T
      // where rho = 1 / (s^T y)
      const double aRho = 1.0 / aSY;

      // Compute H * y
      math_Vector aHy(1, aN, 0.0);
      for (int i = 1; i <= aN; ++i)
      {
        for (int j = 1; j <= aN; ++j)
        {
          aHy(i) += aH(i, j) * aY(j);
        }
      }

      // Compute y^T * H * y
      double aYHy = 0.0;
      for (int i = 1; i <= aN; ++i)
      {
        aYHy += aY(i) * aHy(i);
      }

      // Update H using the formula:
      // H_new = H - (Hy*s^T + s*y^T*H)/(s^T*y) + (1 + y^T*H*y/(s^T*y)) * s*s^T/(s^T*y)
      const double aFactor = 1.0 + aRho * aYHy;

      for (int i = 1; i <= aN; ++i)
      {
        for (int j = 1; j <= aN; ++j)
        {
          aH(i, j) =
            aH(i, j) - aRho * (aHy(i) * aS(j) + aS(i) * aHy(j)) + aFactor * aRho * aS(i) * aS(j);
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
                   int                theMemorySize = 10,
                   const Config&      theConfig     = Config())
{
  VectorResult aResult;

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();
  const int aN     = aUpper - aLower + 1;
  const int aM     = theMemorySize;

  math_Vector aX(aLower, aUpper);
  aX = theStartingPoint;

  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  math_Vector aGrad(aLower, aUpper);
  if (!theFunc.Gradient(aX, aGrad))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Storage for {s, y} pairs (circular buffer)
  NCollection_Vector<math_Vector> aSVec;
  NCollection_Vector<math_Vector> aYVec;
  math_Vector                     aRhoVec(0, aM - 1, 0.0);
  for (int i = 0; i < aM; ++i)
  {
    aSVec.Append(math_Vector(1, aN));
    aYVec.Append(math_Vector(1, aN));
  }
  int aHead  = 0; // Index of oldest entry
  int aCount = 0; // Number of stored pairs

  math_Vector aDir(aLower, aUpper);
  math_Vector aXNew(aLower, aUpper);
  math_Vector aGradNew(aLower, aUpper);
  math_Vector aQ(1, aN);
  math_Vector aAlphaVec(0, aM - 1);

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check gradient convergence
    double aGradNorm = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aGradNorm += MathUtils::Sqr(aGrad(i));
    }
    if (std::sqrt(aGradNorm) < theConfig.FTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      aResult.Gradient = aGrad;
      return aResult;
    }

    // L-BFGS two-loop recursion to compute search direction
    // q = gradient
    for (int i = 1; i <= aN; ++i)
    {
      aQ(i) = aGrad(aLower + i - 1);
    }

    // First loop (backward)
    for (int k = aCount - 1; k >= 0; --k)
    {
      const int aIdx  = (aHead + k) % aM;
      aAlphaVec(aIdx) = aRhoVec(aIdx) * MathUtils::DotProduct(aSVec.Value(aIdx), aQ);
      for (int i = 1; i <= aN; ++i)
      {
        aQ(i) -= aAlphaVec(aIdx) * aYVec.Value(aIdx)(i);
      }
    }

    // Initial Hessian: H0 = gamma*I where gamma = s^T y / y^T y
    double aGamma = 1.0;
    if (aCount > 0)
    {
      const int aLastIdx = (aHead + aCount - 1) % aM;
      double    aYY      = MathUtils::DotProduct(aYVec.Value(aLastIdx), aYVec.Value(aLastIdx));
      if (aYY > MathUtils::THE_ZERO_TOL)
      {
        aGamma = 1.0 / (aRhoVec(aLastIdx) * aYY);
      }
    }

    // r = H0 * q = gamma * q
    math_Vector aR(1, aN);
    for (int i = 1; i <= aN; ++i)
    {
      aR(i) = aGamma * aQ(i);
    }

    // Second loop (forward)
    for (int k = 0; k < aCount; ++k)
    {
      const int    aIdx  = (aHead + k) % aM;
      const double aBeta = aRhoVec(aIdx) * MathUtils::DotProduct(aYVec.Value(aIdx), aR);
      for (int i = 1; i <= aN; ++i)
      {
        aR(i) += (aAlphaVec(aIdx) - aBeta) * aSVec.Value(aIdx)(i);
      }
    }

    // Search direction: p = -r
    for (int i = aLower; i <= aUpper; ++i)
    {
      aDir(i) = -aR(i - aLower + 1);
    }

    // Line search
    MathUtils::LineSearchResult aLineResult =
      MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

    if (!aLineResult.IsValid)
    {
      // Fall back to steepest descent
      for (int i = aLower; i <= aUpper; ++i)
      {
        aDir(i) = -aGrad(i);
      }
      aLineResult = MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, 1.0, 1.0e-4, 0.5, 50);

      if (!aLineResult.IsValid)
      {
        aResult.Status   = Status::NotConverged;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        return aResult;
      }
      // Reset history after steepest descent
      aCount = 0;
    }

    // Compute new point
    for (int i = aLower; i <= aUpper; ++i)
    {
      aXNew(i) = aX(i) + aLineResult.Alpha * aDir(i);
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew(i) - aX(i)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      if (!theFunc.Gradient(aXNew, aGradNew))
      {
        aGradNew = aGrad;
      }
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aLineResult.FNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Evaluate gradient at new point
    if (!theFunc.Gradient(aXNew, aGradNew))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Store new {s, y} pair
    const int aNewIdx = (aHead + aCount) % aM;
    for (int i = 1; i <= aN; ++i)
    {
      aSVec.ChangeValue(aNewIdx)(i) = aXNew(aLower + i - 1) - aX(aLower + i - 1);
      aYVec.ChangeValue(aNewIdx)(i) = aGradNew(aLower + i - 1) - aGrad(aLower + i - 1);
    }

    double aSY = MathUtils::DotProduct(aSVec.Value(aNewIdx), aYVec.Value(aNewIdx));
    if (aSY > MathUtils::THE_ZERO_TOL)
    {
      aRhoVec(aNewIdx) = 1.0 / aSY;
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

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();
  const int aN     = aUpper - aLower + 1;

  // Check dimensions
  if (theLowerBounds.Length() != aN || theUpperBounds.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Lambda to clamp a point to bounds
  auto ClampToBounds = [&](math_Vector& theX) {
    for (int i = aLower; i <= aUpper; ++i)
    {
      const int aBndIdx = theLowerBounds.Lower() + (i - aLower);
      if (theX(i) < theLowerBounds(aBndIdx))
      {
        theX(i) = theLowerBounds(aBndIdx);
      }
      if (theX(i) > theUpperBounds(aBndIdx))
      {
        theX(i) = theUpperBounds(aBndIdx);
      }
    }
  };

  // Lambda to project gradient (zero components at active bounds)
  auto ProjectGradient = [&](const math_Vector& theX, math_Vector& theGrad) {
    for (int i = aLower; i <= aUpper; ++i)
    {
      const int    aBndIdx = theLowerBounds.Lower() + (i - aLower);
      const double aTol    = MathUtils::THE_EPSILON * std::max(1.0, std::abs(theX(i)));

      // At lower bound and gradient points downward -> zero gradient
      if (theX(i) - theLowerBounds(aBndIdx) < aTol && theGrad(i) > 0.0)
      {
        theGrad(i) = 0.0;
      }
      // At upper bound and gradient points upward -> zero gradient
      if (theUpperBounds(aBndIdx) - theX(i) < aTol && theGrad(i) < 0.0)
      {
        theGrad(i) = 0.0;
      }
    }
  };

  // Current point and function value
  math_Vector aX(aLower, aUpper);
  aX = theStartingPoint;
  ClampToBounds(aX);

  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Gradient at current point
  math_Vector aGrad(aLower, aUpper);
  if (!theFunc.Gradient(aX, aGrad))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  ProjectGradient(aX, aGrad);

  // Check if already at minimum
  double aGradNorm = 0.0;
  for (int i = aLower; i <= aUpper; ++i)
  {
    aGradNorm += MathUtils::Sqr(aGrad(i));
  }
  aGradNorm = std::sqrt(aGradNorm);

  if (aGradNorm < theConfig.FTolerance)
  {
    aResult.Status   = Status::OK;
    aResult.Solution = aX;
    aResult.Value    = aFx;
    aResult.Gradient = aGrad;
    return aResult;
  }

  // Initialize inverse Hessian approximation to identity
  math_Matrix aH(1, aN, 1, aN, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    aH(i, i) = 1.0;
  }

  // Working vectors
  math_Vector aDir(aLower, aUpper);
  math_Vector aXNew(aLower, aUpper);
  math_Vector aGradNew(aLower, aUpper);
  math_Vector aS(1, aN);
  math_Vector aY(1, aN);

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute search direction: p = -H * grad
    for (int i = 1; i <= aN; ++i)
    {
      double aSum = 0.0;
      for (int j = 1; j <= aN; ++j)
      {
        aSum += aH(i, j) * aGrad(aLower + j - 1);
      }
      aDir(aLower + i - 1) = -aSum;
    }

    // Line search with bounds-aware step
    double aAlphaMax = 1.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      const int aBndIdx = theLowerBounds.Lower() + (i - aLower);
      if (aDir(i) < -MathUtils::THE_EPSILON)
      {
        // Moving toward lower bound
        double aMaxStep = (theLowerBounds(aBndIdx) - aX(i)) / aDir(i);
        aAlphaMax       = std::min(aAlphaMax, aMaxStep);
      }
      else if (aDir(i) > MathUtils::THE_EPSILON)
      {
        // Moving toward upper bound
        double aMaxStep = (theUpperBounds(aBndIdx) - aX(i)) / aDir(i);
        aAlphaMax       = std::min(aAlphaMax, aMaxStep);
      }
    }
    aAlphaMax = std::max(aAlphaMax, MathUtils::THE_EPSILON);

    MathUtils::LineSearchResult aLineResult =
      MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, aAlphaMax, 1.0e-4, 0.5, 50);

    if (!aLineResult.IsValid || aLineResult.Alpha < MathUtils::THE_EPSILON)
    {
      // Fall back to projected steepest descent
      for (int i = aLower; i <= aUpper; ++i)
      {
        aDir(i) = -aGrad(i);
      }
      // Recompute alpha max for steepest descent
      aAlphaMax = 1.0;
      for (int i = aLower; i <= aUpper; ++i)
      {
        const int aBndIdx = theLowerBounds.Lower() + (i - aLower);
        if (aDir(i) < -MathUtils::THE_EPSILON)
        {
          aAlphaMax = std::min(aAlphaMax, (theLowerBounds(aBndIdx) - aX(i)) / aDir(i));
        }
        else if (aDir(i) > MathUtils::THE_EPSILON)
        {
          aAlphaMax = std::min(aAlphaMax, (theUpperBounds(aBndIdx) - aX(i)) / aDir(i));
        }
      }
      aAlphaMax = std::max(aAlphaMax, MathUtils::THE_EPSILON);

      aLineResult =
        MathUtils::ArmijoBacktrack(theFunc, aX, aDir, aGrad, aFx, aAlphaMax, 1.0e-4, 0.5, 50);

      if (!aLineResult.IsValid)
      {
        aResult.Status   = Status::NotConverged;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        aResult.Gradient = aGrad;
        return aResult;
      }

      // Reset Hessian
      for (int i = 1; i <= aN; ++i)
      {
        for (int j = 1; j <= aN; ++j)
        {
          aH(i, j) = (i == j) ? 1.0 : 0.0;
        }
      }
    }

    // Compute and clamp new point
    for (int i = aLower; i <= aUpper; ++i)
    {
      aXNew(i) = aX(i) + aLineResult.Alpha * aDir(i);
    }
    ClampToBounds(aXNew);

    // Compute s = x_new - x
    for (int i = 1; i <= aN; ++i)
    {
      aS(i) = aXNew(aLower + i - 1) - aX(aLower + i - 1);
    }

    // Evaluate at new point
    double aFxNew = 0.0;
    if (!theFunc.Value(aXNew, aFxNew))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    if (!theFunc.Gradient(aXNew, aGradNew))
    {
      aResult.Status   = Status::NumericalError;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }
    ProjectGradient(aXNew, aGradNew);

    // Check gradient convergence
    aGradNorm = 0.0;
    for (int i = aLower; i <= aUpper; ++i)
    {
      aGradNorm += MathUtils::Sqr(aGradNew(i));
    }
    aGradNorm = std::sqrt(aGradNorm);

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
    for (int i = aLower; i <= aUpper; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aXNew(i) - aX(i)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aXNew;
      aResult.Value    = aFxNew;
      aResult.Gradient = aGradNew;
      return aResult;
    }

    // Compute y = grad_new - grad
    for (int i = 1; i <= aN; ++i)
    {
      aY(i) = aGradNew(aLower + i - 1) - aGrad(aLower + i - 1);
    }

    // Curvature condition
    double aSY = 0.0;
    for (int i = 1; i <= aN; ++i)
    {
      aSY += aS(i) * aY(i);
    }

    if (aSY > MathUtils::THE_ZERO_TOL)
    {
      const double aRho = 1.0 / aSY;

      math_Vector aHy(1, aN, 0.0);
      for (int i = 1; i <= aN; ++i)
      {
        for (int j = 1; j <= aN; ++j)
        {
          aHy(i) += aH(i, j) * aY(j);
        }
      }

      double aYHy = 0.0;
      for (int i = 1; i <= aN; ++i)
      {
        aYHy += aY(i) * aHy(i);
      }

      const double aFactor = 1.0 + aRho * aYHy;
      for (int i = 1; i <= aN; ++i)
      {
        for (int j = 1; j <= aN; ++j)
        {
          aH(i, j) =
            aH(i, j) - aRho * (aHy(i) * aS(j) + aS(i) * aHy(j)) + aFactor * aRho * aS(i) * aS(j);
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
