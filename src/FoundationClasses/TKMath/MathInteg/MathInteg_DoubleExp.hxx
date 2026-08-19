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
#include <limits>

namespace MathInteg
{
using namespace MathUtils;

//! Default callback-evaluation limit for one double-exponential refinement level.
constexpr size_t THE_DOUBLE_EXP_DEFAULT_MAX_POINTS_PER_LEVEL = 10000;

//! Configuration for double exponential integration.
struct DoubleExpConfig : IntegConfig
{
  uint32_t NbLevels          = 10;    //!< Number of refinement levels (each doubles points)
  double   StepFactor        = 0.5;   //!< Initial transformed-coordinate step size
  size_t MaxPointsPerLevel =
    THE_DOUBLE_EXP_DEFAULT_MAX_POINTS_PER_LEVEL; //!< Maximum evaluations at one level

  //! Default constructor.
  DoubleExpConfig() = default;

  //! Constructor with tolerance.
  explicit DoubleExpConfig(double theTolerance, uint32_t theMaxIter = 100)
      : IntegConfig(theTolerance, theMaxIter)
  {
  }
};

//! Largest safe argument used by the hyperbolic transformations.
constexpr double THE_DOUBLE_EXP_MAX_TRANSFORM_ARGUMENT = 700.0;
//! Negative Exp-Sinh transform argument after which contributions are negligible.
constexpr double THE_EXP_SINH_MIN_TRANSFORM_ARGUMENT = -30.0;

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

  if (!std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower) || !std::isfinite(theConfig.Tolerance)
      || theConfig.Tolerance <= 0.0 || !std::isfinite(theConfig.StepFactor)
      || theConfig.StepFactor <= 0.0 || theConfig.NbLevels == 0 || theConfig.MaxIterations == 0
      || theConfig.MaxPointsPerLevel == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (theLower > theUpper)
  {
    aResult = TanhSinh(theFunc, theUpper, theLower, theConfig);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

  const double aHalfPi = M_PI / 2.0;
  const double aHalf   = 0.5 * (theUpper - theLower);
  const double aMid    = theLower + aHalf;

  // Initial step size
  double aH = theConfig.StepFactor;

  // For convergence checking
  double                aPrevSum = 0.0;
  std::optional<double> aLastError;
  size_t                aTotalPoints = 0;
  const uint32_t        aNbLevels = std::min(theConfig.NbLevels, theConfig.MaxIterations);

  // Level-by-level refinement
  for (uint32_t aLevel = 0; aLevel < aNbLevels; ++aLevel)
  {
    double       aSum          = 0.0;
    const size_t aPointsBefore = aTotalPoints;
    size_t       aLevelPoints  = 0;

    // For level 0, evaluate at t = 0, +/-h, +/-2h, ...
    // For level > 0, evaluate only at new points: +/-h/2, +/-3h/2, ...
    const size_t aStart = (aLevel == 0) ? 0 : 1;
    const size_t aStep  = (aLevel == 0) ? 1 : 2;

    // Positive t direction
    for (size_t k = aStart;;)
    {
      double aT = k * aH;

      // Sinh and cosh of t
      double aSinhT = std::sinh(aT);
      double aCoshT = std::cosh(aT);

      // u = pi/2 * sinh(t)
      double aU = aHalfPi * aSinhT;

      // Check for overflow in exp
      if (std::abs(aU) > THE_DOUBLE_EXP_MAX_TRANSFORM_ARGUMENT)
      {
        break; // tanh(u) ~= +/-1, contribution is negligible
      }

      // tanh(u) and sech^2(u) = 1/cosh^2(u)
      double aTanhU  = std::tanh(aU);
      double aCoshU  = std::cosh(aU);
      double aSech2U = 1.0 / (aCoshU * aCoshU);

      // x = mid + half * tanh(u)
      double aX = aMid + aHalf * aTanhU;

      // Check if x is within bounds (with small tolerance)
      if (aX <= theLower || aX >= theUpper)
      {
        break;
      }

      // Weight: h * (b-a)/2 * pi/2 * cosh(t) * sech^2(u)
      double aWeight = aHalf * aHalfPi * aCoshT * aSech2U;

      // Check if weight is negligible
      if (aWeight <= 0.0 || !std::isfinite(aWeight))
      {
        break;
      }

      // Evaluate function
      if (aLevelPoints >= theConfig.MaxPointsPerLevel)
      {
        aResult.Status       = Status::MaxIterations;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        if (aLevel > 0)
        {
          aResult.Value         = aPrevSum;
          aResult.AbsoluteError = aLastError;
        }
        return aResult;
      }
      double aF = 0.0;
      ++aLevelPoints;
      ++aTotalPoints;
      if (!theFunc.Value(aX, aF))
      {
        aResult.Status       = Status::CallbackError;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        return aResult;
      }
      if (!std::isfinite(aF))
      {
        aResult.Status       = Status::NumericalError;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        return aResult;
      }

      aSum += aWeight * aF;
      if (!std::isfinite(aSum))
      {
        aResult.Status       = Status::NumericalError;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        return aResult;
      }
      k += aStep;
    }

    // Negative t direction (skip t=0 which was already counted)
    const size_t aNegStart = (aLevel == 0) ? 1 : aStart;
    for (size_t k = aNegStart;;)
    {
      const double aT = -static_cast<double>(k) * aH;

      double aSinhT = std::sinh(aT);
      double aCoshT = std::cosh(aT);
      double aU     = aHalfPi * aSinhT;

      if (std::abs(aU) > THE_DOUBLE_EXP_MAX_TRANSFORM_ARGUMENT)
      {
        break;
      }

      double aTanhU  = std::tanh(aU);
      double aCoshU  = std::cosh(aU);
      double aSech2U = 1.0 / (aCoshU * aCoshU);

      double aX = aMid + aHalf * aTanhU;

      if (aX <= theLower || aX >= theUpper)
      {
        break;
      }

      double aWeight = aHalf * aHalfPi * aCoshT * aSech2U;

      if (aWeight <= 0.0 || !std::isfinite(aWeight))
      {
        break;
      }

      if (aLevelPoints >= theConfig.MaxPointsPerLevel)
      {
        aResult.Status       = Status::MaxIterations;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        if (aLevel > 0)
        {
          aResult.Value         = aPrevSum;
          aResult.AbsoluteError = aLastError;
        }
        return aResult;
      }
      double aF = 0.0;
      ++aLevelPoints;
      ++aTotalPoints;
      if (!theFunc.Value(aX, aF))
      {
        aResult.Status       = Status::CallbackError;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        return aResult;
      }
      if (!std::isfinite(aF))
      {
        aResult.Status       = Status::NumericalError;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        return aResult;
      }

      aSum += aWeight * aF;
      if (!std::isfinite(aSum))
      {
        aResult.Status       = Status::NumericalError;
        aResult.NbPoints     = aTotalPoints;
        aResult.NbIterations = aLevel + 1;
        return aResult;
      }
      k += aStep;
    }

    // Scale by step size
    double aLevelSum = aH * aSum;
    if (!std::isfinite(aLevelSum))
    {
      aResult.Status       = Status::NumericalError;
      aResult.NbPoints     = aTotalPoints;
      aResult.NbIterations = aLevel + 1;
      return aResult;
    }
    // For level > 0, add to previous sum (trapezoidal refinement)
    // S_new = S_old/2 + h_new * new_points_sum
    double aNewSum = (aLevel == 0) ? aLevelSum : 0.5 * aPrevSum + aLevelSum;

    // Check for convergence
    if (aLevel > 0 && aTotalPoints > aPointsBefore)
    {
      const double aAbsError = std::abs(aNewSum - aPrevSum);
      aLastError             = aAbsError;

      const double anAbsValue = std::abs(aNewSum);
      const bool isConverged = anAbsValue > std::numeric_limits<double>::epsilon()
                                 ? aAbsError <= theConfig.Tolerance * anAbsValue
                                 : aAbsError <= theConfig.Tolerance;
      if (isConverged)
      {
        aResult.Status        = Status::OK;
        aResult.Value         = aNewSum;
        aResult.AbsoluteError = aAbsError;
        if (anAbsValue > std::numeric_limits<double>::epsilon())
        {
          aResult.RelativeError = aAbsError / anAbsValue;
        }
        aResult.NbPoints      = aTotalPoints;
        aResult.NbIterations  = aLevel + 1;
        return aResult;
      }
    }

    aPrevSum = aNewSum;
    aH *= 0.5; // Halve step size for next level
  }

  // Did not converge, return best estimate
  aResult.Status        = Status::MaxIterations;
  aResult.Value         = aPrevSum;
  aResult.AbsoluteError = aLastError;
  if (aLastError && std::abs(aPrevSum) > std::numeric_limits<double>::epsilon())
  {
    aResult.RelativeError = *aLastError / std::abs(aPrevSum);
  }
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = aNbLevels;
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

  if (!std::isfinite(theLower) || !std::isfinite(theConfig.Tolerance) || theConfig.Tolerance <= 0.0
      || !std::isfinite(theConfig.StepFactor) || theConfig.StepFactor <= 0.0
      || theConfig.NbLevels == 0 || theConfig.MaxIterations == 0
      || theConfig.MaxPointsPerLevel == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const double   aHalfPi      = M_PI / 2.0;
  double         aH           = theConfig.StepFactor;
  double                aPrevSum = 0.0;
  std::optional<double> aLastError;
  size_t                aTotalPoints = 0;
  const uint32_t        aNbLevels = std::min(theConfig.NbLevels, theConfig.MaxIterations);

  for (uint32_t aLevel = 0; aLevel < aNbLevels; ++aLevel)
  {
    double       aSum          = 0.0;
    const size_t aPointsBefore = aTotalPoints;
    size_t       aLevelPoints  = 0;

    const size_t aStart = (aLevel == 0) ? 0 : 1;
    const size_t aStep  = (aLevel == 0) ? 1 : 2;

    // Positive and negative t
    for (int aSign = -1; aSign <= 1; aSign += 2)
    {
      for (size_t k = (aSign < 0 && aLevel == 0) ? 1 : aStart;;)
      {
        const double aT = static_cast<double>(aSign) * static_cast<double>(k) * aH;

        double aSinhT = std::sinh(aT);
        double aCoshT = std::cosh(aT);
        double aU     = aHalfPi * aSinhT;

        // exp(u) for x transformation
        if (aU > THE_DOUBLE_EXP_MAX_TRANSFORM_ARGUMENT)
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
        if (aU < THE_EXP_SINH_MIN_TRANSFORM_ARGUMENT)
        {
          break;
        }

        if (aLevelPoints >= theConfig.MaxPointsPerLevel)
        {
          aResult.Status       = Status::MaxIterations;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          if (aLevel > 0)
          {
            aResult.Value         = aPrevSum;
            aResult.AbsoluteError = aLastError;
          }
          return aResult;
        }
        double aF = 0.0;
        ++aLevelPoints;
        ++aTotalPoints;
        if (!std::isfinite(aX))
        {
          aResult.Status       = Status::NumericalError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }
        if (!theFunc.Value(aX, aF))
        {
          aResult.Status       = Status::CallbackError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }
        if (!std::isfinite(aF))
        {
          aResult.Status       = Status::NumericalError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }

        aSum += aWeight * aF;
        if (!std::isfinite(aSum))
        {
          aResult.Status       = Status::NumericalError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }
        k += aStep;
      }
    }

    double aLevelSum = aH * aSum;
    if (!std::isfinite(aLevelSum))
    {
      aResult.Status       = Status::NumericalError;
      aResult.NbPoints     = aTotalPoints;
      aResult.NbIterations = aLevel + 1;
      return aResult;
    }
    // Trapezoidal refinement: S_new = S_old/2 + h_new * new_points_sum
    double aNewSum = (aLevel == 0) ? aLevelSum : 0.5 * aPrevSum + aLevelSum;

    if (aLevel > 0 && aTotalPoints > aPointsBefore)
    {
      const double aAbsError = std::abs(aNewSum - aPrevSum);
      aLastError             = aAbsError;

      const double anAbsValue = std::abs(aNewSum);
      const bool isConverged = anAbsValue > std::numeric_limits<double>::epsilon()
                                 ? aAbsError <= theConfig.Tolerance * anAbsValue
                                 : aAbsError <= theConfig.Tolerance;
      if (isConverged)
      {
        aResult.Status        = Status::OK;
        aResult.Value         = aNewSum;
        aResult.AbsoluteError = aAbsError;
        if (anAbsValue > std::numeric_limits<double>::epsilon())
        {
          aResult.RelativeError = aAbsError / anAbsValue;
        }
        aResult.NbPoints      = aTotalPoints;
        aResult.NbIterations  = aLevel + 1;
        return aResult;
      }
    }

    aPrevSum = aNewSum;
    aH *= 0.5;
  }

  aResult.Status        = Status::MaxIterations;
  aResult.Value         = aPrevSum;
  aResult.AbsoluteError = aLastError;
  if (aLastError && std::abs(aPrevSum) > std::numeric_limits<double>::epsilon())
  {
    aResult.RelativeError = *aLastError / std::abs(aPrevSum);
  }
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = aNbLevels;
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
IntegResult SinhSinh(Function& theFunc, const DoubleExpConfig& theConfig = DoubleExpConfig())
{
  IntegResult aResult;

  if (!std::isfinite(theConfig.Tolerance) || theConfig.Tolerance <= 0.0
      || !std::isfinite(theConfig.StepFactor) || theConfig.StepFactor <= 0.0
      || theConfig.NbLevels == 0 || theConfig.MaxIterations == 0
      || theConfig.MaxPointsPerLevel == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const double   aHalfPi      = M_PI / 2.0;
  double         aH           = theConfig.StepFactor;
  double                aPrevSum = 0.0;
  std::optional<double> aLastError;
  size_t                aTotalPoints = 0;
  const uint32_t        aNbLevels = std::min(theConfig.NbLevels, theConfig.MaxIterations);

  for (uint32_t aLevel = 0; aLevel < aNbLevels; ++aLevel)
  {
    double       aSum          = 0.0;
    const size_t aPointsBefore = aTotalPoints;
    size_t       aLevelPoints  = 0;

    const size_t aStart = (aLevel == 0) ? 0 : 1;
    const size_t aStep  = (aLevel == 0) ? 1 : 2;

    // Positive and negative t
    for (int aSign = -1; aSign <= 1; aSign += 2)
    {
      for (size_t k = (aSign < 0 && aLevel == 0) ? 1 : aStart;;)
      {
        const double aT = static_cast<double>(aSign) * static_cast<double>(k) * aH;

        double aSinhT = std::sinh(aT);
        double aCoshT = std::cosh(aT);
        double aU     = aHalfPi * aSinhT;

        // sinh(u) and cosh(u) for transformation
        if (std::abs(aU) > THE_DOUBLE_EXP_MAX_TRANSFORM_ARGUMENT)
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

        if (aLevelPoints >= theConfig.MaxPointsPerLevel)
        {
          aResult.Status       = Status::MaxIterations;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          if (aLevel > 0)
          {
            aResult.Value         = aPrevSum;
            aResult.AbsoluteError = aLastError;
          }
          return aResult;
        }
        double aF = 0.0;
        ++aLevelPoints;
        ++aTotalPoints;
        if (!std::isfinite(aX))
        {
          aResult.Status       = Status::NumericalError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }
        if (!theFunc.Value(aX, aF))
        {
          aResult.Status       = Status::CallbackError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }
        if (!std::isfinite(aF))
        {
          aResult.Status       = Status::NumericalError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }

        aSum += aWeight * aF;
        if (!std::isfinite(aSum))
        {
          aResult.Status       = Status::NumericalError;
          aResult.NbPoints     = aTotalPoints;
          aResult.NbIterations = aLevel + 1;
          return aResult;
        }
        k += aStep;
      }
    }

    double aLevelSum = aH * aSum;
    if (!std::isfinite(aLevelSum))
    {
      aResult.Status       = Status::NumericalError;
      aResult.NbPoints     = aTotalPoints;
      aResult.NbIterations = aLevel + 1;
      return aResult;
    }
    // Trapezoidal refinement: S_new = S_old/2 + h_new * new_points_sum
    double aNewSum = (aLevel == 0) ? aLevelSum : 0.5 * aPrevSum + aLevelSum;

    if (aLevel > 0 && aTotalPoints > aPointsBefore)
    {
      const double aAbsError = std::abs(aNewSum - aPrevSum);
      aLastError             = aAbsError;

      const double anAbsValue = std::abs(aNewSum);
      const bool isConverged = anAbsValue > std::numeric_limits<double>::epsilon()
                                 ? aAbsError <= theConfig.Tolerance * anAbsValue
                                 : aAbsError <= theConfig.Tolerance;
      if (isConverged)
      {
        aResult.Status        = Status::OK;
        aResult.Value         = aNewSum;
        aResult.AbsoluteError = aAbsError;
        if (anAbsValue > std::numeric_limits<double>::epsilon())
        {
          aResult.RelativeError = aAbsError / anAbsValue;
        }
        aResult.NbPoints      = aTotalPoints;
        aResult.NbIterations  = aLevel + 1;
        return aResult;
      }
    }

    aPrevSum = aNewSum;
    aH *= 0.5;
  }

  aResult.Status        = Status::MaxIterations;
  aResult.Value         = aPrevSum;
  aResult.AbsoluteError = aLastError;
  if (aLastError && std::abs(aPrevSum) > std::numeric_limits<double>::epsilon())
  {
    aResult.RelativeError = *aLastError / std::abs(aPrevSum);
  }
  aResult.NbPoints      = aTotalPoints;
  aResult.NbIterations  = aNbLevels;
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
  IntegResult anInvalidResult;
  if (std::isnan(theLower) || std::isnan(theUpper) || theLower == theUpper
      || (theLower > theUpper && (!std::isfinite(theLower) || !std::isfinite(theUpper))))
  {
    anInvalidResult.Status = Status::InvalidInput;
    return anInvalidResult;
  }

  const bool aIsLowerInf = std::isinf(theLower) && theLower < 0.0;
  const bool aIsUpperInf = std::isinf(theUpper) && theUpper > 0.0;
  if ((std::isinf(theLower) && !aIsLowerInf) || (std::isinf(theUpper) && !aIsUpperInf))
  {
    anInvalidResult.Status = Status::InvalidInput;
    return anInvalidResult;
  }

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

      bool Value(double theX, double& theF) { return myFunc.Value(-theX, theF); }

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

  if (!std::isfinite(theSingularity))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  if (std::isfinite(theLower) && std::isfinite(theUpper) && theLower > theUpper)
  {
    aResult = TanhSinhWithSingularity(theFunc, theUpper, theLower, theSingularity, theConfig);
    if (aResult.Value)
    {
      *aResult.Value = -*aResult.Value;
    }
    return aResult;
  }

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
    if (aLeft.Value && aRight.Value)
    {
      aRight.Value = *aLeft.Value + *aRight.Value;
    }
    aRight.NbPoints += aLeft.NbPoints;
    aRight.NbIterations += aLeft.NbIterations;
    return aRight;
  }

  // Combine results
  aResult.Status       = Status::OK;
  aResult.Value        = *aLeft.Value + *aRight.Value;
  aResult.NbPoints     = aLeft.NbPoints + aRight.NbPoints;
  aResult.NbIterations = aLeft.NbIterations + aRight.NbIterations;

  if (aLeft.AbsoluteError && aRight.AbsoluteError)
  {
    aResult.AbsoluteError = *aLeft.AbsoluteError + *aRight.AbsoluteError;
    if (std::abs(*aResult.Value) > std::numeric_limits<double>::epsilon())
    {
      aResult.RelativeError = *aResult.AbsoluteError / std::abs(*aResult.Value);
    }
  }

  return aResult;
}

} // namespace MathInteg

#endif // _MathInteg_DoubleExp_HeaderFile
