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
#include "MathRoot_Utils.hxx"

#include <cmath>
#include <limits>

namespace MathRoot
{
using namespace MathUtils;

//! Safeguarded Brent method for root finding.
//! Combines inverse quadratic interpolation, secant steps, and bisection safeguards.
//!
//! Algorithm:
//! 1. Start with an ordered bracket [a, b] whose endpoint values have opposite signs
//! 2. Keep the best estimate at b and the opposite side of the bracket at c
//! 3. Attempt inverse quadratic interpolation (or secant interpolation with two points)
//! 4. If interpolation is unsafe, use bisection
//! 5. Report success when either the residual or bracket is within tolerance
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

  if (!Utils::IsValidBounds(theLower, theUpper) || !Utils::IsValidConfig(theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  double aA  = theLower;
  double aB  = theUpper;
  double aC  = aB;
  double aFa = 0.0;
  double aFb = 0.0;
  double aFc = 0.0;
  double aD  = 0.0;
  double anE = 0.0;

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

  aFc = aFb;
  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    if (!Utils::HaveOppositeSigns(aFb, aFc))
    {
      aC  = aA;
      aFc = aFa;
      aD  = aB - aA;
      anE = aD;
    }
    if (std::abs(aFc) < std::abs(aFb))
    {
      aA  = aB;
      aB  = aC;
      aC  = aA;
      aFa = aFb;
      aFb = aFc;
      aFc = aFa;
    }

    const double aTol =
      2.0 * std::numeric_limits<double>::epsilon() * std::abs(aB) + 0.5 * theConfig.XTolerance;
    const double aXm = Utils::Midpoint(aC, -aB);
    if (std::abs(aXm) <= aTol || std::abs(aFb) <= theConfig.FTolerance)
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aB;
      aResult.Value  = aFb;
      return aResult;
    }

    if (std::abs(anE) >= aTol && std::abs(aFa) > std::abs(aFb))
    {
      const double aS = aFb / aFa;
      double       aP = 0.0;
      double       aQ = 0.0;
      if (aA == aC)
      {
        aP = 2.0 * aXm * aS;
        aQ = 1.0 - aS;
      }
      else
      {
        aQ              = aFa / aFc;
        const double aR = aFb / aFc;
        aP              = aS * (2.0 * aXm * aQ * (aQ - aR) - (aB - aA) * (aR - 1.0));
        aQ              = (aQ - 1.0) * (aR - 1.0) * (aS - 1.0);
      }
      if (aP > 0.0)
      {
        aQ = -aQ;
      }
      aP                 = std::abs(aP);
      const double aMin1 = 3.0 * aXm * aQ - std::abs(aTol * aQ);
      const double aMin2 = std::abs(anE * aQ);
      if (2.0 * aP < (aMin1 < aMin2 ? aMin1 : aMin2))
      {
        anE = aD;
        aD  = aP / aQ;
      }
      else
      {
        aD  = aXm;
        anE = aD;
      }
    }
    else
    {
      aD  = aXm;
      anE = aD;
    }

    aA  = aB;
    aFa = aFb;
    if (std::abs(aD) > aTol)
    {
      aB += aD;
    }
    else
    {
      aB += aXm > 0.0 ? std::abs(aTol) : -std::abs(aTol);
    }

    if (!std::isfinite(aB))
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
  }

  // Maximum iterations reached
  aResult.Status = MathUtils::Status::MaxIterations;
  aResult.Root   = aB;
  aResult.Value  = aFb;
  return aResult;
}

} // namespace MathRoot

#endif // _MathRoot_Brent_HeaderFile
