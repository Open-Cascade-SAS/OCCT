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

#ifndef _MathRoot_Brent_HeaderFile
#define _MathRoot_Brent_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <utility>

namespace MathRoot
{
using namespace MathUtils;

//! Brent's method for root finding.
//! Combines bisection, secant, and inverse quadratic interpolation.
//! Guaranteed to converge if a valid bracket is provided.
//!
//! Algorithm:
//! 1. Start with bracket [a, b] where f(a) * f(b) < 0
//! 2. At each step, try inverse quadratic interpolation
//! 3. If interpolation step is rejected, use bisection
//! 4. Acceptance criteria ensure superlinear convergence when possible
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to find root of
//! @param theLower lower bound of bracket (f(theLower) and f(theUpper) must have opposite signs)
//! @param theUpper upper bound of bracket
//! @param theConfig solver configuration
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult Brent(Function&                theFunc,
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

  // Ensure |f(a)| >= |f(b)| (b is the better approximation)
  if (std::abs(aFa) < std::abs(aFb))
  {
    std::swap(aA, aB);
    std::swap(aFa, aFb);
  }

  double aC  = aA; // Previous iterate
  double aFc = aFa;
  double aD  = aB - aA; // Step size
  double aE  = aD;      // Previous step size

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check convergence on function value
    if (std::abs(aFb) < theConfig.FTolerance)
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aB;
      aResult.Value  = aFb;
      return aResult;
    }

    // Check convergence on interval size
    if (std::abs(aB - aA) < theConfig.XTolerance * std::max(1.0, std::abs(aB)))
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aB;
      aResult.Value  = aFb;
      return aResult;
    }

    double aS = 0.0; // New approximation

    // Try inverse quadratic interpolation if we have three distinct points
    if (std::abs(aFa - aFc) > MathUtils::THE_ZERO_TOL
        && std::abs(aFb - aFc) > MathUtils::THE_ZERO_TOL)
    {
      // Inverse quadratic interpolation
      aS = aA * aFb * aFc / ((aFa - aFb) * (aFa - aFc))
           + aB * aFa * aFc / ((aFb - aFa) * (aFb - aFc))
           + aC * aFa * aFb / ((aFc - aFa) * (aFc - aFb));
    }
    else
    {
      // Secant method
      aS = aB - aFb * (aB - aA) / (aFb - aFa);
    }

    // Decide whether to accept the interpolation step
    const double aTol = 2.0 * MathUtils::THE_EPSILON * std::abs(aB) + 0.5 * theConfig.XTolerance;
    const double aM   = 0.5 * (aC - aB);

    bool aUseInterp = false;

    // Check if s is between (3a+b)/4 and b
    const double aBound1 = (3.0 * aA + aB) / 4.0;
    if ((aS > std::min(aBound1, aB) && aS < std::max(aBound1, aB)))
    {
      // Check additional conditions for accepting interpolation
      if (std::abs(aS - aB) < std::abs(aE) / 2.0 && std::abs(aS - aB) >= aTol)
      {
        aUseInterp = true;
      }
    }

    if (!aUseInterp)
    {
      // Bisection step
      aS = aB + aM;
      aE = aM;
      aD = aM;
    }
    else
    {
      aE = aD;
      aD = aS - aB;
    }

    // Update previous values
    aA  = aB;
    aFa = aFb;

    // Compute new point, ensuring minimum step
    if (std::abs(aD) > aTol)
    {
      aB = aS;
    }
    else
    {
      aB += (aM > 0.0) ? aTol : -aTol;
    }

    // Evaluate function at new point
    if (!theFunc.Value(aB, aFb))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      aResult.Root   = aB;
      return aResult;
    }

    // Update bracket
    if (aFb * aFc > 0.0)
    {
      aC  = aA;
      aFc = aFa;
      aD  = aB - aA;
      aE  = aD;
    }
    else if (std::abs(aFc) < std::abs(aFb))
    {
      // Swap b and c if c is better
      aA  = aB;
      aB  = aC;
      aC  = aA;
      aFa = aFb;
      aFb = aFc;
      aFc = aFa;
    }
  }

  // Maximum iterations reached
  aResult.Status = MathUtils::Status::MaxIterations;
  aResult.Root   = aB;
  aResult.Value  = aFb;
  return aResult;
}

} // namespace MathRoot

#endif // _MathRoot_Brent_HeaderFile
