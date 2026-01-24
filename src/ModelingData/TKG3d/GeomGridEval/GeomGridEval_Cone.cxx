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

#include <GeomGridEval_Cone.hxx>

#include <gp_Cone.hxx>

#include <cmath>

//==================================================================================================

GeomGridEval_Cone::Data GeomGridEval_Cone::extractData() const
{
  const gp_Cone& aCone   = myGeom->Cone();
  const gp_Pnt&  aCenter = aCone.Location();
  const gp_Dir&  aXDir   = aCone.Position().XDirection();
  const gp_Dir&  aYDir   = aCone.Position().YDirection();
  const gp_Dir&  aZDir   = aCone.Position().Direction();

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
          aCone.RefRadius(),
          std::sin(aCone.SemiAngle()),
          std::cos(aCone.SemiAngle())};
}

//==================================================================================================

GeomGridEval_Cone::UContext GeomGridEval_Cone::computeUContext(const Data& theData, double theU)
{
  const double cosU = std::cos(theU);
  const double sinU = std::sin(theU);

  // DirU = cosU*XDir + sinU*YDir (radial direction at angle U)
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

gp_Pnt GeomGridEval_Cone::computeD0(const Data& theData, const UContext& theUCtx, double theV)
{
  // K(v) = RefRadius + v * sin(Ang)
  const double K = theData.RefRadius + theV * theData.SinAng;
  // ZOffset(v) = v * cos(Ang)
  const double ZOff = theV * theData.CosAng;

  // P = Center + K(v) * DirU + ZOffset(v) * ZDir
  return gp_Pnt(theData.CX + K * theUCtx.dirUX + ZOff * theData.ZX,
                theData.CY + K * theUCtx.dirUY + ZOff * theData.ZY,
                theData.CZ + K * theUCtx.dirUZ + ZOff * theData.ZZ);
}

//==================================================================================================

GeomGridEval::SurfD1 GeomGridEval_Cone::computeD1(const Data&     theData,
                                                  const UContext& theUCtx,
                                                  double          theV)
{
  const double K    = theData.RefRadius + theV * theData.SinAng;
  const double ZOff = theV * theData.CosAng;

  // D1U = K(v) * dDirU
  // D1V = sin(Ang)*DirU + cos(Ang)*ZDir
  return {gp_Pnt(theData.CX + K * theUCtx.dirUX + ZOff * theData.ZX,
                 theData.CY + K * theUCtx.dirUY + ZOff * theData.ZY,
                 theData.CZ + K * theUCtx.dirUZ + ZOff * theData.ZZ),
          gp_Vec(K * theUCtx.dDirUX, K * theUCtx.dDirUY, K * theUCtx.dDirUZ),
          gp_Vec(theData.SinAng * theUCtx.dirUX + theData.CosAng * theData.ZX,
                 theData.SinAng * theUCtx.dirUY + theData.CosAng * theData.ZY,
                 theData.SinAng * theUCtx.dirUZ + theData.CosAng * theData.ZZ)};
}

//==================================================================================================

GeomGridEval::SurfD2 GeomGridEval_Cone::computeD2(const Data&     theData,
                                                  const UContext& theUCtx,
                                                  double          theV)
{
  const double K    = theData.RefRadius + theV * theData.SinAng;
  const double ZOff = theV * theData.CosAng;

  // D2U = K * (-DirU) = -K * DirU
  // D2V = 0
  // D2UV = sin(Ang) * dDirU
  const gp_Vec aZero(0.0, 0.0, 0.0);

  return {gp_Pnt(theData.CX + K * theUCtx.dirUX + ZOff * theData.ZX,
                 theData.CY + K * theUCtx.dirUY + ZOff * theData.ZY,
                 theData.CZ + K * theUCtx.dirUZ + ZOff * theData.ZZ),
          gp_Vec(K * theUCtx.dDirUX, K * theUCtx.dDirUY, K * theUCtx.dDirUZ),
          gp_Vec(theData.SinAng * theUCtx.dirUX + theData.CosAng * theData.ZX,
                 theData.SinAng * theUCtx.dirUY + theData.CosAng * theData.ZY,
                 theData.SinAng * theUCtx.dirUZ + theData.CosAng * theData.ZZ),
          gp_Vec(-K * theUCtx.dirUX, -K * theUCtx.dirUY, -K * theUCtx.dirUZ),
          aZero,
          gp_Vec(theData.SinAng * theUCtx.dDirUX,
                 theData.SinAng * theUCtx.dDirUY,
                 theData.SinAng * theUCtx.dDirUZ)};
}

//==================================================================================================

GeomGridEval::SurfD3 GeomGridEval_Cone::computeD3(const Data&     theData,
                                                  const UContext& theUCtx,
                                                  double          theV)
{
  const double K    = theData.RefRadius + theV * theData.SinAng;
  const double ZOff = theV * theData.CosAng;

  // D3U = -K * dDirU = -D1U
  // D3V = 0
  // D3UUV = -sin(Ang) * DirU
  // D3UVV = 0
  const gp_Vec aZero(0.0, 0.0, 0.0);

  return {gp_Pnt(theData.CX + K * theUCtx.dirUX + ZOff * theData.ZX,
                 theData.CY + K * theUCtx.dirUY + ZOff * theData.ZY,
                 theData.CZ + K * theUCtx.dirUZ + ZOff * theData.ZZ),
          gp_Vec(K * theUCtx.dDirUX, K * theUCtx.dDirUY, K * theUCtx.dDirUZ),
          gp_Vec(theData.SinAng * theUCtx.dirUX + theData.CosAng * theData.ZX,
                 theData.SinAng * theUCtx.dirUY + theData.CosAng * theData.ZY,
                 theData.SinAng * theUCtx.dirUZ + theData.CosAng * theData.ZZ),
          gp_Vec(-K * theUCtx.dirUX, -K * theUCtx.dirUY, -K * theUCtx.dirUZ),
          aZero,
          gp_Vec(theData.SinAng * theUCtx.dDirUX,
                 theData.SinAng * theUCtx.dDirUY,
                 theData.SinAng * theUCtx.dDirUZ),
          gp_Vec(-K * theUCtx.dDirUX, -K * theUCtx.dDirUY, -K * theUCtx.dDirUZ),
          aZero,
          gp_Vec(-theData.SinAng * theUCtx.dirUX,
                 -theData.SinAng * theUCtx.dirUY,
                 -theData.SinAng * theUCtx.dirUZ),
          aZero};
}

//==================================================================================================

gp_Vec GeomGridEval_Cone::computeDN(const Data&     theData,
                                    const UContext& theUCtx,
                                    double          theV,
                                    int             theNU,
                                    int             theNV)
{
  // For cone P(u,v) = C + K(v) * DirU(u) + v*cos(Ang)*Z
  // where K(v) = RefRadius + v*sin(Ang), DirU(u) = cos(u)*X + sin(u)*Y
  //
  // V is linear, so:
  //   - All derivatives with NV >= 2 are zero
  //   - D_{0,1} = sin(Ang)*DirU + cos(Ang)*Z
  //   - D_{nu,1} = sin(Ang) * d^nu(DirU)/du^nu for nu >= 1
  //   - D_{nu,0} = K(v) * d^nu(DirU)/du^nu

  if (theNV > 1)
  {
    return gp_Vec(0.0, 0.0, 0.0);
  }

  // DirU derivatives cycle with period 4:
  //   phase 0: DirU
  //   phase 1: dDirU
  //   phase 2: -DirU
  //   phase 3: -dDirU
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

  if (theNV == 1)
  {
    if (theNU == 0)
    {
      // D_{0,1} = sin(Ang)*DirU + cos(Ang)*Z
      return gp_Vec(theData.SinAng * theUCtx.dirUX + theData.CosAng * theData.ZX,
                    theData.SinAng * theUCtx.dirUY + theData.CosAng * theData.ZY,
                    theData.SinAng * theUCtx.dirUZ + theData.CosAng * theData.ZZ);
    }

    // D_{nu,1} = sin(Ang) * d^nu(DirU)/du^nu for nu >= 1
    return gp_Vec(theData.SinAng * dirX, theData.SinAng * dirY, theData.SinAng * dirZ);
  }

  // Pure U derivative (theNV == 0, theNU >= 1)
  // D_{nu,0} = K(v) * d^nu(DirU)/du^nu
  const double K = theData.RefRadius + theV * theData.SinAng;
  return gp_Vec(K * dirX, K * dirY, K * dirZ);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_Cone::EvaluateGrid(
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

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Cone::EvaluateGridD1(
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

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Cone::EvaluateGridD2(
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

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Cone::EvaluateGridD3(
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

NCollection_Array2<gp_Vec> GeomGridEval_Cone::EvaluateGridDN(
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

