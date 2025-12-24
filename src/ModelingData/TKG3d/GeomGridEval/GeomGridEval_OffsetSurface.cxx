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

#include <GeomGridEval_OffsetSurface.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurfaceUtils.pxx>
#include <GeomGridEval_Curve.hxx>
#include <GeomGridEval_Surface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

//==================================================================================================

void GeomGridEval_OffsetSurface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                             const TColStd_Array1OfReal& theVParams)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  myUParams.Resize(1, aNbU, false);
  for (int i = 1; i <= aNbU; ++i)
  {
    myUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i - 1));
  }

  myVParams.Resize(1, aNbV, false);
  for (int j = 1; j <= aNbV; ++j)
  {
    myVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j - 1));
  }
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_OffsetSurface::EvaluateGrid() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Fast path: check if offset surface has an equivalent simple surface
  // (e.g., offset of plane/sphere/cylinder/cone/torus with same parameters)
  if (!myGeom.IsNull())
  {
    Handle(Geom_Surface) anEquivSurf = myGeom->Surface();
    if (!anEquivSurf.IsNull() && anEquivSurf.get() != myGeom.get())
    {
      // Use the equivalent surface directly
      GeomGridEval_Surface anEquivEval;
      anEquivEval.Initialize(anEquivSurf);
      anEquivEval.SetUVParams(myUParams, myVParams);
      return anEquivEval.EvaluateGrid();
    }
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Check for isoline case (1xN or Nx1) - use 1D curve evaluation
  const bool isUIso = (aNbU == 1 && aNbV > 1);
  const bool isVIso = (aNbV == 1 && aNbU > 1);

  if ((isUIso || isVIso) && !myGeom.IsNull())
  {
    try
    {
      OCC_CATCH_SIGNALS
      // Extract isoline curve - offset surface isolines are offset curves
      Handle(Geom_Curve) aCurve =
        isUIso ? myGeom->UIso(myUParams.Value(1)) : myGeom->VIso(myVParams.Value(1));

      if (!aCurve.IsNull())
      {
        // Use unified curve evaluator
        GeomGridEval_Curve aCurveEval;
        aCurveEval.Initialize(aCurve);
        aCurveEval.SetParams(isUIso ? myVParams : myUParams);
        NCollection_Array1<gp_Pnt> aCurveResult = aCurveEval.EvaluateGrid();

        if (!aCurveResult.IsEmpty())
        {
          // Reshape 1D curve result to 2D surface result
          NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);
          const int                  aNbPts = isUIso ? aNbV : aNbU;
          for (int k = 1; k <= aNbPts; ++k)
          {
            aResult(isUIso ? 1 : k, isUIso ? k : 1) = aCurveResult(k);
          }
          return aResult;
        }
      }
    }
    catch (const Standard_Failure&)
    {
      // Isoline extraction failed, fall through to surface evaluation
    }
  }

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Batch evaluate basis surface D1 (offset D0 requires basis D1)
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);
  NCollection_Array2<GeomGridEval::SurfD1> aBasisD1 = aBasisEval.EvaluateGridD1();

  if (aBasisD1.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Use GeomAdaptor for evaluation - works with template functions
  GeomAdaptor_Surface aBasisAdaptor(myBasis);

  // Pass Geom_OffsetSurface which has UOsculatingSurface/VOsculatingSurface methods
  // for proper handling of singular points via osculating surface queries
  const Geom_OffsetSurface* anOscQuery = myGeom.get();

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD1& aBasis = aBasisD1.Value(i, j);
      gp_Pnt                      aP     = aBasis.Point;
      Geom_OffsetSurfaceUtils::EvaluateD0(myUParams.Value(i),
                                          myVParams.Value(j),
                                          &aBasisAdaptor,
                                          myOffset,
                                          anOscQuery,
                                          aP,
                                          aBasis.D1U,
                                          aBasis.D1V);
      aResult.SetValue(i, j, aP);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_OffsetSurface::EvaluateGridD1() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // Fast path: check if offset surface has an equivalent simple surface
  if (!myGeom.IsNull())
  {
    Handle(Geom_Surface) anEquivSurf = myGeom->Surface();
    if (!anEquivSurf.IsNull() && anEquivSurf.get() != myGeom.get())
    {
      GeomGridEval_Surface anEquivEval;
      anEquivEval.Initialize(anEquivSurf);
      anEquivEval.SetUVParams(myUParams, myVParams);
      return anEquivEval.EvaluateGridD1();
    }
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  // Batch evaluate basis surface D2 (offset D1 requires basis D2)
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);
  NCollection_Array2<GeomGridEval::SurfD2> aBasisD2 = aBasisEval.EvaluateGridD2();

  if (aBasisD2.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  GeomAdaptor_Surface       aBasisAdaptor(myBasis);
  const Geom_OffsetSurface* anOscQuery = myGeom.get();

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD2& aBasis = aBasisD2.Value(i, j);
      gp_Pnt                      aP     = aBasis.Point;
      gp_Vec                      aD1U   = aBasis.D1U;
      gp_Vec                      aD1V   = aBasis.D1V;
      Geom_OffsetSurfaceUtils::EvaluateD1(myUParams.Value(i),
                                          myVParams.Value(j),
                                          &aBasisAdaptor,
                                          myOffset,
                                          anOscQuery,
                                          aP,
                                          aD1U,
                                          aD1V,
                                          aBasis.D2U,
                                          aBasis.D2V,
                                          aBasis.D2UV);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_OffsetSurface::EvaluateGridD2() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // Fast path: check if offset surface has an equivalent simple surface
  if (!myGeom.IsNull())
  {
    Handle(Geom_Surface) anEquivSurf = myGeom->Surface();
    if (!anEquivSurf.IsNull() && anEquivSurf.get() != myGeom.get())
    {
      GeomGridEval_Surface anEquivEval;
      anEquivEval.Initialize(anEquivSurf);
      anEquivEval.SetUVParams(myUParams, myVParams);
      return anEquivEval.EvaluateGridD2();
    }
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  // Batch evaluate basis surface D3 (offset D2 requires basis D3)
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);
  NCollection_Array2<GeomGridEval::SurfD3> aBasisD3 = aBasisEval.EvaluateGridD3();

  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  GeomAdaptor_Surface       aBasisAdaptor(myBasis);
  const Geom_OffsetSurface* anOscQuery = myGeom.get();

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD3& aBasis = aBasisD3.Value(i, j);
      gp_Pnt                      aP     = aBasis.Point;
      gp_Vec                      aD1U   = aBasis.D1U;
      gp_Vec                      aD1V   = aBasis.D1V;
      gp_Vec                      aD2U   = aBasis.D2U;
      gp_Vec                      aD2V   = aBasis.D2V;
      gp_Vec                      aD2UV  = aBasis.D2UV;
      Geom_OffsetSurfaceUtils::EvaluateD2(myUParams.Value(i),
                                          myVParams.Value(j),
                                          &aBasisAdaptor,
                                          myOffset,
                                          anOscQuery,
                                          aP,
                                          aD1U,
                                          aD1V,
                                          aD2U,
                                          aD2V,
                                          aD2UV,
                                          aBasis.D3U,
                                          aBasis.D3V,
                                          aBasis.D3UUV,
                                          aBasis.D3UVV);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_OffsetSurface::EvaluateGridD3() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // Fast path: check if offset surface has an equivalent simple surface
  if (!myGeom.IsNull())
  {
    Handle(Geom_Surface) anEquivSurf = myGeom->Surface();
    if (!anEquivSurf.IsNull() && anEquivSurf.get() != myGeom.get())
    {
      GeomGridEval_Surface anEquivEval;
      anEquivEval.Initialize(anEquivSurf);
      anEquivEval.SetUVParams(myUParams, myVParams);
      return anEquivEval.EvaluateGridD3();
    }
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  GeomAdaptor_Surface       aBasisAdaptor(myBasis);
  const Geom_OffsetSurface* anOscQuery = myGeom.get();

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      gp_Pnt aP;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      Geom_OffsetSurfaceUtils::EvaluateD3(myUParams.Value(i),
                                          myVParams.Value(j),
                                          &aBasisAdaptor,
                                          myOffset,
                                          anOscQuery,
                                          aP,
                                          aD1U,
                                          aD1V,
                                          aD2U,
                                          aD2V,
                                          aD2UV,
                                          aD3U,
                                          aD3V,
                                          aD3UUV,
                                          aD3UVV);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_OffsetSurface::EvaluateGridDN(int theNU, int theNV) const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  // Fast path: check if offset surface has an equivalent simple surface
  if (!myGeom.IsNull())
  {
    Handle(Geom_Surface) anEquivSurf = myGeom->Surface();
    if (!anEquivSurf.IsNull() && anEquivSurf.get() != myGeom.get())
    {
      GeomGridEval_Surface anEquivEval;
      anEquivEval.Initialize(anEquivSurf);
      anEquivEval.SetUVParams(myUParams, myVParams);
      return anEquivEval.EvaluateGridDN(theNU, theNV);
    }
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  // Batch evaluate basis surface D1 (offset DN requires basis D1)
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);
  NCollection_Array2<GeomGridEval::SurfD1> aBasisD1 = aBasisEval.EvaluateGridD1();

  if (aBasisD1.IsEmpty())
  {
    return NCollection_Array2<gp_Vec>();
  }

  GeomAdaptor_Surface       aBasisAdaptor(myBasis);
  const Geom_OffsetSurface* anOscQuery = myGeom.get();

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD1& aBasis = aBasisD1.Value(i, j);
      gp_Vec                      aDN;
      Geom_OffsetSurfaceUtils::EvaluateDN(myUParams.Value(i),
                                          myVParams.Value(j),
                                          theNU,
                                          theNV,
                                          &aBasisAdaptor,
                                          myOffset,
                                          anOscQuery,
                                          aDN,
                                          aBasis.D1U,
                                          aBasis.D1V);
      aResult.SetValue(i, j, aDN);
    }
  }

  return aResult;
}
