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

#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BSplineSurface::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<gp_Pnt> aGrid(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      myGeom->D0(aU, theVParams.Value(theVParams.Lower() + j), aPoint);
      aGrid.SetValue(i + 1, j + 1, aPoint);
    }
  }

  return aGrid;
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BSplineSurface::EvaluatePoints(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbPoints = theUVPairs.Size();
  NCollection_Array1<gp_Pnt> aPoints(1, aNbPoints);

  for (int i = 0; i < aNbPoints; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    myGeom->D0(aUV.X(), aUV.Y(), aPoint);
    aPoints.SetValue(i + 1, aPoint);
  }

  return aPoints;
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

  NCollection_Array2<GeomGridEval::SurfD1> aGrid(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V;
      myGeom->D1(aU, theVParams.Value(theVParams.Lower() + j), aPoint, aD1U, aD1V);
      aGrid.ChangeValue(i + 1, j + 1) = {aPoint, aD1U, aD1V};
    }
  }

  return aGrid;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD1> GeomGridEval_BSplineSurface::EvaluatePointsD1(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD1>();
  }

  const int                                aNbPoints = theUVPairs.Size();
  NCollection_Array1<GeomGridEval::SurfD1> aResults(1, aNbPoints);

  for (int i = 0; i < aNbPoints; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    gp_Vec          aD1U, aD1V;
    myGeom->D1(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V);
    aResults.ChangeValue(i + 1) = {aPoint, aD1U, aD1V};
  }

  return aResults;
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

  NCollection_Array2<GeomGridEval::SurfD2> aGrid(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      myGeom->D2(aU, theVParams.Value(theVParams.Lower() + j), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aGrid.ChangeValue(i + 1, j + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aGrid;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD2> GeomGridEval_BSplineSurface::EvaluatePointsD2(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD2>();
  }

  const int                                aNbPoints = theUVPairs.Size();
  NCollection_Array1<GeomGridEval::SurfD2> aResults(1, aNbPoints);

  for (int i = 0; i < aNbPoints; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV;
    myGeom->D2(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
    aResults.ChangeValue(i + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
  }

  return aResults;
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

  NCollection_Array2<GeomGridEval::SurfD3> aGrid(1, aNbU, 1, aNbV);

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
      aGrid.ChangeValue(
        i + 1,
        j + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aGrid;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::SurfD3> GeomGridEval_BSplineSurface::EvaluatePointsD3(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::SurfD3>();
  }

  const int                                aNbPoints = theUVPairs.Size();
  NCollection_Array1<GeomGridEval::SurfD3> aResults(1, aNbPoints);

  for (int i = 0; i < aNbPoints; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    gp_Pnt          aPoint;
    gp_Vec          aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
    myGeom
      ->D3(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
    aResults.ChangeValue(i
                         + 1) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
  }

  return aResults;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_BSplineSurface::EvaluateGridDN(
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

  NCollection_Array2<gp_Vec> aGrid(1, aNbU, 1, aNbV);

  for (int i = 0; i < aNbU; ++i)
  {
    const double aU = theUParams.Value(theUParams.Lower() + i);
    for (int j = 0; j < aNbV; ++j)
    {
      aGrid.SetValue(i + 1,
                     j + 1,
                     myGeom->DN(aU, theVParams.Value(theVParams.Lower() + j), theNU, theNV));
    }
  }

  return aGrid;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BSplineSurface::EvaluatePointsDN(
  const NCollection_Array1<gp_Pnt2d>& theUVPairs,
  int                                 theNU,
  int                                 theNV) const
{
  if (myGeom.IsNull() || theUVPairs.IsEmpty() || theNU < 0 || theNV < 0 || (theNU + theNV) < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNbPoints = theUVPairs.Size();
  NCollection_Array1<gp_Vec> aResults(1, aNbPoints);

  for (int i = 0; i < aNbPoints; ++i)
  {
    const gp_Pnt2d& aUV = theUVPairs.Value(theUVPairs.Lower() + i);
    aResults.SetValue(i + 1, myGeom->DN(aUV.X(), aUV.Y(), theNU, theNV));
  }

  return aResults;
}
