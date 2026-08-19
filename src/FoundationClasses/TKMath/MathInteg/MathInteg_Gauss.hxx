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

#include <NCollection_LinearVector.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

//! Numerical integration algorithms.
namespace MathInteg
{
using namespace MathUtils;

//! Largest order supported by the Gauss-Legendre weight generator.
constexpr size_t THE_GAUSS_MAX_ORDER = 61;

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
//! @param theNbPoints number of quadrature points (>= 1)
//! @return result containing integral value
template <typename Function>
IntegResult Gauss(Function& theFunc, double theLower, double theUpper, size_t theNbPoints = 15)
{
  IntegResult aResult;
  if (theNbPoints == 0 || theNbPoints > THE_GAUSS_MAX_ORDER || !std::isfinite(theLower)
      || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (theLower > theUpper)
  {
    aResult = Gauss(theFunc, theUpper, theLower, theNbPoints);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

  // Get quadrature points and weights
  math_Vector aPoints(theNbPoints);
  math_Vector aWeights(theNbPoints);

  if (!MathUtils::GetGaussPointsAndWeights(static_cast<int>(theNbPoints), aPoints, aWeights))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Transform from [-1, 1] to [theLower, theUpper]
  const double aHalfLen = 0.5 * (theUpper - theLower);
  const double aMid     = theLower + aHalfLen;

  double aSum = 0.0;
  for (size_t i = 0; i < theNbPoints; ++i)
  {
    const double aX = aMid + aHalfLen * aPoints.At(i);
    double       aF = 0.0;
    ++aResult.NbPoints;
    if (!std::isfinite(aX))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    if (!theFunc.Value(aX, aF))
    {
      aResult.Status = Status::CallbackError;
      return aResult;
    }
    if (!std::isfinite(aF))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    aSum += aWeights.At(i) * aF;
    if (!std::isfinite(aSum))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
  }

  const double aValue = aHalfLen * aSum;
  if (!std::isfinite(aValue))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  aResult.Status       = Status::OK;
  aResult.Value        = aValue;
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
IntegResult GaussAdaptive(Function&          theFunc,
                          double             theLower,
                          double             theUpper,
                          const IntegConfig& theConfig = IntegConfig())
{
  IntegResult aResult;

  if (!std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower) || !std::isfinite(theConfig.Tolerance)
      || theConfig.Tolerance <= 0.0 || theConfig.InitialOrder < 1
      || theConfig.MaxOrder < theConfig.InitialOrder || theConfig.MaxOrder < 2
      || theConfig.MaxOrder > THE_GAUSS_MAX_ORDER || theConfig.MaxIterations < 1)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (theLower > theUpper)
  {
    aResult = GaussAdaptive(theFunc, theUpper, theLower, theConfig);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

  uint32_t aCoarseOrder = theConfig.InitialOrder;
  uint32_t aFineOrder = std::min(theConfig.MaxOrder,
                                 std::min(static_cast<uint32_t>(THE_GAUSS_MAX_ORDER),
                                          2 * aCoarseOrder));
  if (aFineOrder == aCoarseOrder)
  {
    if (aCoarseOrder > 1)
    {
      aCoarseOrder -= 1;
    }
    else if (theConfig.MaxOrder > 1)
    {
      aFineOrder = 2;
    }
  }

  struct Interval
  {
    double Lower;
    double Upper;
    double Value;
    double Error;
  };

  const auto anEvaluate = [&](double theA, double theB, Interval& theInterval) -> IntegResult {
    IntegResult aCoarse = Gauss(theFunc, theA, theB, aCoarseOrder);
    if (!aCoarse.IsDone())
    {
      return aCoarse;
    }
    IntegResult aFine = Gauss(theFunc, theA, theB, aFineOrder);
    aFine.NbPoints += aCoarse.NbPoints;
    if (!aFine.IsDone())
    {
      return aFine;
    }
    theInterval = {theA, theB, *aFine.Value, std::abs(*aFine.Value - *aCoarse.Value)};
    return aFine;
  };

  Interval    anInitial;
  IntegResult anInitialResult = anEvaluate(theLower, theUpper, anInitial);
  if (!anInitialResult.IsDone())
  {
    return anInitialResult;
  }

  NCollection_LinearVector<Interval> anIntervals;
  anIntervals.Append(anInitial);
  double   aTotalValue  = anInitial.Value;
  double   aTotalError  = anInitial.Error;
  size_t   aTotalPoints = anInitialResult.NbPoints;
  uint32_t aWork        = 1;

  const auto isConverged = [&](double theError, double theValue) {
    const double anAbsValue = std::abs(theValue);
    return anAbsValue > std::numeric_limits<double>::epsilon()
             ? theError <= theConfig.Tolerance * anAbsValue
             : theError <= theConfig.Tolerance;
  };

  while (!isConverged(aTotalError, aTotalValue))
  {
    if (aWork >= theConfig.MaxIterations)
    {
      aResult.Status = Status::MaxIterations;
      break;
    }

    size_t aWorstIndex = 0;
    for (size_t i = 1; i < anIntervals.Size(); ++i)
    {
      if (anIntervals.Value(i).Error > anIntervals.Value(aWorstIndex).Error)
      {
        aWorstIndex = i;
      }
    }

    const Interval aWorst = anIntervals.Value(aWorstIndex);
    const double   aMid   = aWorst.Lower + 0.5 * (aWorst.Upper - aWorst.Lower);
    if (!(aMid > aWorst.Lower && aMid < aWorst.Upper))
    {
      aResult.Status = Status::NotConverged;
      break;
    }

    Interval    aLeft;
    IntegResult aLeftResult = anEvaluate(aWorst.Lower, aMid, aLeft);
    aTotalPoints += aLeftResult.NbPoints;
    if (!aLeftResult.IsDone())
    {
      aResult.Status = aLeftResult.Status;
      break;
    }

    Interval    aRight;
    IntegResult aRightResult = anEvaluate(aMid, aWorst.Upper, aRight);
    aTotalPoints += aRightResult.NbPoints;
    if (!aRightResult.IsDone())
    {
      aResult.Status = aRightResult.Status;
      break;
    }

    aTotalValue += aLeft.Value + aRight.Value - aWorst.Value;
    aTotalError = std::max(0.0, aTotalError + aLeft.Error + aRight.Error - aWorst.Error);
    anIntervals.ChangeValue(aWorstIndex) = aLeft;
    anIntervals.Append(aRight);
    ++aWork;
  }

  if (aResult.Status == Status::NotConverged && isConverged(aTotalError, aTotalValue))
  {
    aResult.Status = Status::OK;
  }
  aResult.Value         = aTotalValue;
  aResult.AbsoluteError = aTotalError;
  if (std::abs(aTotalValue) > std::numeric_limits<double>::epsilon())
  {
    aResult.RelativeError = aTotalError / std::abs(aTotalValue);
  }
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = aWork;
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
//! @param theNbPoints Gauss points per interval (>= 1)
//! @return result containing integral value
template <typename Function>
IntegResult GaussComposite(Function& theFunc,
                           double    theLower,
                           double    theUpper,
                           uint32_t  theNbIntervals,
                           size_t    theNbPoints = 7)
{
  IntegResult aResult;

  if (theNbIntervals == 0 || theNbPoints == 0 || theNbPoints > THE_GAUSS_MAX_ORDER
      || !std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (theLower > theUpper)
  {
    aResult = GaussComposite(theFunc, theUpper, theLower, theNbIntervals, theNbPoints);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

  const double aH           = (theUpper - theLower) / theNbIntervals;
  double       aSum         = 0.0;
  size_t       aTotalPoints = 0;

  for (uint32_t i = 0; i < theNbIntervals; ++i)
  {
    const double aA = theLower + i * aH;
    const double aB = aA + aH;

    IntegResult aSubResult = Gauss(theFunc, aA, aB, theNbPoints);
    if (!aSubResult.IsDone())
    {
      aResult.Status       = aSubResult.Status;
      aResult.Value        = aSum;
      aResult.NbPoints     = aTotalPoints + aSubResult.NbPoints;
      aResult.NbIterations = i + 1;
      return aResult;
    }

    aSum += *aSubResult.Value;
    aTotalPoints += aSubResult.NbPoints;
  }

  aResult.Status       = Status::OK;
  aResult.Value        = aSum;
  aResult.NbPoints     = aTotalPoints;
  aResult.NbIterations = theNbIntervals;
  return aResult;
}

} // namespace MathInteg

#endif // _MathInteg_Gauss_HeaderFile
