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

#ifndef _MathRoot_Newton_HeaderFile
#define _MathRoot_Newton_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_Convergence.hxx>
#include "MathRoot_Utils.hxx"

#include <cmath>
#include <optional>

//! Root finding algorithms for scalar functions.
namespace MathRoot
{
using namespace MathUtils;

//! Newton-Raphson root finding algorithm.
//! Finds x such that f(x) = 0 using Newton's method with derivative.
//!
//! Algorithm:
//! x_{n+1} = x_n - f(x_n) / f'(x_n)
//!
//! Requires a function providing both value and derivative.
//! Converges quadratically near the root for simple roots.
//!
//! @tparam Function type with Values(double theX, double& theF, double& theDf) method
//!         returning bool (true if evaluation succeeded)
//! @param theFunc function object providing value and derivative
//! @param theGuess initial guess for the root
//! @param theConfig solver configuration (tolerances, max iterations)
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult Newton(Function&                theFunc,
                               double                   theGuess,
                               const MathUtils::Config& theConfig = MathUtils::Config())
{
  MathUtils::ScalarResult aResult;
  if (!std::isfinite(theGuess) || !Utils::IsValidConfig(theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  double                aX   = theGuess;
  double                aFx  = 0.0;
  double                aDfx = 0.0;
  std::optional<double> aDx;

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
    if (aFx == 0.0
        || (aDx.has_value() && std::abs(*aDx) <= theConfig.XTolerance
            && std::abs(aFx) <= theConfig.FTolerance))
    {
      aResult.Status     = MathUtils::Status::OK;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    if (aDfx == 0.0)
    {
      aResult.Status     = MathUtils::Status::NumericalError;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }

    aDx                = aFx / aDfx;
    const double aXNew = aX - *aDx;
    if (!std::isfinite(aXNew))
    {
      aResult.Status     = MathUtils::Status::NumericalError;
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

//! Newton-Raphson with bounds checking.
//! Falls back to bisection step if Newton step goes outside bounds.
//! More robust than pure Newton for ill-conditioned problems.
//!
//! @tparam Function type with Values(double theX, double& theF, double& theDf) method
//! @param theFunc function object providing value and derivative
//! @param theGuess initial guess for the root
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig solver configuration
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult NewtonBounded(Function&                theFunc,
                                      double                   theGuess,
                                      double                   theLower,
                                      double                   theUpper,
                                      const MathUtils::Config& theConfig = MathUtils::Config())
{
  MathUtils::ScalarResult aResult;

  if (!Utils::IsValidBounds(theLower, theUpper) || !std::isfinite(theGuess)
      || !Utils::IsValidConfig(theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  double aX   = MathUtils::Clamp(theGuess, theLower, theUpper);
  double aXLo = theLower;
  double aXHi = theUpper;

  double aFx    = 0.0;
  double aDfx   = 0.0;
  double aFLo   = 0.0;
  double aFHi   = 0.0;
  double aDummy = 0.0;

  // Initialize bounds with function values
  if (!theFunc.Values(aXLo, aFLo, aDummy))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!std::isfinite(aFLo) || !std::isfinite(aDummy))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  double aDHi = 0.0;
  if (!theFunc.Values(aXHi, aFHi, aDHi))
  {
    aResult.Status = MathUtils::Status::CallbackError;
    return aResult;
  }
  if (!std::isfinite(aFHi) || !std::isfinite(aDHi))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  if (std::abs(aFLo) <= theConfig.FTolerance)
  {
    aResult.Status     = MathUtils::Status::OK;
    aResult.Root       = aXLo;
    aResult.Value      = aFLo;
    aResult.Derivative = aDummy;
    return aResult;
  }
  if (std::abs(aFHi) <= theConfig.FTolerance)
  {
    aResult.Status     = MathUtils::Status::OK;
    aResult.Root       = aXHi;
    aResult.Value      = aFHi;
    aResult.Derivative = aDHi;
    return aResult;
  }

  const bool isBracketed = Utils::HaveOppositeSigns(aFLo, aFHi);

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

    if (isBracketed)
    {
      if (Utils::HaveOppositeSigns(aFLo, aFx))
      {
        aXHi = aX;
        aFHi = aFx;
      }
      else
      {
        aXLo = aX;
        aFLo = aFx;
      }
    }

    double aXNew = aX;
    if (aDfx != 0.0)
    {
      aXNew = aX - aFx / aDfx;
    }
    if (!std::isfinite(aXNew) || aXNew <= aXLo || aXNew >= aXHi)
    {
      if (isBracketed)
      {
        aXNew = Utils::Midpoint(aXLo, aXHi);
      }
      else
      {
        aXNew = MathUtils::Clamp(aXNew, aXLo, aXHi);
      }
    }

    if (!std::isfinite(aXNew))
    {
      aResult.Status     = MathUtils::Status::NumericalError;
      aResult.Root       = aX;
      aResult.Value      = aFx;
      aResult.Derivative = aDfx;
      return aResult;
    }
    if (std::abs(aXNew - aX) <= theConfig.XTolerance && std::abs(aFx) <= theConfig.FTolerance)
    {
      aResult.Status     = MathUtils::Status::OK;
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

#endif // _MathRoot_Newton_HeaderFile
