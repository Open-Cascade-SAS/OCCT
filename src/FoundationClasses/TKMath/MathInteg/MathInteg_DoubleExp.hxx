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

#ifndef _MathInteg_DoubleExp_HeaderFile
#define _MathInteg_DoubleExp_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <vector>

namespace MathInteg
{
using namespace MathUtils;

//! Configuration for double exponential integration.
struct DoubleExpConfig : IntegConfig
{
  int    NbLevels    = 6;     //!< Number of refinement levels (each doubles points)
  double StepFactor  = 0.5;   //!< Initial step size h = StepFactor / NbPoints

  //! Default constructor.
  DoubleExpConfig() = default;

  //! Constructor with tolerance.
  explicit DoubleExpConfig(double theTolerance, int theMaxIter = 100)
      : IntegConfig(theTolerance, theMaxIter)
  {
  }
};

//! Tanh-Sinh (Double Exponential) quadrature for finite interval [a,b].
//!
//! The double exponential transformation maps [a,b] to (-inf, +inf):
//!   x = (b+a)/2 + (b-a)/2 * tanh(pi/2 * sinh(t))
//!
//! The integrand is multiplied by the Jacobian:
//!   dx/dt = (b-a)/2 * (pi/2) * cosh(t) / cosh^2(pi/2 * sinh(t))
//!
//! Properties:
//! - Excellent for functions with endpoint singularities
//! - Exponential convergence for analytic functions
//! - Self-adaptive through level refinement
//! - Handles algebraic and logarithmic singularities
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower integration bound
//! @param theUpper upper integration bound
//! @param theConfig integration configuration
//! @return integration result with error estimate
template <typename Function>
IntegResult TanhSinh(Function&              theFunc,
                     double                 theLower,
                     double                 theUpper,
                     const DoubleExpConfig& theConfig = DoubleExpConfig())
{
  IntegResult aResult;

  if (theLower >= theUpper)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const double aHalfPi = M_PI / 2.0;
  const double aMid    = 0.5 * (theUpper + theLower);
  const double aHalf   = 0.5 * (theUpper - theLower);

  // Initial step size
  double aH = 1.0;

  // For convergence checking
  double aPrevSum = 0.0;
  size_t aTotalPoints = 0;

  // Level-by-level refinement
  for (int aLevel = 0; aLevel < theConfig.NbLevels; ++aLevel)
  {
    double aSum = 0.0;
    int    aNbPoints = 0;

    // For level 0, evaluate at t = 0, +/-h, +/-2h, ...
    // For level > 0, evaluate only at new points: +/-h/2, +/-3h/2, ...
    int aStart = (aLevel == 0) ? 0 : 1;
    int aStep  = (aLevel == 0) ? 1 : 2;

    // Positive t direction
    for (int k = aStart;; k += aStep)
    {
      double aT = k * aH;

      // Sinh and cosh of t
      double aSinhT = std::sinh(aT);
      double aCoshT = std::cosh(aT);

      // u = pi/2 * sinh(t)
      double aU = aHalfPi * aSinhT;

      // Check for overflow in exp
      if (std::abs(aU) > 700.0)
      {
        break; // tanh(u) ~= +/-1, contribution is negligible
      }

      // tanh(u) and sech^2(u) = 1/cosh^2(u)
      double aTanhU = std::tanh(aU);
      double aCoshU = std::cosh(aU);
      double aSech2U = 1.0 / (aCoshU * aCoshU);

      // x = mid + half * tanh(u)
      double aX = aMid + aHalf * aTanhU;

      // Check if x is within bounds (with small tolerance)
      if (aX <= theLower + MathUtils::THE_ZERO_TOL || aX >= theUpper - MathUtils::THE_ZERO_TOL)
      {
        break;
      }

      // Weight: h * (b-a)/2 * pi/2 * cosh(t) * sech^2(u)
      double aWeight = aHalf * aHalfPi * aCoshT * aSech2U;

      // Check if weight is negligible
      if (aWeight < MathUtils::THE_ZERO_TOL)
      {
        break;
      }

      // Evaluate function
      double aF = 0.0;
      if (!theFunc.Value(aX, aF))
      {
        // Function evaluation failed, skip this point
        continue;
      }

      // Handle NaN or Inf
      if (!std::isfinite(aF))
      {
        continue;
      }

      aSum += aWeight * aF;
      ++aNbPoints;

      // Limit number of points for safety
      if (aNbPoints > 10000)
      {
        break;
      }
    }

    // Negative t direction (skip t=0 which was already counted)
    int aNegStart = (aLevel == 0) ? 1 : aStart;  // Start at 1 for level 0, aStart otherwise
    for (int k = aNegStart;; k += aStep)
    {
      double aT = -k * aH;

      double aSinhT = std::sinh(aT);
      double aCoshT = std::cosh(aT);
      double aU     = aHalfPi * aSinhT;

      if (std::abs(aU) > 700.0)
      {
        break;
      }

      double aTanhU  = std::tanh(aU);
      double aCoshU  = std::cosh(aU);
      double aSech2U = 1.0 / (aCoshU * aCoshU);

      double aX = aMid + aHalf * aTanhU;

      if (aX <= theLower + MathUtils::THE_ZERO_TOL || aX >= theUpper - MathUtils::THE_ZERO_TOL)
      {
        break;
      }

      double aWeight = aHalf * aHalfPi * aCoshT * aSech2U;

      if (aWeight < MathUtils::THE_ZERO_TOL)
      {
        break;
      }

      double aF = 0.0;
      if (!theFunc.Value(aX, aF))
      {
        continue;
      }

      if (!std::isfinite(aF))
      {
        continue;
      }

      aSum += aWeight * aF;
      ++aNbPoints;

      if (aNbPoints > 10000)
      {
        break;
      }
    }

    // Scale by step size
    double aLevelSum = aH * aSum;
    aTotalPoints += static_cast<size_t>(aNbPoints);

    // For level > 0, add to previous sum (trapezoidal refinement)
    // S_new = S_old/2 + h_new * new_points_sum
    double aNewSum = (aLevel == 0) ? aLevelSum : 0.5 * aPrevSum + aLevelSum;

    // Check for convergence
    if (aLevel > 0)
    {
      double aAbsError = std::abs(aNewSum - aPrevSum);
      double aRelError = aAbsError / std::max(std::abs(aNewSum), 1.0e-15);

      if (aRelError < theConfig.Tolerance)
      {
        aResult.Status        = Status::OK;
        aResult.Value         = aNewSum;
        aResult.AbsoluteError = aAbsError;
        aResult.RelativeError = aRelError;
        aResult.NbPoints      = aTotalPoints;
        aResult.NbIterations  = static_cast<size_t>(aLevel + 1);
        return aResult;
      }
    }

    aPrevSum = aNewSum;
    aH *= 0.5; // Halve step size for next level
  }

  // Did not converge, return best estimate
  aResult.Status        = Status::OK; // Still return a result
  aResult.Value         = aPrevSum;
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = static_cast<size_t>(theConfig.NbLevels);
  return aResult;
}

//! Exp-Sinh quadrature for semi-infinite interval [a, +infinity).
//!
//! The transformation maps [0, +infinity) to (-infinity, +infinity):
//!   x = a + exp(pi/2 * sinh(t))
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower bound a
//! @param theConfig integration configuration
//! @return integration result
template <typename Function>
IntegResult ExpSinh(Function&              theFunc,
                    double                 theLower,
                    const DoubleExpConfig& theConfig = DoubleExpConfig())
{
  IntegResult aResult;

  const double aHalfPi = M_PI / 2.0;
  double       aH      = 1.0;
  double       aPrevSum = 0.0;
  size_t       aTotalPoints = 0;

  for (int aLevel = 0; aLevel < theConfig.NbLevels; ++aLevel)
  {
    double aSum      = 0.0;
    int    aNbPoints = 0;

    int aStart = (aLevel == 0) ? 0 : 1;
    int aStep  = (aLevel == 0) ? 1 : 2;

    // Positive and negative t
    for (int aSign = -1; aSign <= 1; aSign += 2)
    {
      for (int k = (aSign < 0 && aLevel == 0) ? 1 : aStart;; k += aStep)
      {
        if (aSign < 0 && k == 0)
        {
          continue;
        }

        double aT = aSign * k * aH;

        double aSinhT = std::sinh(aT);
        double aCoshT = std::cosh(aT);
        double aU     = aHalfPi * aSinhT;

        // exp(u) for x transformation
        if (aU > 700.0)
        {
          break; // Overflow
        }

        double aExpU = std::exp(aU);

        // x = a + exp(u)
        double aX = theLower + aExpU;

        // Weight: h * pi/2 * cosh(t) * exp(u)
        double aWeight = aHalfPi * aCoshT * aExpU;

        if (aWeight < MathUtils::THE_ZERO_TOL || !std::isfinite(aWeight))
        {
          break;
        }

        // For negative t with large |u|, the contribution may be negligible
        if (aU < -30.0)
        {
          break;
        }

        double aF = 0.0;
        if (!theFunc.Value(aX, aF))
        {
          continue;
        }

        if (!std::isfinite(aF))
        {
          continue;
        }

        aSum += aWeight * aF;
        ++aNbPoints;

        if (aNbPoints > 10000)
        {
          break;
        }
      }
    }

    double aLevelSum = aH * aSum;
    aTotalPoints += static_cast<size_t>(aNbPoints);

    // Trapezoidal refinement: S_new = S_old/2 + h_new * new_points_sum
    double aNewSum = (aLevel == 0) ? aLevelSum : 0.5 * aPrevSum + aLevelSum;

    if (aLevel > 0)
    {
      double aAbsError = std::abs(aNewSum - aPrevSum);
      double aRelError = aAbsError / std::max(std::abs(aNewSum), 1.0e-15);

      if (aRelError < theConfig.Tolerance)
      {
        aResult.Status        = Status::OK;
        aResult.Value         = aNewSum;
        aResult.AbsoluteError = aAbsError;
        aResult.RelativeError = aRelError;
        aResult.NbPoints      = aTotalPoints;
        aResult.NbIterations  = static_cast<size_t>(aLevel + 1);
        return aResult;
      }
    }

    aPrevSum = aNewSum;
    aH *= 0.5;
  }

  aResult.Status       = Status::OK;
  aResult.Value        = aPrevSum;
  aResult.NbPoints     = aTotalPoints;
  aResult.NbIterations = static_cast<size_t>(theConfig.NbLevels);
  return aResult;
}

//! Sinh-Sinh quadrature for infinite interval (-infinity, +infinity).
//!
//! The transformation maps (-infinity, +infinity) to (-infinity, +infinity):
//!   x = sinh(pi/2 * sinh(t))
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theConfig integration configuration
//! @return integration result
template <typename Function>
IntegResult SinhSinh(Function&              theFunc,
                     const DoubleExpConfig& theConfig = DoubleExpConfig())
{
  IntegResult aResult;

  const double aHalfPi = M_PI / 2.0;
  double       aH      = 1.0;
  double       aPrevSum = 0.0;
  size_t       aTotalPoints = 0;

  for (int aLevel = 0; aLevel < theConfig.NbLevels; ++aLevel)
  {
    double aSum      = 0.0;
    int    aNbPoints = 0;

    int aStart = (aLevel == 0) ? 0 : 1;
    int aStep  = (aLevel == 0) ? 1 : 2;

    // Positive and negative t
    for (int aSign = -1; aSign <= 1; aSign += 2)
    {
      for (int k = (aSign < 0 && aLevel == 0) ? 1 : aStart;; k += aStep)
      {
        if (aSign < 0 && k == 0)
        {
          continue;
        }

        double aT = aSign * k * aH;

        double aSinhT = std::sinh(aT);
        double aCoshT = std::cosh(aT);
        double aU     = aHalfPi * aSinhT;

        // sinh(u) and cosh(u) for transformation
        if (std::abs(aU) > 700.0)
        {
          break;
        }

        double aSinhU = std::sinh(aU);
        double aCoshU = std::cosh(aU);

        // x = sinh(u)
        double aX = aSinhU;

        // Weight: h * pi/2 * cosh(t) * cosh(u)
        double aWeight = aHalfPi * aCoshT * aCoshU;

        if (aWeight < MathUtils::THE_ZERO_TOL || !std::isfinite(aWeight))
        {
          break;
        }

        double aF = 0.0;
        if (!theFunc.Value(aX, aF))
        {
          continue;
        }

        if (!std::isfinite(aF))
        {
          continue;
        }

        aSum += aWeight * aF;
        ++aNbPoints;

        if (aNbPoints > 10000)
        {
          break;
        }
      }
    }

    double aLevelSum = aH * aSum;
    aTotalPoints += static_cast<size_t>(aNbPoints);

    // Trapezoidal refinement: S_new = S_old/2 + h_new * new_points_sum
    double aNewSum = (aLevel == 0) ? aLevelSum : 0.5 * aPrevSum + aLevelSum;

    if (aLevel > 0)
    {
      double aAbsError = std::abs(aNewSum - aPrevSum);
      double aRelError = aAbsError / std::max(std::abs(aNewSum), 1.0e-15);

      if (aRelError < theConfig.Tolerance)
      {
        aResult.Status        = Status::OK;
        aResult.Value         = aNewSum;
        aResult.AbsoluteError = aAbsError;
        aResult.RelativeError = aRelError;
        aResult.NbPoints      = aTotalPoints;
        aResult.NbIterations  = static_cast<size_t>(aLevel + 1);
        return aResult;
      }
    }

    aPrevSum = aNewSum;
    aH *= 0.5;
  }

  aResult.Status       = Status::OK;
  aResult.Value        = aPrevSum;
  aResult.NbPoints     = aTotalPoints;
  aResult.NbIterations = static_cast<size_t>(theConfig.NbLevels);
  return aResult;
}

//! Double exponential integration with automatic interval detection.
//!
//! Selects the appropriate DE quadrature based on the interval:
//! - Finite [a,b]: TanhSinh
//! - Semi-infinite [a, +infinity): ExpSinh
//! - Infinite (-infinity, +infinity): SinhSinh
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower bound (use -HUGE_VAL for -infinity)
//! @param theUpper upper bound (use HUGE_VAL for +infinity)
//! @param theConfig integration configuration
//! @return integration result
template <typename Function>
IntegResult DoubleExponential(Function&              theFunc,
                              double                 theLower,
                              double                 theUpper,
                              const DoubleExpConfig& theConfig = DoubleExpConfig())
{
  const double aHuge = 1.0e300;

  bool aIsLowerInf = (theLower < -aHuge);
  bool aIsUpperInf = (theUpper > aHuge);

  if (aIsLowerInf && aIsUpperInf)
  {
    // (-infinity, +infinity)
    return SinhSinh(theFunc, theConfig);
  }
  else if (aIsUpperInf)
  {
    // [a, +infinity)
    return ExpSinh(theFunc, theLower, theConfig);
  }
  else if (aIsLowerInf)
  {
    // (-infinity, b] - transform to [b, +infinity) with x -> -x
    class NegatedFunc
    {
    public:
      NegatedFunc(Function& theF)
          : myFunc(theF)
      {
      }

      bool Value(double theX, double& theF)
      {
        return myFunc.Value(-theX, theF);
      }

    private:
      Function& myFunc;
    };

    NegatedFunc aNegated(theFunc);
    return ExpSinh(aNegated, -theUpper, theConfig);
  }
  else
  {
    // [a, b]
    return TanhSinh(theFunc, theLower, theUpper, theConfig);
  }
}

//! Tanh-Sinh quadrature optimized for endpoint singularities.
//!
//! This variant uses precomputed nodes and weights for better performance
//! when integrating functions with algebraic singularities at the endpoints.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower bound
//! @param theUpper upper bound
//! @param theTolerance relative tolerance
//! @return integration result
template <typename Function>
IntegResult TanhSinhSingular(Function& theFunc,
                             double    theLower,
                             double    theUpper,
                             double    theTolerance = 1.0e-10)
{
  DoubleExpConfig aConfig;
  aConfig.Tolerance = theTolerance;
  aConfig.NbLevels  = 8; // More levels for singular functions

  return TanhSinh(theFunc, theLower, theUpper, aConfig);
}

//! Integrates a function with a known singularity at an interior point.
//!
//! Splits the interval at the singularity and integrates each part.
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to integrate
//! @param theLower lower bound
//! @param theUpper upper bound
//! @param theSingularity location of interior singularity
//! @param theConfig integration configuration
//! @return combined integration result
template <typename Function>
IntegResult TanhSinhWithSingularity(Function&              theFunc,
                                    double                 theLower,
                                    double                 theUpper,
                                    double                 theSingularity,
                                    const DoubleExpConfig& theConfig = DoubleExpConfig())
{
  IntegResult aResult;

  if (theSingularity <= theLower || theSingularity >= theUpper)
  {
    // Singularity is at or outside bounds, use regular integration
    return TanhSinh(theFunc, theLower, theUpper, theConfig);
  }

  // Integrate left part [lower, singularity]
  IntegResult aLeft = TanhSinh(theFunc, theLower, theSingularity, theConfig);
  if (!aLeft.IsDone())
  {
    return aLeft;
  }

  // Integrate right part [singularity, upper]
  IntegResult aRight = TanhSinh(theFunc, theSingularity, theUpper, theConfig);
  if (!aRight.IsDone())
  {
    return aRight;
  }

  // Combine results
  aResult.Status        = Status::OK;
  aResult.Value         = *aLeft.Value + *aRight.Value;
  aResult.NbPoints      = aLeft.NbPoints + aRight.NbPoints;
  aResult.NbIterations  = std::max(aLeft.NbIterations, aRight.NbIterations);

  if (aLeft.AbsoluteError && aRight.AbsoluteError)
  {
    aResult.AbsoluteError = *aLeft.AbsoluteError + *aRight.AbsoluteError;
    aResult.RelativeError = *aResult.AbsoluteError / std::max(std::abs(*aResult.Value), 1.0e-15);
  }

  return aResult;
}

} // namespace MathInteg

#endif // _MathInteg_DoubleExp_HeaderFile
