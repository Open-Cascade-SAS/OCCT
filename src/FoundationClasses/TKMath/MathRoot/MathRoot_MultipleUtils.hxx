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
#include <math_Vector.hxx>

#include <NCollection_Vector.hxx>

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
  MathUtils::Status          Status       = MathUtils::Status::NotConverged; //!< Computation status
  size_t                     NbIterations = 0;  //!< Total iterations across all roots
  NCollection_Vector<double> Roots;             //!< Found roots (sorted)
  NCollection_Vector<double> Values;            //!< Function values at roots
  bool                       IsAllNull = false; //!< True if function is essentially zero in range

  //! Returns true if computation succeeded.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Conversion to bool for convenient checking.
  explicit operator bool() const { return IsDone(); }

  //! Returns the number of roots found.
  int NbRoots() const { return Roots.Length(); }

  //! Access root by index (0-based).
  double operator[](int theIndex) const { return Roots.Value(theIndex); }
};

//! Configuration for multiple root finding.
struct MultipleConfig
{
  int    NbSamples     = 100;   //!< Number of sample points for initial search
  double XTolerance    = 1e-10; //!< Tolerance on X for convergence
  double FTolerance    = 1e-10; //!< Tolerance on F(X) for convergence
  double NullTolerance = 1e-12; //!< Tolerance to consider function as null
  int    MaxIterations = 100;   //!< Max iterations per root refinement
  double Offset        = 0.0;   //!< Find roots of f(x) - Offset = 0
};

// ============================================================================
//  Helper functions
// ============================================================================

//! In-place insertion sort of roots and corresponding values by ascending root value.
inline void SortRoots(MultipleResult& theResult)
{
  for (int i = 1; i < theResult.Roots.Length(); ++i)
  {
    const double aKeyRoot = theResult.Roots[i];
    const double aKeyVal  = theResult.Values[i];
    int          j        = i - 1;
    while (j >= 0 && theResult.Roots[j] > aKeyRoot)
    {
      theResult.Roots[j + 1]  = theResult.Roots[j];
      theResult.Values[j + 1] = theResult.Values[j];
      --j;
    }
    theResult.Roots[j + 1]  = aKeyRoot;
    theResult.Values[j + 1] = aKeyVal;
  }
}

//! Helper to add a root if it is not a duplicate of an already found root.
inline void AddRoot(MultipleResult& theResult, double theEpsX, double theRoot, double theValue)
{
  for (int k = 0; k < theResult.Roots.Length(); ++k)
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
  const double aMinEpsX = 1.0e-10 * (std::abs(theLower) + std::abs(theUpper));
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
  Function&    myFunc;
  math_Vector& mySamples;
  const double myOffset;

  bool operator()(int theIndex, double theX) const
  {
    double aF = 0.0;
    if (!myFunc.Value(theX, aF))
      return false;
    mySamples(theIndex) = aF - myOffset;
    return true;
  }
};

//! Returns the sampled value at a given index from a math_Vector.
struct MultipleGetValueFn
{
  const math_Vector& mySamples;

  double operator()(int theIndex) const { return mySamples(theIndex); }
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
  Function& myFunc;

  double operator()(double theX) const
  {
    double aF = 0.0;
    myFunc.Value(theX, aF);
    return aF;
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
    return myFunc.Values(theX, aF, theY);
  }
};

//! Evaluate the original function value.
//! @tparam Function type with Value(double theX, double& theF) method
template <typename Function>
bool EvaluateValue(Function& theFunc, double theX, double& theValue)
{
  return theFunc.Value(theX, theValue);
}

//! Evaluate the function value shifted by the requested offset.
//! @tparam Function type with Value(double theX, double& theF) method
template <typename Function>
bool EvaluateShiftedValue(Function& theFunc, double theX, double theOffset, double& theValue)
{
  if (!theFunc.Value(theX, theValue))
  {
    return false;
  }

  theValue -= theOffset;
  return true;
}

//! Evaluate the function value and derivative, then shift the value by the requested offset.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
template <typename Function>
bool EvaluateShiftedValues(Function& theFunc,
                           double    theX,
                           double    theOffset,
                           double&   theValue,
                           double&   theDerivative)
{
  if (!theFunc.Values(theX, theValue, theDerivative))
  {
    return false;
  }

  theValue -= theOffset;
  return true;
}

//! Refine a bracketed sign change using the same Brent/Newton sequence as math_FunctionRoots.
//! @tparam Function type with Value() and Values() methods
template <typename Function>
bool RefineBracketedRoot(Function&       theFunc,
                         double          theOffset,
                         double          theX1,
                         double          theY1,
                         double          theX2,
                         double          theY2,
                         double          theTolerance,
                         double          theEpsX,
                         MultipleResult& theResult)
{
  constexpr int    THE_MAX_ITERATIONS = 100;
  constexpr double THE_EPS2           = 2.0e-14;
  constexpr double THE_DERIV_EPS      = 1.0e-10;

  int    anIter = 0;
  double aTol2  = 0.5 * theTolerance;
  double aA     = theX1;
  double aB     = theX2;
  double aC     = theX2;
  double aD     = 0.0;
  double anE    = 0.0;
  double aFa    = theY1;
  double aFb    = theY2;
  double aFc    = theY2;

  for (anIter = 1; anIter <= THE_MAX_ITERATIONS; ++anIter)
  {
    if ((aFb > 0.0 && aFc > 0.0) || (aFb < 0.0 && aFc < 0.0))
    {
      aC  = aA;
      aFc = aFa;
      anE = aD = aB - aA;
    }

    if (std::abs(aFc) < std::abs(aFb))
    {
      const double aPrevA  = aA;
      const double aPrevFa = aFa;
      aA                   = aB;
      aB                   = aC;
      aC                   = aPrevA;
      aFa                  = aFb;
      aFb                  = aFc;
      aFc                  = aPrevFa;
    }

    const double aTol1 = THE_EPS2 * std::abs(aB) + aTol2;
    const double aXm   = 0.5 * (aC - aB);
    if (std::abs(aXm) < aTol1 || aFb == 0.0)
    {
      double aNewtonX = aB;
      for (int aNewtonIter = 0; aNewtonIter < 5; ++aNewtonIter)
      {
        double aY    = 0.0;
        double aDfdx = 0.0;
        if (!EvaluateShiftedValues(theFunc, aNewtonX, theOffset, aY, aDfdx))
        {
          return false;
        }

        if (std::abs(aDfdx) <= THE_DERIV_EPS)
        {
          break;
        }

        aNewtonX -= aY / aDfdx;
        if (aNewtonX < theX1 || aNewtonX > theX2)
        {
          break;
        }

        if (!EvaluateShiftedValue(theFunc, aNewtonX, theOffset, aY))
        {
          return false;
        }

        if (std::abs(aY) < std::abs(aFb))
        {
          aB  = aNewtonX;
          aFb = aY;
        }
      }

      double aRootValue = 0.0;
      if (!EvaluateValue(theFunc, aB, aRootValue))
      {
        return false;
      }

      theResult.NbIterations += static_cast<size_t>(anIter);
      AddRoot(theResult, theEpsX, aB, aRootValue);
      return true;
    }

    if (std::abs(anE) >= aTol1 && std::abs(aFa) > std::abs(aFb))
    {
      double       aP = 0.0;
      double       aQ = 0.0;
      const double aS = aFb / aFa;
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
      const double aMin1 = 3.0 * aXm * aQ - std::abs(aTol1 * aQ);
      const double aMin2 = std::abs(anE * aQ);
      if (2.0 * aP < std::min(aMin1, aMin2))
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
    if (std::abs(aD) > aTol1)
    {
      aB += aD;
    }
    else
    {
      aB += (aXm >= 0.0) ? std::abs(aTol1) : -std::abs(aTol1);
    }

    if (!EvaluateShiftedValue(theFunc, aB, theOffset, aFb))
    {
      return false;
    }
  }

  theResult.NbIterations += THE_MAX_ITERATIONS;
  return true;
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
  aResult.Status = MathUtils::Status::OK;

  const double aLower         = std::min(theLower, theUpper);
  const double aUpper         = std::max(theLower, theUpper);
  const int    aNbSamples     = std::max(2 * theConfig.NbSamples, 20);
  const double aDx            = (aUpper - aLower) / aNbSamples;
  const double aEpsX          = EffectiveXTolerance(aLower, aUpper, theConfig.XTolerance);
  const double aRawXTolerance = theConfig.XTolerance;
  const double aMajorDx       = 5.0 * aDx;

  math_Vector aValues(0, aNbSamples);
  double      aX = aLower;
  for (int i = 0; i <= aNbSamples; ++i, aX += aDx)
  {
    if (aX > aUpper)
    {
      aX = aUpper;
    }

    if (!EvaluateShiftedValue(theFunc, aX, theConfig.Offset, aValues(i)))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
  }

  aResult.IsAllNull = true;
  for (int i = 0; i <= aNbSamples; ++i)
  {
    if (aValues(i) > theConfig.NullTolerance || aValues(i) < -theConfig.NullTolerance)
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
  double       aX1        = aLower;
  for (int i = 0, anIp1 = 1; i < aNbSamples; ++i, ++anIp1, aX1 += aDx)
  {
    double aX2 = aX1 + aDx;
    if (aX2 > aUpper)
    {
      aX2 = aUpper;
    }

    if ((aValues(i) < 0.0 && aValues(anIp1) > 0.0) || (aValues(i) > 0.0 && aValues(anIp1) < 0.0))
    {
      if (!RefineBracketedRoot(theFunc,
                               theConfig.Offset,
                               aX1,
                               aValues(i),
                               aX2,
                               aValues(anIp1),
                               aTolerance,
                               aEpsX,
                               aResult))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
    }
  }

  for (int i = 0; i <= aNbSamples; ++i)
  {
    if (aValues(i) != 0.0)
    {
      continue;
    }

    const double aZeroX  = std::min(aLower + i * aDx, aUpper);
    double       aLeftX  = aZeroX + 0.5 * aDx;
    double       aRightX = aLeftX;
    aLeftX               = aZeroX + 0.5 * aDx;
    aRightX              = aZeroX + 0.5 * aDx;
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
    if (!EvaluateShiftedValue(theFunc, aLeftX, theConfig.Offset, aLeftY)
        || !EvaluateShiftedValue(theFunc, aRightX, theConfig.Offset, aRightY))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }

    if (aLeftY * aRightY < 0.0)
    {
      if (!RefineBracketedRoot(theFunc,
                               theConfig.Offset,
                               aLeftX,
                               aLeftY,
                               aRightX,
                               aRightY,
                               aTolerance,
                               aEpsX,
                               aResult))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
    }
    else if (aLeftY != 0.0 || aRightY != 0.0)
    {
      double aRootValue = 0.0;
      if (!EvaluateValue(theFunc, aZeroX, aRootValue))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      AddRoot(aResult, aEpsX, aZeroX, aRootValue);
    }
  }

  if (aValues(0) <= theConfig.FTolerance && aValues(0) >= -theConfig.FTolerance)
  {
    double aRootValue = 0.0;
    if (!EvaluateValue(theFunc, aLower, aRootValue))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    AddRoot(aResult, aEpsX, aLower, aRootValue);
  }

  if (aValues(aNbSamples) <= theConfig.FTolerance && aValues(aNbSamples) >= -theConfig.FTolerance)
  {
    double aRootValue = 0.0;
    if (!EvaluateValue(theFunc, aUpper, aRootValue))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
    AddRoot(aResult, aEpsX, aUpper, aRootValue);
  }

  int    anIm1 = 0;
  int    anIp1 = 2;
  double aMidX = aLower + aDx;
  for (int i = 1; i < aNbSamples; ++i, ++anIm1, ++anIp1, aMidX += aDx)
  {
    if (aMidX > aUpper)
    {
      aMidX = aUpper;
    }

    bool isRediscretize = false;
    if (aValues(i) > 0.0)
    {
      if (aValues(anIm1) > aValues(i) && aValues(anIp1) > aValues(i))
      {
        double aProbeX  = std::max(aLower, aMidX - aDx);
        double aProbeY  = 0.0;
        double aProbeDy = 0.0;
        if (!EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy))
        {
          aResult.Status = MathUtils::Status::NumericalError;
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
          if (!EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy))
          {
            aResult.Status = MathUtils::Status::NumericalError;
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
    else if (aValues(i) < 0.0)
    {
      if (aValues(anIm1) < aValues(i) && aValues(anIp1) < aValues(i))
      {
        double aProbeX  = std::max(aLower, aMidX - aDx);
        double aProbeY  = 0.0;
        double aProbeDy = 0.0;
        if (!EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy))
        {
          aResult.Status = MathUtils::Status::NumericalError;
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
          aProbeX = std::max(aLower, aMidX - aDx);
          if (!EvaluateShiftedValues(theFunc, aProbeX, theConfig.Offset, aProbeY, aProbeDy))
          {
            aResult.Status = MathUtils::Status::NumericalError;
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

    auto aDerivativeRoot = Brent(aDerivativeWrapper, aX0, aX3, aDerivativeConfig);
    aResult.NbIterations += aDerivativeRoot.NbIterations;
    if (aDerivativeRoot.IsDone() && aDerivativeRoot.Root.has_value())
    {
      aRoot1                = *aDerivativeRoot.Root;
      double aOriginalValue = 0.0;
      if (!EvaluateValue(theFunc, aRoot1, aOriginalValue))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      aVal1 = std::abs(aOriginalValue - theConfig.Offset);
      if (aVal1 < theConfig.FTolerance)
      {
        hasRoot1 = true;
        if (!EvaluateShiftedValues(theFunc, aRoot1, theConfig.Offset, aOriginalValue, aDer1))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
      }
    }

    double           aXProbe1         = 0.0;
    double           aXProbe2         = 0.0;
    constexpr double THE_GOLDEN_RATIO = 0.61803399;
    constexpr double THE_GOLDEN_COMP  = 1.0 - THE_GOLDEN_RATIO;
    const double     aTolCR           = aEpsX * 10.0;
    const double     aLocalTolX       = 0.001 * aEpsX;
    double           aF0              = aValues(anIm1);
    double           aF3              = aValues(anIp1);
    const bool       isSearchMinimum  = (aF0 > 0.0);

    if (std::abs(aX3 - aMidX) > std::abs(aX0 - aMidX))
    {
      aXProbe1 = aMidX;
      aXProbe2 = aMidX + THE_GOLDEN_COMP * (aX3 - aMidX);
    }
    else
    {
      aXProbe2 = aMidX;
      aXProbe1 = aMidX - THE_GOLDEN_COMP * (aMidX - aX0);
    }

    double aF1 = 0.0;
    double aF2 = 0.0;
    if (!EvaluateShiftedValue(theFunc, aXProbe1, theConfig.Offset, aF1)
        || !EvaluateShiftedValue(theFunc, aXProbe2, theConfig.Offset, aF2))
    {
      aResult.Status = MathUtils::Status::NumericalError;
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
          aXProbe2 = THE_GOLDEN_RATIO * aXProbe1 + THE_GOLDEN_COMP * aX3;
          aF0      = aF1;
          aF1      = aF2;
          if (!EvaluateShiftedValue(theFunc, aXProbe2, theConfig.Offset, aF2))
          {
            aResult.Status = MathUtils::Status::NumericalError;
            return aResult;
          }
        }
        else
        {
          aX3      = aXProbe2;
          aXProbe2 = aXProbe1;
          aXProbe1 = THE_GOLDEN_RATIO * aXProbe2 + THE_GOLDEN_COMP * aX0;
          aF3      = aF2;
          aF2      = aF1;
          if (!EvaluateShiftedValue(theFunc, aXProbe1, theConfig.Offset, aF1))
          {
            aResult.Status = MathUtils::Status::NumericalError;
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
          aXProbe2 = THE_GOLDEN_RATIO * aXProbe1 + THE_GOLDEN_COMP * aX3;
          aF0      = aF1;
          aF1      = aF2;
          if (!EvaluateShiftedValue(theFunc, aXProbe2, theConfig.Offset, aF2))
          {
            aResult.Status = MathUtils::Status::NumericalError;
            return aResult;
          }
        }
        else
        {
          aX3      = aXProbe2;
          aXProbe2 = aXProbe1;
          aXProbe1 = THE_GOLDEN_RATIO * aXProbe2 + THE_GOLDEN_COMP * aX0;
          aF3      = aF2;
          aF2      = aF1;
          if (!EvaluateShiftedValue(theFunc, aXProbe1, theConfig.Offset, aF1))
          {
            aResult.Status = MathUtils::Status::NumericalError;
            return aResult;
          }
        }
      }

      if (aF1 * aF0 < 0.0)
      {
        if (!RefineBracketedRoot(theFunc,
                                 theConfig.Offset,
                                 aX0,
                                 aF0,
                                 aXProbe1,
                                 aF1,
                                 aTolerance,
                                 aEpsX,
                                 aResult))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
      }

      if (aF2 * aF3 < 0.0)
      {
        if (!RefineBracketedRoot(theFunc,
                                 theConfig.Offset,
                                 aXProbe2,
                                 aF2,
                                 aX3,
                                 aF3,
                                 aTolerance,
                                 aEpsX,
                                 aResult))
        {
          aResult.Status = MathUtils::Status::NumericalError;
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
        if (!EvaluateValue(theFunc, aRoot1, aRootValue))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        AddRoot(aResult, aEpsX, aRoot1, aRootValue);
      }
      else if (aVal1 - aVal2 > theConfig.FTolerance)
      {
        double aRootValue = 0.0;
        if (!EvaluateValue(theFunc, aRoot2, aRootValue))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        AddRoot(aResult, aEpsX, aRoot2, aRootValue);
      }
      else
      {
        double aShiftedValue = 0.0;
        if (!EvaluateShiftedValues(theFunc, aRoot2, theConfig.Offset, aShiftedValue, aDer2))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }

        const double aChosenRoot = (std::abs(aDer1) < std::abs(aDer2)) ? aRoot1 : aRoot2;
        double       aRootValue  = 0.0;
        if (!EvaluateValue(theFunc, aChosenRoot, aRootValue))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        AddRoot(aResult, aEpsX, aChosenRoot, aRootValue);
      }
    }
    else if (hasRoot1)
    {
      double aRootValue = 0.0;
      if (!EvaluateValue(theFunc, aRoot1, aRootValue))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      AddRoot(aResult, aEpsX, aRoot1, aRootValue);
    }
    else if (hasRoot2)
    {
      double aRootValue = 0.0;
      if (!EvaluateValue(theFunc, aRoot2, aRootValue))
      {
        aResult.Status = MathUtils::Status::NumericalError;
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
  void operator()(int, double, double, double, double, MultipleResult&, double) const {}
};

// ============================================================================
//  Core implementation
// ============================================================================

//! Core implementation for finding all roots in an interval.
//! Shared logic for both Value-only and Values (with derivative) interfaces.
//!
//! @tparam SampleFn callable (int theIndex, double theX) -> bool
//! @tparam GetValueFn callable (int theIndex) -> double, returns f(x)-offset at sample
//! @tparam BrentWrapperT type with Value(double, double&) for Brent root finding
//! @tparam GetRootValueFn callable (double theX) -> double, returns original f(x)
//! @tparam IntervalExtraFn callable (int, x0, x1, f0, f1, result, epsX) -> void
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
  aResult.Status = MathUtils::Status::OK;

  // Ensure proper ordering
  const double aLower = std::min(theLower, theUpper);
  const double aUpper = std::max(theLower, theUpper);

  // Minimum samples
  const int    aNbSamples = std::max(2 * theConfig.NbSamples, 20);
  const double aDx        = (aUpper - aLower) / aNbSamples;

  // Ensure EpsX is not too small relative to interval
  const double aEpsX = EffectiveXTolerance(aLower, aUpper, theConfig.XTolerance);

  // Sample function values
  math_Vector aXValues(0, aNbSamples);
  for (int i = 0; i <= aNbSamples; ++i)
  {
    double aX = aLower + i * aDx;
    if (aX > aUpper)
      aX = aUpper;
    aXValues(i) = aX;

    if (!theSampleFn(i, aX))
    {
      aResult.Status = MathUtils::Status::NumericalError;
      return aResult;
    }
  }

  // Check if function is essentially null everywhere
  aResult.IsAllNull = true;
  for (int i = 0; i <= aNbSamples; ++i)
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
  for (int i = 0; i < aNbSamples; ++i)
  {
    const double aF0 = theGetValue(i);
    const double aF1 = theGetValue(i + 1);
    const double aX0 = aXValues(i);
    const double aX1 = aXValues(i + 1);

    // Exact zero at sample point
    if (std::abs(aF0) < theConfig.FTolerance)
    {
      AddRoot(aResult, aEpsX, aX0, aF0 + theConfig.Offset);
      continue;
    }

    // Sign change detected
    if (aF0 * aF1 < 0.0)
    {
      MathUtils::Config aBrentConfig;
      aBrentConfig.XTolerance    = aEpsX;
      aBrentConfig.FTolerance    = theConfig.FTolerance;
      aBrentConfig.MaxIterations = theConfig.MaxIterations;

      MathUtils::ScalarResult aBrentResult = Brent(theBrentWrapper, aX0, aX1, aBrentConfig);
      aResult.NbIterations += aBrentResult.NbIterations;

      if (aBrentResult.IsDone() && aBrentResult.Root.has_value())
      {
        AddRoot(aResult, aEpsX, *aBrentResult.Root, theGetRootValue(*aBrentResult.Root));
      }
    }
    else
    {
      // Additional per-interval processing (e.g., tangential root detection)
      theIntervalExtra(i, aX0, aX1, aF0, aF1, aResult, aEpsX);
    }
  }

  // Check last sample point
  if (std::abs(theGetValue(aNbSamples)) < theConfig.FTolerance)
  {
    AddRoot(aResult, aEpsX, aXValues(aNbSamples), theGetValue(aNbSamples) + theConfig.Offset);
  }

  SortRoots(aResult);
  return aResult;
}

} // namespace MathRoot

#endif // _MathRoot_MultipleUtils_HeaderFile
