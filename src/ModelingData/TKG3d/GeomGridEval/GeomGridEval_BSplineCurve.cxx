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

#include <GeomGridEval_BSplineCurve.hxx>

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>

namespace
{
//! Threshold for using cache vs direct evaluation (same logic as surface).
constexpr int THE_CACHE_THRESHOLD = 4;

//! Iterate over span blocks and dispatch to cache or direct evaluation functors.
template <typename SpanArray,
          typename ParamArray,
          typename BuildCacheFunc,
          typename CacheEvalFunc,
          typename DirectEvalFunc>
void iterateSpanBlocks(const SpanArray&  theSpanRanges,
                       const ParamArray& theParams,
                       BuildCacheFunc&&  theBuildCache,
                       CacheEvalFunc&&   theCacheEval,
                       DirectEvalFunc&&  theDirectEval)
{
  const int aNbRanges = theSpanRanges.Size();
  for (int iRange = 0; iRange < aNbRanges; ++iRange)
  {
    const auto& aRange    = theSpanRanges.Value(iRange);
    const int   aNbInSpan = aRange.EndIdx - aRange.StartIdx;

    if (aNbInSpan >= THE_CACHE_THRESHOLD)
    {
      // Large block: reuse cache
      theBuildCache(theParams.Value(aRange.StartIdx).Param);

      for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
      {
        theCacheEval(i, theParams.Value(i).LocalParam);
      }
    }
    else
    {
      // Small block: cheaper to evaluate directly
      const int aSpanIdx = aRange.SpanIndex;
      for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
      {
        theDirectEval(i, theParams.Value(i).Param, aSpanIdx);
      }
    }
  }
}

} // namespace

//==================================================================================================

void GeomGridEval_BSplineCurve::SetParams(const TColStd_Array1OfReal& theParams)
{
  mySpanRanges = NCollection_Array1<SpanRange>(); // Clear cached span data
  myRawParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myRawParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

void GeomGridEval_BSplineCurve::prepare() const
{
  // Check if already prepared using span ranges emptiness
  if (!mySpanRanges.IsEmpty())
  {
    return;
  }

  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    myParams = NCollection_Array1<ParamWithSpan>();
    return;
  }

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    myParams = NCollection_Array1<ParamWithSpan>();
    return;
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const int aDegree   = myGeom->Degree();
  const int aNbParams = myRawParams.Size();
  myParams.Resize(0, aNbParams - 1, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = aFlatKnots.Lower() + aDegree;

  for (int i = myRawParams.Lower(); i <= myRawParams.Upper(); ++i)
  {
    const double aParam   = myRawParams.Value(i);
    const int    aSpanIdx = locateSpanWithHint(aParam, aPrevSpan, aFlatKnots);
    aPrevSpan             = aSpanIdx;

    // Pre-compute local parameter for this span
    const double aSpanStart  = aFlatKnots.Value(aSpanIdx);
    const double aSpanLength = aFlatKnots.Value(aSpanIdx + 1) - aSpanStart;
    const double aLocalParam = (aParam - aSpanStart) / aSpanLength;

    myParams.SetValue(i - myRawParams.Lower(), ParamWithSpan{aParam, aLocalParam, aSpanIdx});
  }

  // Compute span ranges for optimized iteration
  computeSpanRanges(myParams, aFlatKnots, mySpanRanges);
}

//==================================================================================================

int GeomGridEval_BSplineCurve::locateSpan(double                      theParam,
                                          const TColStd_Array1OfReal& theFlatKnots) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(myGeom->Degree(),
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            myGeom->IsPeriodic(),
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//==================================================================================================

int GeomGridEval_BSplineCurve::locateSpanWithHint(double                      theParam,
                                                  int                         theHint,
                                                  const TColStd_Array1OfReal& theFlatKnots) const
{
  const int aDegree = myGeom->Degree();
  const int aLower  = theFlatKnots.Lower() + aDegree;
  const int aUpper  = theFlatKnots.Upper() - aDegree - 1;

  // Quick check if hint is still valid
  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = theFlatKnots.Value(theHint);
    const double aSpanEnd   = theFlatKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
    {
      return theHint;
    }

    // Check next span (common case for sorted parameters)
    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = theFlatKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
      {
        return theHint + 1;
      }
    }
  }

  // Fall back to standard locate
  return locateSpan(theParam, theFlatKnots);
}

//==================================================================================================

void GeomGridEval_BSplineCurve::computeSpanRanges(
  const NCollection_Array1<ParamWithSpan>& theParams,
  const TColStd_Array1OfReal&              theFlatKnots,
  NCollection_Array1<SpanRange>&           theSpanRanges)
{
  if (theParams.IsEmpty())
  {
    theSpanRanges = NCollection_Array1<SpanRange>();
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

NCollection_Array1<gp_Pnt> GeomGridEval_BSplineCurve::EvaluateGrid() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  // Prepare span data if needed
  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbParams = myParams.Size();
  NCollection_Array1<gp_Pnt> aPoints(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<gp_Pnt>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const Handle(TColgp_HArray1OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array1<gp_Pnt>();
  }
  const TColgp_Array1OfPnt& aPoles = aPolesHandle->Array1();

  const Handle(TColStd_HArray1OfReal)& aWeightsHandle = myGeom->HArrayWeights();
  const bool                           isRational     = myGeom->IsRational();
  if (isRational && aWeightsHandle.IsNull())
  {
    return NCollection_Array1<gp_Pnt>();
  }
  const TColStd_Array1OfReal* aWeights = isRational ? &aWeightsHandle->Array1() : nullptr;

  const int  aDegree    = myGeom->Degree();
  const bool isPeriodic = myGeom->IsPeriodic();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);
  }

  iterateSpanBlocks(
    mySpanRanges,
    myParams,
    [&](double theParam) { myCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    // Cache evaluation (local parameter)
    [&](int theIdx, double theLocalParam) {
      myCache->D0Local(theLocalParam, aPoints.ChangeValue(theIdx + 1));
    },
    // Direct evaluation (global parameter + span index)
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      BSplCLib::D0(theParam,
                   theSpanIdx,
                   aDegree,
                   isPeriodic,
                   aPoles,
                   aWeights,
                   aFlatKnots,
                   nullptr,
                   aPoint);
      aPoints.ChangeValue(theIdx + 1) = aPoint;
    });

  return aPoints;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BSplineCurve::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResults(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const Handle(TColgp_HArray1OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }
  const TColgp_Array1OfPnt& aPoles = aPolesHandle->Array1();

  const Handle(TColStd_HArray1OfReal)& aWeightsHandle = myGeom->HArrayWeights();
  const bool                           isRational     = myGeom->IsRational();
  if (isRational && aWeightsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }
  const TColStd_Array1OfReal* aWeights = isRational ? &aWeightsHandle->Array1() : nullptr;

  const int  aDegree    = myGeom->Degree();
  const bool isPeriodic = myGeom->IsPeriodic();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);
  }

  iterateSpanBlocks(
    mySpanRanges,
    myParams,
    [&](double theParam) { myCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    // Cache evaluation
    [&](int theIdx, double theLocalParam) {
      gp_Pnt aPoint;
      gp_Vec aD1;
      myCache->D1Local(theLocalParam, aPoint, aD1);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1};
    },
    // Direct evaluation
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1;
      BSplCLib::D1(theParam,
                   theSpanIdx,
                   aDegree,
                   isPeriodic,
                   aPoles,
                   aWeights,
                   aFlatKnots,
                   nullptr,
                   aPoint,
                   aD1);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BSplineCurve::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResults(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const Handle(TColgp_HArray1OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }
  const TColgp_Array1OfPnt& aPoles = aPolesHandle->Array1();

  const Handle(TColStd_HArray1OfReal)& aWeightsHandle = myGeom->HArrayWeights();
  const bool                           isRational     = myGeom->IsRational();
  if (isRational && aWeightsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }
  const TColStd_Array1OfReal* aWeights = isRational ? &aWeightsHandle->Array1() : nullptr;

  const int  aDegree    = myGeom->Degree();
  const bool isPeriodic = myGeom->IsPeriodic();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);
  }

  iterateSpanBlocks(
    mySpanRanges,
    myParams,
    [&](double theParam) { myCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    // Cache evaluation
    [&](int theIdx, double theLocalParam) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2;
      myCache->D2Local(theLocalParam, aPoint, aD1, aD2);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2};
    },
    // Direct evaluation
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2;
      BSplCLib::D2(theParam,
                   theSpanIdx,
                   aDegree,
                   isPeriodic,
                   aPoles,
                   aWeights,
                   aFlatKnots,
                   nullptr,
                   aPoint,
                   aD1,
                   aD2);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BSplineCurve::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResults(1, aNbParams);

  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const Handle(TColgp_HArray1OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }
  const TColgp_Array1OfPnt& aPoles = aPolesHandle->Array1();

  const Handle(TColStd_HArray1OfReal)& aWeightsHandle = myGeom->HArrayWeights();
  const bool                           isRational     = myGeom->IsRational();
  if (isRational && aWeightsHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }
  const TColStd_Array1OfReal* aWeights = isRational ? &aWeightsHandle->Array1() : nullptr;

  const int  aDegree    = myGeom->Degree();
  const bool isPeriodic = myGeom->IsPeriodic();

  if (myCache.IsNull())
  {
    myCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);
  }

  iterateSpanBlocks(
    mySpanRanges,
    myParams,
    [&](double theParam) { myCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    // Cache evaluation
    [&](int theIdx, double theLocalParam) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2, aD3;
      myCache->D3Local(theLocalParam, aPoint, aD1, aD2, aD3);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2, aD3};
    },
    // Direct evaluation
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2, aD3;
      BSplCLib::D3(theParam,
                   theSpanIdx,
                   aDegree,
                   isPeriodic,
                   aPoles,
                   aWeights,
                   aFlatKnots,
                   nullptr,
                   aPoint,
                   aD1,
                   aD2,
                   aD3);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2, aD3};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BSplineCurve::EvaluateGridDN(int theN) const
{
  if (myGeom.IsNull() || myRawParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  prepare();

  if (myParams.IsEmpty() || mySpanRanges.IsEmpty())
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNbParams = myParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNbParams);

  // Reuse existing grid evaluators for orders 1-3
  if (theN == 1)
  {
    NCollection_Array1<GeomGridEval::CurveD1> aD1Grid = EvaluateGridD1();
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
  }
  else if (theN == 2)
  {
    NCollection_Array1<GeomGridEval::CurveD2> aD2Grid = EvaluateGridD2();
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
  }
  else if (theN == 3)
  {
    NCollection_Array1<GeomGridEval::CurveD3> aD3Grid = EvaluateGridD3();
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
  }
  else
  {
    // For orders > 3, check if derivative exists (depends on degree)
    // A B-spline of degree n has DN = 0 for N > n
    const int aDegree = myGeom->Degree();
    if (theN > aDegree)
    {
      const gp_Vec aZero(0.0, 0.0, 0.0);
      for (int i = 1; i <= aNbParams; ++i)
      {
        aResult.SetValue(i, aZero);
      }
    }
    else
    {
      // Use geometry DN method for higher orders
      for (int i = 1; i <= aNbParams; ++i)
      {
        aResult.SetValue(i, myGeom->DN(myParams.Value(i - 1).Param, theN));
      }
    }
  }
  return aResult;
}
