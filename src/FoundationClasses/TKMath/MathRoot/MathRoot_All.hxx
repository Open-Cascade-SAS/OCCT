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

#ifndef _MathRoot_All_HeaderFile
#define _MathRoot_All_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathRoot_Multiple.hxx>
#include <math_Vector.hxx>

#include <NCollection_Vector.hxx>

#include <cmath>

namespace MathRoot
{
using namespace MathUtils;

//! Represents an interval where the function is null (within tolerance).
struct NullInterval
{
  double A     = 0.0; //!< Interval start
  double B     = 0.0; //!< Interval end
  int    State = 0;   //!< State number (for parametric curves)
};

//! Result for all roots finder including null intervals.
struct AllRootsResult
{
  MathUtils::Status                Status = MathUtils::Status::NotConverged;
  NCollection_Vector<double>       Roots;         //!< Isolated root locations
  NCollection_Vector<int>          RootStates;    //!< State numbers for roots
  NCollection_Vector<NullInterval> NullIntervals; //!< Intervals where function is null

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }

  int NbRoots() const { return Roots.Length(); }

  int NbIntervals() const { return NullIntervals.Length(); }
};

//! Helper function to find multiple roots with offset.
//! @tparam Func function type
//! @param theFunc function to analyze
//! @param theA interval start
//! @param theB interval end
//! @param theNbSamples number of sample points
//! @param theEpsX tolerance for root x-value
//! @param theEpsF tolerance for function value
//! @param theOffset offset value (find roots of f(x) - offset = 0)
//! @return MultipleResult with found roots
template <typename Func>
MultipleResult FindMultipleRoots(Func&  theFunc,
                                 double theA,
                                 double theB,
                                 int    theNbSamples,
                                 double theEpsX,
                                 double theEpsF,
                                 double theOffset = 0.0)
{
  MultipleConfig aConfig;
  aConfig.NbSamples  = theNbSamples;
  aConfig.XTolerance = theEpsX;
  aConfig.FTolerance = theEpsF;
  aConfig.Offset     = theOffset;
  return FindAllRoots(theFunc, theA, theB, aConfig);
}

//! Find all roots of a function using sampling and refinement.
//!
//! Uses a sample of the function to find:
//! 1. Null intervals: where |F(x)| <= EpsNul for consecutive sample points
//! 2. Isolated roots: single points where F(x) = 0
//!
//! The algorithm:
//! 1. Evaluates F at sample points
//! 2. Identifies null intervals where |F| <= EpsNul for 2+ consecutive points
//! 3. Refines interval boundaries using root finding
//! 4. Finds isolated roots between null intervals using MultipleRoots
//!
//! @tparam Func function type with:
//!   - bool Value(double, double&)
//!   - bool Values(double, double&, double&) for derivative
//! @param theFunc function with derivative to analyze
//! @param theSamples sample points array
//! @param theEpsX tolerance for root x-value
//! @param theEpsF tolerance for function value at root
//! @param theEpsNul tolerance for null interval detection
//! @return AllRootsResult with roots and null intervals
template <typename Func>
AllRootsResult FindAllRootsWithIntervals(Func&              theFunc,
                                         const math_Vector& theSamples,
                                         double             theEpsX   = 1.0e-10,
                                         double             theEpsF   = 1.0e-10,
                                         double             theEpsNul = 1.0e-10)
{
  AllRootsResult aResult;

  const int aNbp = theSamples.Length();
  if (aNbp < 2)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  const int aLower = theSamples.Lower();

  // Evaluate function at first sample point
  double aVal, aPrevVal;
  if (!theFunc.Value(theSamples(aLower), aPrevVal))
  {
    aResult.Status = MathUtils::Status::NotConverged;
    return aResult;
  }

  bool aPrevNul = std::abs(aPrevVal) <= theEpsNul;
  if (!aPrevNul)
  {
    // Save non-null value for later use
  }

  bool   aInInterval = false;
  bool   aNulStart   = false;
  bool   aNulEnd     = false;
  double aDebNul = 0.0, aFinNul = 0.0;
  double aValSav = aPrevVal;

  NCollection_Vector<double> aIntervalStarts, aIntervalEnds;

  // Scan through samples to find null intervals
  for (int i = 1; i < aNbp; ++i)
  {
    if (!theFunc.Value(theSamples(aLower + i), aVal))
    {
      aResult.Status = MathUtils::Status::NotConverged;
      return aResult;
    }

    bool aCurNul = std::abs(aVal) <= theEpsNul;
    if (!aCurNul)
    {
      aValSav = aVal;
    }

    if (aInInterval && !aCurNul)
    {
      // End of null interval
      aInInterval = false;
      aIntervalStarts.Append(aDebNul);

      // Refine end of null interval
      double aCst = (aVal > 0.0) ? theEpsNul : -theEpsNul;

      // Use root finding to locate precise boundary
      MultipleResult aRes = FindMultipleRoots(theFunc,
                                              theSamples(aLower + i - 1),
                                              theSamples(aLower + i),
                                              10,
                                              theEpsX,
                                              theEpsF,
                                              aCst);
      if (aRes.IsDone() && aRes.NbRoots() > 0)
      {
        aFinNul = aRes.Roots[0];
      }
      else
      {
        aFinNul = theSamples(aLower + i - 1);
      }

      // Try opposite sign
      aCst       = -aCst;
      auto aRes2 = FindMultipleRoots(theFunc,
                                     theSamples(aLower + i - 1),
                                     theSamples(aLower + i),
                                     10,
                                     theEpsX,
                                     theEpsF,
                                     aCst);
      if (aRes2.IsDone() && aRes2.NbRoots() > 0)
      {
        if (aRes2.Roots[0] < aFinNul)
        {
          aFinNul = aRes2.Roots[0];
        }
      }

      aIntervalEnds.Append(aFinNul);
    }
    else if (!aInInterval && aPrevNul && aCurNul)
    {
      // Start of null interval
      aInInterval = true;
      if (i == 1)
      {
        aDebNul   = theSamples(aLower);
        aNulStart = true;
      }
      else
      {
        // Refine start of null interval
        double aCst = (aValSav > 0.0) ? theEpsNul : -theEpsNul;

        MultipleResult aRes = FindMultipleRoots(theFunc,
                                                theSamples(aLower + i - 2),
                                                theSamples(aLower + i - 1),
                                                10,
                                                theEpsX,
                                                theEpsF,
                                                aCst);
        if (aRes.IsDone() && aRes.NbRoots() > 0)
        {
          aDebNul = aRes.Roots[aRes.NbRoots() - 1];
        }
        else
        {
          aDebNul = theSamples(aLower + i - 1);
        }

        // Try opposite sign
        aCst       = -aCst;
        auto aRes2 = FindMultipleRoots(theFunc,
                                       theSamples(aLower + i - 2),
                                       theSamples(aLower + i - 1),
                                       10,
                                       theEpsX,
                                       theEpsF,
                                       aCst);
        if (aRes2.IsDone() && aRes2.NbRoots() > 0)
        {
          if (aRes2.Roots[aRes2.NbRoots() - 1] > aDebNul)
          {
            aDebNul = aRes2.Roots[aRes2.NbRoots() - 1];
          }
        }
      }
    }

    aPrevNul = aCurNul;
  }

  // Handle interval ending at last sample
  if (aInInterval)
  {
    aIntervalStarts.Append(aDebNul);
    aFinNul = theSamples(aLower + aNbp - 1);
    aIntervalEnds.Append(aFinNul);
    aNulEnd = true;
  }

  // Store null intervals
  for (int k = 0; k < aIntervalStarts.Length(); ++k)
  {
    NullInterval anInt;
    anInt.A = aIntervalStarts.Value(k);
    anInt.B = aIntervalEnds.Value(k);
    aResult.NullIntervals.Append(anInt);
  }

  const double aSampleFirst = theSamples(aLower);
  const double aSampleLast  = theSamples(aLower + aNbp - 1);

  // Find isolated roots between null intervals
  if (aIntervalStarts.IsEmpty())
  {
    // No null intervals - find all roots in entire range
    MultipleResult aRes =
      FindMultipleRoots(theFunc, aSampleFirst, aSampleLast, aNbp, theEpsX, theEpsF);
    if (aRes.IsDone())
    {
      for (int j = 0; j < aRes.NbRoots(); ++j)
      {
        aResult.Roots.Append(aRes.Roots[j]);
        aResult.RootStates.Append(0);
      }
    }
  }
  else
  {
    // Find roots before first null interval
    if (!aNulStart)
    {
      double aStart = aSampleFirst;
      double aEnd   = aIntervalStarts.Value(0);
      int    aNbrpt =
        std::max(3,
                 static_cast<int>(std::abs((aEnd - aStart) / (aSampleLast - aSampleFirst)) * aNbp));

      MultipleResult aRes = FindMultipleRoots(theFunc, aStart, aEnd, aNbrpt, theEpsX, theEpsF);
      if (aRes.IsDone())
      {
        for (int j = 0; j < aRes.NbRoots(); ++j)
        {
          aResult.Roots.Append(aRes.Roots[j]);
          aResult.RootStates.Append(0);
        }
      }
    }

    // Find roots between null intervals
    for (int k = 1; k < aIntervalStarts.Length(); ++k)
    {
      double aStart = aIntervalEnds.Value(k - 1);
      double aEnd   = aIntervalStarts.Value(k);
      int    aNbrpt =
        std::max(3,
                 static_cast<int>(std::abs((aEnd - aStart) / (aSampleLast - aSampleFirst)) * aNbp));

      MultipleResult aRes = FindMultipleRoots(theFunc, aStart, aEnd, aNbrpt, theEpsX, theEpsF);
      if (aRes.IsDone())
      {
        for (int j = 0; j < aRes.NbRoots(); ++j)
        {
          aResult.Roots.Append(aRes.Roots[j]);
          aResult.RootStates.Append(0);
        }
      }
    }

    // Find roots after last null interval
    if (!aNulEnd)
    {
      double aStart = aIntervalEnds.Value(aIntervalEnds.Length() - 1);
      double aEnd   = aSampleLast;
      int    aNbrpt =
        std::max(3,
                 static_cast<int>(std::abs((aEnd - aStart) / (aSampleLast - aSampleFirst)) * aNbp));

      MultipleResult aRes = FindMultipleRoots(theFunc, aStart, aEnd, aNbrpt, theEpsX, theEpsF);
      if (aRes.IsDone())
      {
        for (int j = 0; j < aRes.NbRoots(); ++j)
        {
          aResult.Roots.Append(aRes.Roots[j]);
          aResult.RootStates.Append(0);
        }
      }
    }
  }

  aResult.Status = MathUtils::Status::OK;
  return aResult;
}

//! Find all roots using uniform sampling.
//!
//! @tparam Func function type with Value and Values methods
//! @param theFunc function with derivative
//! @param theA interval start
//! @param theB interval end
//! @param theNbSamples number of sample points
//! @param theEpsX tolerance for root x-value
//! @param theEpsF tolerance for function value
//! @param theEpsNul tolerance for null interval detection
//! @return AllRootsResult with roots and null intervals
template <typename Func>
AllRootsResult FindAllRootsWithIntervals(Func&  theFunc,
                                         double theA,
                                         double theB,
                                         int    theNbSamples,
                                         double theEpsX   = 1.0e-10,
                                         double theEpsF   = 1.0e-10,
                                         double theEpsNul = 1.0e-10)
{
  math_Vector  aSamples(0, theNbSamples - 1);
  const double aStep = (theB - theA) / (theNbSamples - 1);
  for (int i = 0; i < theNbSamples; ++i)
  {
    aSamples(i) = theA + i * aStep;
  }
  // Ensure last point is exactly theB
  aSamples(theNbSamples - 1) = theB;

  return FindAllRootsWithIntervals(theFunc, aSamples, theEpsX, theEpsF, theEpsNul);
}

} // namespace MathRoot

#endif // _MathRoot_All_HeaderFile
