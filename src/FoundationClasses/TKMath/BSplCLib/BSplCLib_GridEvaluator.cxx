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

#include <BSplCLib_GridEvaluator.hxx>

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>

//==================================================================================================

BSplCLib_GridEvaluator::BSplCLib_GridEvaluator()
    : myDegree(0),
      myRational(false),
      myPeriodic(false),
      myIsInitialized(false)
{
}

//==================================================================================================

bool BSplCLib_GridEvaluator::Initialize(int                                  theDegree,
                                        const Handle(TColgp_HArray1OfPnt)&   thePoles,
                                        const Handle(TColStd_HArray1OfReal)& theWeights,
                                        const Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                        bool                                 theRational,
                                        bool                                 thePeriodic)
{
  myIsInitialized = false;

  // Validate degree
  if (theDegree < 1)
  {
    return false;
  }

  // Validate poles handle
  if (thePoles.IsNull() || thePoles->Length() < 2)
  {
    return false;
  }

  // Validate flat knots handle
  if (theFlatKnots.IsNull())
  {
    return false;
  }

  // Validate rational curve has weights
  if (theRational && theWeights.IsNull())
  {
    return false;
  }

  // Validate weights array size matches poles if provided
  if (!theWeights.IsNull() && theWeights->Length() != thePoles->Length())
  {
    return false;
  }

  myDegree        = theDegree;
  myPoles         = thePoles;
  myWeights       = theWeights;
  myFlatKnots     = theFlatKnots;
  myRational      = theRational;
  myPeriodic      = thePeriodic;
  myIsInitialized = true;

  // Reset cache
  myCache.Nullify();

  return true;
}

//==================================================================================================

void BSplCLib_GridEvaluator::SetParams(const Handle(TColStd_HArray1OfReal)& theParams)
{
  if (!myIsInitialized || myFlatKnots.IsNull() || theParams.IsNull())
  {
    return;
  }

  const int aNbParams = theParams->Length();
  if (aNbParams < 1)
  {
    return;
  }

  const TColStd_Array1OfReal& aKnots = myFlatKnots->Array1();

  myParams.Resize(0, aNbParams - 1, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aKnots.Lower() + myDegree;

  for (int i = 1; i <= aNbParams; ++i)
  {
    const double aParam   = theParams->Value(i);
    const int    aSpanIdx = locateSpanWithHint(aParam, aPrevSpan);
    aPrevSpan             = aSpanIdx;

    // Pre-compute local parameter for this span
    // BSplCLib_Cache uses SpanStart and SpanLength convention
    const double aSpanStart  = aKnots.Value(aSpanIdx);
    const double aSpanLength = aKnots.Value(aSpanIdx + 1) - aSpanStart;
    // Local parameter is (param - spanStart) / spanLength, normalized to [0, 1]
    const double aLocalParam = (aParam - aSpanStart) / aSpanLength;

    myParams.SetValue(i - 1, ParamWithSpan{aParam, aLocalParam, aSpanIdx});
  }

  // Compute span ranges for optimized iteration
  computeSpanRanges(myParams, aKnots, mySpanRanges);
}

//==================================================================================================

int BSplCLib_GridEvaluator::locateSpan(double theParam) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(myDegree,
                            myFlatKnots->Array1(),
                            BSplCLib::NoMults(),
                            theParam,
                            myPeriodic,
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int BSplCLib_GridEvaluator::locateSpanWithHint(double theParam, int theHint) const
{
  const TColStd_Array1OfReal& aKnots = myFlatKnots->Array1();
  const int                   aLower = aKnots.Lower() + myDegree;
  const int                   aUpper = aKnots.Upper() - myDegree - 1;

  // Quick check if hint is still valid
  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = aKnots.Value(theHint);
    const double aSpanEnd   = aKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
    {
      return theHint;
    }

    // Check next span (common case for sorted parameters)
    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = aKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
      {
        return theHint + 1;
      }
    }
  }

  // Fall back to standard locate
  return locateSpan(theParam);
}

//==================================================================================================

void BSplCLib_GridEvaluator::computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                               const TColStd_Array1OfReal&    theFlatKnots,
                                               NCollection_Array1<SpanRange>& theSpanRanges)
{
  if (theParams.IsEmpty())
  {
    theSpanRanges.Resize(0, -1, false);
    return;
  }

  // Count distinct spans
  int aNbSpans     = 1;
  int aCurrentSpan = theParams.Value(0).SpanIndex;
  for (int i = 1; i < theParams.Size(); ++i)
  {
    if (theParams.Value(i).SpanIndex != aCurrentSpan)
    {
      ++aNbSpans;
      aCurrentSpan = theParams.Value(i).SpanIndex;
    }
  }

  theSpanRanges.Resize(0, aNbSpans - 1, false);

  // Fill span ranges
  aCurrentSpan    = theParams.Value(0).SpanIndex;
  int aRangeStart = 0;
  int aRangeIdx   = 0;

  for (int i = 1; i < theParams.Size(); ++i)
  {
    const int aSpan = theParams.Value(i).SpanIndex;
    if (aSpan != aCurrentSpan)
    {
      // Compute span geometry
      const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
      const double aSpanLength  = theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart;
      const double aSpanMid     = aSpanStart + aSpanLength * 0.5;
      const double aSpanHalfLen = aSpanLength * 0.5;

      theSpanRanges.SetValue(aRangeIdx,
                             SpanRange{aCurrentSpan, aRangeStart, i, aSpanMid, aSpanHalfLen});
      ++aRangeIdx;
      aCurrentSpan = aSpan;
      aRangeStart  = i;
    }
  }

  // Add the last range
  const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
  const double aSpanLength  = theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart;
  const double aSpanMid     = aSpanStart + aSpanLength * 0.5;
  const double aSpanHalfLen = aSpanLength * 0.5;

  theSpanRanges.SetValue(
    aRangeIdx,
    SpanRange{aCurrentSpan, aRangeStart, theParams.Size(), aSpanMid, aSpanHalfLen});
}

//==================================================================================================

void BSplCLib_GridEvaluator::rebuildCache(double theParam) const
{
  // Create cache if needed
  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(myDegree,
                                 myPeriodic,
                                 myFlatKnots->Array1(),
                                 myPoles->Array1(),
                                 myWeights.IsNull() ? nullptr : &myWeights->Array1());
  }

  // Build cache for the parameter
  myCache->BuildCache(theParam,
                      myFlatKnots->Array1(),
                      myPoles->Array1(),
                      myWeights.IsNull() ? nullptr : &myWeights->Array1());
}

//==================================================================================================

NCollection_Array1<gp_Pnt> BSplCLib_GridEvaluator::EvaluateGrid() const
{
  if (!myIsInitialized || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbParams = myParams.Size();
  NCollection_Array1<gp_Pnt> aPoints(1, aNbParams);

  // Iterate over pre-computed span ranges
  // For each span, rebuild cache once, then evaluate all points within that span block
  for (int iRange = 0; iRange < mySpanRanges.Size(); ++iRange)
  {
    const SpanRange& aRange = mySpanRanges.Value(iRange);

    // Rebuild cache once for this span block using first parameter in range
    rebuildCache(myParams.Value(aRange.StartIdx).Param);

    // Evaluate all points in this span block using pre-computed local parameters
    for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
    {
      myCache->D0Local(myParams.Value(i).LocalParam, aPoints.ChangeValue(i + 1));
    }
  }

  return aPoints;
}
