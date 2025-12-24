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
#include <BSplCLib_Cache.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>

namespace
{

//! Threshold for using cache vs direct evaluation.
constexpr int THE_CACHE_THRESHOLD = 4;

//! Parameter value with pre-computed span index and local parameter.
struct ParamWithSpan
{
  double Param;      //!< Original parameter value
  double LocalParam; //!< Pre-computed local parameter in [0, 1] range
  int    SpanIndex;  //!< Flat knot index identifying the span
  int    OrigIdx;    //!< Original index in the input array (0-based)
};

//! Range of parameter indices belonging to the same span.
struct SpanRange
{
  int    SpanIndex; //!< Flat knot index of this span
  int    StartIdx;  //!< First parameter index (0-based, inclusive)
  int    EndIdx;    //!< Past-the-end parameter index (exclusive)
  double SpanMid;   //!< Midpoint of span (for cache convention)
};

//! Find span index for a parameter value.
int locateSpan(double                      theParam,
               int                         theDegree,
               bool                        theIsPeriodic,
               const TColStd_Array1OfReal& theFlatKnots)
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;
  BSplCLib::LocateParameter(theDegree,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            theIsPeriodic,
                            aSpanIndex,
                            aNewParam);
  return aSpanIndex;
}

//! Find span index with a hint for better performance on sorted parameters.
int locateSpanWithHint(double                      theParam,
                       int                         theHint,
                       int                         theDegree,
                       bool                        theIsPeriodic,
                       const TColStd_Array1OfReal& theFlatKnots)
{
  const int aLower = theFlatKnots.Lower() + theDegree;
  const int aUpper = theFlatKnots.Upper() - theDegree - 1;

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
  return locateSpan(theParam, theDegree, theIsPeriodic, theFlatKnots);
}

//! Prepare parameters with span data.
void prepareParams(const TColStd_Array1OfReal&      theParams,
                   int                              theDegree,
                   bool                             theIsPeriodic,
                   const TColStd_Array1OfReal&      theFlatKnots,
                   NCollection_Array1<ParamWithSpan>& theParamsWithSpan,
                   NCollection_Array1<SpanRange>&     theSpanRanges)
{
  const int aNbParams = theParams.Size();
  theParamsWithSpan.Resize(0, aNbParams - 1, false);

  // Use hint-based span location for efficiency on sorted parameters
  int aPrevSpan = theFlatKnots.Lower() + theDegree;

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const double aParam   = theParams.Value(i);
    const int    aSpanIdx = locateSpanWithHint(aParam, aPrevSpan, theDegree, theIsPeriodic, theFlatKnots);
    aPrevSpan             = aSpanIdx;

    // Pre-compute local parameter for this span
    const double aSpanStart  = theFlatKnots.Value(aSpanIdx);
    const double aSpanLength = theFlatKnots.Value(aSpanIdx + 1) - aSpanStart;
    const double aLocalParam = (aSpanLength > 0.0) ? (aParam - aSpanStart) / aSpanLength : 0.0;

    const int aIdx = i - theParams.Lower();
    theParamsWithSpan.SetValue(aIdx, ParamWithSpan{aParam, aLocalParam, aSpanIdx, aIdx});
  }

  // Count distinct spans
  int aNbSpans     = 1;
  int aCurrentSpan = theParamsWithSpan.Value(0).SpanIndex;
  for (int i = 1; i < aNbParams; ++i)
  {
    if (theParamsWithSpan.Value(i).SpanIndex != aCurrentSpan)
    {
      ++aNbSpans;
      aCurrentSpan = theParamsWithSpan.Value(i).SpanIndex;
    }
  }

  theSpanRanges.Resize(0, aNbSpans - 1, false);

  // Fill span ranges
  aCurrentSpan    = theParamsWithSpan.Value(0).SpanIndex;
  int aRangeStart = 0;
  int aRangeIdx   = 0;

  for (int i = 1; i < aNbParams; ++i)
  {
    const int aSpan = theParamsWithSpan.Value(i).SpanIndex;
    if (aSpan != aCurrentSpan)
    {
      const double aSpanStart  = theFlatKnots.Value(aCurrentSpan);
      const double aSpanLength = theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart;
      const double aSpanMid    = aSpanStart + aSpanLength * 0.5;

      theSpanRanges.SetValue(aRangeIdx, SpanRange{aCurrentSpan, aRangeStart, i, aSpanMid});
      ++aRangeIdx;
      aCurrentSpan = aSpan;
      aRangeStart  = i;
    }
  }

  // Add the last range
  const double aSpanStart  = theFlatKnots.Value(aCurrentSpan);
  const double aSpanLength = theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart;
  const double aSpanMid    = aSpanStart + aSpanLength * 0.5;

  theSpanRanges.SetValue(aRangeIdx, SpanRange{aCurrentSpan, aRangeStart, aNbParams, aSpanMid});
}

//! Iterate over span blocks and dispatch to cache or direct evaluation functors.
template <typename BuildCacheFunc, typename CacheEvalFunc, typename DirectEvalFunc>
void iterateSpanBlocks(const NCollection_Array1<SpanRange>&     theSpanRanges,
                       const NCollection_Array1<ParamWithSpan>& theParams,
                       BuildCacheFunc&&                         theBuildCache,
                       CacheEvalFunc&&                          theCacheEval,
                       DirectEvalFunc&&                         theDirectEval)
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
        const auto& aP = theParams.Value(i);
        theCacheEval(aP.OrigIdx, aP.LocalParam);
      }
    }
    else
    {
      // Small block: cheaper to evaluate directly
      const int aSpanIdx = aRange.SpanIndex;
      for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
      {
        const auto& aP = theParams.Value(i);
        theDirectEval(aP.OrigIdx, aP.Param, aSpanIdx);
      }
    }
  }
}

} // namespace

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BSplineCurve::EvaluateGrid(
  const TColStd_Array1OfReal& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  // Get flat knots directly from geometry
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

  // Prepare parameters with span data
  NCollection_Array1<ParamWithSpan> aParamsWithSpan;
  NCollection_Array1<SpanRange>     aSpanRanges;
  prepareParams(theParams, aDegree, isPeriodic, aFlatKnots, aParamsWithSpan, aSpanRanges);

  const int                  aNbParams = theParams.Size();
  NCollection_Array1<gp_Pnt> aPoints(1, aNbParams);

  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);

  iterateSpanBlocks(
    aSpanRanges,
    aParamsWithSpan,
    [&](double theParam) { aCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    // Cache evaluation (local parameter)
    [&](int theIdx, double theLocalParam) {
      aCache->D0Local(theLocalParam, aPoints.ChangeValue(theIdx + 1));
    },
    // Direct evaluation (global parameter + span index)
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      BSplCLib::D0(theParam, theSpanIdx, aDegree, isPeriodic, aPoles, aWeights, aFlatKnots, nullptr, aPoint);
      aPoints.ChangeValue(theIdx + 1) = aPoint;
    });

  return aPoints;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BSplineCurve::EvaluateGridD1(
  const TColStd_Array1OfReal& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

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

  NCollection_Array1<ParamWithSpan> aParamsWithSpan;
  NCollection_Array1<SpanRange>     aSpanRanges;
  prepareParams(theParams, aDegree, isPeriodic, aFlatKnots, aParamsWithSpan, aSpanRanges);

  const int                                 aNbParams = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResults(1, aNbParams);

  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);

  iterateSpanBlocks(
    aSpanRanges,
    aParamsWithSpan,
    [&](double theParam) { aCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    [&](int theIdx, double theLocalParam) {
      gp_Pnt aPoint;
      gp_Vec aD1;
      aCache->D1Local(theLocalParam, aPoint, aD1);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1};
    },
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1;
      BSplCLib::D1(theParam, theSpanIdx, aDegree, isPeriodic, aPoles, aWeights, aFlatKnots, nullptr, aPoint, aD1);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BSplineCurve::EvaluateGridD2(
  const TColStd_Array1OfReal& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

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

  NCollection_Array1<ParamWithSpan> aParamsWithSpan;
  NCollection_Array1<SpanRange>     aSpanRanges;
  prepareParams(theParams, aDegree, isPeriodic, aFlatKnots, aParamsWithSpan, aSpanRanges);

  const int                                 aNbParams = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResults(1, aNbParams);

  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);

  iterateSpanBlocks(
    aSpanRanges,
    aParamsWithSpan,
    [&](double theParam) { aCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    [&](int theIdx, double theLocalParam) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2;
      aCache->D2Local(theLocalParam, aPoint, aD1, aD2);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2};
    },
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2;
      BSplCLib::D2(theParam, theSpanIdx, aDegree, isPeriodic, aPoles, aWeights, aFlatKnots, nullptr, aPoint, aD1, aD2);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BSplineCurve::EvaluateGridD3(
  const TColStd_Array1OfReal& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

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

  NCollection_Array1<ParamWithSpan> aParamsWithSpan;
  NCollection_Array1<SpanRange>     aSpanRanges;
  prepareParams(theParams, aDegree, isPeriodic, aFlatKnots, aParamsWithSpan, aSpanRanges);

  const int                                 aNbParams = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResults(1, aNbParams);

  Handle(BSplCLib_Cache) aCache = new BSplCLib_Cache(aDegree, isPeriodic, aFlatKnots, aPoles, aWeights);

  iterateSpanBlocks(
    aSpanRanges,
    aParamsWithSpan,
    [&](double theParam) { aCache->BuildCache(theParam, aFlatKnots, aPoles, aWeights); },
    [&](int theIdx, double theLocalParam) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2, aD3;
      aCache->D3Local(theLocalParam, aPoint, aD1, aD2, aD3);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2, aD3};
    },
    [&](int theIdx, double theParam, int theSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1, aD2, aD3;
      BSplCLib::D3(theParam, theSpanIdx, aDegree, isPeriodic, aPoles, aWeights, aFlatKnots, nullptr, aPoint, aD1, aD2, aD3);
      aResults.ChangeValue(theIdx + 1) = {aPoint, aD1, aD2, aD3};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BSplineCurve::EvaluateGridDN(
  const TColStd_Array1OfReal& theParams,
  int                         theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int aNbParams = theParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNbParams);

  // For B-spline curves, derivatives become zero when order exceeds degree
  const int aDegree = myGeom->Degree();
  if (theN > aDegree)
  {
    const gp_Vec aZeroVec(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aZeroVec);
    }
    return aResult;
  }

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aFlatKnotsHandle = myGeom->HArrayFlatKnots();
  if (aFlatKnotsHandle.IsNull())
  {
    return NCollection_Array1<gp_Vec>();
  }
  const TColStd_Array1OfReal& aFlatKnots = aFlatKnotsHandle->Array1();

  const Handle(TColgp_HArray1OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array1<gp_Vec>();
  }
  const TColgp_Array1OfPnt& aPoles = aPolesHandle->Array1();

  const Handle(TColStd_HArray1OfReal)& aWeightsHandle = myGeom->HArrayWeights();
  const bool                           isRational     = myGeom->IsRational();
  const TColStd_Array1OfReal* aWeights = (isRational && !aWeightsHandle.IsNull()) ? &aWeightsHandle->Array1() : nullptr;

  const TColStd_Array1OfReal&    aKnots     = myGeom->Knots();
  const TColStd_Array1OfInteger& aMults     = myGeom->Multiplicities();
  const bool                     isPeriodic = myGeom->IsPeriodic();

  NCollection_Array1<ParamWithSpan> aParamsWithSpan;
  NCollection_Array1<SpanRange>     aSpanRanges;
  prepareParams(theParams, aDegree, isPeriodic, aFlatKnots, aParamsWithSpan, aSpanRanges);

  // Use BSplCLib::DN directly with pre-computed span indices
  const int aNbRanges = aSpanRanges.Size();
  for (int iRange = 0; iRange < aNbRanges; ++iRange)
  {
    const auto& aRange   = aSpanRanges.Value(iRange);
    const int   aSpanIdx = aRange.SpanIndex;

    for (int i = aRange.StartIdx; i < aRange.EndIdx; ++i)
    {
      const auto& aP = aParamsWithSpan.Value(i);
      gp_Vec aDN;
      BSplCLib::DN(aP.Param, theN, aSpanIdx, aDegree, isPeriodic, aPoles, aWeights, aKnots, &aMults, aDN);
      aResult.SetValue(aP.OrigIdx + 1, aDN);
    }
  }
  return aResult;
}
