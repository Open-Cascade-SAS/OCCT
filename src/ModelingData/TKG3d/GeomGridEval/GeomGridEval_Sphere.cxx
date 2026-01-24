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

#include <GeomGridEval_Sphere.hxx>

#include <gp_Sphere.hxx>

#include <cmath>

//==================================================================================================

GeomGridEval_Sphere::Data GeomGridEval_Sphere::extractData() const
{
  const gp_Sphere& aSph    = myGeom->Sphere();
  const gp_Pnt&    aCenter = aSph.Location();
  const gp_Dir&    aXDir   = aSph.Position().XDirection();
  const gp_Dir&    aYDir   = aSph.Position().YDirection();
  const gp_Dir&    aZDir   = aSph.Position().Direction();

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
          aSph.Radius()};
}

//==================================================================================================

GeomGridEval_Sphere::UContext GeomGridEval_Sphere::computeUContext(const Data& theData, double theU)
{
  const double cosU = std::cos(theU);
  const double sinU = std::sin(theU);

  // DirU = cosU*XDir + sinU*YDir (horizontal direction at angle U)
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

gp_Pnt GeomGridEval_Sphere::computeD0(const Data& theData, const UContext& theUCtx, double theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  // P = Center + R * (cosV * DirU + sinV * ZDir)
  const double RcosV = theData.Radius * cosV;
  const double RsinV = theData.Radius * sinV;

  return gp_Pnt(theData.CX + RcosV * theUCtx.dirUX + RsinV * theData.ZX,
                theData.CY + RcosV * theUCtx.dirUY + RsinV * theData.ZY,
                theData.CZ + RcosV * theUCtx.dirUZ + RsinV * theData.ZZ);
}

//==================================================================================================

GeomGridEval::SurfD1 GeomGridEval_Sphere::computeD1(const Data&     theData,
                                                    const UContext& theUCtx,
                                                    double          theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  const double RcosV = theData.Radius * cosV;
  const double RsinV = theData.Radius * sinV;

  // D1U = R * cosV * dDirU
  // D1V = R * (-sinV * DirU + cosV * ZDir)
  return {gp_Pnt(theData.CX + RcosV * theUCtx.dirUX + RsinV * theData.ZX,
                 theData.CY + RcosV * theUCtx.dirUY + RsinV * theData.ZY,
                 theData.CZ + RcosV * theUCtx.dirUZ + RsinV * theData.ZZ),
          gp_Vec(RcosV * theUCtx.dDirUX, RcosV * theUCtx.dDirUY, RcosV * theUCtx.dDirUZ),
          gp_Vec(-RsinV * theUCtx.dirUX + RcosV * theData.ZX,
                 -RsinV * theUCtx.dirUY + RcosV * theData.ZY,
                 -RsinV * theUCtx.dirUZ + RcosV * theData.ZZ)};
}

//==================================================================================================

GeomGridEval::SurfD2 GeomGridEval_Sphere::computeD2(const Data&     theData,
                                                    const UContext& theUCtx,
                                                    double          theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  const double RcosV = theData.Radius * cosV;
  const double RsinV = theData.Radius * sinV;

  // D2U = R * cosV * (-DirU) = -R * cosV * DirU
  // D2V = R * (-cosV * DirU - sinV * ZDir) = -(P - Center) = -relP
  // D2UV = d/dV(D1U) = d/dV(R * cosV * dDirU) = -R * sinV * dDirU
  return {gp_Pnt(theData.CX + RcosV * theUCtx.dirUX + RsinV * theData.ZX,
                 theData.CY + RcosV * theUCtx.dirUY + RsinV * theData.ZY,
                 theData.CZ + RcosV * theUCtx.dirUZ + RsinV * theData.ZZ),
          gp_Vec(RcosV * theUCtx.dDirUX, RcosV * theUCtx.dDirUY, RcosV * theUCtx.dDirUZ),
          gp_Vec(-RsinV * theUCtx.dirUX + RcosV * theData.ZX,
                 -RsinV * theUCtx.dirUY + RcosV * theData.ZY,
                 -RsinV * theUCtx.dirUZ + RcosV * theData.ZZ),
          gp_Vec(-RcosV * theUCtx.dirUX, -RcosV * theUCtx.dirUY, -RcosV * theUCtx.dirUZ),
          gp_Vec(-RcosV * theUCtx.dirUX - RsinV * theData.ZX,
                 -RcosV * theUCtx.dirUY - RsinV * theData.ZY,
                 -RcosV * theUCtx.dirUZ - RsinV * theData.ZZ),
          gp_Vec(-RsinV * theUCtx.dDirUX, -RsinV * theUCtx.dDirUY, -RsinV * theUCtx.dDirUZ)};
}

//==================================================================================================

GeomGridEval::SurfD3 GeomGridEval_Sphere::computeD3(const Data&     theData,
                                                    const UContext& theUCtx,
                                                    double          theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  const double RcosV = theData.Radius * cosV;
  const double RsinV = theData.Radius * sinV;

  // D3U = R * cosV * dDirU = D1U
  // D3V = R * (sinV * DirU - cosV * ZDir) = -D1V
  // D3UUV = d/dV(D2U) = d/dV(-R * cosV * DirU) = R * sinV * DirU
  // D3UVV = d/dV(D2UV) = d/dV(-R * sinV * dDirU) = -R * cosV * dDirU
  return {gp_Pnt(theData.CX + RcosV * theUCtx.dirUX + RsinV * theData.ZX,
                 theData.CY + RcosV * theUCtx.dirUY + RsinV * theData.ZY,
                 theData.CZ + RcosV * theUCtx.dirUZ + RsinV * theData.ZZ),
          gp_Vec(RcosV * theUCtx.dDirUX, RcosV * theUCtx.dDirUY, RcosV * theUCtx.dDirUZ),
          gp_Vec(-RsinV * theUCtx.dirUX + RcosV * theData.ZX,
                 -RsinV * theUCtx.dirUY + RcosV * theData.ZY,
                 -RsinV * theUCtx.dirUZ + RcosV * theData.ZZ),
          gp_Vec(-RcosV * theUCtx.dirUX, -RcosV * theUCtx.dirUY, -RcosV * theUCtx.dirUZ),
          gp_Vec(-RcosV * theUCtx.dirUX - RsinV * theData.ZX,
                 -RcosV * theUCtx.dirUY - RsinV * theData.ZY,
                 -RcosV * theUCtx.dirUZ - RsinV * theData.ZZ),
          gp_Vec(-RsinV * theUCtx.dDirUX, -RsinV * theUCtx.dDirUY, -RsinV * theUCtx.dDirUZ),
          gp_Vec(-RcosV * theUCtx.dDirUX, -RcosV * theUCtx.dDirUY, -RcosV * theUCtx.dDirUZ),
          gp_Vec(RsinV * theUCtx.dirUX - RcosV * theData.ZX,
                 RsinV * theUCtx.dirUY - RcosV * theData.ZY,
                 RsinV * theUCtx.dirUZ - RcosV * theData.ZZ),
          gp_Vec(RsinV * theUCtx.dirUX, RsinV * theUCtx.dirUY, RsinV * theUCtx.dirUZ),
          gp_Vec(-RcosV * theUCtx.dDirUX, -RcosV * theUCtx.dDirUY, -RcosV * theUCtx.dDirUZ)};
}

//==================================================================================================

gp_Vec GeomGridEval_Sphere::computeDN(const Data&     theData,
                                      const UContext& theUCtx,
                                      double          theV,
                                      int             theNU,
                                      int             theNV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  // Cyclic phases for U and V (period 4)
  const int aPhaseU = theNU % 4;
  const int aPhaseV = theNV % 4;

  // U-phase coefficients: d^n/du^n of cosU and sinU
  // n=0: cosU, sinU -> DirU
  // n=1: -sinU, cosU -> dDirU
  // n=2: -cosU, -sinU -> -DirU
  // n=3: sinU, -cosU -> -dDirU
  double dirX, dirY, dirZ;
  switch (aPhaseU)
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

  // V-phase coefficients for XY term (cosV) and Z term (sinV)
  double aCoeffVXY, aCoeffVZ;
  switch (aPhaseV)
  {
    case 0:
      aCoeffVXY = cosV;
      aCoeffVZ  = sinV;
      break;
    case 1:
      aCoeffVXY = -sinV;
      aCoeffVZ  = cosV;
      break;
    case 2:
      aCoeffVXY = -cosV;
      aCoeffVZ  = -sinV;
      break;
    case 3:
      aCoeffVXY = sinV;
      aCoeffVZ  = -cosV;
      break;
    default:
      aCoeffVXY = 0.0;
      aCoeffVZ  = 0.0;
      break;
  }

  // Z contribution only if NU == 0 (no U derivative on Z term)
  double aZTermX = 0.0, aZTermY = 0.0, aZTermZ = 0.0;
  if (theNU == 0)
  {
    aZTermX = aCoeffVZ * theData.ZX;
    aZTermY = aCoeffVZ * theData.ZY;
    aZTermZ = aCoeffVZ * theData.ZZ;
  }

  return gp_Vec(theData.Radius * (aCoeffVXY * dirX + aZTermX),
                theData.Radius * (aCoeffVXY * dirY + aZTermY),
                theData.Radius * (aCoeffVXY * dirZ + aZTermZ));
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_Sphere::EvaluateGrid(
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

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Sphere::EvaluateGridD1(
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

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Sphere::EvaluateGridD2(
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

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Sphere::EvaluateGridD3(
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

NCollection_Array2<gp_Vec> GeomGridEval_Sphere::EvaluateGridDN(
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
