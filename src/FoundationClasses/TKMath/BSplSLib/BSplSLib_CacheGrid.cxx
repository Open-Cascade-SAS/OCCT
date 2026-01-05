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

#include <BSplSLib_CacheGrid.hxx>
#include <BSplCLib.hxx>
#include <BSplSLib.hxx>

#include <cmath>

namespace
{
// Empty handle for returning null references
const occ::handle<BSplSLib_Cache> THE_NULL_CACHE;
} // namespace

//==================================================================================================

BSplSLib_CacheGrid::BSplSLib_CacheGrid(int                               theDegreeU,
                                       bool                              thePeriodicU,
                                       const NCollection_Array1<double>& theFlatKnotsU,
                                       int                               theDegreeV,
                                       bool                              thePeriodicV,
                                       const NCollection_Array1<double>& theFlatKnotsV)
    : myDegreeU(theDegreeU),
      myDegreeV(theDegreeV),
      myPeriodicU(thePeriodicU),
      myPeriodicV(thePeriodicV),
      myCurrentUSpan(-1),
      myCurrentVSpan(-1)
{
  // Calculate span indices range (similar to BSplCLib_CacheParams)
  mySpanIndexMinU    = theFlatKnotsU.Lower() + theDegreeU;
  int aSpanIndexMaxU = theFlatKnotsU.Upper() - theDegreeU - 1;
  myNbUSpans         = aSpanIndexMaxU - mySpanIndexMinU + 1;

  mySpanIndexMinV    = theFlatKnotsV.Lower() + theDegreeV;
  int aSpanIndexMaxV = theFlatKnotsV.Upper() - theDegreeV - 1;
  myNbVSpans         = aSpanIndexMaxV - mySpanIndexMinV + 1;

  // Store parameter bounds
  myFirstU = theFlatKnotsU.Value(mySpanIndexMinU);
  myLastU  = theFlatKnotsU.Value(theFlatKnotsU.Upper() - theDegreeU);
  myFirstV = theFlatKnotsV.Value(mySpanIndexMinV);
  myLastV  = theFlatKnotsV.Value(theFlatKnotsV.Upper() - theDegreeV);

  // Initialize the cache grid with null handles
  // Using 0-based indexing for simplicity
  myCacheGrid.Resize(0, myNbUSpans - 1, 0, myNbVSpans - 1, false);
}

//==================================================================================================

BSplSLib_CacheGrid::BSplSLib_CacheGrid(const BSplSLib_CacheGrid& theOther)
    : myDegreeU(theOther.myDegreeU),
      myDegreeV(theOther.myDegreeV),
      myPeriodicU(theOther.myPeriodicU),
      myPeriodicV(theOther.myPeriodicV),
      myNbUSpans(theOther.myNbUSpans),
      myNbVSpans(theOther.myNbVSpans),
      mySpanIndexMinU(theOther.mySpanIndexMinU),
      mySpanIndexMinV(theOther.mySpanIndexMinV),
      myFirstU(theOther.myFirstU),
      myLastU(theOther.myLastU),
      myFirstV(theOther.myFirstV),
      myLastV(theOther.myLastV),
      myCacheGrid(theOther.myCacheGrid.LowerRow(),
                  theOther.myCacheGrid.UpperRow(),
                  theOther.myCacheGrid.LowerCol(),
                  theOther.myCacheGrid.UpperCol()),
      myCurrentUSpan(theOther.myCurrentUSpan),
      myCurrentVSpan(theOther.myCurrentVSpan)
{
  // Deep copy all cache handles
  for (int i = theOther.myCacheGrid.LowerRow(); i <= theOther.myCacheGrid.UpperRow(); ++i)
  {
    for (int j = theOther.myCacheGrid.LowerCol(); j <= theOther.myCacheGrid.UpperCol(); ++j)
    {
      const occ::handle<BSplSLib_Cache>& aSrcCache = theOther.myCacheGrid.Value(i, j);
      if (!aSrcCache.IsNull())
      {
        myCacheGrid.SetValue(i, j, new BSplSLib_Cache(*aSrcCache));
      }
    }
  }
}

//==================================================================================================

double BSplSLib_CacheGrid::periodicNormalization(double theParam,
                                                 double theFirst,
                                                 double theLast,
                                                 bool   thePeriodic) const
{
  if (thePeriodic)
  {
    const double aPeriod = theLast - theFirst;
    if (theParam < theFirst)
    {
      const double aScale = std::trunc((theFirst - theParam) / aPeriod);
      return theParam + aPeriod * (aScale + 1.0);
    }
    if (theParam > theLast)
    {
      const double aScale = std::trunc((theParam - theLast) / aPeriod);
      return theParam - aPeriod * (aScale + 1.0);
    }
  }
  return theParam;
}

//==================================================================================================

int BSplSLib_CacheGrid::locateUSpan(double                            theU,
                                    const NCollection_Array1<double>& theFlatKnots) const
{
  double aU = periodicNormalization(theU, myFirstU, myLastU, myPeriodicU);

  int aSpanIndex = 0;
  BSplCLib::LocateParameter(myDegreeU,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            aU,
                            myPeriodicU,
                            aSpanIndex,
                            aU);

  // Convert to 0-based index relative to first span
  return aSpanIndex - mySpanIndexMinU;
}

//==================================================================================================

int BSplSLib_CacheGrid::locateVSpan(double                            theV,
                                    const NCollection_Array1<double>& theFlatKnots) const
{
  double aV = periodicNormalization(theV, myFirstV, myLastV, myPeriodicV);

  int aSpanIndex = 0;
  BSplCLib::LocateParameter(myDegreeV,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            aV,
                            myPeriodicV,
                            aSpanIndex,
                            aV);

  // Convert to 0-based index relative to first span
  return aSpanIndex - mySpanIndexMinV;
}

//==================================================================================================

const occ::handle<BSplSLib_Cache>& BSplSLib_CacheGrid::Cache(
  double                            theU,
  double                            theV,
  const NCollection_Array1<double>& theFlatKnotsU,
  const NCollection_Array1<double>& theFlatKnotsV,
  const NCollection_Array2<gp_Pnt>& thePoles,
  const NCollection_Array2<double>* theWeights)
{
  // Check current span and adjacent spans
  const int aCurrentUSpan = myCurrentUSpan;
  const int aCurrentVSpan = myCurrentVSpan;
  if (aCurrentUSpan >= 0 && aCurrentVSpan >= 0)
  {
    for (int aUOffset : {0, -1, 1})
    {
      for (int aVOffset : {0, -1, 1})
      {
        const int aUSpan = aCurrentUSpan + aUOffset;
        const int aVSpan = aCurrentVSpan + aVOffset;

        if (aUSpan < 0 || aUSpan >= myNbUSpans || aVSpan < 0 || aVSpan >= myNbVSpans)
          continue;

        const occ::handle<BSplSLib_Cache>& aCache = myCacheGrid.Value(aUSpan, aVSpan);
        if (aCache.IsNull())
          continue;

        // Check U validity first - if U is invalid for this U span, skip remaining V offsets
        if (!aCache->IsCacheValidU(theU))
          break;

        if (aCache->IsCacheValid(theU, theV))
        {
          myCurrentUSpan = aUSpan;
          myCurrentVSpan = aVSpan;
          return aCache;
        }
      }
    }
  }

  // Locate the span for the given parameters
  int aUSpan = locateUSpan(theU, theFlatKnotsU);
  int aVSpan = locateVSpan(theV, theFlatKnotsV);

  // Clamp to valid range (in case of edge cases)
  aUSpan = std::max(0, std::min(aUSpan, myNbUSpans - 1));
  aVSpan = std::max(0, std::min(aVSpan, myNbVSpans - 1));

  // Update current span
  myCurrentUSpan = aUSpan;
  myCurrentVSpan = aVSpan;

  // Check if cache already exists for this span
  occ::handle<BSplSLib_Cache>& aCache = myCacheGrid.ChangeValue(aUSpan, aVSpan);
  if (!aCache.IsNull())
  {
    // Cache exists - it should be valid for this span
    return aCache;
  }

  // Cache doesn't exist - create it
  aCache = new BSplSLib_Cache(myDegreeU,
                              myPeriodicU,
                              theFlatKnotsU,
                              myDegreeV,
                              myPeriodicV,
                              theFlatKnotsV,
                              theWeights);

  // Build the cache for this span
  // We need to use a parameter that is guaranteed to be in this span
  // Use the span start + small offset to ensure we're inside the span
  const int aSpanIndexU = aUSpan + mySpanIndexMinU;
  const int aSpanIndexV = aVSpan + mySpanIndexMinV;

  double aSpanStartU = theFlatKnotsU.Value(aSpanIndexU);
  double aSpanEndU   = theFlatKnotsU.Value(aSpanIndexU + 1);
  double aSpanMidU   = 0.5 * (aSpanStartU + aSpanEndU);

  double aSpanStartV = theFlatKnotsV.Value(aSpanIndexV);
  double aSpanEndV   = theFlatKnotsV.Value(aSpanIndexV + 1);
  double aSpanMidV   = 0.5 * (aSpanStartV + aSpanEndV);

  aCache->BuildCache(aSpanMidU, aSpanMidV, theFlatKnotsU, theFlatKnotsV, thePoles, theWeights);

  return aCache;
}
