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

#include <algorithm>
#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

namespace
{
//! Threshold for using cache vs direct evaluation.
//! For small spans (few points), direct BSplSLib evaluation is faster than building cache.
constexpr int THE_CACHE_THRESHOLD = 4;

//! @brief Iterates over sorted UV points with cache-optimal span grouping.
//!
//! This helper processes UV points that have been sorted by (USpanIdx, VSpanIdx, U).
//! It tracks span changes and decides whether to use cache or direct evaluation
//! based on the number of points in each span group.
//!
//! @tparam BuildCacheFunc Functor type for cache initialization: void(double U, double V)
//! @tparam CacheEvalFunc  Functor type for cache-based evaluation: void(const UVPointWithSpan& pt)
//! @tparam DirectEvalFunc Functor type for direct evaluation: void(const UVPointWithSpan& pt)
//!
//! @param theUVPoints    Sorted UV points with span info
//! @param theBuildCache  Functor to rebuild cache for a span
//! @param theCacheEval   Functor called for cache-based evaluation (large spans)
//! @param theDirectEval  Functor called for direct evaluation (small spans)
template <typename BuildCacheFunc, typename CacheEvalFunc, typename DirectEvalFunc>
void iterateSortedUVPoints(const NCollection_Array1<GeomGridEval::UVPointWithSpan>& theUVPoints,
                           BuildCacheFunc&&                                         theBuildCache,
                           CacheEvalFunc&&                                          theCacheEval,
                           DirectEvalFunc&&                                         theDirectEval)
{
  const int aNbPoints = theUVPoints.Size();
  if (aNbPoints == 0)
  {
    return;
  }

  int i = 0;
  while (i < aNbPoints)
  {
    const GeomGridEval::UVPointWithSpan& aFirstPt = theUVPoints.Value(i);
    const int                            aUSpan   = aFirstPt.USpanIdx;
    const int                            aVSpan   = aFirstPt.VSpanIdx;

    // Count points in this span group
    int aGroupEnd = i + 1;
    while (aGroupEnd < aNbPoints)
    {
      const GeomGridEval::UVPointWithSpan& aPt = theUVPoints.Value(aGroupEnd);
      if (aPt.USpanIdx != aUSpan || aPt.VSpanIdx != aVSpan)
      {
        break;
      }
      ++aGroupEnd;
    }
    const int aGroupSize = aGroupEnd - i;

    if (aGroupSize >= THE_CACHE_THRESHOLD)
    {
      // Large group: use cache-based evaluation
      theBuildCache(aFirstPt.U, aFirstPt.V);
      for (int j = i; j < aGroupEnd; ++j)
      {
        theCacheEval(theUVPoints.Value(j));
      }
    }
    else
    {
      // Small group: use direct evaluation (skip cache building)
      for (int j = i; j < aGroupEnd; ++j)
      {
        theDirectEval(theUVPoints.Value(j));
      }
    }

    i = aGroupEnd;
  }
}

//! Find span index for a parameter value.
//! @param[in,out] theParam parameter value (may be adjusted for periodicity)
//! @param theDegree degree in the parameter direction
//! @param theIsPeriodic true if periodic in this direction
//! @param theFlatKnots flat knots array
//! @return span index in flat knots array
int locateSpan(double&                           theParam,
               int                               theDegree,
               bool                              theIsPeriodic,
               const NCollection_Array1<double>& theFlatKnots)
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
  theParam = aNewParam;
  return aSpanIndex;
}

//! Compute span indices and local parameters, then sort by span.
//! @param theUVPoints array of UV points to process (modified in place)
//! @param theUFlatKnots U direction flat knots
//! @param theVFlatKnots V direction flat knots
//! @param theUDegree U degree
//! @param theVDegree V degree
//! @param theIsUPeriodic true if U periodic
//! @param theIsVPeriodic true if V periodic
void computeSpansAndSort(NCollection_Array1<GeomGridEval::UVPointWithSpan>& theUVPoints,
                         const NCollection_Array1<double>&                  theUFlatKnots,
                         const NCollection_Array1<double>&                  theVFlatKnots,
                         int                                                theUDegree,
                         int                                                theVDegree,
                         bool                                               theIsUPeriodic,
                         bool                                               theIsVPeriodic)
{
  if (theUVPoints.IsEmpty())
  {
    return;
  }

  const int aNbPoints = theUVPoints.Size();

  for (int i = 0; i < aNbPoints; ++i)
  {
    GeomGridEval::UVPointWithSpan& aPt = theUVPoints.ChangeValue(i);

    // Locate U span
    double aUParam   = aPt.U;
    int    aUSpanIdx = locateSpan(aUParam, theUDegree, theIsUPeriodic, theUFlatKnots);
    aPt.U            = aUParam; // May be adjusted for periodicity
    aPt.USpanIdx     = aUSpanIdx;

    const double aUSpanStart   = theUFlatKnots.Value(aUSpanIdx);
    const double aUSpanHalfLen = 0.5 * (theUFlatKnots.Value(aUSpanIdx + 1) - aUSpanStart);
    const double aUSpanMid     = aUSpanStart + aUSpanHalfLen;
    aPt.LocalU                 = (aPt.U - aUSpanMid) / aUSpanHalfLen;

    // Locate V span
    double aVParam   = aPt.V;
    int    aVSpanIdx = locateSpan(aVParam, theVDegree, theIsVPeriodic, theVFlatKnots);
    aPt.V            = aVParam; // May be adjusted for periodicity
    aPt.VSpanIdx     = aVSpanIdx;

    const double aVSpanStart   = theVFlatKnots.Value(aVSpanIdx);
    const double aVSpanHalfLen = 0.5 * (theVFlatKnots.Value(aVSpanIdx + 1) - aVSpanStart);
    const double aVSpanMid     = aVSpanStart + aVSpanHalfLen;
    aPt.LocalV                 = (aPt.V - aVSpanMid) / aVSpanHalfLen;
  }

  // Sort by (USpanIdx, VSpanIdx, U) for cache-optimal iteration
  std::sort(theUVPoints.begin(),
            theUVPoints.end(),
            [](const GeomGridEval::UVPointWithSpan& a, const GeomGridEval::UVPointWithSpan& b) {
              if (a.USpanIdx != b.USpanIdx)
                return a.USpanIdx < b.USpanIdx;
              if (a.VSpanIdx != b.VSpanIdx)
                return a.VSpanIdx < b.VSpanIdx;
              return a.U < b.U;
            });
}

//! Prepare UV points from grid parameters and sort for cache-optimal evaluation.
//! @param theUParams array of U parameter values
//! @param theVParams array of V parameter values
//! @param theUFlatKnots U direction flat knots
//! @param theVFlatKnots V direction flat knots
//! @param theUDegree U degree
//! @param theVDegree V degree
//! @param theIsUPeriodic true if U periodic
//! @param theIsVPeriodic true if V periodic
//! @return array of UV points with span info, sorted for cache-optimal evaluation
[[nodiscard]] NCollection_Array1<GeomGridEval::UVPointWithSpan> prepareGridPoints(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  const NCollection_Array1<double>& theUFlatKnots,
  const NCollection_Array1<double>& theVFlatKnots,
  int                               theUDegree,
  int                               theVDegree,
  bool                              theIsUPeriodic,
  bool                              theIsVPeriodic)
{
  const int aNbU      = theUParams.Size();
  const int aNbV      = theVParams.Size();
  const int aNbPoints = aNbU * aNbV;
  const int aULower   = theUParams.Lower();
  const int aVLower   = theVParams.Lower();

  NCollection_Array1<GeomGridEval::UVPointWithSpan> aUVPoints(0, aNbPoints - 1);

  int aIdx = 0;
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(aULower + i);
    for (int j = 0; j < aNbV; ++j)
    {
      const double aV      = theVParams.Value(aVLower + j);
      const int    aOutIdx = i * aNbV + j; // Row-major linear index
      aUVPoints.SetValue(aIdx++, GeomGridEval::UVPointWithSpan{aU, aV, 0.0, 0.0, 0, 0, aOutIdx});
    }
  }

  computeSpansAndSort(aUVPoints,
                      theUFlatKnots,
                      theVFlatKnots,
                      theUDegree,
                      theVDegree,
                      theIsUPeriodic,
                      theIsVPeriodic);
  return aUVPoints;
}

} // namespace

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BSplineSurface::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Get surface data from geometry
  const occ::handle<NCollection_HArray1<double>>& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const occ::handle<NCollection_HArray1<double>>& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  const occ::handle<NCollection_HArray2<gp_Pnt>>& aPolesHandle      = myGeom->HArrayPoles();

  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull() || aPolesHandle.IsNull())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const NCollection_Array1<double>& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const NCollection_Array1<double>& aVFlatKnots = aVFlatKnotsHandle->Array1();
  const NCollection_Array2<gp_Pnt>& aPoles      = aPolesHandle->Array2();
  const NCollection_Array2<double>* aWeights    = myGeom->Weights();

  // Get surface properties
  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Prepare UV points and sort by span
  NCollection_Array1<GeomGridEval::UVPointWithSpan> aUVPoints = prepareGridPoints(theUParams,
                                                                                  theVParams,
                                                                                  aUFlatKnots,
                                                                                  aVFlatKnots,
                                                                                  aUDegree,
                                                                                  aVDegree,
                                                                                  isUPeriodic,
                                                                                  isVPeriodic);

  if (aUVPoints.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Allocate linear result buffer
  const int                  aNbPoints = aNbU * aNbV;
  NCollection_Array1<gp_Pnt> aLinearResult(1, aNbPoints);

  // Create local cache for cache-based evaluation (only used for large span groups)
  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(aUDegree,
                                                          isUPeriodic,
                                                          aUFlatKnots,
                                                          aVDegree,
                                                          isVPeriodic,
                                                          aVFlatKnots,
                                                          aWeights);

  // Evaluate using sorted UV points
  iterateSortedUVPoints(
    aUVPoints,
    [&](double theU, double theV) {
      aCache->BuildCache(theU, theV, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
    },
    [&](const GeomGridEval::UVPointWithSpan& thePt) {
      gp_Pnt aPnt;
      aCache->D0Local(thePt.LocalU, thePt.LocalV, aPnt);
      aLinearResult.SetValue(thePt.OutputIdx + 1, aPnt);
    },
    [&](const GeomGridEval::UVPointWithSpan& thePt) {
      gp_Pnt aPnt;
      BSplSLib::D0(thePt.U,
                   thePt.V,
                   thePt.USpanIdx,
                   thePt.VSpanIdx,
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
                   aPnt);
      aLinearResult.SetValue(thePt.OutputIdx + 1, aPnt);
    });

  // Reshape linear buffer to 2D grid
  NCollection_Array2<gp_Pnt> aGrid(1, aNbU, 1, aNbV);
  for (int i = 0; i < aNbU; ++i)
  {
    for (int j = 0; j < aNbV; ++j)
    {
      const int aLinIdx = i * aNbV + j;
      aGrid.SetValue(i + 1, j + 1, aLinearResult.Value(aLinIdx + 1));
    }
  }
  return aGrid;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BSplineSurface::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array1<GeomGridEval::SurfD1> aLinearResult = evaluateGridLinearD1(theUParams, theVParams);
  if (aLinearResult.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  NCollection_Array2<GeomGridEval::SurfD1> aGrid(1, aNbU, 1, aNbV);
  for (int i = 0; i < aNbU; ++i)
  {
    for (int j = 0; j < aNbV; ++j)
    {
      const int aLinIdx = i * aNbV + j;
      aGrid.SetValue(i + 1, j + 1, aLinearResult.Value(aLinIdx + 1));
    }
  }
  return aGrid;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BSplineSurface::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array1<GeomGridEval::SurfD2> aLinearResult = evaluateGridLinearD2(theUParams, theVParams);
  if (aLinearResult.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  NCollection_Array2<GeomGridEval::SurfD2> aGrid(1, aNbU, 1, aNbV);
  for (int i = 0; i < aNbU; ++i)
  {
    for (int j = 0; j < aNbV; ++j)
    {
      const int aLinIdx = i * aNbV + j;
      aGrid.SetValue(i + 1, j + 1, aLinearResult.Value(aLinIdx + 1));
    }
  }
  return aGrid;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_BSplineSurface::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array1<GeomGridEval::SurfD3> aLinearResult = evaluateGridLinearD3(theUParams, theVParams);
  if (aLinearResult.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  NCollection_Array2<GeomGridEval::SurfD3> aGrid(1, aNbU, 1, aNbV);
  for (int i = 0; i < aNbU; ++i)
  {
    for (int j = 0; j < aNbV; ++j)
    {
      const int aLinIdx = i * aNbV + j;
      aGrid.SetValue(i + 1, j + 1, aLinearResult.Value(aLinIdx + 1));
    }
  }
  return aGrid;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_BSplineSurface::EvaluateGridDN(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  int                               theNU,
  int                               theNV) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array1<gp_Vec> aLinearResult = evaluateGridLinearDN(theUParams, theVParams, theNU, theNV);
  if (aLinearResult.IsEmpty())
  {
    return NCollection_Array2<gp_Vec>();
  }

  NCollection_Array2<gp_Vec> aGrid(1, aNbU, 1, aNbV);
  for (int i = 0; i < aNbU; ++i)
  {
    for (int j = 0; j < aNbV; ++j)
    {
      const int aLinIdx = i * aNbV + j;
      aGrid.SetValue(i + 1, j + 1, aLinearResult.Value(aLinIdx + 1));
    }
  }
  return aGrid;
}

//==================================================================================================

// Private helper methods for grid evaluation (linear result arrays)
// These allow EvaluateGridD* to evaluate using cache-optimal iteration

NCollection_Array1<GeomGridEval::SurfD1> GeomGridEval_BSplineSurface::evaluateGridLinearD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD1>();
  }

  // Get surface data from geometry
  const occ::handle<NCollection_HArray1<double>>& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const occ::handle<NCollection_HArray1<double>>& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  const occ::handle<NCollection_HArray2<gp_Pnt>>& aPolesHandle      = myGeom->HArrayPoles();

  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull() || aPolesHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::SurfD1>();
  }

  const NCollection_Array1<double>& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const NCollection_Array1<double>& aVFlatKnots = aVFlatKnotsHandle->Array1();
  const NCollection_Array2<gp_Pnt>& aPoles      = aPolesHandle->Array2();
  const NCollection_Array2<double>* aWeights    = myGeom->Weights();

  // Get surface properties
  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();

  // Prepare UV points and sort by span
  NCollection_Array1<GeomGridEval::UVPointWithSpan> aUVPoints = prepareGridPoints(theUParams,
                                                                                  theVParams,
                                                                                  aUFlatKnots,
                                                                                  aVFlatKnots,
                                                                                  aUDegree,
                                                                                  aVDegree,
                                                                                  isUPeriodic,
                                                                                  isVPeriodic);

  const int aNbPoints = aUVPoints.Size();
  if (aNbPoints == 0)
  {
    return NCollection_Array1<GeomGridEval::SurfD1>();
  }

  NCollection_Array1<GeomGridEval::SurfD1> aResults(1, aNbPoints);

  // Create local cache for cache-based evaluation (only used for large span groups)
  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(aUDegree,
                                                          isUPeriodic,
                                                          aUFlatKnots,
                                                          aVDegree,
                                                          isVPeriodic,
                                                          aVFlatKnots,
                                                          aWeights);

  iterateSortedUVPoints(
    aUVPoints,
    [&](double theU, double theV) {
      aCache->BuildCache(theU, theV, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
    },
    [&](const GeomGridEval::UVPointWithSpan& thePt) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      aCache->D1Local(thePt.LocalU, thePt.LocalV, aPoint, aD1U, aD1V);
      aResults.SetValue(thePt.OutputIdx + 1, GeomGridEval::SurfD1{aPoint, aD1U, aD1V});
    },
    [&](const GeomGridEval::UVPointWithSpan& thePt) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      BSplSLib::D1(thePt.U,
                   thePt.V,
                   thePt.USpanIdx,
                   thePt.VSpanIdx,
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
      aResults.SetValue(thePt.OutputIdx + 1, GeomGridEval::SurfD1{aPoint, aD1U, aD1V});
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD2> GeomGridEval_BSplineSurface::evaluateGridLinearD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD2>();
  }

  // Get surface data from geometry
  const occ::handle<NCollection_HArray1<double>>& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const occ::handle<NCollection_HArray1<double>>& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  const occ::handle<NCollection_HArray2<gp_Pnt>>& aPolesHandle      = myGeom->HArrayPoles();

  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull() || aPolesHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::SurfD2>();
  }

  const NCollection_Array1<double>& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const NCollection_Array1<double>& aVFlatKnots = aVFlatKnotsHandle->Array1();
  const NCollection_Array2<gp_Pnt>& aPoles      = aPolesHandle->Array2();
  const NCollection_Array2<double>* aWeights    = myGeom->Weights();

  // Get surface properties
  const int  aUDegree    = myGeom->UDegree();
  const int  aVDegree    = myGeom->VDegree();
  const bool isUPeriodic = myGeom->IsUPeriodic();
  const bool isVPeriodic = myGeom->IsVPeriodic();
  const bool isURational = myGeom->IsURational();
  const bool isVRational = myGeom->IsVRational();

  // Prepare UV points and sort by span
  NCollection_Array1<GeomGridEval::UVPointWithSpan> aUVPoints = prepareGridPoints(theUParams,
                                                                                  theVParams,
                                                                                  aUFlatKnots,
                                                                                  aVFlatKnots,
                                                                                  aUDegree,
                                                                                  aVDegree,
                                                                                  isUPeriodic,
                                                                                  isVPeriodic);

  const int aNbPoints = aUVPoints.Size();
  if (aNbPoints == 0)
  {
    return NCollection_Array1<GeomGridEval::SurfD2>();
  }

  NCollection_Array1<GeomGridEval::SurfD2> aResults(1, aNbPoints);

  // Create local cache for cache-based evaluation (only used for large span groups)
  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(aUDegree,
                                                          isUPeriodic,
                                                          aUFlatKnots,
                                                          aVDegree,
                                                          isVPeriodic,
                                                          aVFlatKnots,
                                                          aWeights);

  iterateSortedUVPoints(
    aUVPoints,
    [&](double theU, double theV) {
      aCache->BuildCache(theU, theV, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
    },
    [&](const GeomGridEval::UVPointWithSpan& thePt) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aCache->D2Local(thePt.LocalU, thePt.LocalV, aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResults.SetValue(thePt.OutputIdx + 1,
                        GeomGridEval::SurfD2{aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV});
    },
    [&](const GeomGridEval::UVPointWithSpan& thePt) {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      BSplSLib::D2(thePt.U,
                   thePt.V,
                   thePt.USpanIdx,
                   thePt.VSpanIdx,
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
      aResults.SetValue(thePt.OutputIdx + 1,
                        GeomGridEval::SurfD2{aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV});
    });

  return aResults;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD3> GeomGridEval_BSplineSurface::evaluateGridLinearD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD3>();
  }

  // Get surface data from geometry
  const occ::handle<NCollection_HArray1<double>>& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const occ::handle<NCollection_HArray1<double>>& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  const occ::handle<NCollection_HArray2<gp_Pnt>>& aPolesHandle      = myGeom->HArrayPoles();

  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull() || aPolesHandle.IsNull())
  {
    return NCollection_Array1<GeomGridEval::SurfD3>();
  }

  const NCollection_Array1<double>& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const NCollection_Array1<double>& aVFlatKnots = aVFlatKnotsHandle->Array1();
  const NCollection_Array2<gp_Pnt>& aPoles      = aPolesHandle->Array2();
  const NCollection_Array2<double>* aWeights    = myGeom->Weights();
  const int                         aUDegree    = myGeom->UDegree();
  const int                         aVDegree    = myGeom->VDegree();
  const bool                        isURational = myGeom->IsURational();
  const bool                        isVRational = myGeom->IsVRational();
  const bool                        isUPeriodic = myGeom->IsUPeriodic();
  const bool                        isVPeriodic = myGeom->IsVPeriodic();

  // Prepare UV points and sort by span
  NCollection_Array1<GeomGridEval::UVPointWithSpan> aUVPoints = prepareGridPoints(theUParams,
                                                                                  theVParams,
                                                                                  aUFlatKnots,
                                                                                  aVFlatKnots,
                                                                                  aUDegree,
                                                                                  aVDegree,
                                                                                  isUPeriodic,
                                                                                  isVPeriodic);

  const int aNbPoints = aUVPoints.Size();
  if (aNbPoints == 0)
  {
    return NCollection_Array1<GeomGridEval::SurfD3>();
  }

  NCollection_Array1<GeomGridEval::SurfD3> aResults(1, aNbPoints);

  // D3 uses direct BSplSLib::D3 - no cache available for third derivatives
  for (int i = 0; i < aNbPoints; ++i)
  {
    const GeomGridEval::UVPointWithSpan& aPt = aUVPoints.Value(i);

    gp_Pnt aPoint;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;

    BSplSLib::D3(aPt.U,
                 aPt.V,
                 aPt.USpanIdx,
                 aPt.VSpanIdx,
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

    aResults.SetValue(
      aPt.OutputIdx + 1,
      GeomGridEval::SurfD3{aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV});
  }

  return aResults;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BSplineSurface::evaluateGridLinearDN(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  int                               theNU,
  int                               theNV) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  // Get surface data from geometry
  const occ::handle<NCollection_HArray1<double>>& aUFlatKnotsHandle = myGeom->HArrayUFlatKnots();
  const occ::handle<NCollection_HArray1<double>>& aVFlatKnotsHandle = myGeom->HArrayVFlatKnots();
  const occ::handle<NCollection_HArray2<gp_Pnt>>& aPolesHandle      = myGeom->HArrayPoles();

  if (aUFlatKnotsHandle.IsNull() || aVFlatKnotsHandle.IsNull() || aPolesHandle.IsNull())
  {
    return NCollection_Array1<gp_Vec>();
  }

  const NCollection_Array1<double>& aUFlatKnots = aUFlatKnotsHandle->Array1();
  const NCollection_Array1<double>& aVFlatKnots = aVFlatKnotsHandle->Array1();
  const NCollection_Array2<gp_Pnt>& aPoles      = aPolesHandle->Array2();
  const NCollection_Array2<double>* aWeights    = myGeom->Weights();
  const int                         aUDegree    = myGeom->UDegree();
  const int                         aVDegree    = myGeom->VDegree();
  const bool                        isURational = myGeom->IsURational();
  const bool                        isVRational = myGeom->IsVRational();
  const bool                        isUPeriodic = myGeom->IsUPeriodic();
  const bool                        isVPeriodic = myGeom->IsVPeriodic();

  // Prepare UV points and sort by span
  NCollection_Array1<GeomGridEval::UVPointWithSpan> aUVPoints = prepareGridPoints(theUParams,
                                                                                  theVParams,
                                                                                  aUFlatKnots,
                                                                                  aVFlatKnots,
                                                                                  aUDegree,
                                                                                  aVDegree,
                                                                                  isUPeriodic,
                                                                                  isVPeriodic);

  const int aNbPoints = aUVPoints.Size();
  if (aNbPoints == 0)
  {
    return NCollection_Array1<gp_Vec>();
  }

  NCollection_Array1<gp_Vec> aResults(1, aNbPoints);

  // For B-spline surfaces, derivatives become zero when order exceeds degree in that direction
  if (theNU > aUDegree || theNV > aVDegree)
  {
    const gp_Vec aZeroVec(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbPoints; ++i)
    {
      aResults.SetValue(i, aZeroVec);
    }
    return aResults;
  }

  // Use BSplSLib::DN directly with pre-computed span indices
  for (int i = 0; i < aNbPoints; ++i)
  {
    const GeomGridEval::UVPointWithSpan& aPt = aUVPoints.Value(i);

    gp_Vec aDN;
    BSplSLib::DN(aPt.U,
                 aPt.V,
                 theNU,
                 theNV,
                 aPt.USpanIdx,
                 aPt.VSpanIdx,
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

    aResults.SetValue(aPt.OutputIdx + 1, aDN);
  }

  return aResults;
}
