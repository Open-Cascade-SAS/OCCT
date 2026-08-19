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
#include "MathRoot_Utils.hxx"

#include <cmath>

namespace MathRoot
{
using namespace MathUtils;

//! Bisection method for root finding.
//! Simple and robust for a continuous function with a valid bracket.
//! Converges linearly, halving the interval at each step.
//! Convergence is accepted on either residual or bracket width.
//!
//! Algorithm:
//! 1. Start with an ordered bracket [a, b] whose endpoint values have opposite signs
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

  if (!Utils::IsValidBounds(theLower, theUpper) || !Utils::IsValidConfig(theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  double aA  = theLower;
  double aB  = theUpper;
  double aFa = 0.0;
  double aFb = 0.0;

  // Evaluate at endpoints
  if (!theFunc.Value(aA, aFa))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!std::isfinite(aFa))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  if (!theFunc.Value(aB, aFb))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!std::isfinite(aFb))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  if (std::abs(aFa) <= theConfig.FTolerance)
  {
    aResult.Status = MathUtils::Status::OK;
    aResult.Root   = aA;
    aResult.Value  = aFa;
    return aResult;
  }
  if (std::abs(aFb) <= theConfig.FTolerance)
  {
    aResult.Status = MathUtils::Status::OK;
    aResult.Root   = aB;
    aResult.Value  = aFb;
    return aResult;
  }

  if (!Utils::HaveOppositeSigns(aFa, aFb))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    const double aM  = Utils::Midpoint(aA, aB);
    double       aFm = 0.0;

    if (!theFunc.Value(aM, aFm))
    {
      aResult.Status       = MathUtils::Status::CallbackError;
      aResult.Root         = aM;
      aResult.NbIterations = anIter + 1;
      return aResult;
    }
    if (!std::isfinite(aFm))
    {
      aResult.Status       = MathUtils::Status::NumericalError;
      aResult.Root         = aM;
      aResult.NbIterations = anIter + 1;
      return aResult;
    }

    aResult.NbIterations = anIter + 1;

    // Check convergence on function value
    if (std::abs(aFm) <= theConfig.FTolerance)
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aM;
      aResult.Value  = aFm;
      return aResult;
    }

    if (0.5 * std::abs(aB - aA) <= theConfig.XTolerance)
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aM;
      aResult.Value  = aFm;
      return aResult;
    }

    if (aM == aA || aM == aB)
    {
      aResult.Status = MathUtils::Status::NotConverged;
      aResult.Root   = aM;
      aResult.Value  = aFm;
      return aResult;
    }

    if (Utils::HaveOppositeSigns(aFa, aFm))
    {
      aB  = aM;
      aFb = aFm;
    }
    else
    {
      aA  = aM;
      aFa = aFm;
    }
  }

  // Maximum iterations reached
  aResult.Status = MathUtils::Status::MaxIterations;
  aResult.Root   = (std::abs(aFa) <= std::abs(aFb)) ? aA : aB;
  aResult.Value  = (std::abs(aFa) <= std::abs(aFb)) ? aFa : aFb;
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

  if (!Utils::IsValidBounds(theLower, theUpper) || !Utils::IsValidConfig(theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  double aA     = theLower;
  double aB     = theUpper;
  double aFa    = 0.0;
  double aFb    = 0.0;
  double aDummy = 0.0;

  // Evaluate at endpoints
  if (!theFunc.Values(aA, aFa, aDummy))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!std::isfinite(aFa) || !std::isfinite(aDummy))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  double aDb = 0.0;
  if (!theFunc.Values(aB, aFb, aDb))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!std::isfinite(aFb) || !std::isfinite(aDb))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  if (std::abs(aFa) <= theConfig.FTolerance)
  {
    aResult.Status     = MathUtils::Status::OK;
    aResult.Root       = aA;
    aResult.Value      = aFa;
    aResult.Derivative = aDummy;
    return aResult;
  }
  if (std::abs(aFb) <= theConfig.FTolerance)
  {
    aResult.Status     = MathUtils::Status::OK;
    aResult.Root       = aB;
    aResult.Value      = aFb;
    aResult.Derivative = aDb;
    return aResult;
  }

  if (!Utils::HaveOppositeSigns(aFa, aFb))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  // Start from midpoint
  double aX   = Utils::Midpoint(aA, aB);
  double aFx  = 0.0;
  double aDfx = 0.0;

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    if (!theFunc.Values(aX, aFx, aDfx))
    {
      aResult.Status       = MathUtils::Status::CallbackError;
      aResult.Root         = aX;
      aResult.NbIterations = anIter + 1;
      return aResult;
    }
    if (!std::isfinite(aFx) || !std::isfinite(aDfx))
    {
      aResult.Status       = MathUtils::Status::NumericalError;
      aResult.Root         = aX;
      aResult.NbIterations = anIter + 1;
      return aResult;
    }

    aResult.NbIterations = anIter + 1;

    // Check convergence
    if (std::abs(aFx) <= theConfig.FTolerance)
    {
      aResult.Status     = MathUtils::Status::OK;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    if (Utils::HaveOppositeSigns(aFa, aFx))
    {
      aB  = aX;
      aFb = aFx;
    }
    else
    {
      aA  = aX;
      aFa = aFx;
    }

    // A bracketed method may converge by localization even when no floating-point point has an
    // exact residual, but a stalled Newton step alone is not evidence of a root.
    if (0.5 * std::abs(aB - aA) <= theConfig.XTolerance)
    {
      aResult.Status     = MathUtils::Status::OK;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    // Try Newton step
    const double aMidpoint = Utils::Midpoint(aA, aB);
    double       aXNew     = aMidpoint;
    if (aDfx != 0.0)
    {
      const double aNewtonX = aX - aFx / aDfx;
      if (std::isfinite(aNewtonX) && aNewtonX > aA && aNewtonX < aB
          && std::abs(aNewtonX - aX) > theConfig.XTolerance)
      {
        aXNew = aNewtonX;
      }
    }

    if (aXNew == aX || aXNew == aA || aXNew == aB)
    {
      aResult.Status     = MathUtils::Status::NotConverged;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    if (anIter + 1 < theConfig.MaxIterations)
    {
      aX = aXNew;
    }
  }

  // Maximum iterations reached
  aResult.Status     = MathUtils::Status::MaxIterations;
  aResult.Root       = aX;
  aResult.Value      = aFx;
  aResult.Derivative = aDfx;
  return aResult;
}

} // namespace MathRoot

#endif // _MathRoot_Bisection_HeaderFile
