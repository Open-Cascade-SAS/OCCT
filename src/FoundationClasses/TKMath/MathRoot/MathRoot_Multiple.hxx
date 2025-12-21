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

#ifndef _MathRoot_Multiple_HeaderFile
#define _MathRoot_Multiple_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathRoot_Brent.hxx>
#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>

#include <NCollection_Vector.hxx>

#include <cmath>

//! @file MathRoot_Multiple.hxx
//! @brief Algorithms for finding all roots of a function in a given range.
//!
//! These algorithms sample the function to find sign changes, then refine
//! each root using Brent's method. They can find multiple roots in a single call.

namespace MathRoot
{
using namespace MathUtils;

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

//! Finds all real roots of a function within the range [theLower, theUpper].
//! Uses uniform sampling to detect sign changes, then refines each root using Brent's method.
//!
//! Algorithm:
//! 1. Sample function at NbSamples uniform points
//! 2. Detect all sign changes and zero crossings
//! 3. For each bracket, apply Brent's method to find precise root
//! 4. Check for extrema that might touch zero
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function object to find roots of
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig configuration parameters
//! @return result containing all found roots
template <typename Function>
MultipleResult FindAllRoots(Function&             theFunc,
                            double                theLower,
                            double                theUpper,
                            const MultipleConfig& theConfig = MultipleConfig())
{
  MultipleResult aResult;
  aResult.Status = MathUtils::Status::OK;

  // Ensure proper ordering
  double aLower = std::min(theLower, theUpper);
  double aUpper = std::max(theLower, theUpper);

  // Minimum samples
  const int    aNbSamples = std::max(theConfig.NbSamples, 10);
  const double aDx        = (aUpper - aLower) / aNbSamples;

  // Ensure EpsX is not too small relative to interval
  const double aMinEpsX = 1e-10 * (std::abs(aLower) + std::abs(aUpper));
  const double aEpsX    = std::max(theConfig.XTolerance, aMinEpsX);

  // Sample function values
  math_Vector aSamples(0, aNbSamples);
  math_Vector aXValues(0, aNbSamples);

  bool aAllValid = true;
  for (int i = 0; i <= aNbSamples; ++i)
  {
    double aX = aLower + i * aDx;
    if (aX > aUpper)
      aX = aUpper;
    aXValues(i) = aX;

    double aF = 0.0;
    if (!theFunc.Value(aX, aF))
    {
      aAllValid = false;
      break;
    }
    aSamples(i) = aF - theConfig.Offset;
  }

  if (!aAllValid)
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  // Check if function is essentially null everywhere
  aResult.IsAllNull = true;
  for (int i = 0; i <= aNbSamples; ++i)
  {
    if (std::abs(aSamples(i)) > theConfig.NullTolerance)
    {
      aResult.IsAllNull = false;
      break;
    }
  }

  if (aResult.IsAllNull)
  {
    return aResult;
  }

  // Helper to add root if not duplicate
  auto addRoot = [&](double theRoot, double theValue) {
    // Check for duplicates
    for (int k = 0; k < aResult.Roots.Length(); ++k)
    {
      if (std::abs(theRoot - aResult.Roots.Value(k)) < aEpsX)
      {
        return;
      }
    }
    aResult.Roots.Append(theRoot);
    aResult.Values.Append(theValue);
  };

  // Create wrapper for Brent that uses Value-only interface
  class BrentWrapper
  {
  public:
    BrentWrapper(Function& theF, double theOffset)
        : myFunc(theF),
          myOffset(theOffset)
    {
    }

    bool Value(double theX, double& theY) const
    {
      if (!myFunc.Value(theX, theY))
        return false;
      theY -= myOffset;
      return true;
    }

  private:
    Function& myFunc;
    double    myOffset;
  };

  BrentWrapper aWrapper(theFunc, theConfig.Offset);

  // Find sign changes
  for (int i = 0; i < aNbSamples; ++i)
  {
    const double aF0 = aSamples(i);
    const double aF1 = aSamples(i + 1);
    const double aX0 = aXValues(i);
    const double aX1 = aXValues(i + 1);

    // Exact zero at sample point
    if (std::abs(aF0) < theConfig.FTolerance)
    {
      addRoot(aX0, aF0 + theConfig.Offset);
      continue;
    }

    // Sign change detected
    if (aF0 * aF1 < 0.0)
    {
      MathUtils::Config aBrentConfig;
      aBrentConfig.XTolerance    = aEpsX;
      aBrentConfig.FTolerance    = theConfig.FTolerance;
      aBrentConfig.MaxIterations = theConfig.MaxIterations;

      auto aBrentResult = Brent(aWrapper, aX0, aX1, aBrentConfig);
      aResult.NbIterations += aBrentResult.NbIterations;

      if (aBrentResult.IsDone() && aBrentResult.Root.has_value())
      {
        double aRootValue = 0.0;
        theFunc.Value(*aBrentResult.Root, aRootValue);
        addRoot(*aBrentResult.Root, aRootValue);
      }
    }
  }

  // Check last sample point
  if (std::abs(aSamples(aNbSamples)) < theConfig.FTolerance)
  {
    addRoot(aXValues(aNbSamples), aSamples(aNbSamples) + theConfig.Offset);
  }

  // Sort roots using indices
  const int aNbRoots = aResult.Roots.Length();
  if (aNbRoots > 1)
  {
    math_IntegerVector aIndices(0, aNbRoots - 1);
    for (int i = 0; i < aNbRoots; ++i)
    {
      aIndices(i) = i;
    }

    // Simple insertion sort for small arrays
    for (int i = 1; i < aNbRoots; ++i)
    {
      int aKey = aIndices(i);
      int j    = i - 1;
      while (j >= 0 && aResult.Roots.Value(aIndices(j)) > aResult.Roots.Value(aKey))
      {
        aIndices(j + 1) = aIndices(j);
        --j;
      }
      aIndices(j + 1) = aKey;
    }

    NCollection_Vector<double> aSortedRoots, aSortedValues;
    for (int i = 0; i < aNbRoots; ++i)
    {
      aSortedRoots.Append(aResult.Roots.Value(aIndices(i)));
      aSortedValues.Append(aResult.Values.Value(aIndices(i)));
    }
    aResult.Roots  = aSortedRoots;
    aResult.Values = aSortedValues;
  }

  return aResult;
}

//! Finds all real roots of a function with derivative within range [theLower, theUpper].
//! Can also detect extrema where function touches zero without crossing.
//!
//! This version uses derivative information for better extrema detection.
//!
//! @tparam Function type with Values(double theX, double& theF, double& theDF) method
//! @param theFunc function object with derivative
//! @param theLower lower bound of search interval
//! @param theUpper upper bound of search interval
//! @param theConfig configuration parameters
//! @return result containing all found roots
template <typename Function>
MultipleResult FindAllRootsWithDerivative(Function&             theFunc,
                                          double                theLower,
                                          double                theUpper,
                                          const MultipleConfig& theConfig = MultipleConfig())
{
  MultipleResult aResult;
  aResult.Status = MathUtils::Status::OK;

  double aLower = std::min(theLower, theUpper);
  double aUpper = std::max(theLower, theUpper);

  const int    aNbSamples = std::max(theConfig.NbSamples, 10);
  const double aDx        = (aUpper - aLower) / aNbSamples;

  const double aMinEpsX = 1e-10 * (std::abs(aLower) + std::abs(aUpper));
  const double aEpsX    = std::max(theConfig.XTolerance, aMinEpsX);

  // Sample function values and derivatives
  math_Vector aFValues(0, aNbSamples);
  math_Vector aDFValues(0, aNbSamples);
  math_Vector aXValues(0, aNbSamples);

  bool aAllValid = true;
  for (int i = 0; i <= aNbSamples; ++i)
  {
    double aX = aLower + i * aDx;
    if (aX > aUpper)
      aX = aUpper;
    aXValues(i) = aX;

    double aF = 0.0, aDF = 0.0;
    if (!theFunc.Values(aX, aF, aDF))
    {
      aAllValid = false;
      break;
    }
    aFValues(i)  = aF - theConfig.Offset;
    aDFValues(i) = aDF;
  }

  if (!aAllValid)
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  // Check if function is essentially null
  aResult.IsAllNull = true;
  for (int i = 0; i <= aNbSamples; ++i)
  {
    if (std::abs(aFValues(i)) > theConfig.NullTolerance)
    {
      aResult.IsAllNull = false;
      break;
    }
  }

  if (aResult.IsAllNull)
  {
    return aResult;
  }

  // Helper to add root if not duplicate
  auto addRoot = [&](double theRoot, double theValue) {
    for (int k = 0; k < aResult.Roots.Length(); ++k)
    {
      if (std::abs(theRoot - aResult.Roots.Value(k)) < aEpsX)
      {
        return;
      }
    }
    aResult.Roots.Append(theRoot);
    aResult.Values.Append(theValue);
  };

  // Wrapper for Brent using Values interface
  class BrentWrapper
  {
  public:
    BrentWrapper(Function& theF, double theOffset)
        : myFunc(theF),
          myOffset(theOffset)
    {
    }

    bool Value(double theX, double& theY) const
    {
      double aDF = 0.0;
      if (!myFunc.Values(theX, theY, aDF))
        return false;
      theY -= myOffset;
      return true;
    }

  private:
    Function& myFunc;
    double    myOffset;
  };

  BrentWrapper aWrapper(theFunc, theConfig.Offset);

  // Find sign changes
  for (int i = 0; i < aNbSamples; ++i)
  {
    const double aF0 = aFValues(i);
    const double aF1 = aFValues(i + 1);
    const double aX0 = aXValues(i);
    const double aX1 = aXValues(i + 1);

    // Exact zero at sample point
    if (std::abs(aF0) < theConfig.FTolerance)
    {
      addRoot(aX0, aF0 + theConfig.Offset);
      continue;
    }

    // Sign change
    if (aF0 * aF1 < 0.0)
    {
      MathUtils::Config aBrentConfig;
      aBrentConfig.XTolerance    = aEpsX;
      aBrentConfig.FTolerance    = theConfig.FTolerance;
      aBrentConfig.MaxIterations = theConfig.MaxIterations;

      auto aBrentResult = Brent(aWrapper, aX0, aX1, aBrentConfig);
      aResult.NbIterations += aBrentResult.NbIterations;

      if (aBrentResult.IsDone() && aBrentResult.Root.has_value())
      {
        double aRootValue = 0.0, aDummy = 0.0;
        theFunc.Values(*aBrentResult.Root, aRootValue, aDummy);
        addRoot(*aBrentResult.Root, aRootValue);
      }
    }
    // Check for potential extrema touching zero
    else if (aF0 > 0.0 && aF1 > 0.0)
    {
      // Potential minimum - check if derivative changes sign
      if (aDFValues(i) < 0.0 && aDFValues(i + 1) > 0.0)
      {
        // Find the minimum using bisection on derivative
        double aXL = aX0, aXR = aX1;

        for (int anIter = 0; anIter < 20; ++anIter)
        {
          double aXM = 0.5 * (aXL + aXR);
          double aFM = 0.0, aDFM = 0.0;
          if (!theFunc.Values(aXM, aFM, aDFM))
            break;
          aFM -= theConfig.Offset;

          if (aDFM < 0.0)
          {
            aXL = aXM;
          }
          else
          {
            aXR = aXM;
          }

          // Check if minimum is close enough to zero
          if (std::abs(aFM) < theConfig.FTolerance)
          {
            addRoot(aXM, aFM + theConfig.Offset);
            break;
          }
        }
      }
    }
    else if (aF0 < 0.0 && aF1 < 0.0)
    {
      // Potential maximum - check if derivative changes sign
      if (aDFValues(i) > 0.0 && aDFValues(i + 1) < 0.0)
      {
        double aXL = aX0, aXR = aX1;

        for (int anIter = 0; anIter < 20; ++anIter)
        {
          double aXM = 0.5 * (aXL + aXR);
          double aFM = 0.0, aDFM = 0.0;
          if (!theFunc.Values(aXM, aFM, aDFM))
            break;
          aFM -= theConfig.Offset;

          if (aDFM > 0.0)
          {
            aXL = aXM;
          }
          else
          {
            aXR = aXM;
          }

          if (std::abs(aFM) < theConfig.FTolerance)
          {
            addRoot(aXM, aFM + theConfig.Offset);
            break;
          }
        }
      }
    }
  }

  // Check last sample point
  if (std::abs(aFValues(aNbSamples)) < theConfig.FTolerance)
  {
    addRoot(aXValues(aNbSamples), aFValues(aNbSamples) + theConfig.Offset);
  }

  // Sort roots using indices
  const int aNbRoots = aResult.Roots.Length();
  if (aNbRoots > 1)
  {
    math_IntegerVector aIndices(0, aNbRoots - 1);
    for (int i = 0; i < aNbRoots; ++i)
    {
      aIndices(i) = i;
    }

    // Simple insertion sort for small arrays
    for (int i = 1; i < aNbRoots; ++i)
    {
      int aKey = aIndices(i);
      int j    = i - 1;
      while (j >= 0 && aResult.Roots.Value(aIndices(j)) > aResult.Roots.Value(aKey))
      {
        aIndices(j + 1) = aIndices(j);
        --j;
      }
      aIndices(j + 1) = aKey;
    }

    NCollection_Vector<double> aSortedRoots, aSortedValues;
    for (int i = 0; i < aNbRoots; ++i)
    {
      aSortedRoots.Append(aResult.Roots.Value(aIndices(i)));
      aSortedValues.Append(aResult.Values.Value(aIndices(i)));
    }
    aResult.Roots  = aSortedRoots;
    aResult.Values = aSortedValues;
  }

  return aResult;
}

//! Convenience alias using default configuration.
//! @tparam Function type with Value(double, double&) method
//! @param theFunc function to find roots of
//! @param theLower lower bound
//! @param theUpper upper bound
//! @param theNbSamples number of sample points
//! @return result with all found roots
template <typename Function>
MultipleResult FindAllRoots(Function& theFunc, double theLower, double theUpper, int theNbSamples)
{
  MultipleConfig aConfig;
  aConfig.NbSamples = theNbSamples;
  return FindAllRoots(theFunc, theLower, theUpper, aConfig);
}

} // namespace MathRoot

#endif // _MathRoot_Multiple_HeaderFile
