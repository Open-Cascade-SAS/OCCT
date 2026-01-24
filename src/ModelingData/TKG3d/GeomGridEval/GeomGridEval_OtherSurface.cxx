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

#include <GeomGridEval_OtherSurface.hxx>

//==================================================================================================

void GeomGridEval_OtherSurface::evaluateD0(double theU, double theV, gp_Pnt& thePoint) const
{
  std::visit([theU, theV, &thePoint](const auto& theSurf) { theSurf->D0(theU, theV, thePoint); },
             mySurface);
}

//==================================================================================================

void GeomGridEval_OtherSurface::evaluateD1(double  theU,
                                           double  theV,
                                           gp_Pnt& thePoint,
                                           gp_Vec& theD1U,
                                           gp_Vec& theD1V) const
{
  std::visit([theU, theV, &thePoint, &theD1U, &theD1V](
               const auto& theSurf) { theSurf->D1(theU, theV, thePoint, theD1U, theD1V); },
             mySurface);
}

//==================================================================================================

void GeomGridEval_OtherSurface::evaluateD2(double  theU,
                                           double  theV,
                                           gp_Pnt& thePoint,
                                           gp_Vec& theD1U,
                                           gp_Vec& theD1V,
                                           gp_Vec& theD2U,
                                           gp_Vec& theD2V,
                                           gp_Vec& theD2UV) const
{
  std::visit(
    [theU, theV, &thePoint, &theD1U, &theD1V, &theD2U, &theD2V, &theD2UV](const auto& theSurf) {
      theSurf->D2(theU, theV, thePoint, theD1U, theD1V, theD2U, theD2V, theD2UV);
    },
    mySurface);
}

//==================================================================================================

void GeomGridEval_OtherSurface::evaluateD3(double  theU,
                                           double  theV,
                                           gp_Pnt& thePoint,
                                           gp_Vec& theD1U,
                                           gp_Vec& theD1V,
                                           gp_Vec& theD2U,
                                           gp_Vec& theD2V,
                                           gp_Vec& theD2UV,
                                           gp_Vec& theD3U,
                                           gp_Vec& theD3V,
                                           gp_Vec& theD3UUV,
                                           gp_Vec& theD3UVV) const
{
  std::visit(
    [theU,
     theV,
     &thePoint,
     &theD1U,
     &theD1V,
     &theD2U,
     &theD2V,
     &theD2UV,
     &theD3U,
     &theD3V,
     &theD3UUV,
     &theD3UVV](const auto& theSurf) {
      theSurf->D3(theU,
                  theV,
                  thePoint,
                  theD1U,
                  theD1V,
                  theD2U,
                  theD2V,
                  theD2UV,
                  theD3U,
                  theD3V,
                  theD3UUV,
                  theD3UVV);
    },
    mySurface);
}

//==================================================================================================

gp_Vec GeomGridEval_OtherSurface::evaluateDN(double theU, double theV, int theNU, int theNV) const
{
  return std::visit(
    [theU, theV, theNU, theNV](const auto& theSurf) -> gp_Vec {
      return theSurf->DN(theU, theV, theNU, theNV);
    },
    mySurface);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_OtherSurface::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<gp_Pnt>();
  }

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      gp_Pnt       aPoint;
      evaluateD0(u, v, aPoint);
      aResult.SetValue(iU, iV, aPoint);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_OtherSurface::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      gp_Pnt       aPoint;
      gp_Vec       aD1U, aD1V;
      evaluateD1(u, v, aPoint, aD1U, aD1V);
      aResult.ChangeValue(iU, iV) = {aPoint, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_OtherSurface::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      gp_Pnt       aPoint;
      gp_Vec       aD1U, aD1V, aD2U, aD2V, aD2UV;
      evaluateD2(u, v, aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResult.ChangeValue(iU, iV) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_OtherSurface::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      gp_Pnt       aPoint;
      gp_Vec       aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      evaluateD3(u, v, aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
      aResult.ChangeValue(iU,
                          iV) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_OtherSurface::EvaluateGridDN(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  int                               theNU,
  int                               theNV) const
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0 || theNU < 0 || theNV < 0 || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v   = theVParams.Value(theVParams.Lower() + iV - 1);
      const gp_Vec aDN = evaluateDN(u, v, theNU, theNV);
      aResult.SetValue(iU, iV, aDN);
    }
  }

  return aResult;
}
