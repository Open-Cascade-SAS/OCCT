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

IMPLEMENT_STANDARD_RTTIEXT(BSplSLib_CacheGrid, Standard_Transient)

const occ::handle<BSplSLib_Cache> BSplSLib_CacheGrid::THE_NULL_CACHE;

//==================================================================================================

BSplSLib_CacheGrid::BSplSLib_CacheGrid(int                               theDegreeU,
                                       bool                              thePeriodicU,
                                       const NCollection_Array1<double>& theFlatKnotsU,
                                       int                               theDegreeV,
                                       bool                              thePeriodicV,
                                       const NCollection_Array1<double>& theFlatKnotsV,
                                       const NCollection_Array2<double>* theWeights)
    : myDegreeU(theDegreeU),
      myDegreeV(theDegreeV),
      myPeriodicU(thePeriodicU),
      myPeriodicV(thePeriodicV),
      myIsRational(theWeights != nullptr),
      myCurrentUSpan(-1),
      myCurrentVSpan(-1)
{
  // Calculate span indices range (similar to BSplCLib_CacheParams)
  mySpanIndexMinU = theFlatKnotsU.Lower() + theDegreeU;
  int aSpanIndexMaxU = theFlatKnotsU.Upper() - theDegreeU - 1;
  myNbUSpans = aSpanIndexMaxU - mySpanIndexMinU + 1;

  mySpanIndexMinV = theFlatKnotsV.Lower() + theDegreeV;
  int aSpanIndexMaxV = theFlatKnotsV.Upper() - theDegreeV - 1;
  myNbVSpans = aSpanIndexMaxV - mySpanIndexMinV + 1;

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

int BSplSLib_CacheGrid::locateUSpan(double theU, const NCollection_Array1<double>& theFlatKnots) const
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

int BSplSLib_CacheGrid::locateVSpan(double theV, const NCollection_Array1<double>& theFlatKnots) const
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

bool BSplSLib_CacheGrid::IsCacheValid(double theU, double theV) const
{
  // Check if we have a current span and if the parameters are still in it
  if (myCurrentUSpan < 0 || myCurrentVSpan < 0)
    return false;

  const occ::handle<BSplSLib_Cache>& aCache = myCacheGrid.Value(myCurrentUSpan, myCurrentVSpan);
  if (aCache.IsNull())
    return false;

  return aCache->IsCacheValid(theU, theV);
}

//==================================================================================================

const occ::handle<BSplSLib_Cache>& BSplSLib_CacheGrid::CurrentCache(double theU, double theV) const
{
  if (myCurrentUSpan < 0 || myCurrentVSpan < 0)
    return THE_NULL_CACHE;

  const occ::handle<BSplSLib_Cache>& aCache = myCacheGrid.Value(myCurrentUSpan, myCurrentVSpan);
  if (aCache.IsNull())
    return THE_NULL_CACHE;

  if (!aCache->IsCacheValid(theU, theV))
    return THE_NULL_CACHE;

  return aCache;
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
    // (each cache covers exactly one span)
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

  // Create allocator if needed
  if (myAllocator.IsNull())
    myAllocator = new NCollection_IncAllocator();
  else
    myAllocator->Reset(false); // Keep blocks, reset position

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

  aCache->BuildCache(aSpanMidU,
                     aSpanMidV,
                     theFlatKnotsU,
                     theFlatKnotsV,
                     thePoles,
                     theWeights,
                     myAllocator);

  return aCache;
}

//==================================================================================================

int BSplSLib_CacheGrid::NbCachedSpans() const
{
  int aCount = 0;
  for (int i = myCacheGrid.LowerRow(); i <= myCacheGrid.UpperRow(); ++i)
  {
    for (int j = myCacheGrid.LowerCol(); j <= myCacheGrid.UpperCol(); ++j)
    {
      if (!myCacheGrid.Value(i, j).IsNull())
        ++aCount;
    }
  }
  return aCount;
}
