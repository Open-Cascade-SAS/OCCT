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
#include <GeomEval_HypParaboloidSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_HypParaboloidSurface, Geom_ElementarySurface)

//==================================================================================================

GeomEval_HypParaboloidSurface::GeomEval_HypParaboloidSurface(const gp_Ax3& thePosition,
                                                              double        theA,
                                                              double        theB)
    : myA(theA),
      myB(theB)
{
  if (theA <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_HypParaboloidSurface: semi-axis A must be > 0");
  }
  if (theB <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_HypParaboloidSurface: semi-axis B must be > 0");
  }
  pos = thePosition;
}

//==================================================================================================

double GeomEval_HypParaboloidSurface::SemiAxisA() const
{
  return myA;
}

//==================================================================================================

double GeomEval_HypParaboloidSurface::SemiAxisB() const
{
  return myB;
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::SetSemiAxisA(double theA)
{
  if (theA <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_HypParaboloidSurface::SetSemiAxisA: semi-axis A must be > 0");
  }
  myA = theA;
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::SetSemiAxisB(double theB)
{
  if (theB <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_HypParaboloidSurface::SetSemiAxisB: semi-axis B must be > 0");
  }
  myB = theB;
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::UReverse()
{
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::UReverse");
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::VReverse()
{
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::VReverse");
}

//==================================================================================================

double GeomEval_HypParaboloidSurface::UReversedParameter(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::UReversedParameter");
}

//==================================================================================================

double GeomEval_HypParaboloidSurface::VReversedParameter(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::VReversedParameter");
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::Bounds(double& U1,
                                            double& U2,
                                            double& V1,
                                            double& V2) const
{
  U1 = -Precision::Infinite();
  U2 = Precision::Infinite();
  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
}

//==================================================================================================

bool GeomEval_HypParaboloidSurface::IsUClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_HypParaboloidSurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_HypParaboloidSurface::IsUPeriodic() const
{
  return false;
}

//==================================================================================================

bool GeomEval_HypParaboloidSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_HypParaboloidSurface::UIso(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::UIso");
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_HypParaboloidSurface::VIso(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::VIso");
}

//==================================================================================================

gp_Pnt GeomEval_HypParaboloidSurface::EvalD0(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aA2 = myA * myA;
  const double aB2 = myB * myB;
  const double aZComp = U * U / aA2 - V * V / aB2;

  return gp_Pnt(anO.X() + U * aXDir.X() + V * aYDir.X() + aZComp * aZDir.X(),
                anO.Y() + U * aXDir.Y() + V * aYDir.Y() + aZComp * aZDir.Y(),
                anO.Z() + U * aXDir.Z() + V * aYDir.Z() + aZComp * aZDir.Z());
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_HypParaboloidSurface::EvalD1(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aA2 = myA * myA;
  const double aB2 = myB * myB;
  const double aZComp = U * U / aA2 - V * V / aB2;
  const double aZdU = 2.0 * U / aA2;
  const double aZdV = -2.0 * V / aB2;

  Geom_Surface::ResD1 aResult;

  // P = O + U*XD + V*YD + (U^2/A^2 - V^2/B^2)*ZD
  aResult.Point = gp_Pnt(
    anO.X() + U * aXDir.X() + V * aYDir.X() + aZComp * aZDir.X(),
    anO.Y() + U * aXDir.Y() + V * aYDir.Y() + aZComp * aZDir.Y(),
    anO.Z() + U * aXDir.Z() + V * aYDir.Z() + aZComp * aZDir.Z());

  // dP/du = XD + (2U/A^2)*ZD
  aResult.D1U = gp_Vec(
    aXDir.X() + aZdU * aZDir.X(),
    aXDir.Y() + aZdU * aZDir.Y(),
    aXDir.Z() + aZdU * aZDir.Z());

  // dP/dv = YD - (2V/B^2)*ZD
  aResult.D1V = gp_Vec(
    aYDir.X() + aZdV * aZDir.X(),
    aYDir.Y() + aZdV * aZDir.Y(),
    aYDir.Z() + aZdV * aZDir.Z());

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_HypParaboloidSurface::EvalD2(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aA2 = myA * myA;
  const double aB2 = myB * myB;
  const double aZComp = U * U / aA2 - V * V / aB2;
  const double aZdU = 2.0 * U / aA2;
  const double aZdV = -2.0 * V / aB2;
  const double aZd2U = 2.0 / aA2;
  const double aZd2V = -2.0 / aB2;

  Geom_Surface::ResD2 aResult;

  // P
  aResult.Point = gp_Pnt(
    anO.X() + U * aXDir.X() + V * aYDir.X() + aZComp * aZDir.X(),
    anO.Y() + U * aXDir.Y() + V * aYDir.Y() + aZComp * aZDir.Y(),
    anO.Z() + U * aXDir.Z() + V * aYDir.Z() + aZComp * aZDir.Z());

  // dP/du = XD + (2U/A^2)*ZD
  aResult.D1U = gp_Vec(
    aXDir.X() + aZdU * aZDir.X(),
    aXDir.Y() + aZdU * aZDir.Y(),
    aXDir.Z() + aZdU * aZDir.Z());

  // dP/dv = YD - (2V/B^2)*ZD
  aResult.D1V = gp_Vec(
    aYDir.X() + aZdV * aZDir.X(),
    aYDir.Y() + aZdV * aZDir.Y(),
    aYDir.Z() + aZdV * aZDir.Z());

  // d2P/du2 = (2/A^2)*ZD
  aResult.D2U = gp_Vec(
    aZd2U * aZDir.X(),
    aZd2U * aZDir.Y(),
    aZd2U * aZDir.Z());

  // d2P/dv2 = -(2/B^2)*ZD
  aResult.D2V = gp_Vec(
    aZd2V * aZDir.X(),
    aZd2V * aZDir.Y(),
    aZd2V * aZDir.Z());

  // d2P/dudv = 0
  aResult.D2UV = gp_Vec(0.0, 0.0, 0.0);

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_HypParaboloidSurface::EvalD3(const double U, const double V) const
{
  const gp_Pnt& anO   = pos.Location();
  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aA2 = myA * myA;
  const double aB2 = myB * myB;
  const double aZComp = U * U / aA2 - V * V / aB2;
  const double aZdU = 2.0 * U / aA2;
  const double aZdV = -2.0 * V / aB2;
  const double aZd2U = 2.0 / aA2;
  const double aZd2V = -2.0 / aB2;

  Geom_Surface::ResD3 aResult;

  // P
  aResult.Point = gp_Pnt(
    anO.X() + U * aXDir.X() + V * aYDir.X() + aZComp * aZDir.X(),
    anO.Y() + U * aXDir.Y() + V * aYDir.Y() + aZComp * aZDir.Y(),
    anO.Z() + U * aXDir.Z() + V * aYDir.Z() + aZComp * aZDir.Z());

  // dP/du
  aResult.D1U = gp_Vec(
    aXDir.X() + aZdU * aZDir.X(),
    aXDir.Y() + aZdU * aZDir.Y(),
    aXDir.Z() + aZdU * aZDir.Z());

  // dP/dv
  aResult.D1V = gp_Vec(
    aYDir.X() + aZdV * aZDir.X(),
    aYDir.Y() + aZdV * aZDir.Y(),
    aYDir.Z() + aZdV * aZDir.Z());

  // d2P/du2 = (2/A^2)*ZD
  aResult.D2U = gp_Vec(
    aZd2U * aZDir.X(),
    aZd2U * aZDir.Y(),
    aZd2U * aZDir.Z());

  // d2P/dv2 = -(2/B^2)*ZD
  aResult.D2V = gp_Vec(
    aZd2V * aZDir.X(),
    aZd2V * aZDir.Y(),
    aZd2V * aZDir.Z());

  // d2P/dudv = 0
  aResult.D2UV = gp_Vec(0.0, 0.0, 0.0);

  // All 3rd derivatives are zero (polynomial of degree 2)
  aResult.D3U   = gp_Vec(0.0, 0.0, 0.0);
  aResult.D3V   = gp_Vec(0.0, 0.0, 0.0);
  aResult.D3UUV = gp_Vec(0.0, 0.0, 0.0);
  aResult.D3UVV = gp_Vec(0.0, 0.0, 0.0);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_HypParaboloidSurface::EvalDN(const double U,
                                              const double V,
                                              const int    Nu,
                                              const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative(
      "GeomEval_HypParaboloidSurface::EvalDN: invalid derivative order");
  }

  const gp_Dir& aXDir = pos.XDirection();
  const gp_Dir& aYDir = pos.YDirection();
  const gp_Dir& aZDir = pos.Direction();

  const double aA2 = myA * myA;
  const double aB2 = myB * myB;

  // The parametrization is:
  //   P = O + u*XD + v*YD + (u^2/A^2 - v^2/B^2)*ZD
  //
  // XD coeff = d^Nu[u]/du^Nu * (Nv==0 ? 1 : 0)
  //   = (Nu==0 ? u : (Nu==1 ? 1 : 0)) * (Nv==0 ? 1 : 0)
  //
  // YD coeff = (Nu==0 ? 1 : 0) * d^Nv[v]/dv^Nv
  //   = (Nu==0 ? 1 : 0) * (Nv==0 ? v : (Nv==1 ? 1 : 0))
  //
  // ZD coeff = (Nv==0 ? d^Nu[u^2/A^2] : 0) + (Nu==0 ? d^Nv[-v^2/B^2] : 0)

  const double aXCoeff = (Nv == 0 ? (Nu == 0 ? U : (Nu == 1 ? 1.0 : 0.0)) : 0.0);
  const double aYCoeff = (Nu == 0 ? (Nv == 0 ? V : (Nv == 1 ? 1.0 : 0.0)) : 0.0);

  // ZD coefficient: sum of pure-u and pure-v parts
  double aZCoeff = 0.0;

  // d^Nu[u^2/A^2] contribution (only when Nv == 0)
  if (Nv == 0)
  {
    if (Nu == 0)
    {
      aZCoeff += U * U / aA2;
    }
    else if (Nu == 1)
    {
      aZCoeff += 2.0 * U / aA2;
    }
    else if (Nu == 2)
    {
      aZCoeff += 2.0 / aA2;
    }
    // else 0 for Nu >= 3
  }

  // d^Nv[-v^2/B^2] contribution (only when Nu == 0)
  if (Nu == 0)
  {
    if (Nv == 0)
    {
      aZCoeff += -V * V / aB2;
    }
    else if (Nv == 1)
    {
      aZCoeff += -2.0 * V / aB2;
    }
    else if (Nv == 2)
    {
      aZCoeff += -2.0 / aB2;
    }
    // else 0 for Nv >= 3
  }

  return gp_Vec(aXCoeff * aXDir.X() + aYCoeff * aYDir.X() + aZCoeff * aZDir.X(),
                aXCoeff * aXDir.Y() + aYCoeff * aYDir.Y() + aZCoeff * aZDir.Y(),
                aXCoeff * aXDir.Z() + aYCoeff * aYDir.Z() + aZCoeff * aZDir.Z());
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_HypParaboloidSurface::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_HypParaboloidSurface::Copy() const
{
  return new GeomEval_HypParaboloidSurface(pos, myA, myB);
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myA)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myB)
}

//==================================================================================================

void GeomEval_HypParaboloidSurface::Coefficients(double& A1,
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
  // In the local coordinate system of the hyperbolic paraboloid:
  //   X_L^2/A^2 - Y_L^2/B^2 - Z_L = 0
  //
  // This can be written as:
  //   (1/A^2)*X_L^2 + (-1/B^2)*Y_L^2 + 0*Z_L^2
  //   + 2*(0*X_L*Y_L + 0*X_L*Z_L + 0*Y_L*Z_L)
  //   + 2*(0*X_L + 0*Y_L + (-0.5)*Z_L) + 0 = 0
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

  const double aA2 = myA * myA;
  const double aB2 = myB * myB;

  // Expanding (1/A^2)*(T1i*xi+T14)^2 - (1/B^2)*(T2i*xi+T24)^2 - (T3i*xi+T34) = 0
  // and collecting terms:
  A1 = T11 * T11 / aA2 - T21 * T21 / aB2;
  A2 = T12 * T12 / aA2 - T22 * T22 / aB2;
  A3 = T13 * T13 / aA2 - T23 * T23 / aB2;
  B1 = T11 * T12 / aA2 - T21 * T22 / aB2;
  B2 = T11 * T13 / aA2 - T21 * T23 / aB2;
  B3 = T12 * T13 / aA2 - T22 * T23 / aB2;
  C1 = T11 * T14 / aA2 - T21 * T24 / aB2 - 0.5 * T31;
  C2 = T12 * T14 / aA2 - T22 * T24 / aB2 - 0.5 * T32;
  C3 = T13 * T14 / aA2 - T23 * T24 / aB2 - 0.5 * T33;
  D  = T14 * T14 / aA2 - T24 * T24 / aB2 - T34;
}
