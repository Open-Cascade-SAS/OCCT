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

#ifndef _MathRoot_MultipleUtils_HeaderFile
#define _MathRoot_MultipleUtils_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathRoot_Brent.hxx>
#include "MathRoot_Utils.hxx"
#include <math_Vector.hxx>

#include <NCollection_LinearVector.hxx>

#include <algorithm>
#include <cmath>

//! @file MathRoot_MultipleUtils.hxx
//! @brief Internal utilities for FindAllRoots / FindAllRootsWithDerivative.
//!
//! Contains result/config types, helper functions, functor adapters and the
//! shared core implementation used by MathRoot_Multiple.hxx.

namespace MathRoot
{
using namespace MathUtils;

// ============================================================================
//  Result and configuration types
// ============================================================================

//! Result for multiple root finding.
//! Contains all found roots sorted in ascending order.
struct MultipleResult
{
  MathUtils::Status                Status = MathUtils::Status::NotConverged; //!< Computation status
  uint32_t                         NbIterations = 0; //!< Total iterations across all roots
  NCollection_LinearVector<double> Roots;            //!< Found roots (sorted)
  NCollection_LinearVector<double> Values;           //!< Function values at roots
  bool IsAllNull = false; //!< True if function is essentially zero in range

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }

  //! Returns the number of roots found.
  size_t NbRoots() const { return Roots.Size(); }

  //! Access root by index (0-based).
  double operator[](size_t theIndex) const { return Roots.Value(theIndex); }
};

//! Configuration for multiple root finding.
struct MultipleConfig
{
  size_t   NbSamples     = 100;   //!< Number of sample points for initial search
  double   XTolerance    = 1e-10; //!< Tolerance on X for convergence
  double   FTolerance    = 1e-10; //!< Tolerance on F(X) for convergence
  double   NullTolerance = 1e-12; //!< Tolerance to consider function as null
  uint32_t MaxIterations = 100;   //!< Max iterations per root refinement
  double   Offset        = 0.0;   //!< Find roots of f(x) - Offset = 0
};

namespace Utils
{

//! Newton steps allowed to polish a root already isolated by Brent's method.
inline constexpr uint32_t THE_MAX_NEWTON_POLISH_ITERATIONS = 5;

//! Relative interval scale enforcing the established minimum X tolerance.
inline constexpr double THE_MULTIPLE_RELATIVE_INTERVAL_TOLERANCE = 1.0e-10;

//! Return true when a multiple-root configuration can be sampled safely.
inline bool IsValidMultipleInput(double theLower, double theUpper, const MultipleConfig& theConfig)
{
  return IsValidBounds(theLower, theUpper) && theConfig.NbSamples >= 2
         && theConfig.MaxIterations > 0 && std::isfinite(theConfig.XTolerance)
         && theConfig.XTolerance > 0.0 && std::isfinite(theConfig.FTolerance)
         && theConfig.FTolerance >= 0.0 && std::isfinite(theConfig.NullTolerance)
         && theConfig.NullTolerance >= 0.0 && std::isfinite(theConfig.Offset);
}

// ============================================================================
//  Helper functions
// ============================================================================

//! In-place insertion sort of roots and corresponding values by ascending root value.
inline void SortRoots(MultipleResult& theResult)
{
  for (size_t i = 1; i < theResult.Roots.Size(); ++i)
  {
    const double aKeyRoot = theResult.Roots.Value(i);
    const double aKeyVal  = theResult.Values.Value(i);
    size_t       j        = i;
    while (j > 0 && theResult.Roots.Value(j - 1) > aKeyRoot)
    {
      theResult.Roots.ChangeValue(j)  = theResult.Roots.Value(j - 1);
      theResult.Values.ChangeValue(j) = theResult.Values.Value(j - 1);
      --j;
    }
    theResult.Roots.ChangeValue(j)  = aKeyRoot;
    theResult.Values.ChangeValue(j) = aKeyVal;
  }
}

//! Helper to add a root if it is not a duplicate of an already found root.
inline void AddRoot(MultipleResult& theResult, double theEpsX, double theRoot, double theValue)
{
  for (size_t k = 0; k < theResult.Roots.Size(); ++k)
  {
    if (std::abs(theRoot - theResult.Roots.Value(k)) < theEpsX)
    {
      return;
    }
  }
  theResult.Roots.Append(theRoot);
  theResult.Values.Append(theValue);
}

//! Compute the minimal X tolerance compatible with the established multi-root behavior.
inline double EffectiveXTolerance(double theLower, double theUpper, double theXTolerance)
{
  const double aMaxMagnitude = std::max(std::abs(theLower), std::abs(theUpper));
  const double aMinMagnitude = std::min(std::abs(theLower), std::abs(theUpper));
  if (aMaxMagnitude == 0.0)
  {
    return theXTolerance;
  }
  const double aMinEpsX = THE_MULTIPLE_RELATIVE_INTERVAL_TOLERANCE * aMaxMagnitude
                          * (1.0 + aMinMagnitude / aMaxMagnitude);
  return std::max(theXTolerance, aMinEpsX);
}

// ============================================================================
//  Functor adapters for Value-only interface
// ============================================================================

//! Samples a Value-only function and stores f(x)-offset into a math_Vector.
//! @tparam Function type with Value(double theX, double& theF) method
template <typename Function>
struct MultipleSampleValueFn
{
  Function&                 myFunc;
  math_Vector&              mySamples;
  const double              myOffset;
  mutable MathUtils::Status Status = MathUtils::Status::OK;

  bool operator()(size_t theIndex, double theX) const
  {
    double aF = 0.0;
    if (!myFunc.Value(theX, aF))
    {
      Status = MathUtils::Status::CallbackError;
      return false;
    }
    if (!std::isfinite(aF))
    {
      Status = MathUtils::Status::NumericalError;
      return false;
    }
    mySamples.ChangeAt(theIndex) = aF - myOffset;
    if (!std::isfinite(mySamples.At(theIndex)))
    {
      Status = MathUtils::Status::NumericalError;
      return false;
    }
    return true;
  }
};

//! Returns the sampled value at a given index from a math_Vector.
struct MultipleGetValueFn
{
  const math_Vector& mySamples;

  double operator()(size_t theIndex) const { return mySamples.At(theIndex); }
};

//! Brent wrapper that adapts a Value-only function for offset root finding.
//! @tparam Function type with Value(double theX, double& theF) method
template <typename Function>
struct MultipleBrentValueWrapper
{
  Function& myFunc;
  double    myOffset;

  bool Value(double theX, double& theY) const
  {
    if (!myFunc.Value(theX, theY))
      return false;
    theY -= myOffset;
    return true;
  }
};

//! Evaluates original (non-offset) function value at a root point via Value interface.
//! @tparam Function type with Value(double theX, double& theF) method
template <typename Function>
struct MultipleGetRootValueFn
{
  Function&                 myFunc;
  mutable MathUtils::Status Status = MathUtils::Status::OK;

  bool operator()(double theX, double& theValue) const
  {
    if (!myFunc.Value(theX, theValue))
    {
      Status = MathUtils::Status::CallbackError;
      return false;
    }
    if (!std::isfinite(theValue))
    {
      Status = MathUtils::Status::NumericalError;
      return false;
    }
    return true;
  }
};

// ============================================================================
//  Functor adapters for Values (with derivative) interface
// ============================================================================

//! Wrapper exposing a function derivative through the Value() contract required by Brent.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
template <typename Function>
struct MultipleDerivativeValueWrapper
{
  Function& myFunc;

  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    if (!myFunc.Values(theX, aF, theY))
    {
      return false;
    }
    if (!std::isfinite(aF))
    {
      theY = aF;
    }
    return true;
  }
};

//! Evaluate the original function value.
//! @tparam Function type with Value(double theX, double& theF) method
//! @return OK, CallbackError, or NumericalError
template <typename Function>
MathUtils::Status EvaluateValue(Function& theFunc, double theX, double& theValue)
{
  if (!theFunc.Value(theX, theValue))
  {
    return MathUtils::Status::CallbackError;
  }
  return std::isfinite(theValue) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

//! Evaluate the function value shifted by the requested offset.
//! @tparam Function type with Value(double theX, double& theF) method
//! @return OK, CallbackError, or NumericalError
template <typename Function>
MathUtils::Status EvaluateShiftedValue(Function& theFunc,
                                       double    theX,
                                       double    theOffset,
                                       double&   theValue)
{
  if (!theFunc.Value(theX, theValue))
  {
    return MathUtils::Status::CallbackError;
  }
  if (!std::isfinite(theValue))
  {
    return MathUtils::Status::NumericalError;
  }

  theValue -= theOffset;
  return std::isfinite(theValue) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

//! Evaluate the function value and derivative, then shift the value by the requested offset.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
//! @return OK, CallbackError, or NumericalError
template <typename Function>
MathUtils::Status EvaluateShiftedValues(Function& theFunc,
                                        double    theX,
                                        double    theOffset,
                                        double&   theValue,
                                        double&   theDerivative)
{
  if (!theFunc.Values(theX, theValue, theDerivative))
  {
    return MathUtils::Status::CallbackError;
  }
  if (!std::isfinite(theValue) || !std::isfinite(theDerivative))
  {
    return MathUtils::Status::NumericalError;
  }

  theValue -= theOffset;
  return std::isfinite(theValue) ? MathUtils::Status::OK : MathUtils::Status::NumericalError;
}

//! Refine a bracketed sign change using the same Brent/Newton sequence as math_FunctionRoots.
//! @tparam Function type with Value() and Values() methods
template <typename Function>
MathUtils::Status RefineBracketedRoot(Function&       theFunc,
                                      double          theOffset,
                                      double          theX1,
                                      double          theY1,
                                      double          theX2,
                                      double          theY2,
                                      double          theTolerance,
                                      double          theFTolerance,
                                      uint32_t        theMaxIterations,
                                      double          theEpsX,
                                      MultipleResult& theResult)
{
  if (!std::isfinite(theY1) || !std::isfinite(theY2))
  {
    return MathUtils::Status::NumericalError;
  }

  MultipleBrentValueWrapper<Function> aWrapper{theFunc, theOffset};
  MathUtils::Config                   aConfig;
  aConfig.XTolerance    = theTolerance;
  aConfig.FTolerance    = theFTolerance;
  aConfig.MaxIterations = theMaxIterations;

  MathUtils::ScalarResult aRefined = Brent(aWrapper, theX1, theX2, aConfig);
  theResult.NbIterations += aRefined.NbIterations;
  if (!aRefined.IsDone() || !aRefined.Root.has_value())
  {
    return aRefined.Status;
  }

  double aRoot      = *aRefined.Root;
  double aRootValue = 0.0;
  if (!theFunc.Value(aRoot, aRootValue))
  {
    return MathUtils::Status::CallbackError;
  }
  if (!std::isfinite(aRootValue))
  {
    return MathUtils::Status::NumericalError;
  }
  double aShiftedValue = aRootValue - theOffset;
  for (uint32_t anIteration = 0; anIteration < THE_MAX_NEWTON_POLISH_ITERATIONS; ++anIteration)
  {
    double aValue      = 0.0;
    double aDerivative = 0.0;
    if (!theFunc.Values(aRoot, aValue, aDerivative))
    {
      return MathUtils::Status::CallbackError;
    }
    aValue -= theOffset;
    if (!std::isfinite(aValue) || !std::isfinite(aDerivative))
    {
      return MathUtils::Status::NumericalError;
    }
    if (aDerivative == 0.0)
    {
      break;
    }

    const double aCandidate = aRoot - aValue / aDerivative;
    if (!std::isfinite(aCandidate) || aCandidate < theX1 || aCandidate > theX2)
    {
      break;
    }
    double aCandidateValue = 0.0;
    if (!theFunc.Value(aCandidate, aCandidateValue))
    {
      return MathUtils::Status::CallbackError;
    }
    if (!std::isfinite(aCandidateValue))
    {
      return MathUtils::Status::NumericalError;
    }
    const double aCandidateShiftedValue = aCandidateValue - theOffset;
    if (std::abs(aCandidateShiftedValue) >= std::abs(aShiftedValue))
    {
      break;
    }
    aRoot         = aCandidate;
    aRootValue    = aCandidateValue;
    aShiftedValue = aCandidateShiftedValue;
  }
  AddRoot(theResult, theEpsX, aRoot, aRootValue);
  return MathUtils::Status::OK;
}

//! Derivative-aware implementation mirroring the proven math_FunctionRoots heuristics
//! while operating on the modern callable-based MathRoot API.
//! @tparam Function type with Value() and Values() methods
template <typename Function>
MultipleResult FindAllRootsWithDerivativeImpl(Function&             theFunc,
                                              double                theLower,
                                              double                theUpper,
                                              const MultipleConfig& theConfig)
{
  MultipleResult aResult;
  if (!IsValidMultipleInput(theLower, theUpper, theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  aResult.Status = MathUtils::Status::OK;

  const double aLower         = theLower;
  const double aUpper         = theUpper;
  const size_t aNbSamples     = std::max<size_t>(2 * theConfig.NbSamples, 20);
  const double aDx            = (aUpper / aNbSamples) - (aLower / aNbSamples);
  const double aEpsX          = EffectiveXTolerance(aLower, aUpper, theConfig.XTolerance);
  const double aRawXTolerance = theConfig.XTolerance;
  const double aMajorDx       = 5.0 * aDx;

  math_Vector aValues(aNbSamples + 1);
  math_Vector aXValues(aNbSamples + 1);
  for (size_t i = 0; i <= aNbSamples; ++i)
  {
    const double aRatio  = static_cast<double>(i) / aNbSamples;
    const double aX      = (1.0 - aRatio) * aLower + aRatio * aUpper;
    aXValues.ChangeAt(i) = aX;

    double aValue = 0.0;
    if (!theFunc.Value(aX, aValue))
    {
      aResult.Status = MathUtils::Status::CallbackError;
      return aResult;
    }
    aValue -= theConfig.Offset;
    if (!std::isfinite(aValue))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    aValues.ChangeAt(i) = aValue;
  }

  aResult.IsAllNull = true;
  for (size_t i = 0; i <= aNbSamples; ++i)
  {
    if (aValues.At(i) > theConfig.NullTolerance || aValues.At(i) < -theConfig.NullTolerance)
    {
      aResult.IsAllNull = false;
      break;
    }
  }
  if (aResult.IsAllNull)
  {
    return aResult;
  }

  const double aTolerance = aEpsX;
  for (size_t i = 0, anIp1 = 1; i < aNbSamples; ++i, ++anIp1)
  {
    const double aRatio1 = static_cast<double>(i) / aNbSamples;
    const double aRatio2 = static_cast<double>(anIp1) / aNbSamples;
    const double aX1     = (1.0 - aRatio1) * aLower + aRatio1 * aUpper;
    const double aX2     = (1.0 - aRatio2) * aLower + aRatio2 * aUpper;

    if (HaveOppositeSigns(aValues.At(i), aValues.At(anIp1)))
    {
      const MathUtils::Status aStatus = RefineBracketedRoot(theFunc,
                                                            theConfig.Offset,
                                                            aX1,
                                                            aValues.At(i),
                                                            aX2,
                                                            aValues.At(anIp1),
                                                            aTolerance,
                                                            theConfig.FTolerance,
                                                            theConfig.MaxIterations,
                                                            aEpsX,
                                                            aResult);
      if (aStatus != MathUtils::Status::OK)
      {
        aResult.Status = aStatus;
        return aResult;
      }
    }
  }

  for (size_t i = 0; i <= aNbSamples; ++i)
  {
    if (aValues.At(i) != 0.0)
    {
      continue;
    }

    const double aZeroX  = aXValues.At(i);
    double       aLeftX  = aZeroX - 0.5 * aDx;
    double       aRightX = aZeroX + 0.5 * aDx;
    if (aLeftX < aLower)
    {
      aLeftX = aLower;
    }
    if (aLeftX > aUpper)
    {
      aLeftX = aUpper;
    }
    if (aRightX < aLower)
    {
      aRightX = aLower;
    }
    if (aRightX > aUpper)
    {
      aRightX = aUpper;
    }

    double aLeftY  = 0.0;
    double aRightY = 0.0;
    MathUtils::Status anEvaluationStatus =
      EvaluateShiftedValue(theFunc, aLeftX, theConfig.Offset, aLeftY);
    if (anEvaluationStatus == MathUtils::Status::OK)
    {
      anEvaluationStatus = EvaluateShiftedValue(theFunc, aRightX, theConfig.Offset, aRightY);
    }
    if (anEvaluationStatus != MathUtils::Status::OK)
    {
      aResult.Status = anEvaluationStatus;
      return aResult;
    }

    if (HaveOppositeSigns(aLeftY, aRightY))
    {
      const MathUtils::Status aStatus = RefineBracketedRoot(theFunc,
                                                            theConfig.Offset,
                                                            aLeftX,
                                                            aLeftY,
                                                            aRightX,
                                                            aRightY,
                                                            aTolerance,
                                                            theConfig.FTolerance,
                                                            theConfig.MaxIterations,
                                                            aEpsX,
                                                            aResult);
      if (aStatus != MathUtils::Status::OK)
      {
        aResult.Status = aStatus;
        return aResult;
      }
    }
    else if (aLeftY != 0.0 || aRightY != 0.0)
    {
      double aRootValue = 0.0;
      const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aZeroX, aRootValue);
      if (anEvaluationStatus != MathUtils::Status::OK)
      {
        aResult.Status = anEvaluationStatus;
        return aResult;
      }
      AddRoot(aResult, aEpsX, aZeroX, aRootValue);
    }
  }

  if (aValues.At(0) <= theConfig.FTolerance && aValues.At(0) >= -theConfig.FTolerance)
  {
    double aRootValue = 0.0;
    const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aLower, aRootValue);
    if (anEvaluationStatus != MathUtils::Status::OK)
    {
      aResult.Status = anEvaluationStatus;
      return aResult;
    }
    AddRoot(aResult, aEpsX, aLower, aRootValue);
  }

  if (aValues.At(aNbSamples) <= theConfig.FTolerance
      && aValues.At(aNbSamples) >= -theConfig.FTolerance)
  {
    double aRootValue = 0.0;
    const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aUpper, aRootValue);
    if (anEvaluationStatus != MathUtils::Status::OK)
    {
      aResult.Status = anEvaluationStatus;
      return aResult;
    }
    AddRoot(aResult, aEpsX, aUpper, aRootValue);
  }

  size_t anIm1 = 0;
  size_t anIp1 = 2;
  double aMidX = aLower + aDx;
  for (size_t i = 1; i < aNbSamples; ++i, ++anIm1, ++anIp1, aMidX += aDx)
  {
    if (aMidX > aUpper)
    {
      aMidX = aUpper;
    }

    bool isRediscretize = false;
    if (aValues.At(i) > 0.0)
    {
      if (aValues.At(anIm1) > aValues.At(i) && aValues.At(anIp1) > aValues.At(i))
      {
        double aProbeX  = std::max(aLower, aMidX - aDx);
        double aProbeY  = 0.0;
        double aProbeDy = 0.0;
        const MathUtils::Status anEvaluationStatus =
          EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy);
        if (anEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = anEvaluationStatus;
          return aResult;
        }

        if (std::abs(aProbeDy) > 1.0e-10)
        {
          const double aStep = aProbeY / aProbeDy;
          if (aStep < aMajorDx && aStep > -aMajorDx)
          {
            isRediscretize = true;
          }
        }

        if (!isRediscretize)
        {
          aProbeX = std::min(aUpper, aMidX + aDx);
          const MathUtils::Status anEvaluationStatus =
            EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy);
          if (anEvaluationStatus != MathUtils::Status::OK)
          {
            aResult.Status = anEvaluationStatus;
            return aResult;
          }

          if (std::abs(aProbeDy) > 1.0e-10)
          {
            const double aStep = aProbeY / aProbeDy;
            if (aStep < aMajorDx && aStep > -aMajorDx)
            {
              isRediscretize = true;
            }
          }
        }
      }
    }
    else if (aValues.At(i) < 0.0)
    {
      if (aValues.At(anIm1) < aValues.At(i) && aValues.At(anIp1) < aValues.At(i))
      {
        double aProbeX  = std::max(aLower, aMidX - aDx);
        double aProbeY  = 0.0;
        double aProbeDy = 0.0;
        const MathUtils::Status anEvaluationStatus =
          EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy);
        if (anEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = anEvaluationStatus;
          return aResult;
        }

        if (std::abs(aProbeDy) > 1.0e-10)
        {
          const double aStep = aProbeY / aProbeDy;
          if (aStep < aMajorDx && aStep > -aMajorDx)
          {
            isRediscretize = true;
          }
        }

        if (!isRediscretize)
        {
          aProbeX = std::min(aUpper, aMidX + aDx);
          const MathUtils::Status anEvaluationStatus =
            EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy);
          if (anEvaluationStatus != MathUtils::Status::OK)
          {
            aResult.Status = anEvaluationStatus;
            return aResult;
          }

          if (std::abs(aProbeDy) > 1.0e-10)
          {
            const double aStep = aProbeY / aProbeDy;
            if (aStep < aMajorDx && aStep > -aMajorDx)
            {
              isRediscretize = true;
            }
          }
        }
      }
    }

    if (!isRediscretize)
    {
      continue;
    }

    double aX0      = std::max(aLower, aMidX - aDx);
    double aX3      = std::min(aUpper, aMidX + aDx);
    double aRoot1   = 0.0;
    double aRoot2   = 0.0;
    double aVal1    = 0.0;
    double aVal2    = 0.0;
    double aDer1    = 0.0;
    double aDer2    = 0.0;
    bool   hasRoot1 = false;
    bool   hasRoot2 = false;

    MultipleDerivativeValueWrapper<Function> aDerivativeWrapper{theFunc};
    MathUtils::Config                        aDerivativeConfig;
    aDerivativeConfig.XTolerance    = aRawXTolerance;
    aDerivativeConfig.FTolerance    = 0.0;
    aDerivativeConfig.MaxIterations = theConfig.MaxIterations;

    MathUtils::ScalarResult aDerivativeRoot =
      Brent(aDerivativeWrapper, aX0, aX3, aDerivativeConfig);
    aResult.NbIterations += aDerivativeRoot.NbIterations;
    if (aDerivativeRoot.IsDone() && aDerivativeRoot.Root.has_value())
    {
      aRoot1                = *aDerivativeRoot.Root;
      double aOriginalValue = 0.0;
      const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aRoot1, aOriginalValue);
      if (anEvaluationStatus != MathUtils::Status::OK)
      {
        aResult.Status = anEvaluationStatus;
        return aResult;
      }
      aVal1 = std::abs(aOriginalValue - theConfig.Offset);
      if (aVal1 < theConfig.FTolerance)
      {
        hasRoot1 = true;
        const MathUtils::Status anEvaluationStatus =
          EvaluateShiftedValues(theFunc, aRoot1, theConfig.Offset, aOriginalValue, aDer1);
        if (anEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = anEvaluationStatus;
          return aResult;
        }
      }
    }

    double       aXProbe1        = 0.0;
    double       aXProbe2        = 0.0;
    const double aTolCR          = aEpsX * 10.0;
    const double aLocalTolX      = 0.001 * aEpsX;
    double       aF0             = aValues.At(anIm1);
    double       aF3             = aValues.At(anIp1);
    const bool   isSearchMinimum = (aF0 > 0.0);

    if (std::abs(aX3 - aMidX) > std::abs(aX0 - aMidX))
    {
      aXProbe1 = aMidX;
      aXProbe2 = aMidX + MathUtils::THE_GOLDEN_SECTION * (aX3 - aMidX);
    }
    else
    {
      aXProbe2 = aMidX;
      aXProbe1 = aMidX - MathUtils::THE_GOLDEN_SECTION * (aMidX - aX0);
    }

    double aF1 = 0.0;
    double aF2 = 0.0;
    MathUtils::Status anEvaluationStatus =
      EvaluateShiftedValue(theFunc, aXProbe1, theConfig.Offset, aF1);
    if (anEvaluationStatus == MathUtils::Status::OK)
    {
      anEvaluationStatus = EvaluateShiftedValue(theFunc, aXProbe2, theConfig.Offset, aF2);
    }
    if (anEvaluationStatus != MathUtils::Status::OK)
    {
      aResult.Status = anEvaluationStatus;
      return aResult;
    }

    while (std::abs(aX3 - aX0) > aTolCR * (std::abs(aXProbe1) + std::abs(aXProbe2))
           && std::abs(aXProbe1 - aXProbe2) > aLocalTolX)
    {
      if (isSearchMinimum)
      {
        if (aF2 < aF1)
        {
          aX0      = aXProbe1;
          aXProbe1 = aXProbe2;
          aXProbe2 = (1.0 - MathUtils::THE_GOLDEN_SECTION) * aXProbe1
                     + MathUtils::THE_GOLDEN_SECTION * aX3;
          aF0      = aF1;
          aF1      = aF2;
          const MathUtils::Status anEvaluationStatus =
            EvaluateShiftedValue(theFunc, aXProbe2, theConfig.Offset, aF2);
          if (anEvaluationStatus != MathUtils::Status::OK)
          {
            aResult.Status = anEvaluationStatus;
            return aResult;
          }
        }
        else
        {
          aX3      = aXProbe2;
          aXProbe2 = aXProbe1;
          aXProbe1 = (1.0 - MathUtils::THE_GOLDEN_SECTION) * aXProbe2
                     + MathUtils::THE_GOLDEN_SECTION * aX0;
          aF3      = aF2;
          aF2      = aF1;
          const MathUtils::Status anEvaluationStatus =
            EvaluateShiftedValue(theFunc, aXProbe1, theConfig.Offset, aF1);
          if (anEvaluationStatus != MathUtils::Status::OK)
          {
            aResult.Status = anEvaluationStatus;
            return aResult;
          }
        }
      }
      else
      {
        if (aF2 > aF1)
        {
          aX0      = aXProbe1;
          aXProbe1 = aXProbe2;
          aXProbe2 = (1.0 - MathUtils::THE_GOLDEN_SECTION) * aXProbe1
                     + MathUtils::THE_GOLDEN_SECTION * aX3;
          aF0      = aF1;
          aF1      = aF2;
          const MathUtils::Status anEvaluationStatus =
            EvaluateShiftedValue(theFunc, aXProbe2, theConfig.Offset, aF2);
          if (anEvaluationStatus != MathUtils::Status::OK)
          {
            aResult.Status = anEvaluationStatus;
            return aResult;
          }
        }
        else
        {
          aX3      = aXProbe2;
          aXProbe2 = aXProbe1;
          aXProbe1 = (1.0 - MathUtils::THE_GOLDEN_SECTION) * aXProbe2
                     + MathUtils::THE_GOLDEN_SECTION * aX0;
          aF3      = aF2;
          aF2      = aF1;
          const MathUtils::Status anEvaluationStatus =
            EvaluateShiftedValue(theFunc, aXProbe1, theConfig.Offset, aF1);
          if (anEvaluationStatus != MathUtils::Status::OK)
          {
            aResult.Status = anEvaluationStatus;
            return aResult;
          }
        }
      }

      if (HaveOppositeSigns(aF1, aF0))
      {
        const MathUtils::Status aStatus = RefineBracketedRoot(theFunc,
                                                              theConfig.Offset,
                                                              aX0,
                                                              aF0,
                                                              aXProbe1,
                                                              aF1,
                                                              aTolerance,
                                                              theConfig.FTolerance,
                                                              theConfig.MaxIterations,
                                                              aEpsX,
                                                              aResult);
        if (aStatus != MathUtils::Status::OK)
        {
          aResult.Status = aStatus;
          return aResult;
        }
      }

      if (HaveOppositeSigns(aF2, aF3))
      {
        const MathUtils::Status aStatus = RefineBracketedRoot(theFunc,
                                                              theConfig.Offset,
                                                              aXProbe2,
                                                              aF2,
                                                              aX3,
                                                              aF3,
                                                              aTolerance,
                                                              theConfig.FTolerance,
                                                              theConfig.MaxIterations,
                                                              aEpsX,
                                                              aResult);
        if (aStatus != MathUtils::Status::OK)
        {
          aResult.Status = aStatus;
          return aResult;
        }
      }
    }

    if ((isSearchMinimum && aF1 < aF2) || (!isSearchMinimum && aF1 > aF2))
    {
      if (std::abs(aF1) < theConfig.FTolerance)
      {
        hasRoot2 = true;
        aRoot2   = aXProbe1;
        aVal2    = std::abs(aF1);
      }
    }
    else if (std::abs(aF2) < theConfig.FTolerance)
    {
      hasRoot2 = true;
      aRoot2   = aXProbe2;
      aVal2    = std::abs(aF2);
    }

    if (hasRoot1 && hasRoot2)
    {
      if (aVal2 - aVal1 > theConfig.FTolerance)
      {
        double aRootValue = 0.0;
        const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aRoot1, aRootValue);
        if (anEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = anEvaluationStatus;
          return aResult;
        }
        AddRoot(aResult, aEpsX, aRoot1, aRootValue);
      }
      else if (aVal1 - aVal2 > theConfig.FTolerance)
      {
        double aRootValue = 0.0;
        const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aRoot2, aRootValue);
        if (anEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = anEvaluationStatus;
          return aResult;
        }
        AddRoot(aResult, aEpsX, aRoot2, aRootValue);
      }
      else
      {
        double aShiftedValue = 0.0;
        const MathUtils::Status anEvaluationStatus =
          EvaluateShiftedValues(theFunc, aRoot2, theConfig.Offset, aShiftedValue, aDer2);
        if (anEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = anEvaluationStatus;
          return aResult;
        }

        const double aChosenRoot = (std::abs(aDer1) < std::abs(aDer2)) ? aRoot1 : aRoot2;
        double       aRootValue  = 0.0;
        const MathUtils::Status aRootEvaluationStatus =
          EvaluateValue(theFunc, aChosenRoot, aRootValue);
        if (aRootEvaluationStatus != MathUtils::Status::OK)
        {
          aResult.Status = aRootEvaluationStatus;
          return aResult;
        }
        AddRoot(aResult, aEpsX, aChosenRoot, aRootValue);
      }
    }
    else if (hasRoot1)
    {
      double aRootValue = 0.0;
      const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aRoot1, aRootValue);
      if (anEvaluationStatus != MathUtils::Status::OK)
      {
        aResult.Status = anEvaluationStatus;
        return aResult;
      }
      AddRoot(aResult, aEpsX, aRoot1, aRootValue);
    }
    else if (hasRoot2)
    {
      double aRootValue = 0.0;
      const MathUtils::Status anEvaluationStatus = EvaluateValue(theFunc, aRoot2, aRootValue);
      if (anEvaluationStatus != MathUtils::Status::OK)
      {
        aResult.Status = anEvaluationStatus;
        return aResult;
      }
      AddRoot(aResult, aEpsX, aRoot2, aRootValue);
    }
  }

  SortRoots(aResult);
  return aResult;
}

// ============================================================================
//  Interval handlers
// ============================================================================

//! No-op interval handler for functions without derivative.
struct MultipleNoExtraHandler
{
  void operator()(size_t, double, double, double, double, MultipleResult&, double) const {}
};

// ============================================================================
//  Core implementation
// ============================================================================

//! Core implementation for finding all roots in an interval.
//! Shared logic for both Value-only and Values (with derivative) interfaces.
//!
//! @tparam SampleFn callable (size_t theIndex, double theX) -> bool
//! @tparam GetValueFn callable (size_t theIndex) -> double, returns f(x)-offset at sample
//! @tparam BrentWrapperT type with Value(double, double&) for Brent root finding
//! @tparam GetRootValueFn callable (double theX) -> double, returns original f(x)
//! @tparam IntervalExtraFn callable (size_t, x0, x1, f0, f1, result, epsX) -> void
template <typename SampleFn,
          typename GetValueFn,
          typename BrentWrapperT,
          typename GetRootValueFn,
          typename IntervalExtraFn>
MultipleResult FindAllRootsImpl(double                theLower,
                                double                theUpper,
                                const MultipleConfig& theConfig,
                                SampleFn              theSampleFn,
                                GetValueFn            theGetValue,
                                BrentWrapperT&        theBrentWrapper,
                                GetRootValueFn        theGetRootValue,
                                IntervalExtraFn       theIntervalExtra)
{
  MultipleResult aResult;
  if (!IsValidMultipleInput(theLower, theUpper, theConfig))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  aResult.Status = MathUtils::Status::OK;

  const double aLower = theLower;
  const double aUpper = theUpper;

  // Minimum samples
  const size_t aNbSamples = std::max<size_t>(2 * theConfig.NbSamples, 20);
  // Ensure EpsX is not too small relative to interval
  const double aEpsX = EffectiveXTolerance(aLower, aUpper, theConfig.XTolerance);

  // Sample function values
  math_Vector aXValues(aNbSamples + 1);
  for (size_t i = 0; i <= aNbSamples; ++i)
  {
    const double aRatio  = static_cast<double>(i) / aNbSamples;
    const double aX      = (1.0 - aRatio) * aLower + aRatio * aUpper;
    aXValues.ChangeAt(i) = aX;

    if (!theSampleFn(i, aX))
    {
      aResult.Status = theSampleFn.Status;
      return aResult;
    }
  }

  // Check if function is essentially null everywhere
  aResult.IsAllNull = true;
  for (size_t i = 0; i <= aNbSamples; ++i)
  {
    if (std::abs(theGetValue(i)) > theConfig.NullTolerance)
    {
      aResult.IsAllNull = false;
      break;
    }
  }

  if (aResult.IsAllNull)
  {
    return aResult;
  }

  // Find sign changes
  for (size_t i = 0; i < aNbSamples; ++i)
  {
    const double aF0 = theGetValue(i);
    const double aF1 = theGetValue(i + 1);
    const double aX0 = aXValues.At(i);
    const double aX1 = aXValues.At(i + 1);

    // Exact zero at sample point
    if (std::abs(aF0) <= theConfig.FTolerance)
    {
      double aRootValue = 0.0;
      if (!theGetRootValue(aX0, aRootValue))
      {
        aResult.Status = theGetRootValue.Status;
        return aResult;
      }
      AddRoot(aResult, aEpsX, aX0, aRootValue);
      continue;
    }

    if (HaveOppositeSigns(aF0, aF1))
    {
      MathUtils::Config aBrentConfig;
      aBrentConfig.XTolerance    = aEpsX;
      aBrentConfig.FTolerance    = theConfig.FTolerance;
      aBrentConfig.MaxIterations = theConfig.MaxIterations;

      MathUtils::ScalarResult aBrentResult = Brent(theBrentWrapper, aX0, aX1, aBrentConfig);
      aResult.NbIterations += aBrentResult.NbIterations;

      if (!aBrentResult.IsDone() || !aBrentResult.Root.has_value())
      {
        aResult.Status = aBrentResult.Status;
        return aResult;
      }
      double aRootValue = 0.0;
      if (!theGetRootValue(*aBrentResult.Root, aRootValue))
      {
        aResult.Status = theGetRootValue.Status;
        return aResult;
      }
      AddRoot(aResult, aEpsX, *aBrentResult.Root, aRootValue);
    }
    else
    {
      // Additional per-interval processing (e.g., tangential root detection)
      theIntervalExtra(i, aX0, aX1, aF0, aF1, aResult, aEpsX);
    }
  }

  // Check last sample point
  if (std::abs(theGetValue(aNbSamples)) <= theConfig.FTolerance)
  {
    double aRootValue = 0.0;
    if (!theGetRootValue(aXValues.At(aNbSamples), aRootValue))
    {
      aResult.Status = theGetRootValue.Status;
      return aResult;
    }
    AddRoot(aResult, aEpsX, aXValues.At(aNbSamples), aRootValue);
  }

  SortRoots(aResult);
  return aResult;
}

} // namespace Utils
} // namespace MathRoot

#endif // _MathRoot_MultipleUtils_HeaderFile
