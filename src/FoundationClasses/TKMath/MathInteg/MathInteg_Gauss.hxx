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

#ifndef _MathInteg_Gauss_HeaderFile
#define _MathInteg_Gauss_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_Gauss.hxx>

#include <cmath>

//! Numerical integration algorithms.
namespace MathInteg
{
using namespace MathUtils;

//! Gauss-Legendre quadrature for definite integrals.
//! Computes integral of f(x) from theLower to theUpper using n-point Gauss-Legendre rule.
//!
//! Algorithm:
//! 1. Transform interval [theLower, theUpper] to [-1, 1]
//! 2. Evaluate f at Gauss-Legendre points
//! 3. Sum weighted function values
//!
//! Exact for polynomials of degree up to 2n-1.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theNbPoints number of quadrature points (3, 5, 7, 15, or 21)
//! @return result containing integral value
template <typename Function>
IntegResult Gauss(Function& theFunc, double theLower, double theUpper, int theNbPoints = 15)
{
  IntegResult aResult;

  // Get quadrature points and weights
  const double* aPoints = nullptr;
  const double* aWeights = nullptr;

  if (!MathUtils::GetGaussPointsAndWeights(theNbPoints, aPoints, aWeights))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Transform from [-1, 1] to [theLower, theUpper]
  const double aHalfLen = 0.5 * (theUpper - theLower);
  const double aMid = 0.5 * (theUpper + theLower);

  double aSum = 0.0;
  for (int i = 0; i < theNbPoints; ++i)
  {
    const double aX = aMid + aHalfLen * aPoints[i];
    double       aF = 0.0;
    if (!theFunc.Value(aX, aF))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    aSum += aWeights[i] * aF;
  }

  aResult.Status = Status::OK;
  aResult.Value = aHalfLen * aSum;
  aResult.NbPoints = theNbPoints;
  aResult.NbIterations = 1;
  return aResult;
}

//! Adaptive Gauss-Legendre integration.
//! Recursively subdivides interval until error estimate is below tolerance.
//!
//! Algorithm:
//! 1. Compute integral using n and 2n points
//! 2. Estimate error as difference between the two
//! 3. If error > tolerance, subdivide and recurse
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theConfig integration configuration
//! @return result containing integral value and error estimate
template <typename Function>
IntegResult GaussAdaptive(Function&               theFunc,
                               double                  theLower,
                               double                  theUpper,
                               const IntegConfig& theConfig = IntegConfig())
{
  IntegResult aResult;

  // Compute with coarse and fine grids
  IntegResult aCoarse = Gauss(theFunc, theLower, theUpper, 7);
  if (!aCoarse.IsDone())
  {
    return aCoarse;
  }

  IntegResult aFine = Gauss(theFunc, theLower, theUpper, 15);
  if (!aFine.IsDone())
  {
    return aFine;
  }

  const double aError = std::abs(*aFine.Value - *aCoarse.Value);
  const double aScale = std::max(std::abs(*aFine.Value), 1.0e-15);

  // Check if converged
  if (aError < theConfig.Tolerance * aScale)
  {
    aResult.Status = Status::OK;
    aResult.Value = *aFine.Value;
    aResult.AbsoluteError = aError;
    aResult.RelativeError = aError / aScale;
    aResult.NbPoints = 15;
    aResult.NbIterations = 1;
    return aResult;
  }

  // Need to subdivide - check iteration limit
  if (theConfig.MaxIterations <= 1)
  {
    aResult.Status = Status::MaxIterations;
    aResult.Value = *aFine.Value;
    aResult.AbsoluteError = aError;
    aResult.RelativeError = aError / aScale;
    aResult.NbPoints = 15;
    aResult.NbIterations = 1;
    return aResult;
  }

  // Subdivide interval
  const double aMid = 0.5 * (theLower + theUpper);

  IntegConfig aSubConfig = theConfig;
  aSubConfig.MaxIterations = theConfig.MaxIterations - 1;

  IntegResult aLeft = GaussAdaptive(theFunc, theLower, aMid, aSubConfig);
  if (!aLeft.IsDone())
  {
    aResult.Status = aLeft.Status;
    aResult.Value = aLeft.Value;
    return aResult;
  }

  IntegResult aRight = GaussAdaptive(theFunc, aMid, theUpper, aSubConfig);
  if (!aRight.IsDone())
  {
    aResult.Status = aRight.Status;
    aResult.Value = *aLeft.Value + (aRight.Value ? *aRight.Value : 0.0);
    return aResult;
  }

  aResult.Status = Status::OK;
  aResult.Value = *aLeft.Value + *aRight.Value;
  aResult.AbsoluteError = *aLeft.AbsoluteError + *aRight.AbsoluteError;
  aResult.RelativeError = *aResult.AbsoluteError / std::max(std::abs(*aResult.Value), 1.0e-15);
  aResult.NbPoints = aLeft.NbPoints + aRight.NbPoints;
  aResult.NbIterations = std::max(aLeft.NbIterations, aRight.NbIterations) + 1;
  return aResult;
}

//! Composite Gauss-Legendre integration.
//! Divides interval into subintervals and applies Gauss-Legendre to each.
//! Simple alternative to adaptive integration.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theNbIntervals number of subintervals
//! @param theNbPoints Gauss points per interval (3, 5, 7, 15, or 21)
//! @return result containing integral value
template <typename Function>
IntegResult GaussComposite(Function& theFunc,
                                double    theLower,
                                double    theUpper,
                                int       theNbIntervals,
                                int       theNbPoints = 7)
{
  IntegResult aResult;

  if (theNbIntervals < 1)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const double aH = (theUpper - theLower) / theNbIntervals;
  double       aSum = 0.0;
  int          aTotalPoints = 0;

  for (int i = 0; i < theNbIntervals; ++i)
  {
    const double aA = theLower + i * aH;
    const double aB = aA + aH;

    IntegResult aSubResult = Gauss(theFunc, aA, aB, theNbPoints);
    if (!aSubResult.IsDone())
    {
      aResult.Status = aSubResult.Status;
      aResult.Value = aSum;
      aResult.NbPoints = aTotalPoints;
      return aResult;
    }

    aSum += *aSubResult.Value;
    aTotalPoints += aSubResult.NbPoints;
  }

  aResult.Status = Status::OK;
  aResult.Value = aSum;
  aResult.NbPoints = aTotalPoints;
  aResult.NbIterations = 1;
  return aResult;
}

} // namespace MathInteg

#endif // _MathInteg_Gauss_HeaderFile
