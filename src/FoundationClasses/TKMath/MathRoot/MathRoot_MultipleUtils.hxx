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

//! Samples a function with derivative and stores f(x)-offset and f'(x) into math_Vectors.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
template <typename Function>
struct MultipleSampleDerivFn
{
  Function&    myFunc;
  math_Vector& myFValues;
  math_Vector& myDFValues;
  const double myOffset;

  bool operator()(int theIndex, double theX) const
  {
    double aF = 0.0, aDF = 0.0;
    if (!myFunc.Values(theX, aF, aDF))
      return false;
    myFValues(theIndex)  = aF - myOffset;
    myDFValues(theIndex) = aDF;
    return true;
  }
};

//! Brent wrapper that adapts a Values (with derivative) function for offset root finding.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
template <typename Function>
struct MultipleBrentDerivWrapper
{
  Function& myFunc;
  double    myOffset;

  bool Value(double theX, double& theY) const
  {
    double aDF = 0.0;
    if (!myFunc.Values(theX, theY, aDF))
      return false;
    theY -= myOffset;
    return true;
  }
};

//! Evaluates original (non-offset) function value at a root point via Values interface.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
template <typename Function>
struct MultipleGetRootDerivFn
{
  Function& myFunc;

  double operator()(double theX) const
  {
    double aF = 0.0, aDF = 0.0;
    myFunc.Values(theX, aF, aDF);
    return aF;
  }
};

// ============================================================================
//  Interval handlers
// ============================================================================

//! No-op interval handler for functions without derivative.
struct MultipleNoExtraHandler
{
  void operator()(int, double, double, double, double, MultipleResult&, double) const {}
};

//! Tangential root detection: finds extrema that touch zero without sign change.
//! Uses derivative sign changes to locate potential minima/maxima, then bisects
//! the derivative to check whether the function value is close enough to zero.
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
template <typename Function>
struct MultipleTangentialHandler
{
  Function&          myFunc;
  const math_Vector& myDFValues;
  const double       myOffset;
  const double       myFTolerance;

  void operator()(int             theIndex,
                  double          theX0,
                  double          theX1,
                  double          theF0,
                  double          theF1,
                  MultipleResult& theResult,
                  double          theEpsX) const
  {
    if (theF0 > 0.0 && theF1 > 0.0)
    {
      // Potential minimum - check if derivative changes sign (negative to positive)
      if (myDFValues(theIndex) < 0.0 && myDFValues(theIndex + 1) > 0.0)
      {
        findTangentialRoot(theX0, theX1, true, theResult, theEpsX);
      }
    }
    else if (theF0 < 0.0 && theF1 < 0.0)
    {
      // Potential maximum - check if derivative changes sign (positive to negative)
      if (myDFValues(theIndex) > 0.0 && myDFValues(theIndex + 1) < 0.0)
      {
        findTangentialRoot(theX0, theX1, false, theResult, theEpsX);
      }
    }
  }

private:
  //! Bisects derivative to locate an extremum and checks if it touches zero.
  //! @param theIsMinimum true for minimum (derivative negative->positive),
  //!                     false for maximum (derivative positive->negative)
  void findTangentialRoot(double          theX0,
                          double          theX1,
                          bool            theIsMinimum,
                          MultipleResult& theResult,
                          double          theEpsX) const
  {
    double aXL = theX0, aXR = theX1;
    for (int anIter = 0; anIter < 20; ++anIter)
    {
      double aXM = 0.5 * (aXL + aXR);
      double aFM = 0.0, aDFM = 0.0;
      if (!myFunc.Values(aXM, aFM, aDFM))
        break;
      aFM -= myOffset;

      // For minimum: derivative goes from negative to positive
      // For maximum: derivative goes from positive to negative
      const bool isMoveRight = theIsMinimum ? (aDFM < 0.0) : (aDFM > 0.0);
      if (isMoveRight)
      {
        aXL = aXM;
      }
      else
      {
        aXR = aXM;
      }

      if (std::abs(aFM) < myFTolerance)
      {
        AddRoot(theResult, theEpsX, aXM, aFM + myOffset);
        break;
      }
    }
  }
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
  const int    aNbSamples = std::max(theConfig.NbSamples, 10);
  const double aDx        = (aUpper - aLower) / aNbSamples;

  // Ensure EpsX is not too small relative to interval
  const double aMinEpsX = 1e-10 * (std::abs(aLower) + std::abs(aUpper));
  const double aEpsX    = std::max(theConfig.XTolerance, aMinEpsX);

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
