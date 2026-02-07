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

namespace
{
//! Minimum number of points along varying dimension to use isoline optimization.
//! For small grids (e.g., 1x4), cache-based surface evaluation is faster than
//! extracting an isoline curve and setting up a curve evaluator.
constexpr int THE_ISOLINE_THRESHOLD = 8;

//! Create and build cache for Bezier surface evaluation.
//! Bezier surfaces are single-span, so cache is built once at parameter (0.5, 0.5).
//! @param theGeom the Bezier surface geometry
//! @return the built cache
occ::handle<BSplSLib_Cache> buildBezierCache(const occ::handle<Geom_BezierSurface>& theGeom)
{
  const NCollection_Array1<double>& aUFlatKnots = theGeom->InternalUFlatKnots();
  const NCollection_Array1<double>& aVFlatKnots = theGeom->InternalVFlatKnots();
  const NCollection_Array2<gp_Pnt>& aPoles      = theGeom->Poles();
  const NCollection_Array2<double>* aWeights    = theGeom->Weights();

  occ::handle<BSplSLib_Cache> aCache = new BSplSLib_Cache(theGeom->UDegree(),
                                                          false,
                                                          aUFlatKnots,
                                                          theGeom->VDegree(),
                                                          false,
                                                          aVFlatKnots,
                                                          aWeights);
  aCache->BuildCache(0.5, 0.5, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
  return aCache;
}
} // namespace

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BezierSurface::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Check for V-isoline case (Nx1) - use 1D curve evaluation
  // For U-isoline (1xN), cache-based surface evaluation is efficient since U span is fixed.
  // For V-isoline (Nx1), extracting the isoline curve avoids repeated cache rebuilds.
  // Only use isoline optimization when varying dimension is large enough.
  const bool isVIso = (aNbV == 1 && aNbU >= THE_ISOLINE_THRESHOLD);

  if (isVIso)
  {
    try
    {
      OCC_CATCH_SIGNALS
      // Extract V-isoline curve (parameterized by U)
      occ::handle<Geom_Curve> aCurve = myGeom->VIso(theVParams.Value(theVParams.Lower()));

      if (!aCurve.IsNull())
      {
        // Use unified curve evaluator
        GeomGridEval_Curve aCurveEval;
        aCurveEval.Initialize(aCurve);

        NCollection_Array1<gp_Pnt> aCurveResult = aCurveEval.EvaluateGrid(theUParams);

        // Reshape 1D curve result to 2D surface result (Nx1 grid)
        NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, 1);
        for (int k = 1; k <= aNbU; ++k)
        {
          aResult(k, 1) = aCurveResult(k);
        }
        return aResult;
      }
    }
    catch (const Standard_Failure&)
    {
      // Isoline extraction failed, fall through to surface evaluation
    }
  }

  // Build cache (Bezier is single span, cache is built once)
  occ::handle<BSplSLib_Cache> aCache = buildBezierCache(myGeom);

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Single span - use cache for all points
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      aCache->D0(aU, theVParams.Value(theVParams.Lower() + j), aPoint);
      aResult.SetValue(i + 1, j + 1, aPoint);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BezierSurface::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // Build cache (Bezier is single span, cache is built once)
  occ::handle<BSplSLib_Cache> aCache = buildBezierCache(myGeom);

  const int                                aNbU = theUParams.Size();
  const int                                aNbV = theVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  // Single span - use cache for all points
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      aCache->D1(aU, theVParams.Value(theVParams.Lower() + j), aPoint, aD1U, aD1V);
      aResult.ChangeValue(i + 1, j + 1) = {aPoint, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BezierSurface::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // Build cache (Bezier is single span, cache is built once)
  occ::handle<BSplSLib_Cache> aCache = buildBezierCache(myGeom);

  const int                                aNbU = theUParams.Size();
  const int                                aNbV = theVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  // Single span - use cache for all points
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      aCache
        ->D2(aU, theVParams.Value(theVParams.Lower() + j), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResult.ChangeValue(i + 1, j + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_BezierSurface::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int                                aNbU = theUParams.Size();
  const int                                aNbV = theVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  // Get degrees, flat knots, poles, and weights from geometry
  const int                          aUDegree    = myGeom->UDegree();
  const int                          aVDegree    = myGeom->VDegree();
  const NCollection_Array1<double>&  aUFlatKnots = myGeom->InternalUFlatKnots();
  const NCollection_Array1<double>&  aVFlatKnots = myGeom->InternalVFlatKnots();
  const NCollection_Array2<gp_Pnt>&  aPoles      = myGeom->Poles();
  const NCollection_Array2<double>*  aWeights    = myGeom->Weights();
  const bool                         isRational  = (aWeights != nullptr);

  // D3 evaluation using BSplSLib::D3 directly
  // Bezier surface is single span (span index = 0), non-periodic
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;

      BSplSLib::D3(aU,
                   theVParams.Value(theVParams.Lower() + j),
                   0, // U span index (single span for Bezier)
                   0, // V span index (single span for Bezier)
                   aPoles,
                   aWeights,
                   aUFlatKnots,
                   aVFlatKnots,
                   nullptr, // U multiplicities (nullptr for flat knots)
                   nullptr, // V multiplicities (nullptr for flat knots)
                   aUDegree,
                   aVDegree,
                   isRational,
                   isRational,
                   false, // not U periodic
                   false, // not V periodic
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

      aResult.ChangeValue(
        i + 1,
        j + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}

//==================================================================================================

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

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  // For Bezier surfaces, derivatives become zero when order exceeds degree in that direction
  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  if (theNU > aUDegree || theNV > aVDegree)
  {
    // All derivatives are zero
    const gp_Vec aZeroVec(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aZeroVec);
      }
    }
    return aResult;
  }

  // Get poles, weights, and flat knots from geometry
  const NCollection_Array2<gp_Pnt>&  aPoles      = myGeom->Poles();
  const NCollection_Array2<double>*  aWeights    = myGeom->Weights();
  const bool                         isRational  = (aWeights != nullptr);
  const NCollection_Array1<double>&  aUFlatKnots = myGeom->InternalUFlatKnots();
  const NCollection_Array1<double>&  aVFlatKnots = myGeom->InternalVFlatKnots();

  // Bezier has a single span (index 0 with flat knots), non-periodic
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Vec aDN;
      BSplSLib::DN(aU,
                   theVParams.Value(theVParams.Lower() + j),
                   theNU,
                   theNV,
                   0, // U span index (single span for Bezier with flat knots)
                   0, // V span index (single span for Bezier with flat knots)
                   aPoles,
                   aWeights,
                   aUFlatKnots,
                   aVFlatKnots,
                   nullptr, // no U multiplicities with flat knots
                   nullptr, // no V multiplicities with flat knots
                   aUDegree,
                   aVDegree,
                   isRational,
                   isRational,
                   false, // not U-periodic
                   false, // not V-periodic
                   aDN);
      aResult.SetValue(i + 1, j + 1, aDN);
    }
  }

  return aResult;
}
