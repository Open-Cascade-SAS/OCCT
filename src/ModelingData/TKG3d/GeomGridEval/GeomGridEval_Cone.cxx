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

void GeomGridEval_Cone::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

NCollection_Array2<gp_Pnt> GeomGridEval_Cone::EvaluateGrid() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Extract cone data
  const gp_Cone& aCone      = myGeom->Cone();
  const gp_Pnt&  aCenter    = aCone.Location();
  const gp_Dir&  aXDir      = aCone.Position().XDirection();
  const gp_Dir&  aYDir      = aCone.Position().YDirection();
  const gp_Dir&  aZDir      = aCone.Position().Direction();
  const double   aRefRadius = aCone.RefRadius();
  const double   aSemiAngle = aCone.SemiAngle();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  const double sinAng = std::sin(aSemiAngle);
  const double cosAng = std::cos(aSemiAngle);

  // Pre-compute V-dependent values
  // K(v) = R + v * sin(Ang)
  // ZOffset(v) = v * cos(Ang)
  NCollection_Array1<double> aKv(1, aNbV);
  NCollection_Array1<double> aZOffset(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aKv.SetValue(iV, aRefRadius + v * sinAng);
    aZOffset.SetValue(iV, v * cosAng);
  }

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // DirU = cosU*XDir + sinU*YDir
    const double dirUX = cosU * aXX + sinU * aYX;
    const double dirUY = cosU * aXY + sinU * aYY;
    const double dirUZ = cosU * aXZ + sinU * aYZ;

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double K    = aKv.Value(iV);
      const double ZOff = aZOffset.Value(iV);

      // P = Center + K(v) * DirU + ZOffset(v) * ZDir
      aResult.SetValue(iU,
                       iV,
                       gp_Pnt(aCX + K * dirUX + ZOff * aZX,
                              aCY + K * dirUY + ZOff * aZY,
                              aCZ + K * dirUZ + ZOff * aZZ));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Cone::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  const gp_Cone& aCone      = myGeom->Cone();
  const gp_Pnt&  aCenter    = aCone.Location();
  const gp_Dir&  aXDir      = aCone.Position().XDirection();
  const gp_Dir&  aYDir      = aCone.Position().YDirection();
  const gp_Dir&  aZDir      = aCone.Position().Direction();
  const double   aRefRadius = aCone.RefRadius();
  const double   aSemiAngle = aCone.SemiAngle();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  const double sinAng = std::sin(aSemiAngle);
  const double cosAng = std::cos(aSemiAngle);

  // Pre-compute V-dependent values
  NCollection_Array1<double> aKv(1, aNbV);
  NCollection_Array1<double> aZOffset(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aKv.SetValue(iV, aRefRadius + v * sinAng);
    aZOffset.SetValue(iV, v * cosAng);
  }

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // DirU = cosU*XDir + sinU*YDir
    const double dirUX = cosU * aXX + sinU * aYX;
    const double dirUY = cosU * aXY + sinU * aYY;
    const double dirUZ = cosU * aXZ + sinU * aYZ;

    // DerivDirU = -sinU*XDir + cosU*YDir
    const double dDirUX = -sinU * aXX + cosU * aYX;
    const double dDirUY = -sinU * aXY + cosU * aYY;
    const double dDirUZ = -sinU * aXZ + cosU * aYZ;

    // D1V is constant for a given U: sin(Ang)*DirU + cos(Ang)*ZDir
    const double dV1 = sinAng * dirUX + cosAng * aZX;
    const double dV2 = sinAng * dirUY + cosAng * aZY;
    const double dV3 = sinAng * dirUZ + cosAng * aZZ;

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double K    = aKv.Value(iV);
      const double ZOff = aZOffset.Value(iV);

      // P
      const double pX = aCX + K * dirUX + ZOff * aZX;
      const double pY = aCY + K * dirUY + ZOff * aZY;
      const double pZ = aCZ + K * dirUZ + ZOff * aZZ;

      // D1U = K(v) * DerivDirU
      const double dU1 = K * dDirUX;
      const double dU2 = K * dDirUY;
      const double dU3 = K * dDirUZ;

      aResult.ChangeValue(iU,
                          iV) = {gp_Pnt(pX, pY, pZ), gp_Vec(dU1, dU2, dU3), gp_Vec(dV1, dV2, dV3)};
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Cone::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  const gp_Cone& aCone      = myGeom->Cone();
  const gp_Pnt&  aCenter    = aCone.Location();
  const gp_Dir&  aXDir      = aCone.Position().XDirection();
  const gp_Dir&  aYDir      = aCone.Position().YDirection();
  const gp_Dir&  aZDir      = aCone.Position().Direction();
  const double   aRefRadius = aCone.RefRadius();
  const double   aSemiAngle = aCone.SemiAngle();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  const double sinAng = std::sin(aSemiAngle);
  const double cosAng = std::cos(aSemiAngle);

  // Pre-compute V-dependent values
  NCollection_Array1<double> aKv(1, aNbV);
  NCollection_Array1<double> aZOffset(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aKv.SetValue(iV, aRefRadius + v * sinAng);
    aZOffset.SetValue(iV, v * cosAng);
  }

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // DirU = cosU*XDir + sinU*YDir
    const double dirUX = cosU * aXX + sinU * aYX;
    const double dirUY = cosU * aXY + sinU * aYY;
    const double dirUZ = cosU * aXZ + sinU * aYZ;

    // DerivDirU = -sinU*XDir + cosU*YDir
    const double dDirUX = -sinU * aXX + cosU * aYX;
    const double dDirUY = -sinU * aXY + cosU * aYY;
    const double dDirUZ = -sinU * aXZ + cosU * aYZ;

    // Deriv2DirU = -cosU*XDir - sinU*YDir = -DirU
    const double d2DirUX = -dirUX;
    const double d2DirUY = -dirUY;
    const double d2DirUZ = -dirUZ;

    // D1V is constant for a given U: sin(Ang)*DirU + cos(Ang)*ZDir
    const double dV1 = sinAng * dirUX + cosAng * aZX;
    const double dV2 = sinAng * dirUY + cosAng * aZY;
    const double dV3 = sinAng * dirUZ + cosAng * aZZ;

    // D2UV = sin(Ang) * DerivDirU
    const double d2UV1 = sinAng * dDirUX;
    const double d2UV2 = sinAng * dDirUY;
    const double d2UV3 = sinAng * dDirUZ;

    // D2V = 0
    const gp_Vec aZeroVec(0.0, 0.0, 0.0);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double K    = aKv.Value(iV);
      const double ZOff = aZOffset.Value(iV);

      // P
      const double pX = aCX + K * dirUX + ZOff * aZX;
      const double pY = aCY + K * dirUY + ZOff * aZY;
      const double pZ = aCZ + K * dirUZ + ZOff * aZZ;

      // D1U = K(v) * DerivDirU
      const double dU1 = K * dDirUX;
      const double dU2 = K * dDirUY;
      const double dU3 = K * dDirUZ;

      // D2U = K * Deriv2DirU
      const double d2U1 = K * d2DirUX;
      const double d2U2 = K * d2DirUY;
      const double d2U3 = K * d2DirUZ;

      aResult.ChangeValue(iU, iV) = {gp_Pnt(pX, pY, pZ),
                                     gp_Vec(dU1, dU2, dU3),
                                     gp_Vec(dV1, dV2, dV3),
                                     gp_Vec(d2U1, d2U2, d2U3),
                                     aZeroVec, // D2V
                                     gp_Vec(d2UV1, d2UV2, d2UV3)};
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Cone::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  const gp_Cone& aCone      = myGeom->Cone();
  const gp_Pnt&  aCenter    = aCone.Location();
  const gp_Dir&  aXDir      = aCone.Position().XDirection();
  const gp_Dir&  aYDir      = aCone.Position().YDirection();
  const gp_Dir&  aZDir      = aCone.Position().Direction();
  const double   aRefRadius = aCone.RefRadius();
  const double   aSemiAngle = aCone.SemiAngle();

  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  const double sinAng = std::sin(aSemiAngle);
  const double cosAng = std::cos(aSemiAngle);

  const gp_Vec aZeroVec(0.0, 0.0, 0.0);

  // Pre-compute V-dependent values
  NCollection_Array1<double> aKv(1, aNbV);
  NCollection_Array1<double> aZOffset(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aKv.SetValue(iV, aRefRadius + v * sinAng);
    aZOffset.SetValue(iV, v * cosAng);
  }

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // DirU = cosU*XDir + sinU*YDir
    const double dirUX = cosU * aXX + sinU * aYX;
    const double dirUY = cosU * aXY + sinU * aYY;
    const double dirUZ = cosU * aXZ + sinU * aYZ;

    // DerivDirU = -sinU*XDir + cosU*YDir
    const double dDirUX = -sinU * aXX + cosU * aYX;
    const double dDirUY = -sinU * aXY + cosU * aYY;
    const double dDirUZ = -sinU * aXZ + cosU * aYZ;

    // D1V is constant for a given U: sin(Ang)*DirU + cos(Ang)*ZDir
    const double dV1 = sinAng * dirUX + cosAng * aZX;
    const double dV2 = sinAng * dirUY + cosAng * aZY;
    const double dV3 = sinAng * dirUZ + cosAng * aZZ;

    // D2UV = sin(Ang) * DerivDirU
    const double d2UV1 = sinAng * dDirUX;
    const double d2UV2 = sinAng * dDirUY;
    const double d2UV3 = sinAng * dDirUZ;

    // D3UUV = -sin(Ang) * DirU (derivative of D2UV w.r.t. U)
    const double d3UUV1 = -sinAng * dirUX;
    const double d3UUV2 = -sinAng * dirUY;
    const double d3UUV3 = -sinAng * dirUZ;

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double K    = aKv.Value(iV);
      const double ZOff = aZOffset.Value(iV);

      // P = Center + K * DirU + ZOff * ZDir
      const double pX = aCX + K * dirUX + ZOff * aZX;
      const double pY = aCY + K * dirUY + ZOff * aZY;
      const double pZ = aCZ + K * dirUZ + ZOff * aZZ;

      // D1U = K * DerivDirU
      const double dU1 = K * dDirUX;
      const double dU2 = K * dDirUY;
      const double dU3 = K * dDirUZ;

      // D2U = -K * DirU
      const double d2U1 = -K * dirUX;
      const double d2U2 = -K * dirUY;
      const double d2U3 = -K * dirUZ;

      // D3U = -D1U (derivative of D2U w.r.t. U = -K * DerivDirU)
      const double d3U1 = -dU1;
      const double d3U2 = -dU2;
      const double d3U3 = -dU3;

      aResult.ChangeValue(iU, iV) = {gp_Pnt(pX, pY, pZ),
                                     gp_Vec(dU1, dU2, dU3),
                                     gp_Vec(dV1, dV2, dV3),
                                     gp_Vec(d2U1, d2U2, d2U3),
                                     aZeroVec, // D2V = 0
                                     gp_Vec(d2UV1, d2UV2, d2UV3),
                                     gp_Vec(d3U1, d3U2, d3U3),
                                     aZeroVec, // D3V = 0
                                     gp_Vec(d3UUV1, d3UUV2, d3UUV3),
                                     aZeroVec}; // D3UVV = 0
    }
  }
  return aResult;
}
