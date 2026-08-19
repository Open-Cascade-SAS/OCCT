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

#ifndef _MathInteg_Kronrod_HeaderFile
#define _MathInteg_Kronrod_HeaderFile

// Include wrapper for Gauss-Kronrod weights BEFORE namespace MathInteg is defined
#include <MathUtils_GaussKronrodWeights.hxx>

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>

#include <NCollection_LinearVector.hxx>

#include <cmath>
#include <limits>

namespace MathInteg
{
using namespace MathUtils;

//! Largest Gauss order supported by the Gauss-Kronrod weight generator.
constexpr size_t THE_KRONROD_MAX_GAUSS_ORDER = 30;
//! Initial Gauss order used by automatic order selection.
constexpr size_t THE_KRONROD_AUTO_INITIAL_ORDER = 7;
//! Order increment used by automatic order selection.
constexpr size_t THE_KRONROD_AUTO_ORDER_STEP = 4;
//! Adaptive work limit used after automatic order selection is exhausted.
constexpr uint32_t THE_KRONROD_AUTO_MAX_ITERATIONS = 50;

//! Configuration for Gauss-Kronrod integration.
struct KronrodConfig : IntegConfig
{
  size_t NbGaussPoints = THE_KRONROD_AUTO_INITIAL_ORDER; //!< Gauss points; Kronrod uses 2n+1
  bool   Adaptive      = true; //!< Whether to use adaptive subdivision

  //! Default constructor.
  KronrodConfig() = default;

  //! Constructor with tolerance.
  explicit KronrodConfig(double theTolerance, uint32_t theMaxIter = 100)
      : IntegConfig(theTolerance, theMaxIter)
  {
  }
};

//! Apply Gauss-Kronrod rule to a single interval.
//!
//! The Gauss-Kronrod rule uses n Gauss points embedded in 2n+1 Kronrod points.
//! The difference between the Gauss and Kronrod estimates provides
//! an error estimate without additional function evaluations.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theNbGauss number of Gauss points (determines rule order)
//! @return integration result with error estimate
template <typename Function>
IntegResult KronrodRule(Function& theFunc, double theLower, double theUpper, size_t theNbGauss = 7)
{
  IntegResult aResult;

  if (theNbGauss == 0 || theNbGauss > THE_KRONROD_MAX_GAUSS_ORDER
      || !std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (theLower > theUpper)
  {
    aResult = KronrodRule(theFunc, theUpper, theLower, theNbGauss);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

  // Number of Kronrod points = 2*n + 1
  const size_t aNbKronrod = 2 * theNbGauss + 1;

  // Get Gauss-Kronrod points and weights using global ::math class
  math_Vector aGaussP(theNbGauss);
  math_Vector aGaussW(theNbGauss);
  math_Vector aKronrodP(aNbKronrod);
  math_Vector aKronrodW(aNbKronrod);

  if (!GetKronrodPointsAndWeights(static_cast<int>(aNbKronrod), aKronrodP, aKronrodW)
      || !GetOrderedGaussPointsAndWeights(static_cast<int>(theNbGauss), aGaussP, aGaussW))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Transform interval [theLower, theUpper] to [-1, 1]
  const double aHalfLen = 0.5 * (theUpper - theLower);
  const double aMid     = theLower + aHalfLen;

  // Compute Gauss and Kronrod quadratures simultaneously
  const size_t aHalfCount = (aNbKronrod + 1) / 2;

  double aGaussVal   = 0.0;
  double aKronrodVal = 0.0;
  double anAbsVal    = 0.0;

  const auto anEvaluate = [&](double theX, double& theValue) -> Status {
    ++aResult.NbPoints;
    theValue = 0.0;
    if (!std::isfinite(theX))
    {
      return Status::NumericalError;
    }
    if (!theFunc.Value(theX, theValue))
    {
      return Status::CallbackError;
    }
    return std::isfinite(theValue) ? Status::OK : Status::NumericalError;
  };

  // Function values at symmetric points
  math_Vector aF1(aHalfCount);
  math_Vector aF2(aHalfCount);

  // Offsets corresponding to Gauss points embedded in the Kronrod rule.
  for (size_t i = 1; i + 1 < aHalfCount; i += 2)
  {
    const double aDx   = aHalfLen * aKronrodP.At(i);
    double       aVal1 = 0.0, aVal2 = 0.0;

    const Status aStatus1 = anEvaluate(aMid + aDx, aVal1);
    if (aStatus1 != Status::OK)
    {
      aResult.Status = aStatus1;
      return aResult;
    }
    const Status aStatus2 = anEvaluate(aMid - aDx, aVal2);
    if (aStatus2 != Status::OK)
    {
      aResult.Status = aStatus2;
      return aResult;
    }

    aF1.ChangeAt(i) = aVal1;
    aF2.ChangeAt(i) = aVal2;
    aGaussVal += (aVal1 + aVal2) * aGaussW.At(i / 2);
    aKronrodVal += (aVal1 + aVal2) * aKronrodW.At(i);
    anAbsVal += (std::abs(aVal1) + std::abs(aVal2)) * aKronrodW.At(i);
  }

  // Center point
  double       aFc           = 0.0;
  const Status aCenterStatus = anEvaluate(aMid, aFc);
  if (aCenterStatus != Status::OK)
  {
    aResult.Status = aCenterStatus;
    return aResult;
  }

  aKronrodVal += aFc * aKronrodW.At(aHalfCount - 1);
  anAbsVal += std::abs(aFc) * aKronrodW.At(aHalfCount - 1);

  // Check if center is also a Gauss point
  if (aHalfCount % 2 == 0)
  {
    aGaussVal += aFc * aGaussW.At(aHalfCount / 2 - 1);
  }

  // Offsets corresponding to Kronrod-only points.
  for (size_t i = 0; i + 1 < aHalfCount; i += 2)
  {
    const double aDx   = aHalfLen * aKronrodP.At(i);
    double       aVal1 = 0.0, aVal2 = 0.0;

    const Status aStatus1 = anEvaluate(aMid + aDx, aVal1);
    if (aStatus1 != Status::OK)
    {
      aResult.Status = aStatus1;
      return aResult;
    }
    const Status aStatus2 = anEvaluate(aMid - aDx, aVal2);
    if (aStatus2 != Status::OK)
    {
      aResult.Status = aStatus2;
      return aResult;
    }

    aF1.ChangeAt(i) = aVal1;
    aF2.ChangeAt(i) = aVal2;
    aKronrodVal += (aVal1 + aVal2) * aKronrodW.At(i);
    anAbsVal += (std::abs(aVal1) + std::abs(aVal2)) * aKronrodW.At(i);
  }

  // QUADPACK-style error estimation:
  // Compute asc = integral of |f(x) - mean|, which measures function variability
  const double aMean = 0.5 * aKronrodVal;
  double       aAsc  = std::abs(aFc - aMean) * aKronrodW.At(aHalfCount - 1);
  for (size_t i = 0; i + 1 < aHalfCount; ++i)
  {
    aAsc += aKronrodW.At(i) * (std::abs(aF1.At(i) - aMean) + std::abs(aF2.At(i) - aMean));
  }

  // Scale by interval half-length
  aAsc *= aHalfLen;
  anAbsVal *= aHalfLen;
  aKronrodVal *= aHalfLen;
  aGaussVal *= aHalfLen;

  if (!std::isfinite(aAsc) || !std::isfinite(anAbsVal) || !std::isfinite(aKronrodVal)
      || !std::isfinite(aGaussVal))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Basic error estimate
  double aAbsError = std::abs(aKronrodVal - aGaussVal);

  // QUADPACK rescales the embedded-rule difference using the function variability.
  if (aAsc != 0.0 && aAbsError != 0.0)
  {
    const double aScale = std::pow(200.0 * aAbsError / aAsc, 1.5);
    aAbsError           = aAsc * std::min(1.0, aScale);
  }
  // QUADPACK limits unrealistically small estimates using the absolute integral estimate.
  if (anAbsVal
      > std::numeric_limits<double>::min() / (50.0 * std::numeric_limits<double>::epsilon()))
  {
    aAbsError = std::max(aAbsError, 50.0 * std::numeric_limits<double>::epsilon() * anAbsVal);
  }

  aResult.Status        = Status::OK;
  aResult.Value         = aKronrodVal;
  aResult.AbsoluteError = aAbsError;
  if (std::abs(aKronrodVal) > std::numeric_limits<double>::epsilon())
  {
    aResult.RelativeError = aAbsError / std::abs(aKronrodVal);
  }
  aResult.NbIterations  = 1;
  return aResult;
}

//! Gauss-Kronrod adaptive integration.
//!
//! Uses adaptive bisection to achieve the requested tolerance.
//! At each subdivision, the interval with the largest error estimate
//! is bisected, and both halves are reintegrated.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theConfig integration configuration
//! @return integration result with error estimate
template <typename Function>
IntegResult Kronrod(Function&            theFunc,
                    double               theLower,
                    double               theUpper,
                    const KronrodConfig& theConfig = KronrodConfig())
{
  IntegResult aResult;

  if (!std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower) || theConfig.NbGaussPoints < 1
      || theConfig.NbGaussPoints > THE_KRONROD_MAX_GAUSS_ORDER
      || !std::isfinite(theConfig.Tolerance)
      || theConfig.Tolerance <= 0.0 || theConfig.MaxIterations < 1)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (theLower > theUpper)
  {
    aResult = Kronrod(theFunc, theUpper, theLower, theConfig);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

  if (!theConfig.Adaptive)
  {
    // Single application of Kronrod rule
    return KronrodRule(theFunc, theLower, theUpper, theConfig.NbGaussPoints);
  }

  // Adaptive integration using a heap of intervals
  struct Interval
  {
    double Lower;
    double Upper;
    double Value;
    double Error;
  };

  // Initialize with the whole interval
  IntegResult anInitResult = KronrodRule(theFunc, theLower, theUpper, theConfig.NbGaussPoints);
  if (!anInitResult.IsDone())
  {
    return anInitResult;
  }

  NCollection_LinearVector<Interval> aHeap;
  aHeap.Append({theLower, theUpper, *anInitResult.Value, *anInitResult.AbsoluteError});

  double   aTotalValue  = *anInitResult.Value;
  double   aTotalError  = *anInitResult.AbsoluteError;
  size_t   aTotalPoints = anInitResult.NbPoints;
  uint32_t aIterations  = 1;
  bool     isStagnated  = false;

  const auto isConverged = [&](double theError, double theValue) {
    const double anAbsValue = std::abs(theValue);
    return anAbsValue > std::numeric_limits<double>::epsilon()
             ? theError <= theConfig.Tolerance * anAbsValue
             : theError <= theConfig.Tolerance;
  };

  // Adaptive refinement
  while (aIterations < theConfig.MaxIterations)
  {
    // Check convergence
    if (isConverged(aTotalError, aTotalValue))
    {
      break;
    }

    // Find interval with largest error
    size_t aMaxIdx   = 0;
    double aMaxError = 0.0;
    for (size_t i = 0; i < aHeap.Size(); ++i)
    {
      if (aHeap.Value(i).Error > aMaxError)
      {
        aMaxError = aHeap.Value(i).Error;
        aMaxIdx   = i;
      }
    }

    // Bisect the interval with largest error (copy to avoid reference invalidation)
    const Interval aWorst  = aHeap.Value(aMaxIdx);
    const double   aBisMid = aWorst.Lower + 0.5 * (aWorst.Upper - aWorst.Lower);
    if (!(aBisMid > aWorst.Lower && aBisMid < aWorst.Upper))
    {
      isStagnated = true;
      break;
    }

    IntegResult aLeftResult = KronrodRule(theFunc, aWorst.Lower, aBisMid, theConfig.NbGaussPoints);
    aTotalPoints += aLeftResult.NbPoints;
    if (!aLeftResult.IsDone())
    {
      aResult.Status        = aLeftResult.Status;
      aResult.Value         = aTotalValue;
      aResult.AbsoluteError = aTotalError;
      aResult.NbPoints      = aTotalPoints;
      if (std::abs(aTotalValue) > std::numeric_limits<double>::epsilon())
      {
        aResult.RelativeError = aTotalError / std::abs(aTotalValue);
      }
      aResult.NbIterations  = aIterations;
      return aResult;
    }

    IntegResult aRightResult = KronrodRule(theFunc, aBisMid, aWorst.Upper, theConfig.NbGaussPoints);
    aTotalPoints += aRightResult.NbPoints;
    if (!aRightResult.IsDone())
    {
      aResult.Status        = aRightResult.Status;
      aResult.Value         = aTotalValue;
      aResult.AbsoluteError = aTotalError;
      if (std::abs(aTotalValue) > std::numeric_limits<double>::epsilon())
      {
        aResult.RelativeError = aTotalError / std::abs(aTotalValue);
      }
      aResult.NbPoints      = aTotalPoints;
      aResult.NbIterations  = aIterations;
      return aResult;
    }

    // Update totals
    aTotalValue -= aWorst.Value;
    aTotalError -= aWorst.Error;
    aTotalValue += *aLeftResult.Value + *aRightResult.Value;
    aTotalError += *aLeftResult.AbsoluteError + *aRightResult.AbsoluteError;
    aTotalError = std::max(0.0, aTotalError);
    ++aIterations;

    // Replace the worst interval with the two new intervals
    aHeap.ChangeValue(
      aMaxIdx) = {aWorst.Lower, aBisMid, *aLeftResult.Value, *aLeftResult.AbsoluteError};
    aHeap.Append({aBisMid, aWorst.Upper, *aRightResult.Value, *aRightResult.AbsoluteError});
  }

  const bool hasConverged = isConverged(aTotalError, aTotalValue);
  aResult.Status =
    hasConverged ? Status::OK : (isStagnated ? Status::NotConverged : Status::MaxIterations);
  aResult.Value         = aTotalValue;
  aResult.AbsoluteError = aTotalError;
  if (std::abs(aTotalValue) > std::numeric_limits<double>::epsilon())
  {
    aResult.RelativeError = aTotalError / std::abs(aTotalValue);
  }
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = aIterations;
  return aResult;
}

//! Gauss-Kronrod integration with automatic order selection.
//!
//! Starts with a low-order rule and increases the order until
//! the tolerance is met or the maximum order is reached.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theTolerance relative tolerance
//! @param theMaxOrder maximum Gauss order to try
//! @return integration result
template <typename Function>
IntegResult KronrodAuto(Function& theFunc,
                        double    theLower,
                        double    theUpper,
                        double    theTolerance = 1.0e-10,
                         size_t    theMaxOrder  = THE_KRONROD_MAX_GAUSS_ORDER)
{
  IntegResult aBestResult;
  aBestResult.Status  = Status::NotConverged;
  size_t aTotalPoints = 0;

  if (!std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower) || !std::isfinite(theTolerance) || theTolerance <= 0.0
      || theMaxOrder < THE_KRONROD_AUTO_INITIAL_ORDER
      || theMaxOrder > THE_KRONROD_MAX_GAUSS_ORDER)
  {
    aBestResult.Status = Status::InvalidInput;
    return aBestResult;
  }
  if (theLower > theUpper)
  {
    aBestResult = KronrodAuto(theFunc, theUpper, theLower, theTolerance, theMaxOrder);
    if (aBestResult.Value)
    {
      *aBestResult.Value = -*aBestResult.Value;
    }
    return aBestResult;
  }

  // Try increasing orders
  for (size_t aOrder = THE_KRONROD_AUTO_INITIAL_ORDER; aOrder <= theMaxOrder;
       aOrder += THE_KRONROD_AUTO_ORDER_STEP)
  {
    IntegResult aResult = KronrodRule(theFunc, theLower, theUpper, aOrder);
    aTotalPoints += aResult.NbPoints;
    if (!aResult.IsDone())
    {
      aResult.NbPoints = aTotalPoints;
      return aResult;
    }

    aBestResult = aResult;

    // Check if tolerance is met
    const double anAbsValue = std::abs(*aResult.Value);
    const bool isConverged =
      aResult.AbsoluteError
      && (anAbsValue > std::numeric_limits<double>::epsilon()
            ? *aResult.AbsoluteError <= theTolerance * anAbsValue
            : *aResult.AbsoluteError <= theTolerance);
    if (isConverged)
    {
      aResult.NbPoints = aTotalPoints;
      return aResult;
    }
  }

  // If fixed order didn't work, try adaptive
  KronrodConfig aConfig;
  aConfig.Tolerance     = theTolerance;
  aConfig.NbGaussPoints = THE_KRONROD_AUTO_INITIAL_ORDER;
  aConfig.Adaptive      = true;
  aConfig.MaxIterations = THE_KRONROD_AUTO_MAX_ITERATIONS;

  IntegResult aResult = Kronrod(theFunc, theLower, theUpper, aConfig);
  aResult.NbPoints += aTotalPoints;
  return aResult;
}

//! Gauss-Kronrod integration over semi-infinite interval [a, +infinity).
//!
//! Uses the substitution x = a + t / (1 - t) to map [a, +infinity) to [0, 1).
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower bound a
//! @param theConfig integration configuration
//! @return integration result
template <typename Function>
IntegResult KronrodSemiInfinite(Function&            theFunc,
                                double               theLower,
                                const KronrodConfig& theConfig = KronrodConfig())
{
  if (!std::isfinite(theLower))
  {
    IntegResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  class TransformedFunc
  {
  public:
    TransformedFunc(Function& theF, double theA)
        : myFunc(theF),
          myA(theA)
    {
    }

    bool Value(double theT, double& theF)
    {
      if (theT >= 1.0)
      {
        theF = 0.0;
        return true;
      }

      const double aX     = myA + theT / (1.0 - theT);
      const double aJacob = 1.0 / MathUtils::Sqr(1.0 - theT);

      double aFx = 0.0;
      if (!myFunc.Value(aX, aFx))
      {
        return false;
      }

      theF = aFx * aJacob;
      return true;
    }

  private:
    Function& myFunc;
    double    myA;
  };

  TransformedFunc aTransformed(theFunc, theLower);
  return Kronrod(aTransformed, 0.0, 1.0, theConfig);
}

//! Gauss-Kronrod integration over infinite interval (-infinity, +infinity).
//!
//! Uses the substitution x = t / (1 - t^2) to map (-infinity, +infinity) to (-1, 1).
//! The function must decay sufficiently fast at infinity.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theConfig integration configuration
//! @return integration result
template <typename Function>
IntegResult KronrodInfinite(Function& theFunc, const KronrodConfig& theConfig = KronrodConfig())
{
  class TransformedFunc
  {
  public:
    TransformedFunc(Function& theF)
        : myFunc(theF)
    {
    }

    bool Value(double theT, double& theF)
    {
      if (std::abs(theT) >= 1.0)
      {
        theF = 0.0;
        return true;
      }

      const double aT2    = theT * theT;
      const double aX     = theT / (1.0 - aT2);
      const double aJacob = (1.0 + aT2) / MathUtils::Sqr(1.0 - aT2);

      double aFx = 0.0;
      if (!myFunc.Value(aX, aFx))
      {
        return false;
      }

      theF = aFx * aJacob;
      return true;
    }

  private:
    Function& myFunc;
  };

  TransformedFunc aTransformed(theFunc);
  return Kronrod(aTransformed, -1.0, 1.0, theConfig);
}

} // namespace MathInteg

#endif // _MathInteg_Kronrod_HeaderFile
