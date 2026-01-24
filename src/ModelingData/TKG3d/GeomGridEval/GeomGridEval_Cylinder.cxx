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

#include <GeomGridEval_Cylinder.hxx>

#include <gp_Cylinder.hxx>

#include <cmath>

//==================================================================================================

GeomGridEval_Cylinder::Data GeomGridEval_Cylinder::extractData() const
{
  const gp_Cylinder& aCyl    = myGeom->Cylinder();
  const gp_Pnt&      aCenter = aCyl.Location();
  const gp_Dir&      aXDir   = aCyl.Position().XDirection();
  const gp_Dir&      aYDir   = aCyl.Position().YDirection();
  const gp_Dir&      aZDir   = aCyl.Position().Direction();

  return {aCenter.X(),
          aCenter.Y(),
          aCenter.Z(),
          aXDir.X(),
          aXDir.Y(),
          aXDir.Z(),
          aYDir.X(),
          aYDir.Y(),
          aYDir.Z(),
          aZDir.X(),
          aZDir.Y(),
          aZDir.Z(),
          aCyl.Radius()};
}

//==================================================================================================

GeomGridEval_Cylinder::UContext GeomGridEval_Cylinder::computeUContext(const Data& theData,
                                                                       double      theU)
{
  const double cosU = std::cos(theU);
  const double sinU = std::sin(theU);

  // DirU = cosU*XDir + sinU*YDir (circle point direction at angle U)
  // dDirU = -sinU*XDir + cosU*YDir (derivative of DirU w.r.t. U)
  return {cosU,
          sinU,
          cosU * theData.XX + sinU * theData.YX,
          cosU * theData.XY + sinU * theData.YY,
          cosU * theData.XZ + sinU * theData.YZ,
          -sinU * theData.XX + cosU * theData.YX,
          -sinU * theData.XY + cosU * theData.YY,
          -sinU * theData.XZ + cosU * theData.YZ};
}

//==================================================================================================

gp_Pnt GeomGridEval_Cylinder::computeD0(const Data& theData, const UContext& theUCtx, double theV)
{
  // P = Center + R * DirU + v * ZDir
  return gp_Pnt(theData.CX + theData.Radius * theUCtx.dirUX + theV * theData.ZX,
                theData.CY + theData.Radius * theUCtx.dirUY + theV * theData.ZY,
                theData.CZ + theData.Radius * theUCtx.dirUZ + theV * theData.ZZ);
}

//==================================================================================================

GeomGridEval::SurfD1 GeomGridEval_Cylinder::computeD1(const Data&     theData,
                                                      const UContext& theUCtx,
                                                      double          theV)
{
  // D1U = R * dDirU
  // D1V = ZDir
  return {gp_Pnt(theData.CX + theData.Radius * theUCtx.dirUX + theV * theData.ZX,
                 theData.CY + theData.Radius * theUCtx.dirUY + theV * theData.ZY,
                 theData.CZ + theData.Radius * theUCtx.dirUZ + theV * theData.ZZ),
          gp_Vec(theData.Radius * theUCtx.dDirUX,
                 theData.Radius * theUCtx.dDirUY,
                 theData.Radius * theUCtx.dDirUZ),
          gp_Vec(theData.ZX, theData.ZY, theData.ZZ)};
}

//==================================================================================================

GeomGridEval::SurfD2 GeomGridEval_Cylinder::computeD2(const Data&     theData,
                                                      const UContext& theUCtx,
                                                      double          theV)
{
  // D2U = R * (-DirU) = -R * DirU
  // D2V = 0
  // D2UV = 0
  const gp_Vec aZero(0.0, 0.0, 0.0);

  return {gp_Pnt(theData.CX + theData.Radius * theUCtx.dirUX + theV * theData.ZX,
                 theData.CY + theData.Radius * theUCtx.dirUY + theV * theData.ZY,
                 theData.CZ + theData.Radius * theUCtx.dirUZ + theV * theData.ZZ),
          gp_Vec(theData.Radius * theUCtx.dDirUX,
                 theData.Radius * theUCtx.dDirUY,
                 theData.Radius * theUCtx.dDirUZ),
          gp_Vec(theData.ZX, theData.ZY, theData.ZZ),
          gp_Vec(-theData.Radius * theUCtx.dirUX,
                 -theData.Radius * theUCtx.dirUY,
                 -theData.Radius * theUCtx.dirUZ),
          aZero,
          aZero};
}

//==================================================================================================

GeomGridEval::SurfD3 GeomGridEval_Cylinder::computeD3(const Data&     theData,
                                                      const UContext& theUCtx,
                                                      double          theV)
{
  // D3U = R * (-dDirU) = -D1U
  // D3V = 0
  // D3UUV = 0
  // D3UVV = 0
  const gp_Vec aZero(0.0, 0.0, 0.0);

  return {gp_Pnt(theData.CX + theData.Radius * theUCtx.dirUX + theV * theData.ZX,
                 theData.CY + theData.Radius * theUCtx.dirUY + theV * theData.ZY,
                 theData.CZ + theData.Radius * theUCtx.dirUZ + theV * theData.ZZ),
          gp_Vec(theData.Radius * theUCtx.dDirUX,
                 theData.Radius * theUCtx.dDirUY,
                 theData.Radius * theUCtx.dDirUZ),
          gp_Vec(theData.ZX, theData.ZY, theData.ZZ),
          gp_Vec(-theData.Radius * theUCtx.dirUX,
                 -theData.Radius * theUCtx.dirUY,
                 -theData.Radius * theUCtx.dirUZ),
          aZero,
          aZero,
          gp_Vec(-theData.Radius * theUCtx.dDirUX,
                 -theData.Radius * theUCtx.dDirUY,
                 -theData.Radius * theUCtx.dDirUZ),
          aZero,
          aZero,
          aZero};
}

//==================================================================================================

gp_Vec GeomGridEval_Cylinder::computeDN(const Data&     theData,
                                        const UContext& theUCtx,
                                        double /*theV*/,
                                        int theNU,
                                        int theNV)
{
  // For cylinder P(u,v) = C + R*(cos(u)*X + sin(u)*Y) + v*Z:
  // - V derivatives: only D_{0,1} = Z is non-zero, all D_{nu,nv} = 0 for nv >= 2
  // - Mixed derivatives D_{nu,1} = 0 for nu >= 1 (Z term has no U dependence)
  // - Pure U derivatives cycle with period 4

  if (theNV > 1)
  {
    return gp_Vec(0.0, 0.0, 0.0);
  }

  if (theNV == 1)
  {
    if (theNU == 0)
    {
      return gp_Vec(theData.ZX, theData.ZY, theData.ZZ);
    }
    return gp_Vec(0.0, 0.0, 0.0);
  }

  // Pure U derivatives (theNV == 0, theNU >= 1)
  // d^n/du^n of DirU cycles: DirU, dDirU, -DirU, -dDirU
  const int aPhase = theNU % 4;

  double dirX, dirY, dirZ;
  switch (aPhase)
  {
    case 0:
      dirX = theUCtx.dirUX;
      dirY = theUCtx.dirUY;
      dirZ = theUCtx.dirUZ;
      break;
    case 1:
      dirX = theUCtx.dDirUX;
      dirY = theUCtx.dDirUY;
      dirZ = theUCtx.dDirUZ;
      break;
    case 2:
      dirX = -theUCtx.dirUX;
      dirY = -theUCtx.dirUY;
      dirZ = -theUCtx.dirUZ;
      break;
    case 3:
      dirX = -theUCtx.dDirUX;
      dirY = -theUCtx.dDirUY;
      dirZ = -theUCtx.dDirUZ;
      break;
    default:
      dirX = 0.0;
      dirY = 0.0;
      dirZ = 0.0;
      break;
  }

  return gp_Vec(theData.Radius * dirX, theData.Radius * dirY, theData.Radius * dirZ);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_Cylinder::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const Data aData = extractData();
  const int  aNbU  = theUParams.Length();
  const int  aNbV  = theVParams.Length();

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   aU    = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, aU);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD0(aData, aUCtx, aV));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Cylinder::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const Data aData = extractData();
  const int  aNbU  = theUParams.Length();
  const int  aNbV  = theVParams.Length();

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   aU    = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, aU);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD1(aData, aUCtx, aV));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Cylinder::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const Data aData = extractData();
  const int  aNbU  = theUParams.Length();
  const int  aNbV  = theVParams.Length();

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   aU    = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, aU);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD2(aData, aUCtx, aV));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Cylinder::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const Data aData = extractData();
  const int  aNbU  = theUParams.Length();
  const int  aNbV  = theVParams.Length();

  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   aU    = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, aU);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD3(aData, aUCtx, aV));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_Cylinder::EvaluateGridDN(
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

  const Data aData = extractData();
  const int  aNbU  = theUParams.Length();
  const int  aNbV  = theVParams.Length();

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   aU    = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, aU);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeDN(aData, aUCtx, aV, theNU, theNV));
    }
  }

  return aResult;
}
