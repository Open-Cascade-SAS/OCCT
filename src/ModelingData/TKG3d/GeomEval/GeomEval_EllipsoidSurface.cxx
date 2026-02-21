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

#include <GeomEval_EllipsoidSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_EllipsoidSurface, Geom_ElementarySurface)

//==================================================================================================

GeomEval_EllipsoidSurface::GeomEval_EllipsoidSurface(const gp_Ax3& thePosition,
                                                     double        theA,
                                                     double        theB,
                                                     double        theC)
    : myA(theA),
      myB(theB),
      myC(theC)
{
  if (theA <= 0.0 || theB <= 0.0 || theC <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_EllipsoidSurface: semi-axis must be > 0");
  }
  pos = thePosition;
}

//==================================================================================================

double GeomEval_EllipsoidSurface::SemiAxisA() const
{
  return myA;
}

//==================================================================================================

double GeomEval_EllipsoidSurface::SemiAxisB() const
{
  return myB;
}

//==================================================================================================

double GeomEval_EllipsoidSurface::SemiAxisC() const
{
  return myC;
}

//==================================================================================================

void GeomEval_EllipsoidSurface::SetSemiAxisA(double theA)
{
  if (theA <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_EllipsoidSurface::SetSemiAxisA: value must be > 0");
  }
  myA = theA;
}

//==================================================================================================

void GeomEval_EllipsoidSurface::SetSemiAxisB(double theB)
{
  if (theB <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_EllipsoidSurface::SetSemiAxisB: value must be > 0");
  }
  myB = theB;
}

//==================================================================================================

void GeomEval_EllipsoidSurface::SetSemiAxisC(double theC)
{
  if (theC <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_EllipsoidSurface::SetSemiAxisC: value must be > 0");
  }
  myC = theC;
}

//==================================================================================================

void GeomEval_EllipsoidSurface::UReverse()
{
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::UReverse");
}

//==================================================================================================

void GeomEval_EllipsoidSurface::VReverse()
{
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::VReverse");
}

//==================================================================================================

double GeomEval_EllipsoidSurface::UReversedParameter(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::UReversedParameter");
}

//==================================================================================================

double GeomEval_EllipsoidSurface::VReversedParameter(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::VReversedParameter");
}

//==================================================================================================

void GeomEval_EllipsoidSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = -M_PI / 2.0;
  V2 = M_PI / 2.0;
}

//==================================================================================================

bool GeomEval_EllipsoidSurface::IsUClosed() const
{
  return true;
}

//==================================================================================================

bool GeomEval_EllipsoidSurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_EllipsoidSurface::IsUPeriodic() const
{
  return true;
}

//==================================================================================================

bool GeomEval_EllipsoidSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_EllipsoidSurface::UIso(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::UIso");
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_EllipsoidSurface::VIso(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::VIso");
}

//==================================================================================================

gp_Pnt GeomEval_EllipsoidSurface::EvalD0(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosV = std::cos(V);
  const double aSinV = std::sin(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  return gp_Pnt(anO + myA * aCosV * aCosU * aXD + myB * aCosV * aSinU * aYD + myC * aSinV * aZD);
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_EllipsoidSurface::EvalD1(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosV = std::cos(V);
  const double aSinV = std::sin(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  Geom_Surface::ResD1 aResult;
  aResult.Point =
    gp_Pnt(anO + myA * aCosV * aCosU * aXD + myB * aCosV * aSinU * aYD + myC * aSinV * aZD);

  // dP/du = -A*cv*su*XD + B*cv*cu*YD
  aResult.D1U = gp_Vec(myA * aCosV * (-aSinU) * aXD + myB * aCosV * aCosU * aYD);

  // dP/dv = -A*sv*cu*XD - B*sv*su*YD + C*cv*ZD
  aResult.D1V =
    gp_Vec(myA * (-aSinV) * aCosU * aXD + myB * (-aSinV) * aSinU * aYD + myC * aCosV * aZD);

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_EllipsoidSurface::EvalD2(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosV = std::cos(V);
  const double aSinV = std::sin(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  Geom_Surface::ResD2 aResult;
  aResult.Point =
    gp_Pnt(anO + myA * aCosV * aCosU * aXD + myB * aCosV * aSinU * aYD + myC * aSinV * aZD);

  // dP/du
  aResult.D1U = gp_Vec(myA * aCosV * (-aSinU) * aXD + myB * aCosV * aCosU * aYD);

  // dP/dv
  aResult.D1V =
    gp_Vec(myA * (-aSinV) * aCosU * aXD + myB * (-aSinV) * aSinU * aYD + myC * aCosV * aZD);

  // d2P/du2 = -A*cv*cu*XD - B*cv*su*YD
  aResult.D2U = gp_Vec(myA * aCosV * (-aCosU) * aXD + myB * aCosV * (-aSinU) * aYD);

  // d2P/dv2 = -A*cv*cu*XD - B*cv*su*YD - C*sv*ZD
  aResult.D2V =
    gp_Vec(myA * (-aCosV) * aCosU * aXD + myB * (-aCosV) * aSinU * aYD + myC * (-aSinV) * aZD);

  // d2P/dudv = A*sv*su*XD - B*sv*cu*YD
  aResult.D2UV = gp_Vec(myA * aSinV * aSinU * aXD + myB * (-aSinV) * aCosU * aYD);

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_EllipsoidSurface::EvalD3(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosV = std::cos(V);
  const double aSinV = std::sin(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  Geom_Surface::ResD3 aResult;

  // P
  aResult.Point =
    gp_Pnt(anO + myA * aCosV * aCosU * aXD + myB * aCosV * aSinU * aYD + myC * aSinV * aZD);

  // dP/du
  aResult.D1U = gp_Vec(myA * aCosV * (-aSinU) * aXD + myB * aCosV * aCosU * aYD);

  // dP/dv
  aResult.D1V =
    gp_Vec(myA * (-aSinV) * aCosU * aXD + myB * (-aSinV) * aSinU * aYD + myC * aCosV * aZD);

  // d2P/du2
  aResult.D2U = gp_Vec(myA * aCosV * (-aCosU) * aXD + myB * aCosV * (-aSinU) * aYD);

  // d2P/dv2
  aResult.D2V =
    gp_Vec(myA * (-aCosV) * aCosU * aXD + myB * (-aCosV) * aSinU * aYD + myC * (-aSinV) * aZD);

  // d2P/dudv
  aResult.D2UV = gp_Vec(myA * aSinV * aSinU * aXD + myB * (-aSinV) * aCosU * aYD);

  // d3P/du3 = A*cv*su*XD - B*cv*cu*YD
  aResult.D3U = gp_Vec(myA * aCosV * aSinU * aXD + myB * aCosV * (-aCosU) * aYD);

  // d3P/dv3 = A*sv*cu*XD + B*sv*su*YD - C*cv*ZD
  aResult.D3V =
    gp_Vec(myA * aSinV * aCosU * aXD + myB * aSinV * aSinU * aYD + myC * (-aCosV) * aZD);

  // d3P/du2dv = A*sv*cu*XD + B*sv*su*YD
  aResult.D3UUV = gp_Vec(myA * aSinV * aCosU * aXD + myB * aSinV * aSinU * aYD);

  // d3P/dudv2 = A*cv*su*XD - B*cv*cu*YD
  aResult.D3UVV = gp_Vec(myA * aCosV * aSinU * aXD + myB * (-aCosV) * aCosU * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_EllipsoidSurface::EvalDN(const double U,
                                         const double V,
                                         const int    Nu,
                                         const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative("GeomEval_EllipsoidSurface::EvalDN: invalid derivative order");
  }

  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  // Using phase-shift identities:
  // d^n/du^n[cos(u)] = cos(u + n*Pi/2)
  // d^n/du^n[sin(u)] = sin(u + n*Pi/2)
  const double aPhaseU = Nu * M_PI / 2.0;
  const double aPhaseV = Nv * M_PI / 2.0;

  // XD coefficient: A * d^Nv[cos(v)] * d^Nu[cos(u)]
  const double aCoeffX = myA * std::cos(V + aPhaseV) * std::cos(U + aPhaseU);

  // YD coefficient: B * d^Nv[cos(v)] * d^Nu[sin(u)]
  const double aCoeffY = myB * std::cos(V + aPhaseV) * std::sin(U + aPhaseU);

  // ZD coefficient: C * d^Nv[sin(v)] * (Nu == 0 ? 1 : 0)
  const double aCoeffZ = myC * std::sin(V + aPhaseV) * (Nu == 0 ? 1.0 : 0.0);

  return gp_Vec(aCoeffX * aXD + aCoeffY * aYD + aCoeffZ * aZD);
}

//==================================================================================================

void GeomEval_EllipsoidSurface::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_EllipsoidSurface::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_EllipsoidSurface::Copy() const
{
  return new GeomEval_EllipsoidSurface(pos, myA, myB, myC);
}

//==================================================================================================

void GeomEval_EllipsoidSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myA)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myB)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myC)
}

//==================================================================================================

void GeomEval_EllipsoidSurface::Coefficients(double& A1,
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
  // In the local coordinate system of the ellipsoid:
  // X^2/myA^2 + Y^2/myB^2 + Z^2/myC^2 - 1 = 0
  //
  // i.e. local diagonal coefficients are:
  //   a1 = 1/A^2, a2 = 1/B^2, a3 = 1/C^2, d = -1
  // and all cross/linear terms are zero.
  //
  // We transform from local to global using gp_Trsf from pos,
  // substituting X_local = T11*X + T12*Y + T13*Z + T14, etc.
  // The general quadric equation in global coordinates is:
  //   A1*X^2 + A2*Y^2 + A3*Z^2 + 2*(B1*X*Y + B2*X*Z + B3*Y*Z)
  //   + 2*(C1*X + C2*Y + C3*Z) + D = 0

  const double aInvA2 = 1.0 / (myA * myA);
  const double aInvB2 = 1.0 / (myB * myB);
  const double aInvC2 = 1.0 / (myC * myC);

  gp_Trsf T;
  T.SetTransformation(pos);
  const double T11 = T.Value(1, 1);
  const double T12 = T.Value(1, 2);
  const double T13 = T.Value(1, 3);
  const double T14 = T.Value(1, 4);
  const double T21 = T.Value(2, 1);
  const double T22 = T.Value(2, 2);
  const double T23 = T.Value(2, 3);
  const double T24 = T.Value(2, 4);
  const double T31 = T.Value(3, 1);
  const double T32 = T.Value(3, 2);
  const double T33 = T.Value(3, 3);
  const double T34 = T.Value(3, 4);

  // A1 = a1*T11^2 + a2*T21^2 + a3*T31^2
  A1 = aInvA2 * T11 * T11 + aInvB2 * T21 * T21 + aInvC2 * T31 * T31;

  // A2 = a1*T12^2 + a2*T22^2 + a3*T32^2
  A2 = aInvA2 * T12 * T12 + aInvB2 * T22 * T22 + aInvC2 * T32 * T32;

  // A3 = a1*T13^2 + a2*T23^2 + a3*T33^2
  A3 = aInvA2 * T13 * T13 + aInvB2 * T23 * T23 + aInvC2 * T33 * T33;

  // B1 = a1*T11*T12 + a2*T21*T22 + a3*T31*T32
  B1 = aInvA2 * T11 * T12 + aInvB2 * T21 * T22 + aInvC2 * T31 * T32;

  // B2 = a1*T11*T13 + a2*T21*T23 + a3*T31*T33
  B2 = aInvA2 * T11 * T13 + aInvB2 * T21 * T23 + aInvC2 * T31 * T33;

  // B3 = a1*T12*T13 + a2*T22*T23 + a3*T32*T33
  B3 = aInvA2 * T12 * T13 + aInvB2 * T22 * T23 + aInvC2 * T32 * T33;

  // C1 = a1*T11*T14 + a2*T21*T24 + a3*T31*T34
  C1 = aInvA2 * T11 * T14 + aInvB2 * T21 * T24 + aInvC2 * T31 * T34;

  // C2 = a1*T12*T14 + a2*T22*T24 + a3*T32*T34
  C2 = aInvA2 * T12 * T14 + aInvB2 * T22 * T24 + aInvC2 * T32 * T34;

  // C3 = a1*T13*T14 + a2*T23*T24 + a3*T33*T34
  C3 = aInvA2 * T13 * T14 + aInvB2 * T23 * T24 + aInvC2 * T33 * T34;

  // D = a1*T14^2 + a2*T24^2 + a3*T34^2 - 1
  D = aInvA2 * T14 * T14 + aInvB2 * T24 * T24 + aInvC2 * T34 * T34 - 1.0;
}
