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

#ifndef _MathOpt_Brent_HeaderFile
#define _MathOpt_Brent_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_Bracket.hxx>

#include <cmath>

//! Optimization algorithms for scalar and vector functions.
namespace MathOpt
{
using namespace MathUtils;

//! Brent's method for 1D minimization.
//! Combines golden section search with parabolic interpolation.
//! Guaranteed to converge for unimodal functions within the given interval.
//!
//! Algorithm:
//! 1. Maintain bracket [a, b] with interior point x where f(x) < f(a), f(x) < f(b)
//! 2. Try parabolic interpolation using three points
//! 3. If parabolic step is rejected, use golden section step
//! 4. Update bracket and repeat until convergence
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to minimize
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
ScalarResult Brent(Function&     theFunc,
                   double        theLower,
                   double        theUpper,
                   const Config& theConfig = Config())
{
  ScalarResult aResult;

  double aA = theLower;
  double aB = theUpper;

  // Initial point using golden section
  double aX = aA + MathUtils::THE_GOLDEN_SECTION * (aB - aA);
  double aW = aX;
  double aV = aX;

  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  double aFw = aFx;
  double aFv = aFx;

  double aD = 0.0; // Current step
  double aE = 0.0; // Previous step

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    const double aXm   = 0.5 * (aA + aB);
    const double aTol1 = theConfig.XTolerance * std::abs(aX) + MathUtils::THE_ZERO_TOL / 10.0;
    const double aTol2 = 2.0 * aTol1;

    aResult.NbIterations = anIter + 1;

    // Check convergence
    if (std::abs(aX - aXm) <= (aTol2 - 0.5 * (aB - aA)))
    {
      aResult.Status = Status::OK;
      aResult.Root   = aX;
      aResult.Value  = aFx;
      return aResult;
    }

    double aU            = 0.0;
    bool   aUseParabolic = false;

    // Try parabolic interpolation if step is large enough
    if (std::abs(aE) > aTol1)
    {
      // Parabolic fit through x, w, v
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

      // Check if parabolic step is acceptable
      if (std::abs(aP) < std::abs(0.5 * aQ * aETmp) && aP > aQ * (aA - aX) && aP < aQ * (aB - aX))
      {
        aD = aP / aQ;
        aU = aX + aD;

        // Don't evaluate too close to bounds
        if ((aU - aA) < aTol2 || (aB - aU) < aTol2)
        {
          aD = MathUtils::SignTransfer(aTol1, aXm - aX);
        }
        aUseParabolic = true;
      }
    }

    if (!aUseParabolic)
    {
      // Golden section step
      aE = (aX < aXm) ? (aB - aX) : (aA - aX);
      aD = MathUtils::THE_GOLDEN_SECTION * aE;
    }

    // Ensure step is at least aTol1
    if (std::abs(aD) >= aTol1)
    {
      aU = aX + aD;
    }
    else
    {
      aU = aX + MathUtils::SignTransfer(aTol1, aD);
    }

    double aFu = 0.0;
    if (!theFunc.Value(aU, aFu))
    {
      aResult.Status = Status::NumericalError;
      aResult.Root   = aX;
      aResult.Value  = aFx;
      return aResult;
    }

    // Update bracket and best points
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

  // Maximum iterations reached
  aResult.Status = Status::MaxIterations;
  aResult.Root   = aX;
  aResult.Value  = aFx;
  return aResult;
}

//! Golden section search for 1D minimization.
//! Simpler than Brent but with guaranteed linear convergence.
//! Does not attempt parabolic interpolation.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to minimize
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
ScalarResult Golden(Function&     theFunc,
                    double        theLower,
                    double        theUpper,
                    const Config& theConfig = Config())
{
  ScalarResult aResult;

  constexpr double aR = 0.618033988749895; // (sqrt(5) - 1) / 2
  constexpr double aC = 1.0 - aR;

  double aA = theLower;
  double aB = theUpper;

  // Initialize interior points
  double aX1 = aA + aC * (aB - aA);
  double aX2 = aA + aR * (aB - aA);

  double aF1 = 0.0;
  double aF2 = 0.0;

  if (!theFunc.Value(aX1, aF1))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }
  if (!theFunc.Value(aX2, aF2))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check convergence
    if ((aB - aA) < theConfig.XTolerance * (std::abs(aX1) + std::abs(aX2)))
    {
      aResult.Status = Status::OK;
      if (aF1 < aF2)
      {
        aResult.Root  = aX1;
        aResult.Value = aF1;
      }
      else
      {
        aResult.Root  = aX2;
        aResult.Value = aF2;
      }
      return aResult;
    }

    if (aF1 < aF2)
    {
      // Minimum is in [a, x2]
      aB  = aX2;
      aX2 = aX1;
      aF2 = aF1;
      aX1 = aA + aC * (aB - aA);
      if (!theFunc.Value(aX1, aF1))
      {
        aResult.Status = Status::NumericalError;
        aResult.Root   = aX2;
        aResult.Value  = aF2;
        return aResult;
      }
    }
    else
    {
      // Minimum is in [x1, b]
      aA  = aX1;
      aX1 = aX2;
      aF1 = aF2;
      aX2 = aA + aR * (aB - aA);
      if (!theFunc.Value(aX2, aF2))
      {
        aResult.Status = Status::NumericalError;
        aResult.Root   = aX1;
        aResult.Value  = aF1;
        return aResult;
      }
    }
  }

  // Maximum iterations reached
  aResult.Status = Status::MaxIterations;
  if (aF1 < aF2)
  {
    aResult.Root  = aX1;
    aResult.Value = aF1;
  }
  else
  {
    aResult.Root  = aX2;
    aResult.Value = aF2;
  }
  return aResult;
}

//! Brent's method with automatic bracket search.
//! First attempts to bracket a minimum, then applies Brent's method.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to minimize
//! @param theGuess initial guess
//! @param theStep initial step size for bracket search
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
ScalarResult BrentWithBracket(Function&     theFunc,
                              double        theGuess,
                              double        theStep   = 1.0,
                              const Config& theConfig = Config())
{
  ScalarResult aResult;

  // Try to bracket minimum
  MathUtils::MinBracketResult aBracket =
    MathUtils::BracketMinimum(theFunc, theGuess, theGuess + theStep);

  if (!aBracket.IsValid)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Apply Brent's method on the bracket
  return Brent(theFunc, aBracket.A, aBracket.C, theConfig);
}

} // namespace MathOpt

#endif // _MathOpt_Brent_HeaderFile
