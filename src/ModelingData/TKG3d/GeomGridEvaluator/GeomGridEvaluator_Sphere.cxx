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

#include <GeomGridEvaluator_Sphere.hxx>

#include <gp_Sphere.hxx>

#include <cmath>

//==================================================================================================

void GeomGridEvaluator_Sphere::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

NCollection_Array2<gp_Pnt> GeomGridEvaluator_Sphere::EvaluateGrid() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  // Extract sphere data from geometry
  const gp_Sphere& aSph    = myGeom->Sphere();
  const gp_Pnt&    aCenter = aSph.Location();
  const gp_Dir&    aXDir   = aSph.Position().XDirection();
  const gp_Dir&    aYDir   = aSph.Position().YDirection();
  const gp_Dir&    aZDir   = aSph.Position().Direction();
  const double     aRadius = aSph.Radius();

  // Pre-extract coordinates for performance
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

  // Pre-compute V-dependent values (sin/cos of latitude)
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

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      // P = Center + R * (cosV*cosU*XDir + cosV*sinU*YDir + sinV*ZDir)
      const double coeff1 = aRadius * cosV * cosU;
      const double coeff2 = aRadius * cosV * sinU;
      const double coeff3 = aRadius * sinV;

      aResult.SetValue(iU,
                       iV,
                       gp_Pnt(aCX + coeff1 * aXX + coeff2 * aYX + coeff3 * aZX,
                              aCY + coeff1 * aXY + coeff2 * aYY + coeff3 * aZY,
                              aCZ + coeff1 * aXZ + coeff2 * aYZ + coeff3 * aZZ));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEvaluator_Sphere::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  const gp_Sphere& aSph    = myGeom->Sphere();
  const gp_Pnt&    aCenter = aSph.Location();
  const gp_Dir&    aXDir   = aSph.Position().XDirection();
  const gp_Dir&    aYDir   = aSph.Position().YDirection();
  const gp_Dir&    aZDir   = aSph.Position().Direction();
  const double     aRadius = aSph.Radius();

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

  // Pre-compute V-dependent values
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

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      // P = Center + R * (cosV*cosU*XDir + cosV*sinU*YDir + sinV*ZDir)
      const double coeff1 = aRadius * cosV * cosU;
      const double coeff2 = aRadius * cosV * sinU;
      const double coeff3 = aRadius * sinV;

      // D1U = R * cosV * (-sinU*XDir + cosU*YDir)
      const double dU1 = aRadius * cosV * (-sinU);
      const double dU2 = aRadius * cosV * cosU;

      // D1V = R * (-sinV*cosU*XDir - sinV*sinU*YDir + cosV*ZDir)
      const double dV1 = aRadius * (-sinV) * cosU;
      const double dV2 = aRadius * (-sinV) * sinU;
      const double dV3 = aRadius * cosV;

      aResult.ChangeValue(iU, iV) = {
        gp_Pnt(aCX + coeff1 * aXX + coeff2 * aYX + coeff3 * aZX,
               aCY + coeff1 * aXY + coeff2 * aYY + coeff3 * aZY,
               aCZ + coeff1 * aXZ + coeff2 * aYZ + coeff3 * aZZ),
        gp_Vec(dU1 * aXX + dU2 * aYX, dU1 * aXY + dU2 * aYY, dU1 * aXZ + dU2 * aYZ),
        gp_Vec(dV1 * aXX + dV2 * aYX + dV3 * aZX,
               dV1 * aXY + dV2 * aYY + dV3 * aZY,
               dV1 * aXZ + dV2 * aYZ + dV3 * aZZ)};
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEvaluator_Sphere::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  const gp_Sphere& aSph    = myGeom->Sphere();
  const gp_Pnt&    aCenter = aSph.Location();
  const gp_Dir&    aXDir   = aSph.Position().XDirection();
  const gp_Dir&    aYDir   = aSph.Position().YDirection();
  const gp_Dir&    aZDir   = aSph.Position().Direction();
  const double     aRadius = aSph.Radius();

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

  // Pre-compute V-dependent values
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

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      // P = Center + R * (cosV*cosU*XDir + cosV*sinU*YDir + sinV*ZDir)
      const double coeff1 = aRadius * cosV * cosU;
      const double coeff2 = aRadius * cosV * sinU;
      const double coeff3 = aRadius * sinV;

      // D1U = R * cosV * (-sinU*XDir + cosU*YDir)
      const double dU1 = aRadius * cosV * (-sinU);
      const double dU2 = aRadius * cosV * cosU;

      // D1V = R * (-sinV*cosU*XDir - sinV*sinU*YDir + cosV*ZDir)
      const double dV1 = aRadius * (-sinV) * cosU;
      const double dV2 = aRadius * (-sinV) * sinU;
      const double dV3 = aRadius * cosV;

      // D2U = R * cosV * (-cosU*XDir - sinU*YDir)
      const double d2U1 = aRadius * cosV * (-cosU);
      const double d2U2 = aRadius * cosV * (-sinU);

      // D2V = R * (-cosV*cosU*XDir - cosV*sinU*YDir - sinV*ZDir) = -P (relative to center)
      const double d2V1 = -coeff1;
      const double d2V2 = -coeff2;
      const double d2V3 = -coeff3;

      // D2UV = R * (-sinV) * (-sinU*XDir + cosU*YDir) = sinV * R * (sinU*XDir - cosU*YDir)
      const double d2UV1 = aRadius * sinV * sinU;
      const double d2UV2 = aRadius * sinV * (-cosU);

      aResult.ChangeValue(iU, iV) = {
        gp_Pnt(aCX + coeff1 * aXX + coeff2 * aYX + coeff3 * aZX,
               aCY + coeff1 * aXY + coeff2 * aYY + coeff3 * aZY,
               aCZ + coeff1 * aXZ + coeff2 * aYZ + coeff3 * aZZ),
        gp_Vec(dU1 * aXX + dU2 * aYX, dU1 * aXY + dU2 * aYY, dU1 * aXZ + dU2 * aYZ),
        gp_Vec(dV1 * aXX + dV2 * aYX + dV3 * aZX,
               dV1 * aXY + dV2 * aYY + dV3 * aZY,
               dV1 * aXZ + dV2 * aYZ + dV3 * aZZ),
        gp_Vec(d2U1 * aXX + d2U2 * aYX, d2U1 * aXY + d2U2 * aYY, d2U1 * aXZ + d2U2 * aYZ),
        gp_Vec(d2V1 * aXX + d2V2 * aYX + d2V3 * aZX,
               d2V1 * aXY + d2V2 * aYY + d2V3 * aZY,
               d2V1 * aXZ + d2V2 * aYZ + d2V3 * aZZ),
        gp_Vec(d2UV1 * aXX + d2UV2 * aYX, d2UV1 * aXY + d2UV2 * aYY, d2UV1 * aXZ + d2UV2 * aYZ)};
    }
  }
  return aResult;
}
