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

#include <GeomGridEval_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>

namespace
{
//! Minimum number of points along varying dimension to use isoline optimization.
//! For small grids (e.g., 1x4), direct surface evaluation is faster than
//! extracting an isoline curve and setting up a curve evaluator.
constexpr int THE_ISOLINE_THRESHOLD = 8;
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

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      myGeom->D0(aU, theVParams.Value(theVParams.Lower() + j), aPoint);
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

  const int                                aNbU = theUParams.Size();
  const int                                aNbV = theVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      myGeom->D1(aU, theVParams.Value(theVParams.Lower() + j), aPoint, aD1U, aD1V);
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

  const int                                aNbU = theUParams.Size();
  const int                                aNbV = theVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      myGeom->D2(aU, theVParams.Value(theVParams.Lower() + j), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
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

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      myGeom->D3(aU,
                 theVParams.Value(theVParams.Lower() + j),
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

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      aResult.SetValue(i + 1,
                       j + 1,
                       myGeom->DN(aU, theVParams.Value(theVParams.Lower() + j), theNU, theNV));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BezierSurface::EvaluatePoints(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbPts = theUVPairs.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNbPts);

  for (int i = 0; i < aNbPts; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    myGeom->D0(aUV.X(), aUV.Y(), aPoint);
    aResult.SetValue(i + 1, aPoint);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD1> GeomGridEval_BezierSurface::EvaluatePointsD1(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD1>();
  }

  const int                                aNbPts = theUVPairs.Size();
  NCollection_Array1<GeomGridEval::SurfD1> aResult(1, aNbPts);

  for (int i = 0; i < aNbPts; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    gp_Vec          aD1U, aD1V;
    myGeom->D1(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V);
    aResult.ChangeValue(i + 1) = {aPoint, aD1U, aD1V};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD2> GeomGridEval_BezierSurface::EvaluatePointsD2(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD2>();
  }

  const int                                aNbPts = theUVPairs.Size();
  NCollection_Array1<GeomGridEval::SurfD2> aResult(1, aNbPts);

  for (int i = 0; i < aNbPts; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV;
    myGeom->D2(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
    aResult.ChangeValue(i + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD3> GeomGridEval_BezierSurface::EvaluatePointsD3(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD3>();
  }

  const int                                aNbPts = theUVPairs.Size();
  NCollection_Array1<GeomGridEval::SurfD3> aResult(1, aNbPts);

  for (int i = 0; i < aNbPts; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
    myGeom
      ->D3(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
    aResult.ChangeValue(i
                        + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BezierSurface::EvaluatePointsDN(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs,
  int                                 theNU,
  int                                 theNV) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty() || theNU < 0 || theNV < 0 || (theNU + theNV) < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int aNbPts = theUVPairs.Size();

  NCollection_Array1<gp_Vec> aResult(1, aNbPts);

  for (int i = 0; i < aNbPts; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    aResult.SetValue(i + 1, myGeom->DN(aUV.X(), aUV.Y(), theNU, theNV));
  }

  return aResult;
}
