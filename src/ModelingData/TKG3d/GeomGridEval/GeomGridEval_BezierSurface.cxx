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

#include <GeomGridEval_BezierSurface.hxx>

#include <BSplSLib.hxx>
#include <GeomGridEval_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_LocalArray.hxx>

#include <algorithm>
#include <utility>

namespace
{
//! Conservative measured isoline crossovers; base costs are relative point-degree work units.
constexpr size_t THE_ISOLINE_MIN_POINTS           = 8;
constexpr size_t THE_POLYNOMIAL_ISOLINE_BASE_COST = 128;
constexpr size_t THE_RATIONAL_ISOLINE_BASE_COST   = 48;

//! Create and build cache for Bezier surface evaluation.
//! Bezier surfaces are single-span, so cache is built once at parameter (0.5, 0.5).
//! @param theGeom the Bezier surface geometry
//! @return the built cache
occ::handle<BSplSLib_Cache> buildBezierCache(const occ::handle<Geom_BezierSurface>& theGeom)
{
  const NCollection_Array1<double>& aUKnotSequence = theGeom->UKnotSequence();
  const NCollection_Array1<double>& aVKnotSequence = theGeom->VKnotSequence();
  const NCollection_Array2<gp_Pnt>& aPoles         = theGeom->Poles();
  const NCollection_Array2<double>* aWeights       = theGeom->Weights();

  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(theGeom->UDegree(),
                                                          false,
                                                          aUKnotSequence,
                                                          theGeom->VDegree(),
                                                          false,
                                                          aVKnotSequence,
                                                          aWeights);
  aCache->BuildCache(0.5, 0.5, aUKnotSequence, aVKnotSequence, aPoles, aWeights);
  return aCache;
}

void prepareLocalParams(const NCollection_Array1<double>& theParams,
                        NCollection_LocalArray<double>&   theLocalParams)
{
  for (size_t anIndex = 0; anIndex < theParams.Size(); ++anIndex)
  {
    theLocalParams[anIndex] = (theParams.At(anIndex) - 0.5) / 0.5;
  }
}

//! Tests whether isoline extraction has a predictable measured benefit.
bool isIsoPreferred(const occ::handle<Geom_BezierSurface>& theGeom,
                    const size_t                           theNbParams,
                    const int                              theFixedDegree,
                    const int                              theVaryingDegree)
{
  if (!theGeom->IsURational() && !theGeom->IsVRational())
  {
    if (theFixedDegree != theVaryingDegree || theNbParams < THE_ISOLINE_MIN_POINTS)
    {
      return false;
    }

    // Isoline extraction has a fixed cost, while the saved surface work grows with point count and
    // degree. Keeping this rule symmetric avoids direction-dependent surprises for polynomials.
    return theNbParams * static_cast<size_t>(theFixedDegree + 1)
           >= THE_POLYNOMIAL_ISOLINE_BASE_COST;
  }

  if (theNbParams < THE_ISOLINE_MIN_POINTS)
  {
    return false;
  }

  // Rational isoline construction is directional. Compare its degree-dependent fixed cost with
  // the work saved over the evaluated points. The squared degree ratio accounts for the measured
  // construction penalty on anisotropic surfaces.
  const size_t aFixedWeight     = static_cast<size_t>(theFixedDegree + 1);
  const size_t aVaryingWeight   = static_cast<size_t>(theVaryingDegree + 1);
  const size_t aMinWeight       = std::min(aFixedWeight, aVaryingWeight);
  const size_t aMaxWeight       = std::max(aFixedWeight, aVaryingWeight);
  const size_t anEvaluationWork = theNbParams * aFixedWeight * aMinWeight * aMinWeight;
  const size_t anIsolineCost    = THE_RATIONAL_ISOLINE_BASE_COST * aMaxWeight * aMaxWeight;
  return anEvaluationWork >= anIsolineCost;
}

//! Evaluates a Bezier grid through the surface cache, sharing iteration and local-parameter setup
//! between D0, D1, and D2.
template <typename ResultT, typename LocalEvaluator, typename DirectEvaluator>
NCollection_Array2<ResultT> evaluateCacheGrid(const occ::handle<Geom_BezierSurface>& theGeom,
                                              const NCollection_Array1<double>&      theUParams,
                                              const NCollection_Array1<double>&      theVParams,
                                              const bool                             theUseLocal,
                                              LocalEvaluator&&  theLocalEvaluator,
                                              DirectEvaluator&& theDirectEvaluator)
{
  const size_t                      aNbU   = theUParams.Size();
  const size_t                      aNbV   = theVParams.Size();
  const occ::handle<BSplSLib_Cache> aCache = buildBezierCache(theGeom);
  NCollection_Array2<ResultT>       aResult(1, static_cast<int>(aNbU), 1, static_cast<int>(aNbV));
  if (theUseLocal)
  {
    NCollection_LocalArray<double> aLocalU(aNbU);
    NCollection_LocalArray<double> aLocalV(aNbV);
    prepareLocalParams(theUParams, aLocalU);
    prepareLocalParams(theVParams, aLocalV);
    for (size_t aUIndex = 0; aUIndex < aNbU; ++aUIndex)
    {
      for (size_t aVIndex = 0; aVIndex < aNbV; ++aVIndex)
      {
        theLocalEvaluator(*aCache,
                          aLocalU[aUIndex],
                          aLocalV[aVIndex],
                          aResult.ChangeAt(aUIndex * aNbV + aVIndex));
      }
    }
  }
  else
  {
    for (size_t aUIndex = 0; aUIndex < aNbU; ++aUIndex)
    {
      for (size_t aVIndex = 0; aVIndex < aNbV; ++aVIndex)
      {
        theDirectEvaluator(*aCache,
                           theUParams.At(aUIndex),
                           theVParams.At(aVIndex),
                           aResult.ChangeAt(aUIndex * aNbV + aVIndex));
      }
    }
  }
  return aResult;
}
} // namespace

//=================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BezierSurface::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const size_t aNbU = theUParams.Size();
  const size_t aNbV = theVParams.Size();
  if (aNbU == 1 && aNbV == 1)
  {
    NCollection_Array2<gp_Pnt> aResult(1, 1, 1, 1);
    aResult.ChangeAt(0) = myGeom->EvalD0(theUParams.At(0), theVParams.At(0));
    return aResult;
  }

  const bool   isVIso         = aNbV == 1;
  const size_t aNbIsoParams   = isVIso ? aNbU : aNbV;
  const int    aFixedDegree   = isVIso ? myGeom->VDegree() : myGeom->UDegree();
  const int    aVaryingDegree = isVIso ? myGeom->UDegree() : myGeom->VDegree();
  if ((isVIso || aNbU == 1) && isIsoPreferred(myGeom, aNbIsoParams, aFixedDegree, aVaryingDegree))
  {
    try
    {
      OCC_CATCH_SIGNALS
      const occ::handle<Geom_Curve> aCurve =
        isVIso ? myGeom->VIso(theVParams.At(0)) : myGeom->UIso(theUParams.At(0));
      if (!aCurve.IsNull())
      {
        const NCollection_Array1<double>& anIsoParams = isVIso ? theUParams : theVParams;
        GeomGridEval_Curve                aCurveEvaluator(aCurve);
        NCollection_Array1<gp_Pnt>        aCurveResult = aCurveEvaluator.EvaluateGrid(anIsoParams);
        return NCollection_Array2<gp_Pnt>(std::move(aCurveResult),
                                          1,
                                          isVIso ? static_cast<int>(aNbU) : 1,
                                          1,
                                          isVIso ? 1 : static_cast<int>(aNbV));
      }
    }
    catch (const Standard_Failure&)
    {
      // Fall back to surface-cache evaluation if isoline construction fails.
    }
  }

  const bool isLocalEvaluationUsed = myGeom->IsURational() || myGeom->IsVRational()
                                     || (myGeom->UDegree() == 1 && myGeom->VDegree() == 1);
  return evaluateCacheGrid<gp_Pnt>(
    myGeom,
    theUParams,
    theVParams,
    isLocalEvaluationUsed,
    [](const BSplSLib_Cache& theCache, double theU, double theV, gp_Pnt& theResult) {
      theCache.D0Local(theU, theV, theResult);
    },
    [](const BSplSLib_Cache& theCache, double theU, double theV, gp_Pnt& theResult) {
      theCache.D0(theU, theV, theResult);
    });
}

//=================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BezierSurface::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }
  const size_t aNbU = theUParams.Size();
  const size_t aNbV = theVParams.Size();
  if (aNbU == 1 && aNbV == 1)
  {
    NCollection_Array2<GeomGridEval::SurfD1> aResult(1, 1, 1, 1);
    aResult.ChangeAt(0) = myGeom->EvalD1(theUParams.At(0), theVParams.At(0));
    return aResult;
  }

  const bool isLocalEvaluationUsed = myGeom->IsURational() || myGeom->IsVRational();
  return evaluateCacheGrid<GeomGridEval::SurfD1>(
    myGeom,
    theUParams,
    theVParams,
    isLocalEvaluationUsed,
    [](const BSplSLib_Cache& theCache, double theU, double theV, GeomGridEval::SurfD1& theResult) {
      theCache.D1Local(theU, theV, theResult.Point, theResult.D1U, theResult.D1V);
    },
    [](const BSplSLib_Cache& theCache, double theU, double theV, GeomGridEval::SurfD1& theResult) {
      theCache.D1(theU, theV, theResult.Point, theResult.D1U, theResult.D1V);
    });
}

//=================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BezierSurface::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }
  const size_t aNbU = theUParams.Size();
  const size_t aNbV = theVParams.Size();
  if (aNbU == 1 && aNbV == 1)
  {
    NCollection_Array2<GeomGridEval::SurfD2> aResult(1, 1, 1, 1);
    aResult.ChangeAt(0) = myGeom->EvalD2(theUParams.At(0), theVParams.At(0));
    return aResult;
  }

  const bool isLocalEvaluationUsed = myGeom->IsURational() || myGeom->IsVRational()
                                     || (myGeom->UDegree() == 1 && myGeom->VDegree() == 1);
  return evaluateCacheGrid<GeomGridEval::SurfD2>(
    myGeom,
    theUParams,
    theVParams,
    isLocalEvaluationUsed,
    [](const BSplSLib_Cache& theCache, double theU, double theV, GeomGridEval::SurfD2& theResult) {
      theCache.D2Local(theU,
                       theV,
                       theResult.Point,
                       theResult.D1U,
                       theResult.D1V,
                       theResult.D2U,
                       theResult.D2V,
                       theResult.D2UV);
    },
    [](const BSplSLib_Cache& theCache, double theU, double theV, GeomGridEval::SurfD2& theResult) {
      theCache.D2(theU,
                  theV,
                  theResult.Point,
                  theResult.D1U,
                  theResult.D1V,
                  theResult.D2U,
                  theResult.D2V,
                  theResult.D2UV);
    });
}

//=================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_BezierSurface::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const size_t                             aNbU = theUParams.Size();
  const size_t                             aNbV = theVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1,
                                                   static_cast<int>(aNbU),
                                                   1,
                                                   static_cast<int>(aNbV));

  const int                         aUDegree       = myGeom->UDegree();
  const int                         aVDegree       = myGeom->VDegree();
  const NCollection_Array1<double>& aUKnotSequence = myGeom->UKnotSequence();
  const NCollection_Array1<double>& aVKnotSequence = myGeom->VKnotSequence();
  const NCollection_Array2<gp_Pnt>& aPoles         = myGeom->Poles();
  const NCollection_Array2<double>* aWeights       = myGeom->Weights();
  const bool                        isRational     = (aWeights != nullptr);

  for (size_t aUIndex = 0; aUIndex < aNbU; ++aUIndex)
  {
    const double aU = theUParams.At(aUIndex);
    for (size_t aVIndex = 0; aVIndex < aNbV; ++aVIndex)
    {
      GeomGridEval::SurfD3& aValue = aResult.ChangeAt(aUIndex * aNbV + aVIndex);
      BSplSLib::D3(aU,
                   theVParams.At(aVIndex),
                   0, // U span index (single span for Bezier with flat knots)
                   0, // V span index (single span for Bezier with flat knots)
                   aPoles,
                   aWeights,
                   aUKnotSequence,
                   aVKnotSequence,
                   BSplCLib::NoMults(),
                   BSplCLib::NoMults(),
                   aUDegree,
                   aVDegree,
                   isRational,
                   isRational,
                   false, // not U-periodic
                   false, // not V-periodic
                   aValue.Point,
                   aValue.D1U,
                   aValue.D1V,
                   aValue.D2U,
                   aValue.D2V,
                   aValue.D2UV,
                   aValue.D3U,
                   aValue.D3V,
                   aValue.D3UUV,
                   aValue.D3UVV);
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_BezierSurface::EvaluateGridDN(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  int                               theNU,
  int                               theNV) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const size_t aNbU = theUParams.Size();
  const size_t aNbV = theVParams.Size();

  NCollection_Array2<gp_Vec> aResult(1, static_cast<int>(aNbU), 1, static_cast<int>(aNbV));

  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  // Bezier derivatives are zero when the requested order exceeds either directional degree.
  if (theNU > aUDegree || theNV > aVDegree)
  {
    aResult.Init(gp_Vec(0.0, 0.0, 0.0));
    return aResult;
  }

  const NCollection_Array2<gp_Pnt>& aPoles         = myGeom->Poles();
  const NCollection_Array2<double>* aWeights       = myGeom->Weights();
  const bool                        isRational     = (aWeights != nullptr);
  const NCollection_Array1<double>& aUKnotSequence = myGeom->UKnotSequence();
  const NCollection_Array1<double>& aVKnotSequence = myGeom->VKnotSequence();

  for (size_t aUIndex = 0; aUIndex < aNbU; ++aUIndex)
  {
    const double aU = theUParams.At(aUIndex);
    for (size_t aVIndex = 0; aVIndex < aNbV; ++aVIndex)
    {
      BSplSLib::DN(aU,
                   theVParams.At(aVIndex),
                   theNU,
                   theNV,
                   0, // U span index (single span for Bezier with flat knots)
                   0, // V span index (single span for Bezier with flat knots)
                   aPoles,
                   aWeights,
                   aUKnotSequence,
                   aVKnotSequence,
                   BSplCLib::NoMults(),
                   BSplCLib::NoMults(),
                   aUDegree,
                   aVDegree,
                   isRational,
                   isRational,
                   false, // not U-periodic
                   false, // not V-periodic
                   aResult.ChangeAt(aUIndex * aNbV + aVIndex));
    }
  }

  return aResult;
}
