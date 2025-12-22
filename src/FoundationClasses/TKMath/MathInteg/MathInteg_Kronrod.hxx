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

#include <NCollection_Vector.hxx>

#include <cmath>

namespace MathInteg
{
using namespace MathUtils;

//! Configuration for Gauss-Kronrod integration.
struct KronrodConfig : IntegConfig
{
  int  NbGaussPoints = 7;    //!< Number of Gauss points (n), Kronrod will use 2n+1 points
  bool Adaptive      = true; //!< Whether to use adaptive subdivision

  //! Default constructor.
  KronrodConfig() = default;

  //! Constructor with tolerance.
  explicit KronrodConfig(double theTolerance, int theMaxIter = 100)
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
IntegResult KronrodRule(Function& theFunc, double theLower, double theUpper, int theNbGauss = 7)
{
  IntegResult aResult;

  // Number of Kronrod points = 2*n + 1
  const int aNbKronrod = 2 * theNbGauss + 1;

  // Get Gauss-Kronrod points and weights using global ::math class
  math_Vector aGaussP(1, theNbGauss);
  math_Vector aGaussW(1, theNbGauss);
  math_Vector aKronrodP(1, aNbKronrod);
  math_Vector aKronrodW(1, aNbKronrod);

  if (!GetKronrodPointsAndWeights(aNbKronrod, aKronrodP, aKronrodW)
      || !GetOrderedGaussPointsAndWeights(theNbGauss, aGaussP, aGaussW))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Transform interval [theLower, theUpper] to [-1, 1]
  const double aHalfLen = 0.5 * (theUpper - theLower);
  const double aMid     = 0.5 * (theUpper + theLower);

  // Compute Gauss and Kronrod quadratures simultaneously
  const int aNPnt2 = (aNbKronrod + 1) / 2;

  double aGaussVal   = 0.0;
  double aKronrodVal = 0.0;

  // Function values at symmetric points
  math_Vector aF1(0, aNPnt2 - 1);
  math_Vector aF2(0, aNPnt2 - 1);

  // Even indices (Gauss points embedded in Kronrod)
  for (int i = 2; i < aNPnt2; i += 2)
  {
    const double aDx   = aHalfLen * aKronrodP(i);
    double       aVal1 = 0.0, aVal2 = 0.0;

    if (!theFunc.Value(aMid + aDx, aVal1) || !theFunc.Value(aMid - aDx, aVal2))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    aF1(i) = aVal1;
    aF2(i) = aVal2;
    aGaussVal += (aVal1 + aVal2) * aGaussW(i / 2);
    aKronrodVal += (aVal1 + aVal2) * aKronrodW(i);
  }

  // Center point
  double aFc = 0.0;
  if (!theFunc.Value(aMid, aFc))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  aKronrodVal += aFc * aKronrodW(aNPnt2);

  // Check if center is also a Gauss point
  if (aNPnt2 % 2 == 0)
  {
    aGaussVal += aFc * aGaussW(aNPnt2 / 2);
  }

  // Odd indices (Kronrod-only points)
  for (int i = 1; i < aNPnt2; i += 2)
  {
    const double aDx   = aHalfLen * aKronrodP(i);
    double       aVal1 = 0.0, aVal2 = 0.0;

    if (!theFunc.Value(aMid + aDx, aVal1) || !theFunc.Value(aMid - aDx, aVal2))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    aF1(i) = aVal1;
    aF2(i) = aVal2;
    aKronrodVal += (aVal1 + aVal2) * aKronrodW(i);
  }

  // QUADPACK-style error estimation:
  // Compute asc = integral of |f(x) - mean|, which measures function variability
  const double aMean = 0.5 * aKronrodVal;
  double       aAsc  = std::abs(aFc - aMean) * aKronrodW(aNPnt2);
  for (int i = 1; i < aNPnt2; ++i)
  {
    aAsc += aKronrodW(i) * (std::abs(aF1(i) - aMean) + std::abs(aF2(i) - aMean));
  }

  // Scale by interval half-length
  aAsc *= aHalfLen;
  aKronrodVal *= aHalfLen;
  aGaussVal *= aHalfLen;

  // Basic error estimate
  double aAbsError = std::abs(aKronrodVal - aGaussVal);

  // QUADPACK scaling: when error is small relative to function variability,
  // the actual error may be even smaller
  if (aAsc != 0.0 && aAbsError != 0.0)
  {
    const double aScale = std::pow(200.0 * aAbsError / aAsc, 1.5);
    if (aScale < 1.0)
    {
      aAbsError = std::min(aAbsError, aAsc * aScale);
    }
  }

  aResult.Status        = Status::OK;
  aResult.Value         = aKronrodVal;
  aResult.AbsoluteError = aAbsError;
  aResult.RelativeError = aAbsError / std::max(std::abs(aKronrodVal), 1.0e-15);
  aResult.NbPoints      = static_cast<size_t>(aNbKronrod);
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

  NCollection_Vector<Interval> aHeap;
  aHeap.Append({theLower, theUpper, *anInitResult.Value, *anInitResult.AbsoluteError});

  double aTotalValue  = *anInitResult.Value;
  double aTotalError  = *anInitResult.AbsoluteError;
  size_t aTotalPoints = anInitResult.NbPoints;
  int    aIterations  = 1;

  // Adaptive refinement
  while (aIterations < theConfig.MaxIterations)
  {
    // Check convergence
    if (aTotalError < theConfig.Tolerance * std::max(std::abs(aTotalValue), 1.0e-15))
    {
      break;
    }

    // Find interval with largest error
    int    aMaxIdx   = 0;
    double aMaxError = 0.0;
    for (int i = 0; i < aHeap.Length(); ++i)
    {
      if (aHeap.Value(i).Error > aMaxError)
      {
        aMaxError = aHeap.Value(i).Error;
        aMaxIdx   = i;
      }
    }

    if (aMaxError < MathUtils::THE_ZERO_TOL)
    {
      break; // No more refinement needed
    }

    // Bisect the interval with largest error (copy to avoid reference invalidation)
    const Interval aWorst  = aHeap.Value(aMaxIdx);
    const double   aBisMid = 0.5 * (aWorst.Lower + aWorst.Upper);

    IntegResult aLeftResult  = KronrodRule(theFunc, aWorst.Lower, aBisMid, theConfig.NbGaussPoints);
    IntegResult aRightResult = KronrodRule(theFunc, aBisMid, aWorst.Upper, theConfig.NbGaussPoints);

    if (!aLeftResult.IsDone() || !aRightResult.IsDone())
    {
      aResult.Status        = Status::NumericalError;
      aResult.Value         = aTotalValue;
      aResult.AbsoluteError = aTotalError;
      aResult.NbPoints      = aTotalPoints;
      aResult.NbIterations  = static_cast<size_t>(aIterations);
      return aResult;
    }

    // Update totals
    aTotalValue -= aWorst.Value;
    aTotalError -= aWorst.Error;
    aTotalValue += *aLeftResult.Value + *aRightResult.Value;
    aTotalError += *aLeftResult.AbsoluteError + *aRightResult.AbsoluteError;
    aTotalPoints += aLeftResult.NbPoints + aRightResult.NbPoints;
    ++aIterations;

    // Replace the worst interval with the two new intervals
    aHeap.ChangeValue(
      aMaxIdx) = {aWorst.Lower, aBisMid, *aLeftResult.Value, *aLeftResult.AbsoluteError};
    aHeap.Append({aBisMid, aWorst.Upper, *aRightResult.Value, *aRightResult.AbsoluteError});
  }

  aResult.Status        = Status::OK;
  aResult.Value         = aTotalValue;
  aResult.AbsoluteError = aTotalError;
  aResult.RelativeError = aTotalError / std::max(std::abs(aTotalValue), 1.0e-15);
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = static_cast<size_t>(aIterations);
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
                        int       theMaxOrder  = 30)
{
  IntegResult aBestResult;
  aBestResult.Status = Status::NotConverged;

  // Try increasing orders
  for (int aOrder = 7; aOrder <= theMaxOrder; aOrder += 4)
  {
    IntegResult aResult = KronrodRule(theFunc, theLower, theUpper, aOrder);
    if (!aResult.IsDone())
    {
      continue;
    }

    aBestResult = aResult;

    // Check if tolerance is met
    if (aResult.RelativeError && *aResult.RelativeError < theTolerance)
    {
      return aResult;
    }
  }

  // If fixed order didn't work, try adaptive
  KronrodConfig aConfig;
  aConfig.Tolerance     = theTolerance;
  aConfig.NbGaussPoints = 7;
  aConfig.Adaptive      = true;
  aConfig.MaxIterations = 50;

  return Kronrod(theFunc, theLower, theUpper, aConfig);
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
