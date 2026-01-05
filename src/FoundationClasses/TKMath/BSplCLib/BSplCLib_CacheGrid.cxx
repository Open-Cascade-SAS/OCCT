// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <BSplCLib_CacheGrid.hxx>
#include <BSplCLib.hxx>

#include <cmath>

namespace
{
// Empty handle for returning null references
const occ::handle<BSplCLib_Cache> THE_NULL_CACHE;
} // namespace

//==================================================================================================

BSplCLib_CacheGrid::BSplCLib_CacheGrid(int                               theDegree,
                                       bool                              thePeriodic,
                                       const NCollection_Array1<double>& theFlatKnots)
    : myDegree(theDegree),
      myPeriodic(thePeriodic),
      myCurrentSpan(-1)
{
  // Calculate span indices range (similar to BSplCLib_CacheParams)
  mySpanIndexMin    = theFlatKnots.Lower() + theDegree;
  int aSpanIndexMax = theFlatKnots.Upper() - theDegree - 1;
  myNbSpans         = aSpanIndexMax - mySpanIndexMin + 1;

  // Store parameter bounds
  myFirst = theFlatKnots.Value(mySpanIndexMin);
  myLast  = theFlatKnots.Value(theFlatKnots.Upper() - theDegree);

  // Initialize the cache grid with null handles
  // Using 0-based indexing for simplicity
  myCacheGrid.Resize(0, myNbSpans - 1, false);
}

//==================================================================================================

BSplCLib_CacheGrid::BSplCLib_CacheGrid(const BSplCLib_CacheGrid& theOther)
    : myDegree(theOther.myDegree),
      myPeriodic(theOther.myPeriodic),
      myNbSpans(theOther.myNbSpans),
      mySpanIndexMin(theOther.mySpanIndexMin),
      myFirst(theOther.myFirst),
      myLast(theOther.myLast),
      myCacheGrid(theOther.myCacheGrid.Lower(), theOther.myCacheGrid.Upper()),
      myCurrentSpan(theOther.myCurrentSpan)
{
  // Deep copy all cache handles
  for (int i = theOther.myCacheGrid.Lower(); i <= theOther.myCacheGrid.Upper(); ++i)
  {
    const occ::handle<BSplCLib_Cache>& aSrcCache = theOther.myCacheGrid.Value(i);
    if (!aSrcCache.IsNull())
    {
      myCacheGrid.SetValue(i, new BSplCLib_Cache(*aSrcCache));
    }
  }
}

//==================================================================================================

double BSplCLib_CacheGrid::periodicNormalization(double theParam) const
{
  if (myPeriodic)
  {
    const double aPeriod = myLast - myFirst;
    if (theParam < myFirst)
    {
      const double aScale = std::trunc((myFirst - theParam) / aPeriod);
      return theParam + aPeriod * (aScale + 1.0);
    }
    if (theParam > myLast)
    {
      const double aScale = std::trunc((theParam - myLast) / aPeriod);
      return theParam - aPeriod * (aScale + 1.0);
    }
  }
  return theParam;
}

//==================================================================================================

int BSplCLib_CacheGrid::locateSpan(double                            theParameter,
                                   const NCollection_Array1<double>& theFlatKnots) const
{
  double aParam = periodicNormalization(theParameter);

  int aSpanIndex = 0;
  BSplCLib::LocateParameter(myDegree,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            aParam,
                            myPeriodic,
                            aSpanIndex,
                            aParam);

  // Convert to 0-based index relative to first span
  return aSpanIndex - mySpanIndexMin;
}

//==================================================================================================

const occ::handle<BSplCLib_Cache>& BSplCLib_CacheGrid::Cache(
  double                            theParameter,
  const NCollection_Array1<double>& theFlatKnots,
  const NCollection_Array1<gp_Pnt>& thePoles,
  const NCollection_Array1<double>* theWeights)
{
  // Check current span and adjacent spans (current, previous, next)
  const int aCurrentSpan = myCurrentSpan;
  if (aCurrentSpan >= 0)
  {
    for (const int anOffset : {0, -1, 1})
    {
      const int aSpan = aCurrentSpan + anOffset;
      if (aSpan < 0 || aSpan >= myNbSpans)
        continue;

      const occ::handle<BSplCLib_Cache>& aCache = myCacheGrid.Value(aSpan);
      if (!aCache.IsNull() && aCache->IsCacheValid(theParameter))
      {
        myCurrentSpan = aSpan;
        return aCache;
      }
    }
  }

  // Locate the span for the given parameter
  int aSpan = locateSpan(theParameter, theFlatKnots);

  // Clamp to valid range (in case of edge cases)
  aSpan = std::max(0, std::min(aSpan, myNbSpans - 1));

  // Update current span
  myCurrentSpan = aSpan;

  // Check if cache already exists for this span
  occ::handle<BSplCLib_Cache>& aCache = myCacheGrid.ChangeValue(aSpan);
  if (!aCache.IsNull())
  {
    // Cache exists - it should be valid for this span
    // (each cache covers exactly one span)
    return aCache;
  }

  // Cache doesn't exist - create it
  aCache = new BSplCLib_Cache(myDegree, myPeriodic, theFlatKnots, thePoles, theWeights);

  // Build the cache for this span
  // Use the span midpoint to ensure we're inside the span
  const int aSpanIndex = aSpan + mySpanIndexMin;

  double aSpanStart = theFlatKnots.Value(aSpanIndex);
  double aSpanEnd   = theFlatKnots.Value(aSpanIndex + 1);
  double aSpanMid   = 0.5 * (aSpanStart + aSpanEnd);

  aCache->BuildCache(aSpanMid, theFlatKnots, thePoles, theWeights);

  return aCache;
}

//==================================================================================================

const occ::handle<BSplCLib_Cache>& BSplCLib_CacheGrid::Cache(
  double                              theParameter,
  const NCollection_Array1<double>&   theFlatKnots,
  const NCollection_Array1<gp_Pnt2d>& thePoles2d,
  const NCollection_Array1<double>*   theWeights)
{
  // Locate the span for the given parameter
  int aSpan = locateSpan(theParameter, theFlatKnots);

  // Clamp to valid range (in case of edge cases)
  aSpan = std::max(0, std::min(aSpan, myNbSpans - 1));

  // Update current span
  myCurrentSpan = aSpan;

  // Check if cache already exists for this span
  occ::handle<BSplCLib_Cache>& aCache = myCacheGrid.ChangeValue(aSpan);
  if (!aCache.IsNull())
  {
    // Cache exists - it should be valid for this span
    // (each cache covers exactly one span)
    return aCache;
  }

  // Cache doesn't exist - create it
  aCache = new BSplCLib_Cache(myDegree, myPeriodic, theFlatKnots, thePoles2d, theWeights);

  // Build the cache for this span
  // Use the span midpoint to ensure we're inside the span
  const int aSpanIndex = aSpan + mySpanIndexMin;

  double aSpanStart = theFlatKnots.Value(aSpanIndex);
  double aSpanEnd   = theFlatKnots.Value(aSpanIndex + 1);
  double aSpanMid   = 0.5 * (aSpanStart + aSpanEnd);

  aCache->BuildCache(aSpanMid, theFlatKnots, thePoles2d, theWeights);

  return aCache;
}
