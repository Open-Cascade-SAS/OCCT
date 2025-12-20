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

#ifndef _MathRoot_Secant_HeaderFile
#define _MathRoot_Secant_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_Convergence.hxx>

#include <cmath>

namespace MathRoot
{
using namespace MathUtils;

//! Secant method for root finding.
//! Does not require derivative, uses finite difference approximation.
//! Converges superlinearly (order ~1.618, the golden ratio).
//!
//! Algorithm:
//! 1. Start with two initial points x0, x1
//! 2. Approximate derivative: f'(x) ~ (f(x1) - f(x0)) / (x1 - x0)
//! 3. Newton-like update: x2 = x1 - f(x1) * (x1 - x0) / (f(x1) - f(x0))
//! 4. Repeat with x0 = x1, x1 = x2
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function object providing only value
//! @param theX0 first initial point
//! @param theX1 second initial point (different from theX0)
//! @param theConfig solver configuration
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult Secant(Function&     theFunc,
                    double        theX0,
                    double        theX1,
                    const MathUtils::Config& theConfig = MathUtils::Config())
{
  MathUtils::ScalarResult aResult;

  double aX0 = theX0;
  double aX1 = theX1;
  double aF0 = 0.0;
  double aF1 = 0.0;

  // Evaluate at initial points
  if (!theFunc.Value(aX0, aF0))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }
  if (!theFunc.Value(aX1, aF1))
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Check convergence
    if (MathUtils::IsFConverged(aF1, theConfig.FTolerance))
    {
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aX1;
      aResult.Value  = aF1;
      return aResult;
    }

    // Secant step
    const double aDenom = aF1 - aF0;
    if (MathUtils::IsZero(aDenom))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      aResult.Root   = aX1;
      aResult.Value  = aF1;
      return aResult;
    }

    const double aXNew = aX1 - aF1 * (aX1 - aX0) / aDenom;

    // Check X convergence
    if (MathUtils::IsXConverged(aX1, aXNew, theConfig.XTolerance))
    {
      double aFNew = 0.0;
      theFunc.Value(aXNew, aFNew);
      aResult.Status = MathUtils::Status::OK;
      aResult.Root   = aXNew;
      aResult.Value  = aFNew;
      return aResult;
    }

    // Update for next iteration
    aX0 = aX1;
    aF0 = aF1;
    aX1 = aXNew;

    if (!theFunc.Value(aX1, aF1))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      aResult.Root   = aX1;
      return aResult;
    }
  }

  aResult.Status = MathUtils::Status::MaxIterations;
  aResult.Root   = aX1;
  aResult.Value  = aF1;
  return aResult;
}

//! Secant method with automatic initial points.
//! Creates second point by small perturbation of the initial guess.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function object providing only value
//! @param theX0 initial guess
//! @param theConfig solver configuration
//! @return result containing root location and convergence status
template <typename Function>
MathUtils::ScalarResult SecantAuto(Function&     theFunc,
                        double        theX0,
                        const MathUtils::Config& theConfig = MathUtils::Config())
{
  // Create second point by small perturbation
  const double aDelta = (std::abs(theX0) > 1.0) ? 0.01 * theX0 : 0.01;
  return Secant(theFunc, theX0, theX0 + aDelta, theConfig);
}

} // namespace MathRoot

#endif // _MathRoot_Secant_HeaderFile
