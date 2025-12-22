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

#ifndef _MathRoot_Bisection_HeaderFile
#define _MathRoot_Bisection_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <utility>

namespace MathRoot
{
using namespace MathUtils;

//! Bisection method for root finding.
//! Simple and robust, guaranteed to converge if a valid bracket is provided.
//! Converges linearly, halving the interval at each step.
//!
//! Algorithm:
//! 1. Start with bracket [a, b] where f(a) * f(b) < 0
//! 2. Compute midpoint m = (a + b) / 2
//! 3. If f(m) has same sign as f(a), set a = m, else set b = m
//! 4. Repeat until convergence
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to find root of
//! @param theLower lower bound of bracket
//! @param theUpper upper bound of bracket
//! @param theConfig solver configuration
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult Bisection(Function&                theFunc,
                                  double                   theLower,
                                  double                   theUpper,
                                  const MathUtils::Config& theConfig = MathUtils::Config())
{
  MathUtils::ScalarResult aResult;

  double aA  = theLower;
  double aB  = theUpper;
  double aFa = 0.0;
  double aFb = 0.0;

  // Evaluate at endpoints
  if (!theFunc.Value(aA, aFa))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  if (!theFunc.Value(aB, aFb))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  // Check that bracket is valid (sign change)
  if (aFa * aFb > 0.0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  // Ensure a < b
  if (aA > aB)
  {
    std::swap(aA, aB);
    std::swap(aFa, aFb);
  }

  // Track which endpoint has the negative value
  // We use separate tracking instead of swapping to maintain aA < aB
  bool aIsNegAtA = (aFa < 0.0);

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    // Compute midpoint
    const double aM  = 0.5 * (aA + aB);
    double       aFm = 0.0;

    if (!theFunc.Value(aM, aFm))
    {
      aResult.Status       = MathUtils::Status::NumericalError;
      aResult.Root         = aM;
      aResult.NbIterations = anIter + 1;
      return aResult;
    }

    aResult.NbIterations = anIter + 1;

    // Check convergence on function value
    if (std::abs(aFm) < theConfig.FTolerance)
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aM;
      aResult.Value  = aFm;
      return aResult;
    }

    // Check convergence on interval size
    if ((aB - aA) < theConfig.XTolerance * std::max(1.0, std::abs(aM)))
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aM;
      aResult.Value  = aFm;
      return aResult;
    }

    // Update bracket based on sign tracking
    const bool aIsNegAtM = (aFm < 0.0);
    if (aIsNegAtM == aIsNegAtA)
    {
      // Same sign as endpoint A, replace A
      aA  = aM;
      aFa = aFm;
    }
    else
    {
      // Same sign as endpoint B, replace B
      aB  = aM;
      aFb = aFm;
    }
  }

  // Maximum iterations reached
  aResult.Status = MathUtils::Status::MaxIterations;
  aResult.Root   = 0.5 * (aA + aB);
  aResult.Value  = 0.0;
  theFunc.Value(*aResult.Root, *aResult.Value);
  return aResult;
}

//! Hybrid bisection-Newton method.
//! Combines the robustness of bisection with the speed of Newton's method.
//! Uses Newton step when it stays within bracket, otherwise bisects.
//!
//! @tparam Function type with Values(double theX, double& theF, double& theDf) method
//! @param theFunc function with value and derivative
//! @param theLower lower bound of bracket
//! @param theUpper upper bound of bracket
//! @param theConfig solver configuration
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult BisectionNewton(Function&                theFunc,
                                        double                   theLower,
                                        double                   theUpper,
                                        const MathUtils::Config& theConfig = MathUtils::Config())
{
  MathUtils::ScalarResult aResult;

  double aA     = theLower;
  double aB     = theUpper;
  double aFa    = 0.0;
  double aFb    = 0.0;
  double aDummy = 0.0;

  // Evaluate at endpoints
  if (!theFunc.Values(aA, aFa, aDummy))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  if (!theFunc.Values(aB, aFb, aDummy))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  // Check that bracket is valid
  if (aFa * aFb > 0.0)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  // Ensure a < b
  if (aA > aB)
  {
    std::swap(aA, aB);
    std::swap(aFa, aFb);
  }

  // Track which endpoint has the negative value
  bool aIsNegAtA = (aFa < 0.0);

  // Start from midpoint
  double aX   = 0.5 * (aA + aB);
  double aFx  = 0.0;
  double aDfx = 0.0;

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    if (!theFunc.Values(aX, aFx, aDfx))
    {
      aResult.Status       = MathUtils::Status::NumericalError;
      aResult.Root         = aX;
      aResult.NbIterations = anIter + 1;
      return aResult;
    }

    aResult.NbIterations = anIter + 1;

    // Check convergence
    if (std::abs(aFx) < theConfig.FTolerance)
    {
      aResult.Status     = MathUtils::Status::OK;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    if ((aB - aA) < theConfig.XTolerance * std::max(1.0, std::abs(aX)))
    {
      aResult.Status     = MathUtils::Status::OK;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    // Try Newton step
    double aXNew = aX;
    if (!MathUtils::IsZero(aDfx))
    {
      aXNew = aX - aFx / aDfx;
    }

    // Check if Newton step stays within bracket
    if (aXNew <= aA || aXNew >= aB)
    {
      // Fall back to bisection
      aXNew = 0.5 * (aA + aB);
    }

    // Update bracket based on current point using sign tracking
    const bool aIsNegAtX = (aFx < 0.0);
    if (aIsNegAtX == aIsNegAtA)
    {
      // Same sign as endpoint A, replace A
      aA = aX;
    }
    else
    {
      // Same sign as endpoint B, replace B
      aB = aX;
    }

    aX = aXNew;
  }

  // Maximum iterations reached
  aResult.Status     = MathUtils::Status::MaxIterations;
  aResult.Root       = aX;
  aResult.Value      = 0.0;
  aResult.Derivative = 0.0;
  theFunc.Values(*aResult.Root, *aResult.Value, *aResult.Derivative);
  return aResult;
}

} // namespace MathRoot

#endif // _MathRoot_Bisection_HeaderFile
