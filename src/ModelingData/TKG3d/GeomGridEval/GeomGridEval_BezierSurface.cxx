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

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

//==================================================================================================

void GeomGridEval_BezierSurface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                             const TColStd_Array1OfReal& theVParams)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  myUParams.Resize(0, aNbU - 1, false);
  for (int i = 0; i < aNbU; ++i)
  {
    myUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i));
  }

  myVParams.Resize(0, aNbV - 1, false);
  for (int j = 0; j < aNbV; ++j)
  {
    myVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j));
  }

  // Invalidate cache when parameters change
  myCache.Nullify();
}

//==================================================================================================

void GeomGridEval_BezierSurface::buildCache() const
{
  if (myGeom.IsNull())
  {
    return;
  }

  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  // Use pre-defined flat knots from BSplCLib
  TColStd_Array1OfReal aUFlatKnots(BSplCLib::FlatBezierKnots(aUDegree), 1, 2 * (aUDegree + 1));
  TColStd_Array1OfReal aVFlatKnots(BSplCLib::FlatBezierKnots(aVDegree), 1, 2 * (aVDegree + 1));

  // Get poles and weights directly (const references, no copy)
  const TColgp_Array2OfPnt&   aPoles   = myGeom->Poles();
  const TColStd_Array2OfReal* aWeights = myGeom->Weights();

  // Create cache (Bezier is non-periodic)
  myCache = new BSplSLib_Cache(aUDegree,
                               false, // not periodic
                               aUFlatKnots,
                               aVDegree,
                               false, // not periodic
                               aVFlatKnots,
                               aWeights);

  // Build cache at parameter 0.5 (middle of single span)
  myCache->BuildCache(0.5, 0.5, aUFlatKnots, aVFlatKnots, aPoles, aWeights);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BezierSurface::EvaluateGrid() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                  aNbU = myUParams.Size();
  const int                  aNbV = myVParams.Size();
  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Single span - use cache for all points
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      myCache->D0(aU, myVParams.Value(j), aPoint);
      aResult.SetValue(i + 1, j + 1, aPoint);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BezierSurface::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  // Single span - use cache for all points
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      myCache->D1(aU, myVParams.Value(j), aPoint, aD1U, aD1V);
      aResult.ChangeValue(i + 1, j + 1) = {aPoint, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BezierSurface::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  // Single span - use cache for all points
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      myCache->D2(aU, myVParams.Value(j), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResult.ChangeValue(i + 1, j + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_BezierSurface::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  // Get degrees and create Bezier flat knots
  const int aUDegree = myGeom->UDegree();
  const int aVDegree = myGeom->VDegree();

  TColStd_Array1OfReal aUFlatKnots(BSplCLib::FlatBezierKnots(aUDegree), 1, 2 * (aUDegree + 1));
  TColStd_Array1OfReal aVFlatKnots(BSplCLib::FlatBezierKnots(aVDegree), 1, 2 * (aVDegree + 1));

  // Get poles and weights
  const TColgp_Array2OfPnt&   aPoles     = myGeom->Poles();
  const TColStd_Array2OfReal* aWeights   = myGeom->Weights();
  const bool                  isRational = (aWeights != nullptr);

  // D3 evaluation using BSplSLib::D3 directly
  // Bezier surface is single span (span index = 0), non-periodic
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;

      BSplSLib::D3(aU,
                   myVParams.Value(j),
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

NCollection_Array2<gp_Vec> GeomGridEval_BezierSurface::EvaluateGridDN(int theNU, int theNV) const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

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

  // Get poles and weights from geometry
  const TColgp_Array2OfPnt&   aPoles     = myGeom->Poles();
  const TColStd_Array2OfReal* aWeights   = myGeom->Weights();
  const bool                  isRational = (aWeights != nullptr);

  // Use pre-defined flat knots from BSplCLib
  TColStd_Array1OfReal aUFlatKnots(BSplCLib::FlatBezierKnots(aUDegree), 1, 2 * (aUDegree + 1));
  TColStd_Array1OfReal aVFlatKnots(BSplCLib::FlatBezierKnots(aVDegree), 1, 2 * (aVDegree + 1));

  // Bezier has a single span (index 0 with flat knots), non-periodic
  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Vec aDN;
      BSplSLib::DN(aU,
                   myVParams.Value(j),
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
