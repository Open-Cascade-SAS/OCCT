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
#include <NCollection_Array1.hxx>

namespace
{

//! Threshold for using cache vs direct evaluation.
constexpr int THE_CACHE_THRESHOLD = 4;

//! Helper structure holding extracted B-spline curve data.
struct CurveData
{
  const NCollection_Array1<double>* FlatKnots;
  const NCollection_Array1<gp_Pnt>* Poles;
  const NCollection_Array1<double>* Weights;
  const NCollection_Array1<double>* Knots;
  const NCollection_Array1<int>*    Mults;
  int                               Degree;
  bool                              IsPeriodic;
};

//! Extract curve data from geometry. Returns false if data is invalid.
bool extractCurveData(const occ::handle<Geom_BSplineCurve>& theGeom, CurveData& theData)
{
  if (theGeom.IsNull())
    return false;

  const occ::handle<NCollection_HArray1<double>>& aFlatKnots = theGeom->HArrayFlatKnots();
  const occ::handle<NCollection_HArray1<gp_Pnt>>& aPoles     = theGeom->HArrayPoles();

  if (aFlatKnots.IsNull() || aPoles.IsNull())
    return false;

  const bool                                      isRational = theGeom->IsRational();
  const occ::handle<NCollection_HArray1<double>>& aWeights   = theGeom->HArrayWeights();
  if (isRational && aWeights.IsNull())
    return false;

  theData.FlatKnots  = &aFlatKnots->Array1();
  theData.Poles      = &aPoles->Array1();
  theData.Weights    = isRational ? &aWeights->Array1() : nullptr;
  theData.Knots      = &theGeom->Knots();
  theData.Mults      = &theGeom->Multiplicities();
  theData.Degree     = theGeom->Degree();
  theData.IsPeriodic = theGeom->IsPeriodic();
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

//! Compute span local parameter coefficients (start and length).
//! Note: Curve cache uses [0,1] local parameter range (unlike surface which uses [-1,1]).
inline void computeSpanCoeffs(const NCollection_Array1<double>& theFlatKnots,
                              int                               theSpan,
                              double&                           theStart,
                              double&                           theLen)
{
  theStart = theFlatKnots.Value(theSpan);
  theLen   = theFlatKnots.Value(theSpan + 1) - theStart;
}

//! Template helper for cached grid evaluation.
//! @tparam ResultT result type (gp_Pnt, CurveD1, CurveD2, CurveD3)
//! @tparam CacheEvalF functor: (cache, localParam, result) -> void
//! @tparam DirectEvalF functor: (data, param, spanIdx, result) -> void
template <typename ResultT, typename CacheEvalF, typename DirectEvalF>
NCollection_Array1<ResultT> evaluateGridCached(const CurveData&                  theData,
                                               const NCollection_Array1<double>& theParams,
                                               CacheEvalF                        theCacheEval,
                                               DirectEvalF                       theDirectEval)
{
  const int aNbParams = theParams.Size();
  const int aLow      = theParams.Lower();

  NCollection_Array1<ResultT> aResults(1, aNbParams);
  occ::handle<BSplCLib_Cache> aCache = new BSplCLib_Cache(theData.Degree,
                                                          theData.IsPeriodic,
                                                          *theData.FlatKnots,
                                                          *theData.Poles,
                                                          theData.Weights);

  int    aPrevSpan  = -1;
  int    aSpanSize  = 0;
  double aSpanStart = 0.0;
  double aSpanLen   = 1.0;
  bool   aUseCache  = false;

  for (int i = 0; i < aNbParams; ++i)
  {
    const double aParam    = theParams.Value(aLow + i);
    double       aAdjParam = aParam;
    int          aSpan     = 0;
    locateSpan(aParam, *theData.FlatKnots, theData.Degree, theData.IsPeriodic, aAdjParam, aSpan);

    // Update span info when span changes
    if (aSpan != aPrevSpan)
    {
      aPrevSpan = aSpan;
      aSpanSize =
        countSpanSize(theParams, *theData.FlatKnots, theData.Degree, theData.IsPeriodic, i, aSpan);
      computeSpanCoeffs(*theData.FlatKnots, aSpan, aSpanStart, aSpanLen);

      aUseCache = (aSpanSize >= THE_CACHE_THRESHOLD);
      if (aUseCache)
      {
        aCache->BuildCache(aAdjParam, *theData.FlatKnots, *theData.Poles, theData.Weights);
      }
    }

    ResultT& aResult = aResults.ChangeValue(i + 1);
    if (aUseCache)
    {
      // Curve cache uses [0,1] local parameter range
      const double aLocalParam = (aSpanLen > 0.0) ? (aAdjParam - aSpanStart) / aSpanLen : 0.0;
      theCacheEval(aCache, aLocalParam, aResult);
    }
    else
    {
      theDirectEval(theData, aAdjParam, aSpan, aResult);
    }
  }

  return aResults;
}

} // namespace

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BSplineCurve::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  CurveData aData;
  if (theParams.IsEmpty() || !extractCurveData(myGeom, aData))
  {
    return NCollection_Array1<gp_Pnt>();
  }

  return evaluateGridCached<gp_Pnt>(
    aData,
    theParams,
    [](const occ::handle<BSplCLib_Cache>& theCache, double theLocalParam, gp_Pnt& theResult) {
      theCache->D0Local(theLocalParam, theResult);
    },
    [](const CurveData& theData, double theParam, int theSpanIdx, gp_Pnt& theResult) {
      BSplCLib::D0(theParam,
                   theSpanIdx,
                   theData.Degree,
                   theData.IsPeriodic,
                   *theData.Poles,
                   theData.Weights,
                   *theData.FlatKnots,
                   nullptr,
                   theResult);
    });
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BSplineCurve::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  CurveData aData;
  if (theParams.IsEmpty() || !extractCurveData(myGeom, aData))
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  return evaluateGridCached<GeomGridEval::CurveD1>(
    aData,
    theParams,
    [](const occ::handle<BSplCLib_Cache>& theCache,
       double                             theLocalParam,
       GeomGridEval::CurveD1&             theResult) {
      theCache->D1Local(theLocalParam, theResult.Point, theResult.D1);
    },
    [](const CurveData&       theData,
       double                 theParam,
       int                    theSpanIdx,
       GeomGridEval::CurveD1& theResult) {
      BSplCLib::D1(theParam,
                   theSpanIdx,
                   theData.Degree,
                   theData.IsPeriodic,
                   *theData.Poles,
                   theData.Weights,
                   *theData.FlatKnots,
                   nullptr,
                   theResult.Point,
                   theResult.D1);
    });
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BSplineCurve::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  CurveData aData;
  if (theParams.IsEmpty() || !extractCurveData(myGeom, aData))
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  return evaluateGridCached<GeomGridEval::CurveD2>(
    aData,
    theParams,
    [](const occ::handle<BSplCLib_Cache>& theCache,
       double                             theLocalParam,
       GeomGridEval::CurveD2&             theResult) {
      theCache->D2Local(theLocalParam, theResult.Point, theResult.D1, theResult.D2);
    },
    [](const CurveData&       theData,
       double                 theParam,
       int                    theSpanIdx,
       GeomGridEval::CurveD2& theResult) {
      BSplCLib::D2(theParam,
                   theSpanIdx,
                   theData.Degree,
                   theData.IsPeriodic,
                   *theData.Poles,
                   theData.Weights,
                   *theData.FlatKnots,
                   nullptr,
                   theResult.Point,
                   theResult.D1,
                   theResult.D2);
    });
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BSplineCurve::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  CurveData aData;
  if (theParams.IsEmpty() || !extractCurveData(myGeom, aData))
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  return evaluateGridCached<GeomGridEval::CurveD3>(
    aData,
    theParams,
    [](const occ::handle<BSplCLib_Cache>& theCache,
       double                             theLocalParam,
       GeomGridEval::CurveD3&             theResult) {
      theCache->D3Local(theLocalParam, theResult.Point, theResult.D1, theResult.D2, theResult.D3);
    },
    [](const CurveData&       theData,
       double                 theParam,
       int                    theSpanIdx,
       GeomGridEval::CurveD3& theResult) {
      BSplCLib::D3(theParam,
                   theSpanIdx,
                   theData.Degree,
                   theData.IsPeriodic,
                   *theData.Poles,
                   theData.Weights,
                   *theData.FlatKnots,
                   nullptr,
                   theResult.Point,
                   theResult.D1,
                   theResult.D2,
                   theResult.D3);
    });
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BSplineCurve::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  CurveData aData;
  if (theParams.IsEmpty() || theN < 1 || !extractCurveData(myGeom, aData))
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int aNbParams = theParams.Size();
  const int aLow      = theParams.Lower();

  // Derivatives beyond degree are zero
  if (theN > aData.Degree)
  {
    NCollection_Array1<gp_Vec> aResult(1, aNbParams);
    const gp_Vec               aZero(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aZero);
    }
    return aResult;
  }

  NCollection_Array1<gp_Vec> aResult(1, aNbParams);

  // DN uses direct evaluation (no cache available)
  for (int i = 0; i < aNbParams; ++i)
  {
    const double aParam    = theParams.Value(aLow + i);
    double       aAdjParam = aParam;
    int          aSpan     = 0;
    locateSpan(aParam, *aData.FlatKnots, aData.Degree, aData.IsPeriodic, aAdjParam, aSpan);

    gp_Vec aDN;
    BSplCLib::DN(aAdjParam,
                 theN,
                 aSpan,
                 aData.Degree,
                 aData.IsPeriodic,
                 *aData.Poles,
                 aData.Weights,
                 *aData.Knots,
                 aData.Mults,
                 aDN);
    aResult.SetValue(i + 1, aDN);
  }

  return aResult;
}
