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

GeomGridEval_Torus::Data GeomGridEval_Torus::extractData() const
{
  const gp_Torus& aTorus  = myGeom->Torus();
  const gp_Pnt&   aCenter = aTorus.Location();
  const gp_Dir&   aXDir   = aTorus.Position().XDirection();
  const gp_Dir&   aYDir   = aTorus.Position().YDirection();
  const gp_Dir&   aZDir   = aTorus.Position().Direction();

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
          aTorus.MajorRadius(),
          aTorus.MinorRadius()};
}

//==================================================================================================

GeomGridEval_Torus::UContext GeomGridEval_Torus::computeUContext(const Data& theData, double theU)
{
  const double cosU = std::cos(theU);
  const double sinU = std::sin(theU);

  return {cosU,
          sinU,
          cosU * theData.XX + sinU * theData.YX,   // dirUX
          cosU * theData.XY + sinU * theData.YY,   // dirUY
          cosU * theData.XZ + sinU * theData.YZ,   // dirUZ
          -sinU * theData.XX + cosU * theData.YX,  // dDirUX
          -sinU * theData.XY + cosU * theData.YY,  // dDirUY
          -sinU * theData.XZ + cosU * theData.YZ}; // dDirUZ
}

//==================================================================================================

gp_Pnt GeomGridEval_Torus::computeD0(const Data& theData, const UContext& theUCtx, double theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  // K = MajorRadius + MinorRadius * cos(v)
  const double K = theData.MajorRadius + theData.MinorRadius * cosV;

  // P = Center + K * DirU + MinorRadius * sinV * ZDir
  return gp_Pnt(theData.CX + K * theUCtx.dirUX + theData.MinorRadius * sinV * theData.ZX,
                theData.CY + K * theUCtx.dirUY + theData.MinorRadius * sinV * theData.ZY,
                theData.CZ + K * theUCtx.dirUZ + theData.MinorRadius * sinV * theData.ZZ);
}

//==================================================================================================

GeomGridEval::SurfD1 GeomGridEval_Torus::computeD1(const Data&     theData,
                                                   const UContext& theUCtx,
                                                   double          theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  const double K = theData.MajorRadius + theData.MinorRadius * cosV;

  // D1U = K * DerivDirU
  const double dU1 = K * theUCtx.dDirUX;
  const double dU2 = K * theUCtx.dDirUY;
  const double dU3 = K * theUCtx.dDirUZ;

  // D1V = -r * sinV * DirU + r * cosV * ZDir
  const double dV1 =
    -theData.MinorRadius * sinV * theUCtx.dirUX + theData.MinorRadius * cosV * theData.ZX;
  const double dV2 =
    -theData.MinorRadius * sinV * theUCtx.dirUY + theData.MinorRadius * cosV * theData.ZY;
  const double dV3 =
    -theData.MinorRadius * sinV * theUCtx.dirUZ + theData.MinorRadius * cosV * theData.ZZ;

  return {gp_Pnt(theData.CX + K * theUCtx.dirUX + theData.MinorRadius * sinV * theData.ZX,
                 theData.CY + K * theUCtx.dirUY + theData.MinorRadius * sinV * theData.ZY,
                 theData.CZ + K * theUCtx.dirUZ + theData.MinorRadius * sinV * theData.ZZ),
          gp_Vec(dU1, dU2, dU3),
          gp_Vec(dV1, dV2, dV3)};
}

//==================================================================================================

GeomGridEval::SurfD2 GeomGridEval_Torus::computeD2(const Data&     theData,
                                                   const UContext& theUCtx,
                                                   double          theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  const double K = theData.MajorRadius + theData.MinorRadius * cosV;

  // D1U = K * DerivDirU
  const double dU1 = K * theUCtx.dDirUX;
  const double dU2 = K * theUCtx.dDirUY;
  const double dU3 = K * theUCtx.dDirUZ;

  // D1V = -r * sinV * DirU + r * cosV * ZDir
  const double dV1 =
    -theData.MinorRadius * sinV * theUCtx.dirUX + theData.MinorRadius * cosV * theData.ZX;
  const double dV2 =
    -theData.MinorRadius * sinV * theUCtx.dirUY + theData.MinorRadius * cosV * theData.ZY;
  const double dV3 =
    -theData.MinorRadius * sinV * theUCtx.dirUZ + theData.MinorRadius * cosV * theData.ZZ;

  // D2U = K * Deriv2DirU = -K * DirU
  const double d2U1 = -K * theUCtx.dirUX;
  const double d2U2 = -K * theUCtx.dirUY;
  const double d2U3 = -K * theUCtx.dirUZ;

  // D2V = -r * cosV * DirU - r * sinV * ZDir
  const double d2V1 =
    -theData.MinorRadius * cosV * theUCtx.dirUX - theData.MinorRadius * sinV * theData.ZX;
  const double d2V2 =
    -theData.MinorRadius * cosV * theUCtx.dirUY - theData.MinorRadius * sinV * theData.ZY;
  const double d2V3 =
    -theData.MinorRadius * cosV * theUCtx.dirUZ - theData.MinorRadius * sinV * theData.ZZ;

  // D2UV = -r * sinV * DerivDirU
  const double d2UV1 = -theData.MinorRadius * sinV * theUCtx.dDirUX;
  const double d2UV2 = -theData.MinorRadius * sinV * theUCtx.dDirUY;
  const double d2UV3 = -theData.MinorRadius * sinV * theUCtx.dDirUZ;

  return {gp_Pnt(theData.CX + K * theUCtx.dirUX + theData.MinorRadius * sinV * theData.ZX,
                 theData.CY + K * theUCtx.dirUY + theData.MinorRadius * sinV * theData.ZY,
                 theData.CZ + K * theUCtx.dirUZ + theData.MinorRadius * sinV * theData.ZZ),
          gp_Vec(dU1, dU2, dU3),
          gp_Vec(dV1, dV2, dV3),
          gp_Vec(d2U1, d2U2, d2U3),
          gp_Vec(d2V1, d2V2, d2V3),
          gp_Vec(d2UV1, d2UV2, d2UV3)};
}

//==================================================================================================

GeomGridEval::SurfD3 GeomGridEval_Torus::computeD3(const Data&     theData,
                                                   const UContext& theUCtx,
                                                   double          theV)
{
  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  // R1 = r * cosV, R2 = r * sinV
  const double R1 = theData.MinorRadius * cosV;
  const double R2 = theData.MinorRadius * sinV;

  // K = R + r * cosV
  const double K = theData.MajorRadius + R1;

  // A3 = R2 * cosU, A4 = R2 * sinU (for Som3)
  const double A3 = R2 * theUCtx.cosU;
  const double A4 = R2 * theUCtx.sinU;
  // A5 = R1 * cosU, A6 = R1 * sinU (for Vvv, Vuvv)
  const double A5 = R1 * theUCtx.cosU;
  const double A6 = R1 * theUCtx.sinU;

  // Som1 = K * DirU (for P and D2U)
  const double Som1X = K * theUCtx.dirUX;
  const double Som1Y = K * theUCtx.dirUY;
  const double Som1Z = K * theUCtx.dirUZ;

  // Som3 = R2 * (cosU*X + sinU*Y) = A3*X + A4*Y
  const double Som3X = A3 * theData.XX + A4 * theData.YX;
  const double Som3Y = A3 * theData.XY + A4 * theData.YY;
  const double Som3Z = A3 * theData.XZ + A4 * theData.YZ;

  // D1U = K * DerivDirU
  const double dU1 = K * theUCtx.dDirUX;
  const double dU2 = K * theUCtx.dDirUY;
  const double dU3 = K * theUCtx.dDirUZ;

  // D1V = -Som3 + R1*Z = -r*sinV*DirU + r*cosV*Z
  const double dV1 = -Som3X + R1 * theData.ZX;
  const double dV2 = -Som3Y + R1 * theData.ZY;
  const double dV3 = -Som3Z + R1 * theData.ZZ;

  // D2U = -Som1 = -K * DirU
  const double d2U1 = -Som1X;
  const double d2U2 = -Som1Y;
  const double d2U3 = -Som1Z;

  // D2V = -R1*DirU - R2*Z = -(A5*X + A6*Y) - R2*Z
  const double d2V1 = -A5 * theData.XX - A6 * theData.YX - R2 * theData.ZX;
  const double d2V2 = -A5 * theData.XY - A6 * theData.YY - R2 * theData.ZY;
  const double d2V3 = -A5 * theData.XZ - A6 * theData.YZ - R2 * theData.ZZ;

  // D2UV = r*sinV*(sinU*X - cosU*Y) = A4*X - A3*Y
  const double d2UV1 = A4 * theData.XX - A3 * theData.YX;
  const double d2UV2 = A4 * theData.XY - A3 * theData.YY;
  const double d2UV3 = A4 * theData.XZ - A3 * theData.YZ;

  // D3U = -D1U (Vuuu = Dif1 = -DerivDirU*K)
  const double d3U1 = -dU1;
  const double d3U2 = -dU2;
  const double d3U3 = -dU3;

  // D3V = -D1V (Vvvv = Som3 - R1*Z)
  const double d3V1 = Som3X - R1 * theData.ZX;
  const double d3V2 = Som3Y - R1 * theData.ZY;
  const double d3V3 = Som3Z - R1 * theData.ZZ;

  // D3UUV = Som3 = r*sinV*(cosU*X + sinU*Y)
  const double d3UUV1 = Som3X;
  const double d3UUV2 = Som3Y;
  const double d3UUV3 = Som3Z;

  // D3UVV = r*cosV*(sinU*X - cosU*Y) = A6*X - A5*Y
  const double d3UVV1 = A6 * theData.XX - A5 * theData.YX;
  const double d3UVV2 = A6 * theData.XY - A5 * theData.YY;
  const double d3UVV3 = A6 * theData.XZ - A5 * theData.YZ;

  return {gp_Pnt(theData.CX + Som1X + R2 * theData.ZX,
                 theData.CY + Som1Y + R2 * theData.ZY,
                 theData.CZ + Som1Z + R2 * theData.ZZ),
          gp_Vec(dU1, dU2, dU3),
          gp_Vec(dV1, dV2, dV3),
          gp_Vec(d2U1, d2U2, d2U3),
          gp_Vec(d2V1, d2V2, d2V3),
          gp_Vec(d2UV1, d2UV2, d2UV3),
          gp_Vec(d3U1, d3U2, d3U3),
          gp_Vec(d3V1, d3V2, d3V3),
          gp_Vec(d3UUV1, d3UUV2, d3UUV3),
          gp_Vec(d3UVV1, d3UVV2, d3UVV3)};
}

//==================================================================================================

gp_Vec GeomGridEval_Torus::computeDN(const Data&     theData,
                                     const UContext& theUCtx,
                                     double          theV,
                                     int             theNU,
                                     int             theNV)
{
  // For torus P(u,v) = C + K(v)*DirU(u) + r*sin(v)*Z
  // where K(v) = R + r*cos(v), DirU(u) = cos(u)*X + sin(u)*Y
  //
  // Both U and V derivatives are cyclic with period 4:
  //
  // d^n(DirU)/du^n cycles: cos->-sin->-cos->sin (phase = n % 4)
  // d^n(cos(v))/dv^n cycles: cos->-sin->-cos->sin (phase = n % 4)
  // d^n(sin(v))/dv^n cycles: sin->cos->-sin->-cos (phase = n % 4)
  //
  // The partial derivative D_{nu,nv} involves:
  // - d^nv(K)/dv^nv = r * d^nv(cos(v))/dv^nv
  // - d^nu(DirU)/du^nu
  // - d^nv(r*sin(v))/dv^nv = r * d^nv(sin(v))/dv^nv (only contributes when nu == 0)

  const double cosV = std::cos(theV);
  const double sinV = std::sin(theV);

  // Phase for U derivatives (period 4)
  const int aPhaseU = theNU % 4;
  // Phase for V derivatives (period 4)
  const int aPhaseV = theNV % 4;

  // Compute d^nu(DirU)/du^nu based on phase (using pre-computed cosU, sinU from UContext)
  double dirDnX, dirDnY, dirDnZ;
  switch (aPhaseU)
  {
    case 0: // cos(u)*X + sin(u)*Y = DirU
      dirDnX = theUCtx.dirUX;
      dirDnY = theUCtx.dirUY;
      dirDnZ = theUCtx.dirUZ;
      break;
    case 1: // -sin(u)*X + cos(u)*Y = DerivDirU
      dirDnX = theUCtx.dDirUX;
      dirDnY = theUCtx.dDirUY;
      dirDnZ = theUCtx.dDirUZ;
      break;
    case 2: // -cos(u)*X - sin(u)*Y = -DirU
      dirDnX = -theUCtx.dirUX;
      dirDnY = -theUCtx.dirUY;
      dirDnZ = -theUCtx.dirUZ;
      break;
    case 3: // sin(u)*X - cos(u)*Y = -DerivDirU
    default:
      dirDnX = -theUCtx.dDirUX;
      dirDnY = -theUCtx.dDirUY;
      dirDnZ = -theUCtx.dDirUZ;
      break;
  }

  // Compute d^nv(cos(v))/dv^nv based on phase
  double dCosV;
  switch (aPhaseV)
  {
    case 0:
      dCosV = cosV;
      break;
    case 1:
      dCosV = -sinV;
      break;
    case 2:
      dCosV = -cosV;
      break;
    case 3:
    default:
      dCosV = sinV;
      break;
  }

  // Compute d^nv(sin(v))/dv^nv based on phase
  double dSinV;
  switch (aPhaseV)
  {
    case 0:
      dSinV = sinV;
      break;
    case 1:
      dSinV = cosV;
      break;
    case 2:
      dSinV = -sinV;
      break;
    case 3:
    default:
      dSinV = -cosV;
      break;
  }

  // For the general derivative D_{nu,nv}:
  // - If nv == 0: D = K(v) * d^nu(DirU) where K = R + r*cos(v)
  // - If nv >= 1 and nu == 0: D = d^nv(K)/dv^nv * DirU + r * d^nv(sin(v))/dv^nv * Z
  // - If nv >= 1 and nu >= 1: D = r * d^nv(cos(v))/dv^nv * d^nu(DirU)/du^nu

  double resX, resY, resZ;

  if (theNV == 0)
  {
    // Pure U derivative: K(v) * d^nu(DirU)/du^nu
    const double K = theData.MajorRadius + theData.MinorRadius * cosV;
    resX           = K * dirDnX;
    resY           = K * dirDnY;
    resZ           = K * dirDnZ;
  }
  else if (theNU == 0)
  {
    // Pure V derivative with nu=0: d^nv(K)/dv^nv * DirU + r * d^nv(sin(v))/dv^nv * Z
    // where d^nv(K)/dv^nv = r * d^nv(cos(v))/dv^nv
    const double dK = theData.MinorRadius * dCosV;
    resX            = dK * dirDnX + theData.MinorRadius * dSinV * theData.ZX;
    resY            = dK * dirDnY + theData.MinorRadius * dSinV * theData.ZY;
    resZ            = dK * dirDnZ + theData.MinorRadius * dSinV * theData.ZZ;
  }
  else
  {
    // Mixed derivative with both nu >= 1 and nv >= 1:
    // D = r * d^nv(cos(v))/dv^nv * d^nu(DirU)/du^nu
    const double coeff = theData.MinorRadius * dCosV;
    resX               = coeff * dirDnX;
    resY               = coeff * dirDnY;
    resZ               = coeff * dirDnZ;
  }

  return gp_Vec(resX, resY, resZ);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_Torus::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);
  const Data                 aData = extractData();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   u     = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, u);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD0(aData, aUCtx, v));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Torus::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);
  const Data                               aData = extractData();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   u     = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, u);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD1(aData, aUCtx, v));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Torus::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);
  const Data                               aData = extractData();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   u     = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, u);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD2(aData, aUCtx, v));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Torus::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myGeom.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);
  const Data                               aData = extractData();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   u     = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, u);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeD3(aData, aUCtx, v));
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_Torus::EvaluateGridDN(
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
  const Data                 aData = extractData();

  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double   u     = theUParams.Value(theUParams.Lower() + iU - 1);
    const UContext aUCtx = computeUContext(aData, u);

    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double v = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, computeDN(aData, aUCtx, v, theNU, theNV));
    }
  }
  return aResult;
}

