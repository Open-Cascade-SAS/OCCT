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

namespace
{

//! Threshold for using cache vs direct evaluation.
//! For small spans (few points), direct BSplSLib evaluation is faster than building cache.
constexpr int THE_CACHE_THRESHOLD = 4;

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
    return false;

  theData.UFlatKnots  = &theGeom->InternalUFlatKnots();
  theData.VFlatKnots  = &theGeom->InternalVFlatKnots();
  theData.Poles       = &theGeom->InternalPoles();
  theData.Weights     = theGeom->InternalWeights();
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

//! Count consecutive parameters in the same span (for cache threshold decision).
inline int countSpanSize(const NCollection_Array1<double>& theParams,
                         const NCollection_Array1<double>& theFlatKnots,
                         int                               theDegree,
                         bool                              theIsPeriodic,
                         int                               theStartIdx,
                         int                               theTargetSpan)
{
  const int aLower = theParams.Lower();
  const int aNb    = theParams.Size();
  int       aCount = 1;

  for (int i = theStartIdx + 1; i < aNb; ++i)
  {
    double aParam    = theParams.Value(aLower + i);
    double aAdjusted = aParam;
    int    aSpan     = 0;
    BSplCLib::LocateParameter(theDegree,
                              theFlatKnots,
                              BSplCLib::NoMults(),
                              aParam,
                              theIsPeriodic,
                              aSpan,
                              aAdjusted);
    if (aSpan == theTargetSpan)
      ++aCount;
    else
      break;
  }
  return aCount;
}

//! Compute span local parameter coefficients (mid and half-length).
inline void computeSpanCoeffs(const NCollection_Array1<double>& theFlatKnots,
                              int                               theSpan,
                              double&                           theMid,
                              double&                           theHalfLen)
{
  const double aStart = theFlatKnots.Value(theSpan);
  theHalfLen          = 0.5 * (theFlatKnots.Value(theSpan + 1) - aStart);
  theMid              = aStart + theHalfLen;
}

//! Template helper for cached grid evaluation (D0, D1, D2).
//! @tparam ResultT result type (gp_Pnt, SurfD1, SurfD2)
//! @tparam CacheEvalF functor: (cache, localU, localV, result) -> void
//! @tparam DirectEvalF functor: (data, adjU, adjV, uSpan, vSpan, result) -> void
template <typename ResultT, typename CacheEvalF, typename DirectEvalF>
NCollection_Array2<ResultT> evaluateGridCached(const SurfaceData&                theData,
                                               const NCollection_Array1<double>& theUParams,
                                               const NCollection_Array1<double>& theVParams,
                                               CacheEvalF                        theCacheEval,
                                               DirectEvalF                       theDirectEval)
{
  const int aNbU  = theUParams.Size();
  const int aNbV  = theVParams.Size();
  const int aLowU = theUParams.Lower();
  const int aLowV = theVParams.Lower();

  NCollection_Array2<ResultT> aGrid(1, aNbU, 1, aNbV);
  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(theData.UDegree,
                                                          theData.IsUPeriodic,
                                                          *theData.UFlatKnots,
                                                          theData.VDegree,
                                                          theData.IsVPeriodic,
                                                          *theData.VFlatKnots,
                                                          theData.Weights);

  int    aPrevUSpan    = -1;
  int    aUSpanSize    = 0;
  double aUSpanMid     = 0.0;
  double aUSpanHalfLen = 1.0;

  for (int ui = 0; ui < aNbU; ++ui)
  {
    const double aU     = theUParams.Value(aLowU + ui);
    double       aAdjU  = aU;
    int          aUSpan = 0;
    locateSpan(aU, *theData.UFlatKnots, theData.UDegree, theData.IsUPeriodic, aAdjU, aUSpan);

    if (aUSpan != aPrevUSpan)
    {
      aPrevUSpan = aUSpan;
      aUSpanSize = countSpanSize(theUParams,
                                 *theData.UFlatKnots,
                                 theData.UDegree,
                                 theData.IsUPeriodic,
                                 ui,
                                 aUSpan);
      computeSpanCoeffs(*theData.UFlatKnots, aUSpan, aUSpanMid, aUSpanHalfLen);
    }

    int    aPrevVSpan    = -1;
    int    aVSpanSize    = 0;
    double aVSpanMid     = 0.0;
    double aVSpanHalfLen = 1.0;
    bool   aUseCache     = false;

    for (int vi = 0; vi < aNbV; ++vi)
    {
      const double aV     = theVParams.Value(aLowV + vi);
      double       aAdjV  = aV;
      int          aVSpan = 0;
      locateSpan(aV, *theData.VFlatKnots, theData.VDegree, theData.IsVPeriodic, aAdjV, aVSpan);

      if (aVSpan != aPrevVSpan)
      {
        aPrevVSpan = aVSpan;
        aVSpanSize = countSpanSize(theVParams,
                                   *theData.VFlatKnots,
                                   theData.VDegree,
                                   theData.IsVPeriodic,
                                   vi,
                                   aVSpan);
        computeSpanCoeffs(*theData.VFlatKnots, aVSpan, aVSpanMid, aVSpanHalfLen);

        aUseCache = (aUSpanSize * aVSpanSize >= THE_CACHE_THRESHOLD);
        if (aUseCache)
        {
          aCache->BuildCache(aAdjU,
                             aAdjV,
                             *theData.UFlatKnots,
                             *theData.VFlatKnots,
                             *theData.Poles,
                             theData.Weights);
        }
      }

      ResultT aResult;
      if (aUseCache)
      {
        const double aLocalU = (aAdjU - aUSpanMid) / aUSpanHalfLen;
        const double aLocalV = (aAdjV - aVSpanMid) / aVSpanHalfLen;
        theCacheEval(aCache, aLocalU, aLocalV, aResult);
      }
      else
      {
        theDirectEval(theData, aAdjU, aAdjV, aUSpan, aVSpan, aResult);
      }
      aGrid.SetValue(ui + 1, vi + 1, aResult);
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
  const int aNbU  = theUParams.Size();
  const int aNbV  = theVParams.Size();
  const int aLowU = theUParams.Lower();
  const int aLowV = theVParams.Lower();

  NCollection_Array2<ResultT> aGrid(1, aNbU, 1, aNbV);

  for (int ui = 0; ui < aNbU; ++ui)
  {
    const double aU     = theUParams.Value(aLowU + ui);
    double       aAdjU  = aU;
    int          aUSpan = 0;
    locateSpan(aU, *theData.UFlatKnots, theData.UDegree, theData.IsUPeriodic, aAdjU, aUSpan);

    for (int vi = 0; vi < aNbV; ++vi)
    {
      const double aV     = theVParams.Value(aLowV + vi);
      double       aAdjV  = aV;
      int          aVSpan = 0;
      locateSpan(aV, *theData.VFlatKnots, theData.VDegree, theData.IsVPeriodic, aAdjV, aVSpan);

      ResultT aResult;
      theEval(theData, aAdjU, aAdjV, aUSpan, aVSpan, aResult);
      aGrid.SetValue(ui + 1, vi + 1, aResult);
    }
  }

  return aGrid;
}

} // namespace

//==================================================================================================

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
    // Cache evaluation
    [](const occ::handle<BSplSLib_Cache>& theCache,
       double                             theLocalU,
       double                             theLocalV,
       gp_Pnt& theResult) { theCache->D0Local(theLocalU, theLocalV, theResult); },
    // Direct evaluation
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
                   nullptr,
                   nullptr,
                   theData.UDegree,
                   theData.VDegree,
                   theData.IsURational,
                   theData.IsVRational,
                   theData.IsUPeriodic,
                   theData.IsVPeriodic,
                   theResult);
    });
}

//==================================================================================================

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
    // Cache evaluation
    [](const occ::handle<BSplSLib_Cache>& theCache,
       double                             theLocalU,
       double                             theLocalV,
       GeomGridEval::SurfD1&              theResult) {
      theCache->D1Local(theLocalU, theLocalV, theResult.Point, theResult.D1U, theResult.D1V);
    },
    // Direct evaluation
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
                   nullptr,
                   nullptr,
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

//==================================================================================================

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
    // Cache evaluation
    [](const occ::handle<BSplSLib_Cache>& theCache,
       double                             theLocalU,
       double                             theLocalV,
       GeomGridEval::SurfD2&              theResult) {
      theCache->D2Local(theLocalU,
                        theLocalV,
                        theResult.Point,
                        theResult.D1U,
                        theResult.D1V,
                        theResult.D2U,
                        theResult.D2V,
                        theResult.D2UV);
    },
    // Direct evaluation
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
                   nullptr,
                   nullptr,
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

//==================================================================================================

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
                                                    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
                                                    BSplSLib::D3(theU,
                                                                 theV,
                                                                 theUSpan,
                                                                 theVSpan,
                                                                 *theData.Poles,
                                                                 theData.Weights,
                                                                 *theData.UFlatKnots,
                                                                 *theData.VFlatKnots,
                                                                 nullptr,
                                                                 nullptr,
                                                                 theData.UDegree,
                                                                 theData.VDegree,
                                                                 theData.IsURational,
                                                                 theData.IsVRational,
                                                                 theData.IsUPeriodic,
                                                                 theData.IsVPeriodic,
                                                                 theResult.Point,
                                                                 aD1U,
                                                                 aD1V,
                                                                 aD2U,
                                                                 aD2V,
                                                                 aD2UV,
                                                                 theResult.D3U,
                                                                 theResult.D3V,
                                                                 theResult.D3UUV,
                                                                 theResult.D3UVV);
                                                    theResult.D1U  = aD1U;
                                                    theResult.D1V  = aD1V;
                                                    theResult.D2U  = aD2U;
                                                    theResult.D2V  = aD2V;
                                                    theResult.D2UV = aD2UV;
                                                  });
}

//==================================================================================================

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

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Derivatives beyond degree are zero
  if (theNU > aData.UDegree || theNV > aData.VDegree)
  {
    NCollection_Array2<gp_Vec> aGrid(1, aNbU, 1, aNbV);
    const gp_Vec               aZero(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        aGrid.SetValue(i, j, aZero);
      }
    }
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
                                                   nullptr,
                                                   nullptr,
                                                   theData.UDegree,
                                                   theData.VDegree,
                                                   theData.IsURational,
                                                   theData.IsVRational,
                                                   theData.IsUPeriodic,
                                                   theData.IsVPeriodic,
                                                   theResult);
                                    });
}
