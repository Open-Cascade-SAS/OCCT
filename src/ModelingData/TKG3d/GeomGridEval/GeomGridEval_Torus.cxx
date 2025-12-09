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

#include <GeomGridEval_Torus.hxx>

#include <gp_Torus.hxx>

#include <cmath>

//==================================================================================================

void GeomGridEval_Torus::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

NCollection_Array2<gp_Pnt> GeomGridEval_Torus::EvaluateGrid() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Extract torus data
  const gp_Torus& aTorus       = myGeom->Torus();
  const gp_Pnt&   aCenter      = aTorus.Location();
  const gp_Dir&   aXDir        = aTorus.Position().XDirection();
  const gp_Dir&   aYDir        = aTorus.Position().YDirection();
  const gp_Dir&   aZDir        = aTorus.Position().Direction();
  const double    aMajorRadius = aTorus.MajorRadius();
  const double    aMinorRadius = aTorus.MinorRadius();

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

  // Pre-compute V-dependent values (sin/cos of minor angle)
  NCollection_Array1<double> aCosV(1, aNbV);
  NCollection_Array1<double> aSinV(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aCosV.SetValue(iV, std::cos(v));
    aSinV.SetValue(iV, std::sin(v));
  }

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // Common term for X and Y components based on U
    const double aDirUX = cosU * aXX + sinU * aYX;
    const double aDirUY = cosU * aXY + sinU * aYY;
    const double aDirUZ = cosU * aXZ + sinU * aYZ;

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      // K = MajorRadius + MinorRadius * cos(v)
      const double K = aMajorRadius + aMinorRadius * cosV;

      // P = Center + K * (cosU*XDir + sinU*YDir) + MinorRadius * sinV * ZDir
      aResult.SetValue(iU,
                       iV,
                       gp_Pnt(aCX + K * aDirUX + aMinorRadius * sinV * aZX,
                              aCY + K * aDirUY + aMinorRadius * sinV * aZY,
                              aCZ + K * aDirUZ + aMinorRadius * sinV * aZZ));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Torus::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  const gp_Torus& aTorus       = myGeom->Torus();
  const gp_Pnt&   aCenter      = aTorus.Location();
  const gp_Dir&   aXDir        = aTorus.Position().XDirection();
  const gp_Dir&   aYDir        = aTorus.Position().YDirection();
  const gp_Dir&   aZDir        = aTorus.Position().Direction();
  const double    aMajorRadius = aTorus.MajorRadius();
  const double    aMinorRadius = aTorus.MinorRadius();

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

  NCollection_Array1<double> aCosV(1, aNbV);
  NCollection_Array1<double> aSinV(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aCosV.SetValue(iV, std::cos(v));
    aSinV.SetValue(iV, std::sin(v));
  }

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    // Direction vectors for U
    // DirU = cosU*XDir + sinU*YDir
    const double dirUX = cosU * aXX + sinU * aYX;
    const double dirUY = cosU * aXY + sinU * aYY;
    const double dirUZ = cosU * aXZ + sinU * aYZ;

    // DerivDirU = -sinU*XDir + cosU*YDir
    const double dDirUX = -sinU * aXX + cosU * aYX;
    const double dDirUY = -sinU * aXY + cosU * aYY;
    const double dDirUZ = -sinU * aXZ + cosU * aYZ;

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      const double K = aMajorRadius + aMinorRadius * cosV;

      // P = Center + K * DirU + r * sinV * ZDir
      const double pX = aCX + K * dirUX + aMinorRadius * sinV * aZX;
      const double pY = aCY + K * dirUY + aMinorRadius * sinV * aZY;
      const double pZ = aCZ + K * dirUZ + aMinorRadius * sinV * aZZ;

      // D1U = K * DerivDirU
      const double dU1 = K * dDirUX;
      const double dU2 = K * dDirUY;
      const double dU3 = K * dDirUZ;

      // D1V = -r * sinV * DirU + r * cosV * ZDir
      const double dV1 = -aMinorRadius * sinV * dirUX + aMinorRadius * cosV * aZX;
      const double dV2 = -aMinorRadius * sinV * dirUY + aMinorRadius * cosV * aZY;
      const double dV3 = -aMinorRadius * sinV * dirUZ + aMinorRadius * cosV * aZZ;

      aResult.ChangeValue(iU,
                          iV) = {gp_Pnt(pX, pY, pZ), gp_Vec(dU1, dU2, dU3), gp_Vec(dV1, dV2, dV3)};
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Torus::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  const gp_Torus& aTorus       = myGeom->Torus();
  const gp_Pnt&   aCenter      = aTorus.Location();
  const gp_Dir&   aXDir        = aTorus.Position().XDirection();
  const gp_Dir&   aYDir        = aTorus.Position().YDirection();
  const gp_Dir&   aZDir        = aTorus.Position().Direction();
  const double    aMajorRadius = aTorus.MajorRadius();
  const double    aMinorRadius = aTorus.MinorRadius();

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

  NCollection_Array1<double> aCosV(1, aNbV);
  NCollection_Array1<double> aSinV(1, aNbV);
  for (int iV = 1; iV <= aNbV; ++iV)
  {
    const double v = myVParams.Value(iV);
    aCosV.SetValue(iV, std::cos(v));
    aSinV.SetValue(iV, std::sin(v));
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

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      const double K = aMajorRadius + aMinorRadius * cosV;

      // P
      const double pX = aCX + K * dirUX + aMinorRadius * sinV * aZX;
      const double pY = aCY + K * dirUY + aMinorRadius * sinV * aZY;
      const double pZ = aCZ + K * dirUZ + aMinorRadius * sinV * aZZ;

      // D1U = K * DerivDirU
      const double dU1 = K * dDirUX;
      const double dU2 = K * dDirUY;
      const double dU3 = K * dDirUZ;

      // D1V = -r * sinV * DirU + r * cosV * ZDir
      const double dV1 = -aMinorRadius * sinV * dirUX + aMinorRadius * cosV * aZX;
      const double dV2 = -aMinorRadius * sinV * dirUY + aMinorRadius * cosV * aZY;
      const double dV3 = -aMinorRadius * sinV * dirUZ + aMinorRadius * cosV * aZZ;

      // D2U = K * Deriv2DirU = -K * DirU
      const double d2U1 = K * d2DirUX;
      const double d2U2 = K * d2DirUY;
      const double d2U3 = K * d2DirUZ;

      // D2V = -r * cosV * DirU - r * sinV * ZDir
      const double d2V1 = -aMinorRadius * cosV * dirUX - aMinorRadius * sinV * aZX;
      const double d2V2 = -aMinorRadius * cosV * dirUY - aMinorRadius * sinV * aZY;
      const double d2V3 = -aMinorRadius * cosV * dirUZ - aMinorRadius * sinV * aZZ;

      // D2UV = -r * sinV * DerivDirU
      const double d2UV1 = -aMinorRadius * sinV * dDirUX;
      const double d2UV2 = -aMinorRadius * sinV * dDirUY;
      const double d2UV3 = -aMinorRadius * sinV * dDirUZ;

      aResult.ChangeValue(iU, iV) = {gp_Pnt(pX, pY, pZ),
                                     gp_Vec(dU1, dU2, dU3),
                                     gp_Vec(dV1, dV2, dV3),
                                     gp_Vec(d2U1, d2U2, d2U3),
                                     gp_Vec(d2V1, d2V2, d2V3),
                                     gp_Vec(d2UV1, d2UV2, d2UV3)};
    }
  }
  return aResult;
}
