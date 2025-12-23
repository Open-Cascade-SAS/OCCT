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

void GeomGridEval_Sphere::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

NCollection_Array2<gp_Pnt> GeomGridEval_Sphere::EvaluateGrid() const
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

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Sphere::EvaluateGridD1() const
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

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Sphere::EvaluateGridD2() const
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

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Sphere::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

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

      // R1 = R * cosV, R2 = R * sinV
      const double R1 = aRadius * cosV;
      const double R2 = aRadius * sinV;

      // A1 = R1 * cosU, A2 = R1 * sinU, A3 = R2 * cosU, A4 = R2 * sinU
      const double A1 = R1 * cosU;
      const double A2 = R1 * sinU;
      const double A3 = R2 * cosU;
      const double A4 = R2 * sinU;

      // Som1 = A1*X + A2*Y (R*cosV*Vxy where Vxy = cosU*X + sinU*Y)
      const double Som1X = A1 * aXX + A2 * aYX;
      const double Som1Y = A1 * aXY + A2 * aYY;
      const double Som1Z = A1 * aXZ + A2 * aYZ;

      // Som3 = A3*X + A4*Y (R*sinV*Vxy)
      const double Som3X = A3 * aXX + A4 * aYX;
      const double Som3Y = A3 * aXY + A4 * aYY;
      const double Som3Z = A3 * aXZ + A4 * aYZ;

      // Dif1 = A2*X - A1*Y = R1*sinU*X - R1*cosU*Y = -R*cosV*DVxy
      // where DVxy = -sinU*X + cosU*Y
      // So actual DVxy component is: -Dif1/R1 gives sinU*X - cosU*Y... let's use direct formula
      // D1U = R*cosV*(-sinU*X + cosU*Y) = -A2*X + A1*Y... wait that's wrong sign
      // Actually: D1U = R*cosV*DVxy where DVxy = -sinU*X + cosU*Y
      // So D1U = R1*(-sinU)*X + R1*cosU*Y = -A2*X/cosU*sinU... simpler:
      // D1U_coeff1 = R1*(-sinU), D1U_coeff2 = R1*cosU
      const double dU1 = -A2; // R1*(-sinU) = -R*cosV*sinU
      const double dU2 = A1;  // R1*cosU = R*cosV*cosU

      // D1V = R*(-sinV*cosU*X - sinV*sinU*Y + cosV*Z) = -A3*X - A4*Y + R1*Z
      const double dV1 = -A3;
      const double dV2 = -A4;
      const double dV3 = R1;

      // D2U = -R*cosV*Vxy = -Som1
      const double d2UX = -Som1X;
      const double d2UY = -Som1Y;
      const double d2UZ = -Som1Z;

      // D2V = -R*cosV*Vxy - R*sinV*Z = -Som1 - R2*Z
      const double d2VX = -Som1X - R2 * aZX;
      const double d2VY = -Som1Y - R2 * aZY;
      const double d2VZ = -Som1Z - R2 * aZZ;

      // D2UV = -R*sinV*DVxy = R*sinV*(sinU*X - cosU*Y) = A4*X - A3*Y
      const double d2UVX = A4 * aXX - A3 * aYX;
      const double d2UVY = A4 * aXY - A3 * aYY;
      const double d2UVZ = A4 * aXZ - A3 * aYZ;

      // D3U = -D1U (Vuuu = -Vu)
      const double d3UX = -dU1 * aXX - dU2 * aYX;
      const double d3UY = -dU1 * aXY - dU2 * aYY;
      const double d3UZ = -dU1 * aXZ - dU2 * aYZ;

      // D3V = -D1V (Vvvv = -Vv)
      const double d3VX = -dV1 * aXX - dV2 * aYX - dV3 * aZX;
      const double d3VY = -dV1 * aXY - dV2 * aYY - dV3 * aZY;
      const double d3VZ = -dV1 * aXZ - dV2 * aYZ - dV3 * aZZ;

      // D3UUV = R*sinV*Vxy = Som3
      const double d3UUVX = Som3X;
      const double d3UUVY = Som3Y;
      const double d3UUVZ = Som3Z;

      // D3UVV = -D1U (Vuvv = -Vu = Vuuu)
      const double d3UVVX = d3UX;
      const double d3UVVY = d3UY;
      const double d3UVVZ = d3UZ;

      aResult.ChangeValue(iU, iV) = {
        gp_Pnt(aCX + Som1X + R2 * aZX, aCY + Som1Y + R2 * aZY, aCZ + Som1Z + R2 * aZZ),
        gp_Vec(dU1 * aXX + dU2 * aYX, dU1 * aXY + dU2 * aYY, dU1 * aXZ + dU2 * aYZ),
        gp_Vec(dV1 * aXX + dV2 * aYX + dV3 * aZX,
               dV1 * aXY + dV2 * aYY + dV3 * aZY,
               dV1 * aXZ + dV2 * aYZ + dV3 * aZZ),
        gp_Vec(d2UX, d2UY, d2UZ),
        gp_Vec(d2VX, d2VY, d2VZ),
        gp_Vec(d2UVX, d2UVY, d2UVZ),
        gp_Vec(d3UX, d3UY, d3UZ),
        gp_Vec(d3VX, d3VY, d3VZ),
        gp_Vec(d3UUVX, d3UUVY, d3UUVZ),
        gp_Vec(d3UVVX, d3UVVY, d3UVVZ)};
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_Sphere::EvaluateGridDN(int theNU, int theNV) const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  // For sphere, derivatives are cyclic with period 4 in both U and V
  // P(u,v) = C + R * (cosV*cosU*X + cosV*sinU*Y + sinV*Z)
  // The derivative pattern depends on (NU % 4, NV % 4)

  const gp_Sphere& aSph    = myGeom->Sphere();
  const gp_Dir&    aXDir   = aSph.Position().XDirection();
  const gp_Dir&    aYDir   = aSph.Position().YDirection();
  const gp_Dir&    aZDir   = aSph.Position().Direction();
  const double     aRadius = aSph.Radius();

  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();
  const double aZX = aZDir.X();
  const double aZY = aZDir.Y();
  const double aZZ = aZDir.Z();

  // Cyclic phases for U and V (period 4)
  // Phase 0: cos, Phase 1: -sin, Phase 2: -cos, Phase 3: sin
  const int aPhaseU = theNU % 4;
  const int aPhaseV = theNV % 4;

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

    // U-phase coefficients for X and Y directions
    double aCoeffUX, aCoeffUY;
    switch (aPhaseU)
    {
      case 0:
        aCoeffUX = cosU;
        aCoeffUY = sinU;
        break;
      case 1:
        aCoeffUX = -sinU;
        aCoeffUY = cosU;
        break;
      case 2:
        aCoeffUX = -cosU;
        aCoeffUY = -sinU;
        break;
      case 3:
        aCoeffUX = sinU;
        aCoeffUY = -cosU;
        break;
      default:
        aCoeffUX = 0.0;
        aCoeffUY = 0.0;
        break;
    }

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      // V-phase coefficients: determines contribution of XY-plane vs Z-axis
      // d^nv/dv^nv (cosV * XYterm + sinV * Z)
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

      // Combined coefficient: R * VCoeff_XY * (UCoeffX * X + UCoeffY * Y) + R * VCoeff_Z * Z
      // But Z only appears in the V-derivative chain, not U
      // For pure U derivatives (NV=0): Z term is sinV (phase 0 for V)
      // For mixed derivatives: need both components

      const double aXYTerm  = aCoeffVXY * (aCoeffUX * aXX + aCoeffUY * aYX);
      const double aXYTermY = aCoeffVXY * (aCoeffUX * aXY + aCoeffUY * aYY);
      const double aXYTermZ = aCoeffVXY * (aCoeffUX * aXZ + aCoeffUY * aYZ);

      // Z contribution only if NU == 0 (no U derivative on Z term)
      double aZTerm = 0.0, aZTermY = 0.0, aZTermZ = 0.0;
      if (theNU == 0)
      {
        aZTerm  = aCoeffVZ * aZX;
        aZTermY = aCoeffVZ * aZY;
        aZTermZ = aCoeffVZ * aZZ;
      }

      aResult.SetValue(iU,
                       iV,
                       gp_Vec(aRadius * (aXYTerm + aZTerm),
                              aRadius * (aXYTermY + aZTermY),
                              aRadius * (aXYTermZ + aZTermZ)));
    }
  }
  return aResult;
}
