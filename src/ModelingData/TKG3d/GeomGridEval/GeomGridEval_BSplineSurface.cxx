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
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_LocalArray.hxx>

namespace
{

//! Minimum number of points in one U/V span-block product that amortizes cache construction.
constexpr size_t THE_POLYNOMIAL_CACHE_MIN_POINTS = 8;

//! Measured rational cache crossovers for one Cartesian span block.
constexpr size_t THE_RATIONAL_D0_D1_CACHE_MIN_POINTS = 4;
constexpr size_t THE_RATIONAL_D2_CACHE_MIN_POINTS    = 6;

//! Helper structure holding extracted B-spline surface data.
struct SurfaceData
{
  const NCollection_Array1<double>* UFlatKnots;
  const NCollection_Array1<double>* VFlatKnots;
  const NCollection_Array2<gp_Pnt>* Poles;
  const NCollection_Array2<double>* Weights;
  int                               UDegree;
  int                               VDegree;
  bool                              IsUPeriodic;
  bool                              IsVPeriodic;
  bool                              IsURational;
  bool                              IsVRational;
};

//! Extract surface data from geometry. Returns false if data is invalid.
bool extractSurfaceData(const occ::handle<Geom_BSplineSurface>& theGeom, SurfaceData& theData)
{
  if (theGeom.IsNull())
  {
    return false;
  }

  theData.UFlatKnots  = &theGeom->UKnotSequence();
  theData.VFlatKnots  = &theGeom->VKnotSequence();
  theData.Poles       = &theGeom->Poles();
  theData.Weights     = theGeom->Weights();
  theData.UDegree     = theGeom->UDegree();
  theData.VDegree     = theGeom->VDegree();
  theData.IsUPeriodic = theGeom->IsUPeriodic();
  theData.IsVPeriodic = theGeom->IsVPeriodic();
  theData.IsURational = theGeom->IsURational();
  theData.IsVRational = theGeom->IsVRational();
  return true;
}

//! Locate parameter and compute span index.
inline void locateSpan(double                            theParam,
                       const NCollection_Array1<double>& theFlatKnots,
                       int                               theDegree,
                       bool                              theIsPeriodic,
                       double&                           theAdjustedParam,
                       int&                              theSpanIdx)
{
  theAdjustedParam = theParam;
  BSplCLib::LocateParameter(theDegree,
                            theFlatKnots,
                            BSplCLib::NoMults(),
                            theParam,
                            theIsPeriodic,
                            theSpanIdx,
                            theAdjustedParam);
}

//! Compute a span midpoint without adding both potentially large knot values.
inline double spanMidpoint(const NCollection_Array1<double>& theFlatKnots, const int theSpan)
{
  const double aStart = theFlatKnots.Value(theSpan);
  return aStart + 0.5 * (theFlatKnots.Value(theSpan + 1) - aStart);
}

//! Compute span local parameter coefficients (mid and half-length).
inline void computeSpanCoeffs(const NCollection_Array1<double>& theFlatKnots,
                              const int                         theSpan,
                              double&                           theMid,
                              double&                           theHalfLen)
{
  const double aStart = theFlatKnots.Value(theSpan);
  theMid              = spanMidpoint(theFlatKnots, theSpan);
  theHalfLen          = theMid - aStart;
}

struct SpanBlock
{
  size_t Start;
  size_t Count;
  int    Span;
  double Mid;
};

struct PreparedParam
{
  double Adjusted;
  double Local;
};

using PreparedParams = NCollection_LocalArray<PreparedParam, 64>;
using SpanBlocks     = NCollection_LocalArray<SpanBlock, 32>;

size_t prepareSpanBlocks(const NCollection_Array1<double>& theParams,
                         const NCollection_Array1<double>& theFlatKnots,
                         const int                         theDegree,
                         const bool                        theIsPeriodic,
                         PreparedParams&                   thePreparedParams,
                         SpanBlocks&                       theBlocks)
{
  const size_t aNbParams     = theParams.Size();
  size_t       aNbBlocks     = 0;
  int          aPreviousSpan = -1;
  double       aSpanStart    = 0.0;
  double       aSpanEnd      = 0.0;
  double       aMid          = 0.0;
  double       aHalfLen      = 1.0;
  for (size_t anIndex = 0; anIndex < aNbParams; ++anIndex)
  {
    double anAdjusted = theParams.At(anIndex);
    int    aSpan      = 0;
    if (!theIsPeriodic && aPreviousSpan >= 0 && anAdjusted >= aSpanStart && anAdjusted < aSpanEnd)
    {
      aSpan = aPreviousSpan;
    }
    else
    {
      locateSpan(anAdjusted, theFlatKnots, theDegree, theIsPeriodic, anAdjusted, aSpan);
    }
    if (aSpan != aPreviousSpan)
    {
      computeSpanCoeffs(theFlatKnots, aSpan, aMid, aHalfLen);
      aSpanStart             = theFlatKnots.Value(aSpan);
      aSpanEnd               = theFlatKnots.Value(aSpan + 1);
      theBlocks[aNbBlocks++] = {anIndex, 1, aSpan, aMid};
      aPreviousSpan          = aSpan;
    }
    else
    {
      ++theBlocks[aNbBlocks - 1].Count;
    }
    thePreparedParams[anIndex] = {anAdjusted, (anAdjusted - aMid) / aHalfLen};
  }
  return aNbBlocks;
}

//! Evaluates Cartesian parameter blocks, using the cache only when its construction is amortized.
template <typename ResultT, typename CacheEvaluator, typename DirectEvaluator>
NCollection_Array2<ResultT> evaluateGridCached(const SurfaceData&                theData,
                                               const NCollection_Array1<double>& theUParams,
                                               const NCollection_Array1<double>& theVParams,
                                               const size_t                      theCacheThreshold,
                                               CacheEvaluator&&                  theCacheEvaluator,
                                               DirectEvaluator&&                 theDirectEvaluator)
{
  const size_t                aNbU = theUParams.Size();
  const size_t                aNbV = theVParams.Size();
  NCollection_Array2<ResultT> aGrid(1, static_cast<int>(aNbU), 1, static_cast<int>(aNbV));
  PreparedParams              aPreparedU(aNbU);
  PreparedParams              aPreparedV(aNbV);
  SpanBlocks                  aUBlocks(aNbU);
  SpanBlocks                  aVBlocks(aNbV);
  const size_t                aNbUBlocks = prepareSpanBlocks(theUParams,
                                                             *theData.UFlatKnots,
                                                             theData.UDegree,
                                                             theData.IsUPeriodic,
                                                             aPreparedU,
                                                             aUBlocks);
  const size_t                aNbVBlocks = prepareSpanBlocks(theVParams,
                                                             *theData.VFlatKnots,
                                                             theData.VDegree,
                                                             theData.IsVPeriodic,
                                                             aPreparedV,
                                                             aVBlocks);
  occ::handle<BSplSLib_Cache> aCache;

  for (size_t aUBlockIndex = 0; aUBlockIndex < aNbUBlocks; ++aUBlockIndex)
  {
    const SpanBlock& aUBlock = aUBlocks[aUBlockIndex];
    for (size_t aVBlockIndex = 0; aVBlockIndex < aNbVBlocks; ++aVBlockIndex)
    {
      const SpanBlock& aVBlock     = aVBlocks[aVBlockIndex];
      const bool       isCacheUsed = aUBlock.Count * aVBlock.Count >= theCacheThreshold;
      if (isCacheUsed)
      {
        if (aCache.IsNull())
        {
          aCache = new BSplSLib_Cache(theData.UDegree,
                                      theData.IsUPeriodic,
                                      *theData.UFlatKnots,
                                      theData.VDegree,
                                      theData.IsVPeriodic,
                                      *theData.VFlatKnots,
                                      theData.Weights);
        }
        aCache->BuildCache(aUBlock.Mid,
                           aVBlock.Mid,
                           *theData.UFlatKnots,
                           *theData.VFlatKnots,
                           *theData.Poles,
                           theData.Weights);
      }

      for (size_t aUIndex = aUBlock.Start; aUIndex < aUBlock.Start + aUBlock.Count; ++aUIndex)
      {
        for (size_t aVIndex = aVBlock.Start; aVIndex < aVBlock.Start + aVBlock.Count; ++aVIndex)
        {
          ResultT& aResult = aGrid.ChangeAt(aUIndex * aNbV + aVIndex);
          if (isCacheUsed)
          {
            theCacheEvaluator(*aCache,
                              aPreparedU[aUIndex].Local,
                              aPreparedV[aVIndex].Local,
                              aResult);
          }
          else
          {
            theDirectEvaluator(theData,
                               aPreparedU[aUIndex].Adjusted,
                               aPreparedV[aVIndex].Adjusted,
                               aUBlock.Span,
                               aVBlock.Span,
                               aResult);
          }
        }
      }
    }
  }
  return aGrid;
}

//! Template helper for direct-only grid evaluation (D3, DN).
//! @tparam ResultT result type
//! @tparam EvalF functor: (data, adjU, adjV, uSpan, vSpan, result) -> void
template <typename ResultT, typename EvalF>
NCollection_Array2<ResultT> evaluateGridDirect(const SurfaceData&                theData,
                                               const NCollection_Array1<double>& theUParams,
                                               const NCollection_Array1<double>& theVParams,
                                               EvalF                             theEval)
{
  const size_t aNbU = theUParams.Size();
  const size_t aNbV = theVParams.Size();

  NCollection_Array2<ResultT> aGrid(1, static_cast<int>(aNbU), 1, static_cast<int>(aNbV));

  int    aPrevUSpan  = -1;
  double aUSpanStart = 0.0;
  double aUSpanEnd   = 0.0;

  for (size_t aUIndex = 0; aUIndex < aNbU; ++aUIndex)
  {
    const double aU     = theUParams.At(aUIndex);
    double       aAdjU  = aU;
    int          aUSpan = 0;

    if (!theData.IsUPeriodic && aPrevUSpan >= 0 && aU >= aUSpanStart && aU < aUSpanEnd)
    {
      aUSpan = aPrevUSpan;
    }
    else
    {
      locateSpan(aU, *theData.UFlatKnots, theData.UDegree, theData.IsUPeriodic, aAdjU, aUSpan);
    }
    if (aUSpan != aPrevUSpan)
    {
      aPrevUSpan  = aUSpan;
      aUSpanStart = theData.UFlatKnots->Value(aUSpan);
      aUSpanEnd   = theData.UFlatKnots->Value(aUSpan + 1);
    }

    int    aPrevVSpan  = -1;
    double aVSpanStart = 0.0;
    double aVSpanEnd   = 0.0;

    for (size_t aVIndex = 0; aVIndex < aNbV; ++aVIndex)
    {
      const double aV     = theVParams.At(aVIndex);
      double       aAdjV  = aV;
      int          aVSpan = 0;

      if (!theData.IsVPeriodic && aPrevVSpan >= 0 && aV >= aVSpanStart && aV < aVSpanEnd)
      {
        aVSpan = aPrevVSpan;
      }
      else
      {
        locateSpan(aV, *theData.VFlatKnots, theData.VDegree, theData.IsVPeriodic, aAdjV, aVSpan);
      }
      if (aVSpan != aPrevVSpan)
      {
        aPrevVSpan  = aVSpan;
        aVSpanStart = theData.VFlatKnots->Value(aVSpan);
        aVSpanEnd   = theData.VFlatKnots->Value(aVSpan + 1);
      }

      theEval(theData, aAdjU, aAdjV, aUSpan, aVSpan, aGrid.ChangeAt(aUIndex * aNbV + aVIndex));
    }
  }

  return aGrid;
}

} // namespace

//=================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BSplineSurface::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  SurfaceData aData;
  if (theUParams.IsEmpty() || theVParams.IsEmpty() || !extractSurfaceData(myGeom, aData))
  {
    return NCollection_Array2<gp_Pnt>();
  }

  return evaluateGridCached<gp_Pnt>(
    aData,
    theUParams,
    theVParams,
    aData.Weights != nullptr ? THE_RATIONAL_D0_D1_CACHE_MIN_POINTS
                             : THE_POLYNOMIAL_CACHE_MIN_POINTS,
    [](const BSplSLib_Cache& theCache, double theLocalU, double theLocalV, gp_Pnt& theResult) {
      theCache.D0Local(theLocalU, theLocalV, theResult);
    },
    [](const SurfaceData& theData,
       double             theU,
       double             theV,
       int                theUSpan,
       int                theVSpan,
       gp_Pnt&            theResult) {
      BSplSLib::D0(theU,
                   theV,
                   theUSpan,
                   theVSpan,
                   *theData.Poles,
                   theData.Weights,
                   *theData.UFlatKnots,
                   *theData.VFlatKnots,
                   BSplCLib::NoMults(),
                   BSplCLib::NoMults(),
                   theData.UDegree,
                   theData.VDegree,
                   theData.IsURational,
                   theData.IsVRational,
                   theData.IsUPeriodic,
                   theData.IsVPeriodic,
                   theResult);
    });
}

//=================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BSplineSurface::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  SurfaceData aData;
  if (theUParams.IsEmpty() || theVParams.IsEmpty() || !extractSurfaceData(myGeom, aData))
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }
  return evaluateGridCached<GeomGridEval::SurfD1>(
    aData,
    theUParams,
    theVParams,
    aData.Weights != nullptr ? THE_RATIONAL_D0_D1_CACHE_MIN_POINTS
                             : THE_POLYNOMIAL_CACHE_MIN_POINTS,
    [](const BSplSLib_Cache& theCache,
       double                theLocalU,
       double                theLocalV,
       GeomGridEval::SurfD1& theResult) {
      theCache.D1Local(theLocalU, theLocalV, theResult.Point, theResult.D1U, theResult.D1V);
    },
    [](const SurfaceData&    theData,
       double                theU,
       double                theV,
       int                   theUSpan,
       int                   theVSpan,
       GeomGridEval::SurfD1& theResult) {
      BSplSLib::D1(theU,
                   theV,
                   theUSpan,
                   theVSpan,
                   *theData.Poles,
                   theData.Weights,
                   *theData.UFlatKnots,
                   *theData.VFlatKnots,
                   BSplCLib::NoMults(),
                   BSplCLib::NoMults(),
                   theData.UDegree,
                   theData.VDegree,
                   theData.IsURational,
                   theData.IsVRational,
                   theData.IsUPeriodic,
                   theData.IsVPeriodic,
                   theResult.Point,
                   theResult.D1U,
                   theResult.D1V);
    });
}

//=================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BSplineSurface::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  SurfaceData aData;
  if (theUParams.IsEmpty() || theVParams.IsEmpty() || !extractSurfaceData(myGeom, aData))
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }
  return evaluateGridCached<GeomGridEval::SurfD2>(
    aData,
    theUParams,
    theVParams,
    aData.Weights != nullptr ? THE_RATIONAL_D2_CACHE_MIN_POINTS : THE_POLYNOMIAL_CACHE_MIN_POINTS,
    [](const BSplSLib_Cache& theCache,
       double                theLocalU,
       double                theLocalV,
       GeomGridEval::SurfD2& theResult) {
      theCache.D2Local(theLocalU,
                       theLocalV,
                       theResult.Point,
                       theResult.D1U,
                       theResult.D1V,
                       theResult.D2U,
                       theResult.D2V,
                       theResult.D2UV);
    },
    [](const SurfaceData&    theData,
       double                theU,
       double                theV,
       int                   theUSpan,
       int                   theVSpan,
       GeomGridEval::SurfD2& theResult) {
      BSplSLib::D2(theU,
                   theV,
                   theUSpan,
                   theVSpan,
                   *theData.Poles,
                   theData.Weights,
                   *theData.UFlatKnots,
                   *theData.VFlatKnots,
                   BSplCLib::NoMults(),
                   BSplCLib::NoMults(),
                   theData.UDegree,
                   theData.VDegree,
                   theData.IsURational,
                   theData.IsVRational,
                   theData.IsUPeriodic,
                   theData.IsVPeriodic,
                   theResult.Point,
                   theResult.D1U,
                   theResult.D1V,
                   theResult.D2U,
                   theResult.D2V,
                   theResult.D2UV);
    });
}

//=================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_BSplineSurface::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  SurfaceData aData;
  if (theUParams.IsEmpty() || theVParams.IsEmpty() || !extractSurfaceData(myGeom, aData))
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  return evaluateGridDirect<GeomGridEval::SurfD3>(aData,
                                                  theUParams,
                                                  theVParams,
                                                  [](const SurfaceData&    theData,
                                                     double                theU,
                                                     double                theV,
                                                     int                   theUSpan,
                                                     int                   theVSpan,
                                                     GeomGridEval::SurfD3& theResult) {
                                                    BSplSLib::D3(theU,
                                                                 theV,
                                                                 theUSpan,
                                                                 theVSpan,
                                                                 *theData.Poles,
                                                                 theData.Weights,
                                                                 *theData.UFlatKnots,
                                                                 *theData.VFlatKnots,
                                                                 BSplCLib::NoMults(),
                                                                 BSplCLib::NoMults(),
                                                                 theData.UDegree,
                                                                 theData.VDegree,
                                                                 theData.IsURational,
                                                                 theData.IsVRational,
                                                                 theData.IsUPeriodic,
                                                                 theData.IsVPeriodic,
                                                                 theResult.Point,
                                                                 theResult.D1U,
                                                                 theResult.D1V,
                                                                 theResult.D2U,
                                                                 theResult.D2V,
                                                                 theResult.D2UV,
                                                                 theResult.D3U,
                                                                 theResult.D3V,
                                                                 theResult.D3UUV,
                                                                 theResult.D3UVV);
                                                  });
}

//=================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_BSplineSurface::EvaluateGridDN(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  int                               theNU,
  int                               theNV) const
{
  SurfaceData aData;
  if (theUParams.IsEmpty() || theVParams.IsEmpty() || theNU < 0 || theNV < 0 || (theNU + theNV) < 1
      || !extractSurfaceData(myGeom, aData))
  {
    return NCollection_Array2<gp_Vec>();
  }

  const size_t aNbU = theUParams.Size();
  const size_t aNbV = theVParams.Size();

  // Derivatives beyond degree are zero
  if (theNU > aData.UDegree || theNV > aData.VDegree)
  {
    NCollection_Array2<gp_Vec> aGrid(1, static_cast<int>(aNbU), 1, static_cast<int>(aNbV));
    aGrid.Init(gp_Vec(0.0, 0.0, 0.0));
    return aGrid;
  }

  return evaluateGridDirect<gp_Vec>(aData,
                                    theUParams,
                                    theVParams,
                                    [theNU, theNV](const SurfaceData& theData,
                                                   double             theU,
                                                   double             theV,
                                                   int                theUSpan,
                                                   int                theVSpan,
                                                   gp_Vec&            theResult) {
                                      BSplSLib::DN(theU,
                                                   theV,
                                                   theNU,
                                                   theNV,
                                                   theUSpan,
                                                   theVSpan,
                                                   *theData.Poles,
                                                   theData.Weights,
                                                   *theData.UFlatKnots,
                                                   *theData.VFlatKnots,
                                                   BSplCLib::NoMults(),
                                                   BSplCLib::NoMults(),
                                                   theData.UDegree,
                                                   theData.VDegree,
                                                   theData.IsURational,
                                                   theData.IsVRational,
                                                   theData.IsUPeriodic,
                                                   theData.IsVPeriodic,
                                                   theResult);
                                    });
}
