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

#include <BSplSLib_GridEvaluator.hxx>

#include <BSplSLib.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>

//==================================================================================================

BSplSLib_GridEvaluator::BSplSLib_GridEvaluator()
    : myDegreeU(0),
      myDegreeV(0),
      myURational(false),
      myVRational(false),
      myUPeriodic(false),
      myVPeriodic(false),
      myIsInitialized(false)
{
}

//==================================================================================================

bool BSplSLib_GridEvaluator::Initialize(int                                  theDegreeU,
                                        int                                  theDegreeV,
                                        const Handle(TColgp_HArray2OfPnt)&   thePoles,
                                        const Handle(TColStd_HArray2OfReal)& theWeights,
                                        const Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                        const Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                        bool                                 theURational,
                                        bool                                 theVRational,
                                        bool                                 theUPeriodic,
                                        bool                                 theVPeriodic)
{
  myIsInitialized = false;

  // Validate inputs
  if (theDegreeU < 1 || theDegreeV < 1)
    return false;
  if (thePoles.IsNull())
    return false;
  if (theUFlatKnots.IsNull() || theVFlatKnots.IsNull())
    return false;
  if ((theURational || theVRational) && theWeights.IsNull())
    return false;

  // Validate weights dimensions
  if (!theWeights.IsNull())
  {
    if (theWeights->RowLength() != thePoles->RowLength()
        || theWeights->ColLength() != thePoles->ColLength())
      return false;
  }

  // Validate flat knots sizes (non-periodic case)
  const int aNbUPoles        = thePoles->ColLength();
  const int aNbVPoles        = thePoles->RowLength();
  const int anExpectedUKnots = aNbUPoles + theDegreeU + 1;
  const int anExpectedVKnots = aNbVPoles + theDegreeV + 1;

  if (!theUPeriodic && theUFlatKnots->Length() != anExpectedUKnots)
    return false;
  if (!theVPeriodic && theVFlatKnots->Length() != anExpectedVKnots)
    return false;

  myDegreeU       = theDegreeU;
  myDegreeV       = theDegreeV;
  myPoles         = thePoles;
  myWeights       = theWeights;
  myUFlatKnots    = theUFlatKnots;
  myVFlatKnots    = theVFlatKnots;
  myURational     = theURational;
  myVRational     = theVRational;
  myUPeriodic     = theUPeriodic;
  myVPeriodic     = theVPeriodic;
  myIsInitialized = true;

  // Clear old parameters
  myUParams.Resize(0, -1, false);
  myVParams.Resize(0, -1, false);
  myUSpanRanges.Resize(0, -1, false);
  myVSpanRanges.Resize(0, -1, false);

  // Reset cache
  myCache.Nullify();

  return true;
}

//==================================================================================================

void BSplSLib_GridEvaluator::SetUParams(const TColStd_Array1OfReal& theUParams)
{
  if (!myIsInitialized)
    return;

  const int aNbParams = theUParams.Length();
  if (aNbParams < 2)
    return;

  // Resize array (0-based indexing)
  myUParams.Resize(0, aNbParams - 1, false);

  const TColStd_Array1OfReal& aKnots = myUFlatKnots->Array1();

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aKnots.Lower() + myDegreeU;

  for (int i = 0; i < aNbParams; ++i)
  {
    const double aParam   = theUParams.Value(theUParams.Lower() + i);
    const int    aSpanIdx = locateSpanWithHint(aParam, true, aPrevSpan);
    aPrevSpan             = aSpanIdx;

    // Pre-compute local parameter in [-1, 1] range using BSplSLib convention:
    // SpanMid = SpanStart + SpanLength/2, SpanHalfLen = SpanLength/2
    // LocalParam = (Param - SpanMid) / SpanHalfLen
    const double aSpanStart   = aKnots.Value(aSpanIdx);
    const double aSpanHalfLen = 0.5 * (aKnots.Value(aSpanIdx + 1) - aSpanStart);
    const double aSpanMid     = aSpanStart + aSpanHalfLen;
    const double aLocalParam  = (aParam - aSpanMid) / aSpanHalfLen;

    myUParams.SetValue(i, {aParam, aLocalParam, aSpanIdx});
  }

  // Pre-compute span ranges with adjusted span mid/half-length for cache convention
  computeSpanRanges(myUParams, aKnots, myUSpanRanges);
}

//==================================================================================================

void BSplSLib_GridEvaluator::SetVParams(const TColStd_Array1OfReal& theVParams)
{
  if (!myIsInitialized)
    return;

  const int aNbParams = theVParams.Length();
  if (aNbParams < 2)
    return;

  // Resize array (0-based indexing)
  myVParams.Resize(0, aNbParams - 1, false);

  const TColStd_Array1OfReal& aKnots = myVFlatKnots->Array1();

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aKnots.Lower() + myDegreeV;

  for (int i = 0; i < aNbParams; ++i)
  {
    const double aParam   = theVParams.Value(theVParams.Lower() + i);
    const int    aSpanIdx = locateSpanWithHint(aParam, false, aPrevSpan);
    aPrevSpan             = aSpanIdx;

    // Pre-compute local parameter in [-1, 1] range using BSplSLib convention:
    // SpanMid = SpanStart + SpanLength/2, SpanHalfLen = SpanLength/2
    // LocalParam = (Param - SpanMid) / SpanHalfLen
    const double aSpanStart   = aKnots.Value(aSpanIdx);
    const double aSpanHalfLen = 0.5 * (aKnots.Value(aSpanIdx + 1) - aSpanStart);
    const double aSpanMid     = aSpanStart + aSpanHalfLen;
    const double aLocalParam  = (aParam - aSpanMid) / aSpanHalfLen;

    myVParams.SetValue(i, {aParam, aLocalParam, aSpanIdx});
  }

  // Pre-compute span ranges with adjusted span mid/half-length for cache convention
  computeSpanRanges(myVParams, aKnots, myVSpanRanges);
}

//==================================================================================================

int BSplSLib_GridEvaluator::locateSpan(double theParam, bool theUDir) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;

  if (theUDir)
  {
    BSplCLib::LocateParameter(myDegreeU,
                              myUFlatKnots->Array1(),
                              BSplCLib::NoMults(),
                              theParam,
                              myUPeriodic,
                              aSpanIndex,
                              aNewParam);
  }
  else
  {
    BSplCLib::LocateParameter(myDegreeV,
                              myVFlatKnots->Array1(),
                              BSplCLib::NoMults(),
                              theParam,
                              myVPeriodic,
                              aSpanIndex,
                              aNewParam);
  }
  return aSpanIndex;
}

//==================================================================================================

int BSplSLib_GridEvaluator::locateSpanWithHint(double theParam, bool theUDir, int theHint) const
{
  const TColStd_Array1OfReal& aKnots = theUDir ? myUFlatKnots->Array1() : myVFlatKnots->Array1();
  const int                   aDeg   = theUDir ? myDegreeU : myDegreeV;
  const int                   aLower = aKnots.Lower() + aDeg;
  const int                   aUpper = aKnots.Upper() - aDeg - 1;

  // Quick check if hint is still valid
  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = aKnots.Value(theHint);
    const double aSpanEnd   = aKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
      return theHint;

    // Check next span (common case for sorted parameters)
    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = aKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
        return theHint + 1;
    }
  }

  // Fall back to standard locate
  return locateSpan(theParam, theUDir);
}

//==================================================================================================

void BSplSLib_GridEvaluator::computeSpanRanges(const NCollection_Array1<ParamWithSpan>& theParams,
                                               const TColStd_Array1OfReal&    theFlatKnots,
                                               NCollection_Array1<SpanRange>& theSpanRanges)
{
  const int aNbParams = theParams.Size();
  if (aNbParams == 0)
  {
    theSpanRanges.Resize(0, -1, false);
    return;
  }

  // First pass: count distinct spans
  int aNbRanges    = 1;
  int aCurrentSpan = theParams.Value(0).SpanIndex;
  for (int i = 1; i < aNbParams; ++i)
  {
    if (theParams.Value(i).SpanIndex != aCurrentSpan)
    {
      ++aNbRanges;
      aCurrentSpan = theParams.Value(i).SpanIndex;
    }
  }

  // Allocate exactly the needed size (0-based indexing)
  theSpanRanges.Resize(0, aNbRanges - 1, false);

  // Second pass: fill ranges with adjusted span mid/half-length (BSplSLib cache convention)
  aCurrentSpan    = theParams.Value(0).SpanIndex;
  int aRangeStart = 0;
  int aRangeIdx   = 0;
  for (int i = 1; i < aNbParams; ++i)
  {
    if (theParams.Value(i).SpanIndex != aCurrentSpan)
    {
      const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
      const double aSpanHalfLen = 0.5 * (theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart);
      const double aSpanMid     = aSpanStart + aSpanHalfLen;
      theSpanRanges.SetValue(aRangeIdx++, {aCurrentSpan, aRangeStart, i, aSpanMid, aSpanHalfLen});
      aCurrentSpan = theParams.Value(i).SpanIndex;
      aRangeStart  = i;
    }
  }
  // Add the last range
  const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
  const double aSpanHalfLen = 0.5 * (theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart);
  const double aSpanMid     = aSpanStart + aSpanHalfLen;
  theSpanRanges.SetValue(aRangeIdx, {aCurrentSpan, aRangeStart, aNbParams, aSpanMid, aSpanHalfLen});
}

//==================================================================================================

void BSplSLib_GridEvaluator::rebuildCache(double theUParam, double theVParam) const
{
  // Create cache if needed
  if (myCache.IsNull())
  {
    myCache = new BSplSLib_Cache(myDegreeU,
                                 myUPeriodic,
                                 myUFlatKnots->Array1(),
                                 myDegreeV,
                                 myVPeriodic,
                                 myVFlatKnots->Array1(),
                                 myWeights.IsNull() ? nullptr : &myWeights->Array2());
  }

  // Build cache for the span
  myCache->BuildCache(theUParam,
                      theVParam,
                      myUFlatKnots->Array1(),
                      myVFlatKnots->Array1(),
                      myPoles->Array2(),
                      myWeights.IsNull() ? nullptr : &myWeights->Array2());
}

//==================================================================================================

NCollection_Array2<gp_Pnt> BSplSLib_GridEvaluator::EvaluateGrid() const
{
  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (!myIsInitialized || aNbUParams == 0 || aNbVParams == 0)
  {
    return NCollection_Array2<gp_Pnt>();
  }

  NCollection_Array2<gp_Pnt> aPoints(1, aNbUParams, 1, aNbVParams);

  // Threshold: minimum points in a span block to justify cache rebuild overhead.
  // Below this threshold, direct BSplSLib::D0 is faster than cache setup + evaluation.
  // Value determined empirically - cache BuildCache() has fixed overhead.
  constexpr int THE_CACHE_THRESHOLD = 4;

  // Pre-fetch arrays for direct evaluation path
  const TColgp_Array2OfPnt&   aPoles      = myPoles->Array2();
  const TColStd_Array2OfReal* aWeights    = myWeights.IsNull() ? nullptr : &myWeights->Array2();
  const TColStd_Array1OfReal& aUFlatKnots = myUFlatKnots->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = myVFlatKnots->Array1();

  // Iterate over all (U_span, V_span) combinations.
  const int aNbUSpans = myUSpanRanges.Size();
  const int aNbVSpans = myVSpanRanges.Size();

  for (int iURange = 0; iURange < aNbUSpans; ++iURange)
  {
    const SpanRange& aURange    = myUSpanRanges.Value(iURange);
    const int        aNbUInSpan = aURange.EndIdx - aURange.StartIdx;

    for (int iVRange = 0; iVRange < aNbVSpans; ++iVRange)
    {
      const SpanRange& aVRange    = myVSpanRanges.Value(iVRange);
      const int        aNbVInSpan = aVRange.EndIdx - aVRange.StartIdx;
      const int        aNbPoints  = aNbUInSpan * aNbVInSpan;

      if (aNbPoints >= THE_CACHE_THRESHOLD)
      {
        // Large block: use cache with pre-computed local parameters for efficiency
        rebuildCache(myUParams.Value(aURange.StartIdx).Param,
                     myVParams.Value(aVRange.StartIdx).Param);

        for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
        {
          const double aLocalU = myUParams.Value(iu).LocalParam;
          for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
          {
            // Use D0Local with pre-computed local parameters - bypasses
            // PeriodicNormalization and local param calculation per point
            myCache->D0Local(aLocalU,
                             myVParams.Value(iv).LocalParam,
                             aPoints.ChangeValue(iu + 1, iv + 1));
          }
        }
      }
      else
      {
        // Small block: direct evaluation without cache overhead
        for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
        {
          const double aUParam   = myUParams.Value(iu).Param;
          const int    aUSpanIdx = aURange.SpanIndex;
          for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
          {
            const double aVParam   = myVParams.Value(iv).Param;
            const int    aVSpanIdx = aVRange.SpanIndex;
            BSplSLib::D0(aUParam,
                         aVParam,
                         aUSpanIdx,
                         aVSpanIdx,
                         aPoles,
                         aWeights,
                         aUFlatKnots,
                         aVFlatKnots,
                         nullptr, // UMults - nullptr for flat knots
                         nullptr, // VMults - nullptr for flat knots
                         myDegreeU,
                         myDegreeV,
                         myURational,
                         myVRational,
                         myUPeriodic,
                         myVPeriodic,
                         aPoints.ChangeValue(iu + 1, iv + 1));
          }
        }
      }
    }
  }

  return aPoints;
}
