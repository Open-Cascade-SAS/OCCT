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

#include <Geom_Geometry.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <GeomEval_ParaboloidSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_ParaboloidSurface, Geom_ElementarySurface)

//==================================================================================================

GeomEval_ParaboloidSurface::GeomEval_ParaboloidSurface(const gp_Ax3& thePosition,
                                                       double        theFocal)
    : myFocal(theFocal)
{
  if (theFocal <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_ParaboloidSurface: focal distance must be > 0");
  }
  pos = thePosition;
}

//==================================================================================================

double GeomEval_ParaboloidSurface::Focal() const
{
  return myFocal;
}

//==================================================================================================

void GeomEval_ParaboloidSurface::SetFocal(double theFocal)
{
  if (theFocal <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_ParaboloidSurface::SetFocal: focal distance must be > 0");
  }
  myFocal = theFocal;
}

//==================================================================================================

double GeomEval_ParaboloidSurface::UReversedParameter(const double U) const
{
  return (2.0 * M_PI - U);
}

//==================================================================================================

double GeomEval_ParaboloidSurface::VReversedParameter(const double V) const
{
  return -V;
}

//==================================================================================================

void GeomEval_ParaboloidSurface::Bounds(double& U1,
                                        double& U2,
                                        double& V1,
                                        double& V2) const
{
  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
}

//==================================================================================================

bool GeomEval_ParaboloidSurface::IsUClosed() const
{
  return true;
}

//==================================================================================================

bool GeomEval_ParaboloidSurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_ParaboloidSurface::IsUPeriodic() const
{
  return true;
}

//==================================================================================================

bool GeomEval_ParaboloidSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_ParaboloidSurface::UIso(const double /*U*/) const
{
  return nullptr;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_ParaboloidSurface::VIso(const double /*V*/) const
{
  return nullptr;
}

//==================================================================================================

gp_Pnt GeomEval_ParaboloidSurface::EvalD0(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aZComp = V * V / (4.0 * myFocal);

  return gp_Pnt(anO.X() + V * aCosU * aXDir.X() + V * aSinU * aYDir.X() + aZComp * aZDir.X(),
                anO.Y() + V * aCosU * aXDir.Y() + V * aSinU * aYDir.Y() + aZComp * aZDir.Y(),
                anO.Z() + V * aCosU * aXDir.Z() + V * aSinU * aYDir.Z() + aZComp * aZDir.Z());
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_ParaboloidSurface::EvalD1(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aZComp = V * V / (4.0 * myFocal);
  const double aVOver2F = V / (2.0 * myFocal);

  Geom_Surface::ResD1 aResult;

  // P = O + V*cosU*XD + V*sinU*YD + V^2/(4F)*ZD
  aResult.Point = gp_Pnt(
    anO.X() + V * aCosU * aXDir.X() + V * aSinU * aYDir.X() + aZComp * aZDir.X(),
    anO.Y() + V * aCosU * aXDir.Y() + V * aSinU * aYDir.Y() + aZComp * aZDir.Y(),
    anO.Z() + V * aCosU * aXDir.Z() + V * aSinU * aYDir.Z() + aZComp * aZDir.Z());

  // dP/du = -V*sinU*XD + V*cosU*YD
  aResult.D1U = gp_Vec(
    -V * aSinU * aXDir.X() + V * aCosU * aYDir.X(),
    -V * aSinU * aXDir.Y() + V * aCosU * aYDir.Y(),
    -V * aSinU * aXDir.Z() + V * aCosU * aYDir.Z());

  // dP/dv = cosU*XD + sinU*YD + V/(2F)*ZD
  aResult.D1V = gp_Vec(
    aCosU * aXDir.X() + aSinU * aYDir.X() + aVOver2F * aZDir.X(),
    aCosU * aXDir.Y() + aSinU * aYDir.Y() + aVOver2F * aZDir.Y(),
    aCosU * aXDir.Z() + aSinU * aYDir.Z() + aVOver2F * aZDir.Z());

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_ParaboloidSurface::EvalD2(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aZComp = V * V / (4.0 * myFocal);
  const double aVOver2F = V / (2.0 * myFocal);
  const double aInv2F = 1.0 / (2.0 * myFocal);

  Geom_Surface::ResD2 aResult;

  // P
  aResult.Point = gp_Pnt(
    anO.X() + V * aCosU * aXDir.X() + V * aSinU * aYDir.X() + aZComp * aZDir.X(),
    anO.Y() + V * aCosU * aXDir.Y() + V * aSinU * aYDir.Y() + aZComp * aZDir.Y(),
    anO.Z() + V * aCosU * aXDir.Z() + V * aSinU * aYDir.Z() + aZComp * aZDir.Z());

  // dP/du
  aResult.D1U = gp_Vec(
    -V * aSinU * aXDir.X() + V * aCosU * aYDir.X(),
    -V * aSinU * aXDir.Y() + V * aCosU * aYDir.Y(),
    -V * aSinU * aXDir.Z() + V * aCosU * aYDir.Z());

  // dP/dv
  aResult.D1V = gp_Vec(
    aCosU * aXDir.X() + aSinU * aYDir.X() + aVOver2F * aZDir.X(),
    aCosU * aXDir.Y() + aSinU * aYDir.Y() + aVOver2F * aZDir.Y(),
    aCosU * aXDir.Z() + aSinU * aYDir.Z() + aVOver2F * aZDir.Z());

  // d2P/du2 = -V*cosU*XD - V*sinU*YD
  aResult.D2U = gp_Vec(
    -V * aCosU * aXDir.X() - V * aSinU * aYDir.X(),
    -V * aCosU * aXDir.Y() - V * aSinU * aYDir.Y(),
    -V * aCosU * aXDir.Z() - V * aSinU * aYDir.Z());

  // d2P/dv2 = 1/(2F)*ZD
  aResult.D2V = gp_Vec(
    aInv2F * aZDir.X(),
    aInv2F * aZDir.Y(),
    aInv2F * aZDir.Z());

  // d2P/dudv = -sinU*XD + cosU*YD
  aResult.D2UV = gp_Vec(
    -aSinU * aXDir.X() + aCosU * aYDir.X(),
    -aSinU * aXDir.Y() + aCosU * aYDir.Y(),
    -aSinU * aXDir.Z() + aCosU * aYDir.Z());

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_ParaboloidSurface::EvalD3(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aZComp = V * V / (4.0 * myFocal);
  const double aVOver2F = V / (2.0 * myFocal);
  const double aInv2F = 1.0 / (2.0 * myFocal);

  Geom_Surface::ResD3 aResult;

  // P
  aResult.Point = gp_Pnt(
    anO.X() + V * aCosU * aXDir.X() + V * aSinU * aYDir.X() + aZComp * aZDir.X(),
    anO.Y() + V * aCosU * aXDir.Y() + V * aSinU * aYDir.Y() + aZComp * aZDir.Y(),
    anO.Z() + V * aCosU * aXDir.Z() + V * aSinU * aYDir.Z() + aZComp * aZDir.Z());

  // dP/du
  aResult.D1U = gp_Vec(
    -V * aSinU * aXDir.X() + V * aCosU * aYDir.X(),
    -V * aSinU * aXDir.Y() + V * aCosU * aYDir.Y(),
    -V * aSinU * aXDir.Z() + V * aCosU * aYDir.Z());

  // dP/dv
  aResult.D1V = gp_Vec(
    aCosU * aXDir.X() + aSinU * aYDir.X() + aVOver2F * aZDir.X(),
    aCosU * aXDir.Y() + aSinU * aYDir.Y() + aVOver2F * aZDir.Y(),
    aCosU * aXDir.Z() + aSinU * aYDir.Z() + aVOver2F * aZDir.Z());

  // d2P/du2
  aResult.D2U = gp_Vec(
    -V * aCosU * aXDir.X() - V * aSinU * aYDir.X(),
    -V * aCosU * aXDir.Y() - V * aSinU * aYDir.Y(),
    -V * aCosU * aXDir.Z() - V * aSinU * aYDir.Z());

  // d2P/dv2
  aResult.D2V = gp_Vec(
    aInv2F * aZDir.X(),
    aInv2F * aZDir.Y(),
    aInv2F * aZDir.Z());

  // d2P/dudv
  aResult.D2UV = gp_Vec(
    -aSinU * aXDir.X() + aCosU * aYDir.X(),
    -aSinU * aXDir.Y() + aCosU * aYDir.Y(),
    -aSinU * aXDir.Z() + aCosU * aYDir.Z());

  // d3P/du3 = V*sinU*XD - V*cosU*YD
  aResult.D3U = gp_Vec(
    V * aSinU * aXDir.X() - V * aCosU * aYDir.X(),
    V * aSinU * aXDir.Y() - V * aCosU * aYDir.Y(),
    V * aSinU * aXDir.Z() - V * aCosU * aYDir.Z());

  // d3P/dv3 = 0
  aResult.D3V = gp_Vec(0.0, 0.0, 0.0);

  // d3P/du2dv = -cosU*XD - sinU*YD
  aResult.D3UUV = gp_Vec(
    -aCosU * aXDir.X() - aSinU * aYDir.X(),
    -aCosU * aXDir.Y() - aSinU * aYDir.Y(),
    -aCosU * aXDir.Z() - aSinU * aYDir.Z());

  // d3P/dudv2 = 0
  aResult.D3UVV = gp_Vec(0.0, 0.0, 0.0);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_ParaboloidSurface::EvalDN(const double U,
                                          const double V,
                                          const int    Nu,
                                          const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative(
      "GeomEval_ParaboloidSurface::EvalDN: invalid derivative order");
  }

  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  // The parametrization is:
  //   P = O + v*cos(u)*XD + v*sin(u)*YD + v^2/(4F)*ZD
  //
  // The mixed partial d^(Nu+Nv)P / du^Nu dv^Nv is computed component by component:
  //
  // XD component: d^Nu[cos(u)]/du^Nu * d^Nv[v]/dv^Nv
  //   = cos(u + Nu*PI/2) * (Nv==0 ? v : (Nv==1 ? 1 : 0))
  //
  // YD component: d^Nu[sin(u)]/du^Nu * d^Nv[v]/dv^Nv
  //   = sin(u + Nu*PI/2) * (Nv==0 ? v : (Nv==1 ? 1 : 0))
  //
  // ZD component: (Nu==0 ? 1 : 0) * d^Nv[v^2/(4F)]/dv^Nv
  //   where d^Nv[v^2/(4F)]/dv^Nv = (Nv==0 ? v^2/(4F) : (Nv==1 ? v/(2F) : (Nv==2 ? 1/(2F) : 0)))

  // XD and YD terms: derivative of v^1 with respect to v
  double aVDerivXY = 0.0;
  if (Nv == 0)
  {
    aVDerivXY = V;
  }
  else if (Nv == 1)
  {
    aVDerivXY = 1.0;
  }
  // else aVDerivXY = 0 for Nv >= 2

  // Trig derivatives: d^Nu[cos(u)]/du^Nu = cos(u + Nu*PI/2)
  const double aPhase = U + Nu * M_PI_2;
  const double aCosNu = std::cos(aPhase);
  const double aSinNu = std::sin(aPhase);

  const double aXCoeff = aCosNu * aVDerivXY;
  const double aYCoeff = aSinNu * aVDerivXY;

  // ZD term: only present when Nu == 0
  double aZCoeff = 0.0;
  if (Nu == 0)
  {
    if (Nv == 0)
    {
      aZCoeff = V * V / (4.0 * myFocal);
    }
    else if (Nv == 1)
    {
      aZCoeff = V / (2.0 * myFocal);
    }
    else if (Nv == 2)
    {
      aZCoeff = 1.0 / (2.0 * myFocal);
    }
    // else 0 for Nv >= 3
  }

  return gp_Vec(aXCoeff * aXDir.X() + aYCoeff * aYDir.X() + aZCoeff * aZDir.X(),
                aXCoeff * aXDir.Y() + aYCoeff * aYDir.Y() + aZCoeff * aZDir.Y(),
                aXCoeff * aXDir.Z() + aYCoeff * aYDir.Z() + aZCoeff * aZDir.Z());
}

//==================================================================================================

void GeomEval_ParaboloidSurface::Transform(const gp_Trsf& T)
{
  myFocal = myFocal * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_ParaboloidSurface::Copy() const
{
  return new GeomEval_ParaboloidSurface(pos, myFocal);
}

//==================================================================================================

void GeomEval_ParaboloidSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFocal)
}

//==================================================================================================

void GeomEval_ParaboloidSurface::Coefficients(double& A1,
                                              double& A2,
                                              double& A3,
                                              double& B1,
                                              double& B2,
                                              double& B3,
                                              double& C1,
                                              double& C2,
                                              double& C3,
                                              double& D) const
{
  // In the local coordinate system of the paraboloid:
  //   X_L^2 + Y_L^2 - 4*F*Z_L = 0
  //
  // Transform to global coordinates using:
  //   (X_L, Y_L, Z_L)^T = T * (x, y, z, 1)^T
  // where T is the transformation matrix from pos.

  gp_Trsf aT;
  aT.SetTransformation(pos);
  const double T11 = aT.Value(1, 1);
  const double T12 = aT.Value(1, 2);
  const double T13 = aT.Value(1, 3);
  const double T14 = aT.Value(1, 4);
  const double T21 = aT.Value(2, 1);
  const double T22 = aT.Value(2, 2);
  const double T23 = aT.Value(2, 3);
  const double T24 = aT.Value(2, 4);
  const double T31 = aT.Value(3, 1);
  const double T32 = aT.Value(3, 2);
  const double T33 = aT.Value(3, 3);
  const double T34 = aT.Value(3, 4);

  // Expanding (T11*x + T12*y + T13*z + T14)^2 + (T21*x + T22*y + T23*z + T24)^2
  //   - 4*F*(T31*x + T32*y + T33*z + T34) = 0
  // and collecting terms:
  A1 = T11 * T11 + T21 * T21;
  A2 = T12 * T12 + T22 * T22;
  A3 = T13 * T13 + T23 * T23;
  B1 = T11 * T12 + T21 * T22;
  B2 = T11 * T13 + T21 * T23;
  B3 = T12 * T13 + T22 * T23;
  C1 = T11 * T14 + T21 * T24 - 2.0 * myFocal * T31;
  C2 = T12 * T14 + T22 * T24 - 2.0 * myFocal * T32;
  C3 = T13 * T14 + T23 * T24 - 2.0 * myFocal * T33;
  D  = T14 * T14 + T24 * T24 - 4.0 * myFocal * T34;
}
