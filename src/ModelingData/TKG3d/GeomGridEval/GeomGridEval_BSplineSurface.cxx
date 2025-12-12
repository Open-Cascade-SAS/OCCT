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

#include <GeomGridEval_BSplineSurface.hxx>

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

namespace
{
//! Threshold for using cache vs direct evaluation.
//! For small spans (few points), direct BSplSLib evaluation is faster than building cache.
constexpr int THE_CACHE_THRESHOLD = 4;

//! @brief Iterates over UV span blocks and invokes evaluation functors.
//!
//! This helper encapsulates the common pattern of iterating over pre-computed span ranges,
//! deciding whether to use cache or direct evaluation based on block size, and invoking
//! the appropriate evaluation functor.
//!
//! @tparam CacheEvalFunc   Functor type for cache-based evaluation: void(int iu, int iv, double
//! uParam, double vParam)
//! @tparam DirectEvalFunc  Functor type for direct evaluation: void(int iu, int iv, double uParam,
//! double vParam, int uSpanIdx, int vSpanIdx)
//!
//! @param theUSpanRanges  Pre-computed U span ranges
//! @param theVSpanRanges  Pre-computed V span ranges
//! @param theUParams      U parameters with span info
//! @param theVParams      V parameters with span info
//! @param theBuildCache   Functor that rebuilds cache for a span block
//! @param theCacheEval    Functor called for cache evaluation (large blocks)
//! @param theDirectEval   Functor called for direct evaluation (small blocks)
template <typename BuildCacheFunc, typename CacheEvalFunc, typename DirectEvalFunc>
void iterateSpanBlocks(
  const NCollection_Array1<GeomGridEval_BSplineSurface::SpanRange>&     theUSpanRanges,
  const NCollection_Array1<GeomGridEval_BSplineSurface::SpanRange>&     theVSpanRanges,
  const NCollection_Array1<GeomGridEval_BSplineSurface::ParamWithSpan>& theUParams,
  const NCollection_Array1<GeomGridEval_BSplineSurface::ParamWithSpan>& theVParams,
  BuildCacheFunc&&                                                      theBuildCache,
  CacheEvalFunc&&                                                       theCacheEval,
  DirectEvalFunc&&                                                      theDirectEval)
{
  const int aNbUSpans = theUSpanRanges.Size();
  const int aNbVSpans = theVSpanRanges.Size();

  for (int iURange = 0; iURange < aNbUSpans; ++iURange)
  {
    const auto& aURange    = theUSpanRanges.Value(iURange);
    const int   aNbUInSpan = aURange.EndIdx - aURange.StartIdx;

    for (int iVRange = 0; iVRange < aNbVSpans; ++iVRange)
    {
      const auto& aVRange    = theVSpanRanges.Value(iVRange);
      const int   aNbVInSpan = aVRange.EndIdx - aVRange.StartIdx;
      const int   aNbPoints  = aNbUInSpan * aNbVInSpan;

      if (aNbPoints >= THE_CACHE_THRESHOLD)
      {
        // Large block: use cache
        theBuildCache(theUParams.Value(aURange.StartIdx).Param,
                      theVParams.Value(aVRange.StartIdx).Param);

        for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
        {
          const double aLocalU = theUParams.Value(iu).LocalParam;
          for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
          {
            theCacheEval(iu, iv, aLocalU, theVParams.Value(iv).LocalParam);
          }
        }
      }
      else
      {
        // Small block: direct evaluation
        for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
        {
          const double aUParam   = theUParams.Value(iu).Param;
          const int    aUSpanIdx = aURange.SpanIndex;

          for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
          {
            theDirectEval(iu,
                          iv,
                          aUParam,
                          theVParams.Value(iv).Param,
                          aUSpanIdx,
                          aVRange.SpanIndex);
          }
        }
      }
    }
  }
}

} // namespace

//==================================================================================================

void GeomGridEval_BSplineSurface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                              const TColStd_Array1OfReal& theVParams)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  myRawUParams.Resize(1, aNbU, false);
  for (int i = 1; i <= aNbU; ++i)
  {
    myRawUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i - 1));
  }

  myRawVParams.Resize(1, aNbV, false);
  for (int j = 1; j <= aNbV; ++j)
  {
    myRawVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j - 1));
  }

  // Clear cached span data and cache
  myUSpanRanges = NCollection_Array1<SpanRange>();
  myVSpanRanges = NCollection_Array1<SpanRange>();
  myCache.Nullify();
}

//==================================================================================================

void GeomGridEval_BSplineSurface::prepare() const
{
  // Check if already prepared using span ranges emptiness
  if (!myUSpanRanges.IsEmpty())
  {
    return;
  }

  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    myUParams     = NCollection_Array1<ParamWithSpan>();
    myVParams     = NCollection_Array1<ParamWithSpan>();
    myUSpanRanges = NCollection_Array1<SpanRange>();
    myVSpanRanges = NCollection_Array1<SpanRange>();
    myCache.Nullify();
    return;
  }

  // Get flat knots directly from geometry
  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    myUParams     = NCollection_Array1<ParamWithSpan>();
    myVParams     = NCollection_Array1<ParamWithSpan>();
    myUSpanRanges = NCollection_Array1<SpanRange>();
    myVSpanRanges = NCollection_Array1<SpanRange>();
    myCache.Nullify();
    return;
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  const int aNbU = myRawUParams.Size();
  const int aNbV = myRawVParams.Size();

  // Process U parameters
  myUParams.Resize(0, aNbU - 1, false);

  int aPrevUSpan = aUFlatKnots.Lower() + aUDegree;
  for (int i = 1; i <= aNbU; ++i)
  {
    double    aParam   = myRawUParams.Value(i);
    const int aSpanIdx = locateSpanWithHint(aParam, true, aPrevUSpan, aUFlatKnots);
    aPrevUSpan         = aSpanIdx;

    const double aSpanStart   = aUFlatKnots.Value(aSpanIdx);
    const double aSpanHalfLen = 0.5 * (aUFlatKnots.Value(aSpanIdx + 1) - aSpanStart);
    const double aSpanMid     = aSpanStart + aSpanHalfLen;
    const double aLocalParam  = (aParam - aSpanMid) / aSpanHalfLen;

    myUParams.SetValue(i - 1, ParamWithSpan{myRawUParams.Value(i), aLocalParam, aSpanIdx});
  }

  // Process V parameters
  myVParams.Resize(0, aNbV - 1, false);

  int aPrevVSpan = aVFlatKnots.Lower() + aVDegree;
  for (int j = 1; j <= aNbV; ++j)
  {
    double    aParam   = myRawVParams.Value(j);
    const int aSpanIdx = locateSpanWithHint(aParam, false, aPrevVSpan, aVFlatKnots);
    aPrevVSpan         = aSpanIdx;

    const double aSpanStart   = aVFlatKnots.Value(aSpanIdx);
    const double aSpanHalfLen = 0.5 * (aVFlatKnots.Value(aSpanIdx + 1) - aSpanStart);
    const double aSpanMid     = aSpanStart + aSpanHalfLen;
    const double aLocalParam  = (aParam - aSpanMid) / aSpanHalfLen;

    myVParams.SetValue(j - 1, ParamWithSpan{myRawVParams.Value(j), aLocalParam, aSpanIdx});
  }

  // Compute span ranges
  computeSpanRanges(myUParams, aUFlatKnots, myUSpanRanges);
  computeSpanRanges(myVParams, aVFlatKnots, myVSpanRanges);
}

//==================================================================================================

int GeomGridEval_BSplineSurface::locateSpan(double&                     theParam,
                                            bool                        theUDir,
                                            const TColStd_Array1OfReal& theFlatKnots) const
{
  int    aSpanIndex = 0;
  double aNewParam  = theParam;

  const int  aDegree    = theUDir ? myGeom->UDegree() : myGeom->VDegree();
  const bool isPeriodic = theUDir ? myGeom->IsUPeriodic() : myGeom->IsVPeriodic();

  BSplCLib::LocateParameter(aDegree,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            isPeriodic,
                            aSpanIndex,
                            aNewParam);
  theParam = aNewParam;
  return aSpanIndex;
}

//==================================================================================================

int GeomGridEval_BSplineSurface::locateSpanWithHint(double&                     theParam,
                                                    bool                        theUDir,
                                                    int                         theHint,
                                                    const TColStd_Array1OfReal& theFlatKnots) const
{
  const int aDegree = theUDir ? myGeom->UDegree() : myGeom->VDegree();
  const int aLower  = theFlatKnots.Lower() + aDegree;
  const int aUpper  = theFlatKnots.Upper() - aDegree - 1;

  if (theHint >= aLower && theHint <= aUpper)
  {
    const double aSpanStart = theFlatKnots.Value(theHint);
    const double aSpanEnd   = theFlatKnots.Value(theHint + 1);

    if (theParam >= aSpanStart && theParam < aSpanEnd)
    {
      return theHint;
    }

    if (theHint < aUpper && theParam >= aSpanEnd)
    {
      const double aNextEnd = theFlatKnots.Value(theHint + 2);
      if (theParam < aNextEnd)
      {
        return theHint + 1;
      }
    }
  }

  return locateSpan(theParam, theUDir, theFlatKnots);
}

//==================================================================================================

void GeomGridEval_BSplineSurface::computeSpanRanges(
  const NCollection_Array1<ParamWithSpan>& theParams,
  const TColStd_Array1OfReal&              theFlatKnots,
  NCollection_Array1<SpanRange>&           theSpanRanges)
{
  const int aNbParams = theParams.Size();
  if (aNbParams == 0)
  {
    theSpanRanges = NCollection_Array1<SpanRange>();
    return;
  }

  // Count distinct spans
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

  theSpanRanges.Resize(0, aNbRanges - 1, false);

  // Fill ranges
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

      theSpanRanges.SetValue(aRangeIdx++,
                             SpanRange{aCurrentSpan, aRangeStart, i, aSpanMid, aSpanHalfLen});
      aCurrentSpan = theParams.Value(i).SpanIndex;
      aRangeStart  = i;
    }
  }

  // Last range
  const double aSpanStart   = theFlatKnots.Value(aCurrentSpan);
  const double aSpanHalfLen = 0.5 * (theFlatKnots.Value(aCurrentSpan + 1) - aSpanStart);
  const double aSpanMid     = aSpanStart + aSpanHalfLen;

  theSpanRanges.SetValue(aRangeIdx,
                         SpanRange{aCurrentSpan, aRangeStart, aNbParams, aSpanMid, aSpanHalfLen});
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BSplineSurface::EvaluateGrid() const
{
  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  prepare();

  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (aNbUParams == 0 || aNbVParams == 0 || myUSpanRanges.IsEmpty() || myVSpanRanges.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  NCollection_Array2<gp_Pnt> aPoints(1, aNbUParams, 1, aNbVParams);

  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    return NCollection_Array2<gp_Pnt>();
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();

  const Handle(TColgp_HArray2OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array2<gp_Pnt>();
  }
  const TColgp_Array2OfPnt& aPoles = aPolesHandle->Array2();

  const TColStd_Array2OfReal* aWeights = myGeom->Weights();

  if (myCache.IsNull())
  {
    myCache = new BSplSLib_Cache(aUDegree,
                                 myGeom->IsUPeriodic(),
                                 aUFlatKnots,
                                 aVDegree,
                                 myGeom->IsVPeriodic(),
                                 aVFlatKnots,
                                 aWeights);
  }

  // Use helper with lambdas for cache and direct evaluation
  iterateSpanBlocks(
    myUSpanRanges,
    myVSpanRanges,
    myUParams,
    myVParams,
    [&](double theUParam, double theVParam) {
      myCache->BuildCache(theUParam, theVParam, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
    },
    // Cache evaluation (local parameters)
    [&](int iu, int iv, double theLocalU, double theLocalV) {
      myCache->D0Local(theLocalU, theLocalV, aPoints.ChangeValue(iu + 1, iv + 1));
    },
    // Direct evaluation (global parameters + span indices)
    [&](int iu, int iv, double theUParam, double theVParam, int theUSpanIdx, int theVSpanIdx) {
      BSplSLib::D0(theUParam,
                   theVParam,
                   theUSpanIdx,
                   theVSpanIdx,
                   aPoles,
                   aWeights,
                   aUFlatKnots,
                   aVFlatKnots,
                   nullptr,
                   nullptr,
                   aUDegree,
                   aVDegree,
                   isURational,
                   isVRational,
                   isUPeriodic,
                   isVPeriodic,
                   aPoints.ChangeValue(iu + 1, iv + 1));
    });

  return aPoints;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BSplineSurface::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  prepare();

  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (aNbUParams == 0 || aNbVParams == 0 || myUSpanRanges.IsEmpty() || myVSpanRanges.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  NCollection_Array2<GeomGridEval::SurfD1> aResults(1, aNbUParams, 1, aNbVParams);

  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();

  const Handle(TColgp_HArray2OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }
  const TColgp_Array2OfPnt& aPoles = aPolesHandle->Array2();

  const TColStd_Array2OfReal* aWeights = myGeom->Weights();

  if (myCache.IsNull())
  {
    myCache = new BSplSLib_Cache(aUDegree,
                                 myGeom->IsUPeriodic(),
                                 aUFlatKnots,
                                 aVDegree,
                                 myGeom->IsVPeriodic(),
                                 aVFlatKnots,
                                 aWeights);
  }

  // Use helper with lambdas for cache and direct evaluation
  iterateSpanBlocks(
    myUSpanRanges,
    myVSpanRanges,
    myUParams,
    myVParams,
    [&](double theUParam, double theVParam) {
      myCache->BuildCache(theUParam, theVParam, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
    },
    // Cache evaluation (local parameters)
    [&](int iu, int iv, double theLocalU, double theLocalV) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      myCache->D1Local(theLocalU, theLocalV, aPoint, aD1U, aD1V);
      aResults.ChangeValue(iu + 1, iv + 1) = {aPoint, aD1U, aD1V};
    },
    // Direct evaluation (global parameters + span indices)
    [&](int iu, int iv, double theUParam, double theVParam, int theUSpanIdx, int theVSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      BSplSLib::D1(theUParam,
                   theVParam,
                   theUSpanIdx,
                   theVSpanIdx,
                   aPoles,
                   aWeights,
                   aUFlatKnots,
                   aVFlatKnots,
                   nullptr,
                   nullptr,
                   aUDegree,
                   aVDegree,
                   isURational,
                   isVRational,
                   isUPeriodic,
                   isVPeriodic,
                   aPoint,
                   aD1U,
                   aD1V);
      aResults.ChangeValue(iu + 1, iv + 1) = {aPoint, aD1U, aD1V};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BSplineSurface::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  prepare();

  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (aNbUParams == 0 || aNbVParams == 0 || myUSpanRanges.IsEmpty() || myVSpanRanges.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  NCollection_Array2<GeomGridEval::SurfD2> aResults(1, aNbUParams, 1, aNbVParams);

  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();

  const Handle(TColgp_HArray2OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }
  const TColgp_Array2OfPnt& aPoles = aPolesHandle->Array2();

  const TColStd_Array2OfReal* aWeights = myGeom->Weights();

  if (myCache.IsNull())
  {
    myCache = new BSplSLib_Cache(aUDegree,
                                 myGeom->IsUPeriodic(),
                                 aUFlatKnots,
                                 aVDegree,
                                 myGeom->IsVPeriodic(),
                                 aVFlatKnots,
                                 aWeights);
  }

  // Use helper with lambdas for cache and direct evaluation
  iterateSpanBlocks(
    myUSpanRanges,
    myVSpanRanges,
    myUParams,
    myVParams,
    [&](double theUParam, double theVParam) {
      myCache->BuildCache(theUParam, theVParam, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
    },
    // Cache evaluation (local parameters)
    [&](int iu, int iv, double theLocalU, double theLocalV) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      myCache->D2Local(theLocalU, theLocalV, aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResults.ChangeValue(iu + 1, iv + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    },
    // Direct evaluation (global parameters + span indices)
    [&](int iu, int iv, double theUParam, double theVParam, int theUSpanIdx, int theVSpanIdx) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      BSplSLib::D2(theUParam,
                   theVParam,
                   theUSpanIdx,
                   theVSpanIdx,
                   aPoles,
                   aWeights,
                   aUFlatKnots,
                   aVFlatKnots,
                   nullptr,
                   nullptr,
                   aUDegree,
                   aVDegree,
                   isURational,
                   isVRational,
                   isUPeriodic,
                   isVPeriodic,
                   aPoint,
                   aD1U,
                   aD1V,
                   aD2U,
                   aD2V,
                   aD2UV);
      aResults.ChangeValue(iu + 1, iv + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    });

  return aResults;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_BSplineSurface::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  prepare();

  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (aNbUParams == 0 || aNbVParams == 0 || myUSpanRanges.IsEmpty() || myVSpanRanges.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  NCollection_Array2<GeomGridEval::SurfD3> aResults(1, aNbUParams, 1, aNbVParams);

  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();

  const Handle(TColgp_HArray2OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }
  const TColgp_Array2OfPnt& aPoles = aPolesHandle->Array2();

  const TColStd_Array2OfReal* aWeights = myGeom->Weights();

  // D3 evaluation uses direct BSplSLib::D3 - no cache available for third derivatives
  const int aNbUSpans = myUSpanRanges.Size();
  const int aNbVSpans = myVSpanRanges.Size();

  for (int iURange = 0; iURange < aNbUSpans; ++iURange)
  {
    const auto& aURange = myUSpanRanges.Value(iURange);

    for (int iVRange = 0; iVRange < aNbVSpans; ++iVRange)
    {
      const auto& aVRange = myVSpanRanges.Value(iVRange);

      for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
      {
        const double aUParam   = myUParams.Value(iu).Param;
        const int    aUSpanIdx = aURange.SpanIndex;

        for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
        {
          gp_Pnt aPoint;
          gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;

          BSplSLib::D3(aUParam,
                       myVParams.Value(iv).Param,
                       aUSpanIdx,
                       aVRange.SpanIndex,
                       aPoles,
                       aWeights,
                       aUFlatKnots,
                       aVFlatKnots,
                       nullptr,
                       nullptr,
                       aUDegree,
                       aVDegree,
                       isURational,
                       isVRational,
                       isUPeriodic,
                       isVPeriodic,
                       aPoint,
                       aD1U,
                       aD1V,
                       aD2U,
                       aD2V,
                       aD2UV,
                       aD3U,
                       aD3V,
                       aD3UUV,
                       aD3UVV);

          aResults.ChangeValue(iu + 1, iv + 1) =
            {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
        }
      }
    }
  }

  return aResults;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_BSplineSurface::EvaluateGridDN(int theNU, int theNV) const
{
  if (myGeom.IsNull() || myRawUParams.IsEmpty() || myRawVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  prepare();

  const int aNbUParams = myUParams.Size();
  const int aNbVParams = myVParams.Size();

  if (aNbUParams == 0 || aNbVParams == 0 || myUSpanRanges.IsEmpty() || myVSpanRanges.IsEmpty())
  {
    return NCollection_Array2<gp_Vec>();
  }

  NCollection_Array2<gp_Vec> aResult(1, aNbUParams, 1, aNbVParams);

  // For B-spline surfaces, derivatives become zero when order exceeds degree in that direction
  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  if (theNU > aUDegree || theNV > aVDegree)
  {
    // All derivatives are zero
    const gp_Vec aZeroVec(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbUParams; ++i)
    {
      for (int j = 1; j <= aNbVParams; ++j)
      {
        aResult.SetValue(i, j, aZeroVec);
      }
    }
    return aResult;
  }

  // Get surface data for direct BSplSLib::DN call
  // Use flat knots with nullptr multiplicities (like D1/D2/D3 methods)
  const Handle(TColStd_HArray1OfReal)& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const Handle(TColStd_HArray1OfReal)& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull())
  {
    return NCollection_Array2<gp_Vec>();
  }
  const TColStd_Array1OfReal& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const TColStd_Array1OfReal& aVFlatKnots = aVFlatKnotsHandle->Array1();

  const Handle(TColgp_HArray2OfPnt)& aPolesHandle = myGeom->HArrayPoles();
  if (aPolesHandle.IsNull())
  {
    return NCollection_Array2<gp_Vec>();
  }
  const TColgp_Array2OfPnt& aPoles = aPolesHandle->Array2();

  const TColStd_Array2OfReal* aWeights    = myGeom->Weights();
  const bool                  isURational = myGeom->IsURational();
  const bool                  isVRational = myGeom->IsVRational();
  const bool                  isUPeriodic = myGeom->IsUPeriodic();
  const bool                  isVPeriodic = myGeom->IsVPeriodic();

  // Use BSplSLib::DN directly with pre-computed span indices
  const int aNbUSpans = myUSpanRanges.Size();
  const int aNbVSpans = myVSpanRanges.Size();

  for (int iURange = 0; iURange < aNbUSpans; ++iURange)
  {
    const auto& aURange = myUSpanRanges.Value(iURange);

    for (int iVRange = 0; iVRange < aNbVSpans; ++iVRange)
    {
      const auto& aVRange = myVSpanRanges.Value(iVRange);

      for (int iu = aURange.StartIdx; iu < aURange.EndIdx; ++iu)
      {
        const double aUParam   = myUParams.Value(iu).Param;
        const int    aUSpanIdx = aURange.SpanIndex;

        for (int iv = aVRange.StartIdx; iv < aVRange.EndIdx; ++iv)
        {
          gp_Vec aDN;
          BSplSLib::DN(aUParam,
                       myVParams.Value(iv).Param,
                       theNU,
                       theNV,
                       aUSpanIdx,
                       aVRange.SpanIndex,
                       aPoles,
                       aWeights,
                       aUFlatKnots,
                       aVFlatKnots,
                       nullptr,
                       nullptr,
                       aUDegree,
                       aVDegree,
                       isURational,
                       isVRational,
                       isUPeriodic,
                       isVPeriodic,
                       aDN);
          aResult.SetValue(iu + 1, iv + 1, aDN);
        }
      }
    }
  }

  return aResult;
}
