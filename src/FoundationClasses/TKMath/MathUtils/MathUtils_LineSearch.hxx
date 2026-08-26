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

#ifndef _MathUtils_LineSearch_HeaderFile
#define _MathUtils_LineSearch_HeaderFile

#include <math_Vector.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_Types.hxx>

#include <cmath>

//! Core utilities for modern math solvers.
namespace MathUtils
{

//! Historical default bound for exact line-search step magnitude.
inline constexpr double THE_EXACT_LINE_SEARCH_ALPHA_MAX = 10.0;

//! Result of line search operation.
struct LineSearchResult
{
  MathUtils::Status Status  = MathUtils::Status::NotConverged; //!< Detailed outcome
  bool              IsValid = false; //!< True only when the documented conditions hold
  double            Alpha   = 0.0;   //!< Step size for FNew, including on partial failure
  double            FNew = std::numeric_limits<double>::quiet_NaN(); //!< Value at Alpha if finite
  size_t            NbEvals = 0; //!< Number of callback evaluations
};

//! Backtracking line search with Armijo condition.
//! Finds alpha such that f(x + alpha*d) <= f(x) + c1*alpha*grad(f).d
//! (sufficient decrease condition)
//!
//! Algorithm:
//! 1. Start with alpha = alpha_init
//! 2. If Armijo condition satisfied, return alpha
//! 3. Otherwise alpha = rho * alpha and repeat
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc objective function
//! @param theX current point
//! @param theDir search direction (should be descent direction)
//! @param theGrad gradient at current point
//! @param theFx function value at current point
//! @param theAlphaInit initial step size (default 1.0)
//! @param theC1 Armijo parameter (default 1e-4)
//! @param theRho backtracking factor (default 0.5)
//! @param theMaxIter maximum iterations (default 50)
//! @return line search result with step size
template <typename Function>
LineSearchResult ArmijoBacktrack(Function&          theFunc,
                                 const math_Vector& theX,
                                 const math_Vector& theDir,
                                 const math_Vector& theGrad,
                                 double             theFx,
                                 double             theAlphaInit = 1.0,
                                 double             theC1        = 1.0e-4,
                                 double             theRho       = 0.5,
                                 uint32_t           theMaxIter   = 50)
{
  LineSearchResult aResult;
  aResult.Alpha   = theAlphaInit;
  aResult.NbEvals = 0;

  if (theDir.Size() != theX.Size() || theGrad.Size() != theX.Size() || !std::isfinite(theFx)
      || !std::isfinite(theAlphaInit) || theAlphaInit <= 0.0 || !std::isfinite(theC1)
      || theC1 <= 0.0 || theC1 >= 1.0 || !std::isfinite(theRho) || theRho <= 0.0 || theRho >= 1.0
      || theMaxIter <= 0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  // Compute directional derivative: grad(f) . d
  double aDirDeriv = 0.0;
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    const double aGrad = theGrad.At(i);
    const double aDir  = theDir.At(i);
    if (!std::isfinite(theX.At(i)) || !std::isfinite(aGrad) || !std::isfinite(aDir))
    {
      aResult.Status = MathUtils::Status::InvalidInput;
      return aResult;
    }
    aDirDeriv += aGrad * aDir;
  }

  // Ensure descent direction
  if (!std::isfinite(aDirDeriv) || aDirDeriv >= 0.0)
  {
    // Not a descent direction, return failure
    aResult.Status = std::isfinite(aDirDeriv) ? MathUtils::Status::NonDescentDirection
                                              : MathUtils::Status::NumericalError;
    return aResult;
  }

  // Temporary vector for new point
  math_Vector aXNew(theX.Size());

  for (uint32_t k = 0; k < theMaxIter; ++k)
  {
    // Compute new point: x + alpha*d
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      aXNew.ChangeAt(i) = theX.At(i) + aResult.Alpha * theDir.At(i);
      if (!std::isfinite(aXNew.At(i)))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
    }

    // Evaluate function at new point
    double aFNew = 0.0;
    if (!theFunc.Value(aXNew, aFNew))
    {
      ++aResult.NbEvals;
      aResult.Status = MathUtils::Status::CallbackError;
      return aResult;
    }
    ++aResult.NbEvals;
    aResult.FNew = aFNew;
    if (!std::isfinite(aFNew))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }

    // Check Armijo condition: f(x + alpha*d) <= f(x) + c1*alpha*phi'(0)
    if (aFNew <= theFx + theC1 * aResult.Alpha * aDirDeriv)
    {
      aResult.IsValid = true;
      aResult.Status  = MathUtils::Status::OK;
      aResult.FNew    = aFNew;
      return aResult;
    }

    const double aNextAlpha = aResult.Alpha * theRho;

    // Check for too small step
    if (aNextAlpha < THE_EPSILON || k + 1 == theMaxIter)
    {
      break;
    }
    aResult.Alpha = aNextAlpha;
  }

  // Failed to satisfy Armijo condition
  aResult.Status = MathUtils::Status::MaxIterations;
  return aResult;
}

//! Strong Wolfe line search.
//! Finds alpha satisfying both:
//! 1. Armijo: f(x + alpha*d) <= f(x) + c1*alpha*grad(f).d
//! 2. Curvature: |grad(f)(x + alpha*d).d| <= c2*|grad(f).d|
//!
//! Uses bracketing and zoom procedure for guaranteed convergence.
//!
//! @tparam Function type with:
//!   - Value(const math_Vector&, double&) for function value
//!   - Gradient(const math_Vector&, math_Vector&) for gradient
//! @param theFunc objective function with gradient
//! @param theX current point
//! @param theDir search direction
//! @param theGrad gradient at current point
//! @param theFx function value at current point
//! @param theAlphaInit initial step size
//! @param theC1 Armijo parameter (default 1e-4)
//! @param theC2 curvature parameter (default 0.9)
//! @param theMaxIter maximum iterations
//! @return line search result
template <typename Function>
LineSearchResult WolfeSearch(Function&          theFunc,
                             const math_Vector& theX,
                             const math_Vector& theDir,
                             const math_Vector& theGrad,
                             double             theFx,
                             double             theAlphaInit = 1.0,
                             double             theC1        = 1.0e-4,
                             double             theC2        = 0.9,
                             uint32_t           theMaxIter   = 20)
{
  LineSearchResult aResult;
  if (theDir.Size() != theX.Size() || theGrad.Size() != theX.Size() || !std::isfinite(theFx)
      || !std::isfinite(theAlphaInit) || theAlphaInit <= 0.0 || !std::isfinite(theC1)
      || !std::isfinite(theC2) || theC1 <= 0.0 || theC1 >= theC2 || theC2 >= 1.0 || theMaxIter <= 0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  double aPhi0Prime = 0.0;
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    const double aDir  = theDir.At(i);
    const double aGrad = theGrad.At(i);
    if (!std::isfinite(theX.At(i)) || !std::isfinite(aDir) || !std::isfinite(aGrad))
    {
      aResult.Status = MathUtils::Status::InvalidInput;
      return aResult;
    }
    aPhi0Prime += aGrad * aDir;
  }
  if (!std::isfinite(aPhi0Prime) || aPhi0Prime >= 0.0)
  {
    aResult.Status = std::isfinite(aPhi0Prime) ? MathUtils::Status::NonDescentDirection
                                               : MathUtils::Status::NumericalError;
    return aResult;
  }

  math_Vector aXNew(theX.Size());
  math_Vector aGradNew(theGrad.Size());
  auto        anEvaluateValue = [&](double theAlpha, double& thePhi) {
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      aXNew.ChangeAt(i) = theX.At(i) + theAlpha * theDir.At(i);
      if (!std::isfinite(aXNew.At(i)))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return false;
      }
    }
    ++aResult.NbEvals;
    if (!theFunc.Value(aXNew, thePhi))
    {
      aResult.Status = MathUtils::Status::CallbackError;
      return false;
    }
    aResult.Alpha = theAlpha;
    aResult.FNew  = thePhi;
    if (!std::isfinite(thePhi))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return false;
    }
    return true;
  };
  auto anEvaluateDerivative = [&](double& theDerivative) {
    ++aResult.NbEvals;
    if (!theFunc.Gradient(aXNew, aGradNew))
    {
      aResult.Status = MathUtils::Status::CallbackError;
      return false;
    }
    theDerivative = 0.0;
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      const double aGrad = aGradNew.At(i);
      if (!std::isfinite(aGrad))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return false;
      }
      theDerivative += aGrad * theDir.At(i);
    }
    if (!std::isfinite(theDerivative))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return false;
    }
    return true;
  };

  double aAlphaPrev = 0.0;
  double aPhiPrev   = theFx;
  double aAlpha     = theAlphaInit;
  double aAlphaLo   = 0.0;
  double aAlphaHi   = 0.0;
  double aPhiLo     = theFx;
  bool   hasBracket = false;

  for (uint32_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    double aPhi = 0.0;
    if (!anEvaluateValue(aAlpha, aPhi))
    {
      return aResult;
    }
    if (aPhi > theFx + theC1 * aAlpha * aPhi0Prime || (anIter > 0 && aPhi >= aPhiPrev))
    {
      aAlphaLo   = aAlphaPrev;
      aPhiLo     = aPhiPrev;
      aAlphaHi   = aAlpha;
      hasBracket = true;
      break;
    }
    double aPhiPrime = 0.0;
    if (!anEvaluateDerivative(aPhiPrime))
    {
      return aResult;
    }
    if (std::abs(aPhiPrime) <= -theC2 * aPhi0Prime)
    {
      aResult.Status  = MathUtils::Status::OK;
      aResult.IsValid = true;
      return aResult;
    }
    if (aPhiPrime >= 0.0)
    {
      aAlphaLo   = aAlpha;
      aPhiLo     = aPhi;
      aAlphaHi   = aAlphaPrev;
      hasBracket = true;
      break;
    }
    aAlphaPrev = aAlpha;
    aPhiPrev   = aPhi;
    aAlpha *= 2.0;
    if (!std::isfinite(aAlpha))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
  }
  if (!hasBracket)
  {
    aResult.Status = MathUtils::Status::MaxIterations;
    return aResult;
  }

  for (uint32_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    aAlpha = 0.5 * aAlphaLo + 0.5 * aAlphaHi;
    if (aAlpha == aAlphaLo || aAlpha == aAlphaHi)
    {
      aResult.Status = MathUtils::Status::NotConverged;
      return aResult;
    }
    double aPhi = 0.0;
    if (!anEvaluateValue(aAlpha, aPhi))
    {
      return aResult;
    }
    if (aPhi > theFx + theC1 * aAlpha * aPhi0Prime || aPhi >= aPhiLo)
    {
      aAlphaHi = aAlpha;
      continue;
    }
    double aPhiPrime = 0.0;
    if (!anEvaluateDerivative(aPhiPrime))
    {
      return aResult;
    }
    if (std::abs(aPhiPrime) <= -theC2 * aPhi0Prime)
    {
      aResult.Status  = MathUtils::Status::OK;
      aResult.IsValid = true;
      return aResult;
    }
    if (aPhiPrime * (aAlphaHi - aAlphaLo) >= 0.0)
    {
      aAlphaHi = aAlphaLo;
    }
    aAlphaLo = aAlpha;
    aPhiLo   = aPhi;
  }
  aResult.Status = MathUtils::Status::MaxIterations;
  return aResult;
}

//! Exact line search using Brent's method over a bounded step interval.
//! Minimizes f(x + alpha*d) for alpha in [-theAlphaMax, theAlphaMax].
//! More expensive than inexact line search but can be more robust.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc objective function
//! @param theX current point
//! @param theDir search direction
//! @param theAlphaMax maximum absolute step size
//! @param theTolerance convergence tolerance
//! @param theMaxIter maximum iterations
//! @return line search result
template <typename Function>
LineSearchResult ExactLineSearch(Function&          theFunc,
                                 const math_Vector& theX,
                                 const math_Vector& theDir,
                                 double             theAlphaMax  = THE_EXACT_LINE_SEARCH_ALPHA_MAX,
                                 double             theTolerance = 1.0e-6,
                                 uint32_t           theMaxIter   = 100)
{
  LineSearchResult aResult;
  aResult.NbEvals = 0;

  if (theDir.Size() != theX.Size() || !std::isfinite(theAlphaMax) || theAlphaMax <= 0.0
      || !std::isfinite(theTolerance) || theTolerance <= 0.0 || theMaxIter <= 0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    if (!std::isfinite(theX.At(i)) || !std::isfinite(theDir.At(i)))
    {
      aResult.Status = MathUtils::Status::InvalidInput;
      return aResult;
    }
  }

  math_Vector aXNew(theX.Size());

  double aBestAlpha = 0.0;
  double aBestF     = std::numeric_limits<double>::quiet_NaN();

  // Evaluate phi(alpha) = f(x + alpha*d), retaining the best successful evaluation.
  auto aEvalPhi = [&](double theAlpha, double& thePhi) -> bool {
    if (!std::isfinite(theAlpha))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return false;
    }
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      aXNew.ChangeAt(i) = theX.At(i) + theAlpha * theDir.At(i);
      if (!std::isfinite(aXNew.At(i)))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return false;
      }
    }
    ++aResult.NbEvals;
    if (!theFunc.Value(aXNew, thePhi))
    {
      aResult.Status = MathUtils::Status::CallbackError;
      return false;
    }
    if (!std::isfinite(thePhi))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return false;
    }
    if (!std::isfinite(aBestF) || thePhi < aBestF)
    {
      aBestAlpha    = theAlpha;
      aBestF        = thePhi;
      aResult.Alpha = aBestAlpha;
      aResult.FNew  = aBestF;
    }
    return true;
  };

  double aA = -theAlphaMax;
  double aB = theAlphaMax;
  double aX = 0.0;
  double aW = aX;
  double aV = aX;

  double aFx = 0.0;
  if (!aEvalPhi(aX, aFx))
  {
    return aResult;
  }
  double aFw = aFx;
  double aFv = aFx;

  double aD = 0.0;
  double aE = 0.0;

  for (uint32_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    const double aXm   = 0.5 * aA + 0.5 * aB;
    const double aTol1 = theTolerance * std::abs(aX) + THE_ZERO_TOL / 10.0;
    const double aTol2 = 2.0 * aTol1;
    if (!std::isfinite(aTol1) || !std::isfinite(aTol2))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }

    // Check convergence
    if (std::abs(aX - aXm) <= (aTol2 - 0.5 * (aB - aA)))
    {
      aResult.IsValid = true;
      aResult.Status  = MathUtils::Status::OK;
      aResult.Alpha   = aX;
      aResult.FNew    = aFx;
      return aResult;
    }

    double aU            = 0.0;
    bool   aUseParabolic = false;

    // Try parabolic interpolation
    if (std::abs(aE) > aTol1)
    {
      const double aR = (aX - aW) * (aFx - aFv);
      double       aQ = (aX - aV) * (aFx - aFw);
      double       aP = (aX - aV) * aQ - (aX - aW) * aR;
      aQ              = 2.0 * (aQ - aR);

      if (aQ > 0.0)
      {
        aP = -aP;
      }
      else
      {
        aQ = -aQ;
      }

      const double aETmp = aE;
      aE                 = aD;

      if (std::isfinite(aP) && std::isfinite(aQ) && std::abs(aP) < std::abs(0.5 * aQ * aETmp)
          && aP > aQ * (aA - aX) && aP < aQ * (aB - aX))
      {
        aD = aP / aQ;
        aU = aX + aD;
        if ((aU - aA) < aTol2 || (aB - aU) < aTol2)
        {
          aD = SignTransfer(aTol1, aXm - aX);
        }
        aUseParabolic = true;
      }
    }

    if (!aUseParabolic)
    {
      aE = (aX < aXm) ? (aB - aX) : (aA - aX);
      aD = THE_GOLDEN_SECTION * aE;
    }

    if (std::abs(aD) >= aTol1)
    {
      aU = aX + aD;
    }
    else
    {
      aU = aX + SignTransfer(aTol1, aD);
    }

    if (!std::isfinite(aU))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }

    double aFu = 0.0;
    if (!aEvalPhi(aU, aFu))
    {
      aResult.IsValid = false;
      return aResult;
    }

    // Update bracket
    if (aFu <= aFx)
    {
      if (aU < aX)
      {
        aB = aX;
      }
      else
      {
        aA = aX;
      }

      aV  = aW;
      aW  = aX;
      aX  = aU;
      aFv = aFw;
      aFw = aFx;
      aFx = aFu;
    }
    else
    {
      if (aU < aX)
      {
        aA = aU;
      }
      else
      {
        aB = aU;
      }

      if (aFu <= aFw || aW == aX)
      {
        aV  = aW;
        aW  = aU;
        aFv = aFw;
        aFw = aFu;
      }
      else if (aFu <= aFv || aV == aX || aV == aW)
      {
        aV  = aU;
        aFv = aFu;
      }
    }
  }

  aResult.Status = MathUtils::Status::MaxIterations;
  return aResult;
}

//! Quadratic interpolation step for line search.
//! Given phi(0), phi'(0), and phi(alpha1), finds minimum of quadratic fit.
//!
//! @param thePhi0 function value at 0
//! @param thePhi0Prime directional derivative at 0
//! @param theAlpha1 current step size
//! @param thePhi1 function value at alpha1
//! @return interpolated step size
inline double QuadraticInterpolation(double thePhi0,
                                     double thePhi0Prime,
                                     double theAlpha1,
                                     double thePhi1)
{
  // Quadratic: phi(alpha) = phi(0) + phi'(0)*alpha + c*alpha^2
  // where c = (phi(alpha1) - phi(0) - phi'(0)*alpha1) / alpha1^2
  // Minimum at alpha* = -phi'(0) / (2c)
  const double aNum   = thePhi0Prime * theAlpha1 * theAlpha1;
  const double aDenom = 2.0 * (thePhi1 - thePhi0 - thePhi0Prime * theAlpha1);

  if (std::abs(aDenom) < THE_ZERO_TOL)
  {
    return 0.5 * theAlpha1;
  }

  double aAlphaNew = -aNum / aDenom;

  // Safeguard: ensure we stay within reasonable bounds
  if (aAlphaNew < 0.1 * theAlpha1)
  {
    aAlphaNew = 0.1 * theAlpha1;
  }
  else if (aAlphaNew > 0.9 * theAlpha1)
  {
    aAlphaNew = 0.5 * theAlpha1;
  }

  return aAlphaNew;
}

//! Brent's method for 1D minimization along a single coordinate axis.
//! Operates in-place on one coordinate of thePoint, avoiding vector allocations.
//! Unlike ExactLineSearch which works with arbitrary direction vectors and allocates
//! internal temporaries, this function modifies only thePoint.ChangeAt(theDimIdx) during the
//! search and restores it if no improvement is found.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param thePoint current point (the active coordinate is modified during search,
//!                 restored if no improvement)
//! @param theDimIdx index of the coordinate to optimize
//! @param theLoBound lower bound for this coordinate
//! @param theUpBound upper bound for this coordinate
//! @param theFx current function value at thePoint (updated if improved)
//! @param theTolerance convergence tolerance
//! @param theMaxIter maximum iterations
//! @param theEvalCount incremented by the number of function evaluations used
//! @return true if an improvement was found; thePoint and theFx are updated accordingly
template <typename Function>
bool BrentAlongCoordinate(Function&    theFunc,
                          math_Vector& thePoint,
                          size_t       theDimIdx,
                          double       theLoBound,
                          double       theUpBound,
                          double&      theFx,
                          double       theTolerance,
                          uint32_t     theMaxIter,
                          size_t&      theEvalCount)
{
  if (theDimIdx >= thePoint.Size() || !std::isfinite(theLoBound) || !std::isfinite(theUpBound)
      || theLoBound > theUpBound || !std::isfinite(theTolerance) || theTolerance <= 0.0
      || theMaxIter <= 0)
  {
    return false;
  }
  const double aOrigCoord = thePoint.At(theDimIdx);
  if (!std::isfinite(aOrigCoord) || aOrigCoord < theLoBound || aOrigCoord > theUpBound
      || !std::isfinite(theFx))
  {
    return false;
  }

  // Search interval [aA, aB] in coordinate space (not alpha space)
  double aA = theLoBound;
  double aB = theUpBound;
  double aX = aOrigCoord;
  double aW = aX;
  double aV = aX;

  double aFx = theFx;
  double aFw = aFx;
  double aFv = aFx;

  double aD = 0.0;
  double aE = 0.0;

  for (uint32_t anIter = 0; anIter < theMaxIter; ++anIter)
  {
    const double aXm   = 0.5 * (aA + aB);
    const double aTol1 = theTolerance * std::abs(aX) + THE_ZERO_TOL / 10.0;
    const double aTol2 = 2.0 * aTol1;

    // Check convergence
    if (std::abs(aX - aXm) <= (aTol2 - 0.5 * (aB - aA)))
    {
      break;
    }

    double aU            = 0.0;
    bool   aUseParabolic = false;

    // Try parabolic interpolation
    if (std::abs(aE) > aTol1)
    {
      const double aR = (aX - aW) * (aFx - aFv);
      double       aQ = (aX - aV) * (aFx - aFw);
      double       aP = (aX - aV) * aQ - (aX - aW) * aR;
      aQ              = 2.0 * (aQ - aR);

      if (aQ > 0.0)
      {
        aP = -aP;
      }
      else
      {
        aQ = -aQ;
      }

      const double aETmp = aE;
      aE                 = aD;

      if (std::abs(aP) < std::abs(0.5 * aQ * aETmp) && aP > aQ * (aA - aX) && aP < aQ * (aB - aX))
      {
        aD = aP / aQ;
        aU = aX + aD;
        if ((aU - aA) < aTol2 || (aB - aU) < aTol2)
        {
          aD = SignTransfer(aTol1, aXm - aX);
        }
        aUseParabolic = true;
      }
    }

    if (!aUseParabolic)
    {
      aE = (aX < aXm) ? (aB - aX) : (aA - aX);
      aD = THE_GOLDEN_SECTION * aE;
    }

    if (std::abs(aD) >= aTol1)
    {
      aU = aX + aD;
    }
    else
    {
      aU = aX + SignTransfer(aTol1, aD);
    }

    // Evaluate: only modify the single coordinate
    thePoint.ChangeAt(theDimIdx) = aU;
    double aFu                   = 0.0;
    if (!theFunc.Value(thePoint, aFu) || !std::isfinite(aFu))
    {
      // Restore and abort
      thePoint.ChangeAt(theDimIdx) = aOrigCoord;
      return false;
    }
    ++theEvalCount;

    // Update bracket
    if (aFu <= aFx)
    {
      if (aU < aX)
      {
        aB = aX;
      }
      else
      {
        aA = aX;
      }
      aV  = aW;
      aW  = aX;
      aX  = aU;
      aFv = aFw;
      aFw = aFx;
      aFx = aFu;
    }
    else
    {
      if (aU < aX)
      {
        aA = aU;
      }
      else
      {
        aB = aU;
      }
      if (aFu <= aFw || aW == aX)
      {
        aV  = aW;
        aW  = aU;
        aFv = aFw;
        aFw = aFu;
      }
      else if (aFu <= aFv || aV == aX || aV == aW)
      {
        aV  = aU;
        aFv = aFu;
      }
    }
  }

  // Accept if improved
  if (aFx < theFx)
  {
    thePoint.ChangeAt(theDimIdx) = aX;
    theFx                        = aFx;
    return true;
  }

  // Restore original coordinate
  thePoint.ChangeAt(theDimIdx) = aOrigCoord;
  return false;
}

} // namespace MathUtils

#endif // _MathUtils_LineSearch_HeaderFile
