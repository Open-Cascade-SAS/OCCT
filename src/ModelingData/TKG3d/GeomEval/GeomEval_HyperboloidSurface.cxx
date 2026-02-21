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

#include <GeomEval_HyperboloidSurface.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_HyperboloidSurface, Geom_ElementarySurface)

//==================================================================================================

GeomEval_HyperboloidSurface::GeomEval_HyperboloidSurface(const gp_Ax3& thePosition,
                                                         double        theR1,
                                                         double        theR2,
                                                         SheetMode     theMode)
    : myR1(theR1),
      myR2(theR2),
      myMode(theMode)
{
  if (theR1 <= 0.0 || theR2 <= 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = thePosition;
}

//==================================================================================================

double GeomEval_HyperboloidSurface::R1() const
{
  return myR1;
}

//==================================================================================================

double GeomEval_HyperboloidSurface::R2() const
{
  return myR2;
}

//==================================================================================================

GeomEval_HyperboloidSurface::SheetMode GeomEval_HyperboloidSurface::Mode() const
{
  return myMode;
}

//==================================================================================================

void GeomEval_HyperboloidSurface::SetR1(double theR1)
{
  if (theR1 <= 0.0)
  {
    throw Standard_ConstructionError();
  }
  myR1 = theR1;
}

//==================================================================================================

void GeomEval_HyperboloidSurface::SetR2(double theR2)
{
  if (theR2 <= 0.0)
  {
    throw Standard_ConstructionError();
  }
  myR2 = theR2;
}

//==================================================================================================

void GeomEval_HyperboloidSurface::SetMode(SheetMode theMode)
{
  myMode = theMode;
}

//==================================================================================================

void GeomEval_HyperboloidSurface::UReverse()
{
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::UReverse");
}

//==================================================================================================

void GeomEval_HyperboloidSurface::VReverse()
{
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::VReverse");
}

//==================================================================================================

double GeomEval_HyperboloidSurface::UReversedParameter(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::UReversedParameter");
}

//==================================================================================================

double GeomEval_HyperboloidSurface::VReversedParameter(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::VReversedParameter");
}

//==================================================================================================

void GeomEval_HyperboloidSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
}

//==================================================================================================

bool GeomEval_HyperboloidSurface::IsUClosed() const
{
  return true;
}

//==================================================================================================

bool GeomEval_HyperboloidSurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_HyperboloidSurface::IsUPeriodic() const
{
  return true;
}

//==================================================================================================

bool GeomEval_HyperboloidSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_HyperboloidSurface::UIso(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::UIso");
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_HyperboloidSurface::VIso(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::VIso");
}

//==================================================================================================

gp_Pnt GeomEval_HyperboloidSurface::EvalD0(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosH = std::cosh(V);
  const double aSinH = std::sinh(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  gp_XYZ aP;
  if (myMode == SheetMode::OneSheet)
  {
    // P = O + R1*cosh(v)*cos(u)*XD + R1*cosh(v)*sin(u)*YD + R2*sinh(v)*ZD
    aP = anO + aXD * (myR1 * aCosH * aCosU) + aYD * (myR1 * aCosH * aSinU) + aZD * (myR2 * aSinH);
  }
  else
  {
    // P = O + R2*sinh(v)*cos(u)*XD + R2*sinh(v)*sin(u)*YD + R1*cosh(v)*ZD
    aP = anO + aXD * (myR2 * aSinH * aCosU) + aYD * (myR2 * aSinH * aSinU) + aZD * (myR1 * aCosH);
  }
  return gp_Pnt(aP);
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_HyperboloidSurface::EvalD1(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosH = std::cosh(V);
  const double aSinH = std::sinh(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  Geom_Surface::ResD1 aResult;
  if (myMode == SheetMode::OneSheet)
  {
    aResult.Point = gp_Pnt(anO + aXD * (myR1 * aCosH * aCosU) + aYD * (myR1 * aCosH * aSinU)
                           + aZD * (myR2 * aSinH));
    // dP/du = R1*cosh(v)*(-sin(u))*XD + R1*cosh(v)*cos(u)*YD
    aResult.D1U = gp_Vec(aXD * (myR1 * aCosH * (-aSinU)) + aYD * (myR1 * aCosH * aCosU));
    // dP/dv = R1*sinh(v)*cos(u)*XD + R1*sinh(v)*sin(u)*YD + R2*cosh(v)*ZD
    aResult.D1V =
      gp_Vec(aXD * (myR1 * aSinH * aCosU) + aYD * (myR1 * aSinH * aSinU) + aZD * (myR2 * aCosH));
  }
  else
  {
    aResult.Point = gp_Pnt(anO + aXD * (myR2 * aSinH * aCosU) + aYD * (myR2 * aSinH * aSinU)
                           + aZD * (myR1 * aCosH));
    // dP/du = R2*sinh(v)*(-sin(u))*XD + R2*sinh(v)*cos(u)*YD
    aResult.D1U = gp_Vec(aXD * (myR2 * aSinH * (-aSinU)) + aYD * (myR2 * aSinH * aCosU));
    // dP/dv = R2*cosh(v)*cos(u)*XD + R2*cosh(v)*sin(u)*YD + R1*sinh(v)*ZD
    aResult.D1V =
      gp_Vec(aXD * (myR2 * aCosH * aCosU) + aYD * (myR2 * aCosH * aSinU) + aZD * (myR1 * aSinH));
  }
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_HyperboloidSurface::EvalD2(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosH = std::cosh(V);
  const double aSinH = std::sinh(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  Geom_Surface::ResD2 aResult;
  if (myMode == SheetMode::OneSheet)
  {
    aResult.Point = gp_Pnt(anO + aXD * (myR1 * aCosH * aCosU) + aYD * (myR1 * aCosH * aSinU)
                           + aZD * (myR2 * aSinH));
    aResult.D1U   = gp_Vec(aXD * (myR1 * aCosH * (-aSinU)) + aYD * (myR1 * aCosH * aCosU));
    aResult.D1V =
      gp_Vec(aXD * (myR1 * aSinH * aCosU) + aYD * (myR1 * aSinH * aSinU) + aZD * (myR2 * aCosH));
    // d2P/du2 = R1*cosh(v)*(-cos(u))*XD + R1*cosh(v)*(-sin(u))*YD
    aResult.D2U = gp_Vec(aXD * (myR1 * aCosH * (-aCosU)) + aYD * (myR1 * aCosH * (-aSinU)));
    // d2P/dv2 = R1*cosh(v)*cos(u)*XD + R1*cosh(v)*sin(u)*YD + R2*sinh(v)*ZD
    aResult.D2V =
      gp_Vec(aXD * (myR1 * aCosH * aCosU) + aYD * (myR1 * aCosH * aSinU) + aZD * (myR2 * aSinH));
    // d2P/dudv = R1*sinh(v)*(-sin(u))*XD + R1*sinh(v)*cos(u)*YD
    aResult.D2UV = gp_Vec(aXD * (myR1 * aSinH * (-aSinU)) + aYD * (myR1 * aSinH * aCosU));
  }
  else
  {
    aResult.Point = gp_Pnt(anO + aXD * (myR2 * aSinH * aCosU) + aYD * (myR2 * aSinH * aSinU)
                           + aZD * (myR1 * aCosH));
    aResult.D1U   = gp_Vec(aXD * (myR2 * aSinH * (-aSinU)) + aYD * (myR2 * aSinH * aCosU));
    aResult.D1V =
      gp_Vec(aXD * (myR2 * aCosH * aCosU) + aYD * (myR2 * aCosH * aSinU) + aZD * (myR1 * aSinH));
    // d2P/du2 = R2*sinh(v)*(-cos(u))*XD + R2*sinh(v)*(-sin(u))*YD
    aResult.D2U = gp_Vec(aXD * (myR2 * aSinH * (-aCosU)) + aYD * (myR2 * aSinH * (-aSinU)));
    // d2P/dv2 = R2*sinh(v)*cos(u)*XD + R2*sinh(v)*sin(u)*YD + R1*cosh(v)*ZD
    aResult.D2V =
      gp_Vec(aXD * (myR2 * aSinH * aCosU) + aYD * (myR2 * aSinH * aSinU) + aZD * (myR1 * aCosH));
    // d2P/dudv = R2*cosh(v)*(-sin(u))*XD + R2*cosh(v)*cos(u)*YD
    aResult.D2UV = gp_Vec(aXD * (myR2 * aCosH * (-aSinU)) + aYD * (myR2 * aCosH * aCosU));
  }
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_HyperboloidSurface::EvalD3(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);
  const double aCosH = std::cosh(V);
  const double aSinH = std::sinh(V);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  Geom_Surface::ResD3 aResult;
  if (myMode == SheetMode::OneSheet)
  {
    aResult.Point = gp_Pnt(anO + aXD * (myR1 * aCosH * aCosU) + aYD * (myR1 * aCosH * aSinU)
                           + aZD * (myR2 * aSinH));
    aResult.D1U   = gp_Vec(aXD * (myR1 * aCosH * (-aSinU)) + aYD * (myR1 * aCosH * aCosU));
    aResult.D1V =
      gp_Vec(aXD * (myR1 * aSinH * aCosU) + aYD * (myR1 * aSinH * aSinU) + aZD * (myR2 * aCosH));
    aResult.D2U = gp_Vec(aXD * (myR1 * aCosH * (-aCosU)) + aYD * (myR1 * aCosH * (-aSinU)));
    aResult.D2V =
      gp_Vec(aXD * (myR1 * aCosH * aCosU) + aYD * (myR1 * aCosH * aSinU) + aZD * (myR2 * aSinH));
    aResult.D2UV = gp_Vec(aXD * (myR1 * aSinH * (-aSinU)) + aYD * (myR1 * aSinH * aCosU));
    // d3P/du3 = R1*cosh(v)*sin(u)*XD + R1*cosh(v)*(-cos(u))*YD
    aResult.D3U = gp_Vec(aXD * (myR1 * aCosH * aSinU) + aYD * (myR1 * aCosH * (-aCosU)));
    // d3P/dv3 = R1*sinh(v)*cos(u)*XD + R1*sinh(v)*sin(u)*YD + R2*cosh(v)*ZD
    aResult.D3V =
      gp_Vec(aXD * (myR1 * aSinH * aCosU) + aYD * (myR1 * aSinH * aSinU) + aZD * (myR2 * aCosH));
    // d3P/du2dv = R1*sinh(v)*(-cos(u))*XD + R1*sinh(v)*(-sin(u))*YD
    aResult.D3UUV = gp_Vec(aXD * (myR1 * aSinH * (-aCosU)) + aYD * (myR1 * aSinH * (-aSinU)));
    // d3P/dudv2 = R1*cosh(v)*(-sin(u))*XD + R1*cosh(v)*cos(u)*YD
    aResult.D3UVV = gp_Vec(aXD * (myR1 * aCosH * (-aSinU)) + aYD * (myR1 * aCosH * aCosU));
  }
  else
  {
    aResult.Point = gp_Pnt(anO + aXD * (myR2 * aSinH * aCosU) + aYD * (myR2 * aSinH * aSinU)
                           + aZD * (myR1 * aCosH));
    aResult.D1U   = gp_Vec(aXD * (myR2 * aSinH * (-aSinU)) + aYD * (myR2 * aSinH * aCosU));
    aResult.D1V =
      gp_Vec(aXD * (myR2 * aCosH * aCosU) + aYD * (myR2 * aCosH * aSinU) + aZD * (myR1 * aSinH));
    aResult.D2U = gp_Vec(aXD * (myR2 * aSinH * (-aCosU)) + aYD * (myR2 * aSinH * (-aSinU)));
    aResult.D2V =
      gp_Vec(aXD * (myR2 * aSinH * aCosU) + aYD * (myR2 * aSinH * aSinU) + aZD * (myR1 * aCosH));
    aResult.D2UV = gp_Vec(aXD * (myR2 * aCosH * (-aSinU)) + aYD * (myR2 * aCosH * aCosU));
    // d3P/du3 = R2*sinh(v)*sin(u)*XD + R2*sinh(v)*(-cos(u))*YD
    aResult.D3U = gp_Vec(aXD * (myR2 * aSinH * aSinU) + aYD * (myR2 * aSinH * (-aCosU)));
    // d3P/dv3 = R2*cosh(v)*cos(u)*XD + R2*cosh(v)*sin(u)*YD + R1*sinh(v)*ZD
    aResult.D3V =
      gp_Vec(aXD * (myR2 * aCosH * aCosU) + aYD * (myR2 * aCosH * aSinU) + aZD * (myR1 * aSinH));
    // d3P/du2dv = R2*cosh(v)*(-cos(u))*XD + R2*cosh(v)*(-sin(u))*YD
    aResult.D3UUV = gp_Vec(aXD * (myR2 * aCosH * (-aCosU)) + aYD * (myR2 * aCosH * (-aSinU)));
    // d3P/dudv2 = R2*sinh(v)*(-sin(u))*XD + R2*sinh(v)*cos(u)*YD
    aResult.D3UVV = gp_Vec(aXD * (myR2 * aSinH * (-aSinU)) + aYD * (myR2 * aSinH * aCosU));
  }
  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_HyperboloidSurface::EvalDN(const double U,
                                           const double V,
                                           const int    Nu,
                                           const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative();
  }

  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  // d^n/du^n[cos(u)] = cos(u + n*PI/2)
  // d^n/du^n[sin(u)] = sin(u + n*PI/2)
  const double aUShift = U + Nu * M_PI_2;
  const double aDnCosU = std::cos(aUShift);
  const double aDnSinU = std::sin(aUShift);

  // d^n/dv^n[cosh(v)] = (n%2==0 ? cosh(v) : sinh(v))
  // d^n/dv^n[sinh(v)] = (n%2==0 ? sinh(v) : cosh(v))
  const double aCosH   = std::cosh(V);
  const double aSinH   = std::sinh(V);
  const double aDnCosH = (Nv % 2 == 0) ? aCosH : aSinH;
  const double aDnSinH = (Nv % 2 == 0) ? aSinH : aCosH;

  gp_XYZ aVec;
  if (myMode == SheetMode::OneSheet)
  {
    // XD coeff = R1 * d^Nv[cosh(v)] * d^Nu[cos(u)]
    // YD coeff = R1 * d^Nv[cosh(v)] * d^Nu[sin(u)]
    // ZD coeff = R2 * d^Nv[sinh(v)] * (Nu==0 ? 1 : 0)
    const double aXCoeff = myR1 * aDnCosH * aDnCosU;
    const double aYCoeff = myR1 * aDnCosH * aDnSinU;
    const double aZCoeff = (Nu == 0) ? myR2 * aDnSinH : 0.0;
    aVec                 = aXD * aXCoeff + aYD * aYCoeff + aZD * aZCoeff;
  }
  else
  {
    // XD coeff = R2 * d^Nv[sinh(v)] * d^Nu[cos(u)]
    // YD coeff = R2 * d^Nv[sinh(v)] * d^Nu[sin(u)]
    // ZD coeff = R1 * d^Nv[cosh(v)] * (Nu==0 ? 1 : 0)
    const double aXCoeff = myR2 * aDnSinH * aDnCosU;
    const double aYCoeff = myR2 * aDnSinH * aDnSinU;
    const double aZCoeff = (Nu == 0) ? myR1 * aDnCosH : 0.0;
    aVec                 = aXD * aXCoeff + aYD * aYCoeff + aZD * aZCoeff;
  }
  return gp_Vec(aVec);
}

//==================================================================================================

void GeomEval_HyperboloidSurface::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_HyperboloidSurface::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_HyperboloidSurface::Copy() const
{
  occ::handle<GeomEval_HyperboloidSurface> aCopy =
    new GeomEval_HyperboloidSurface(pos, myR1, myR2, myMode);
  return aCopy;
}

//==================================================================================================

void GeomEval_HyperboloidSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myR1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myR2)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, (int)myMode)
}

//==================================================================================================

void GeomEval_HyperboloidSurface::Coefficients(double& A1,
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
  // Local coordinate equation:
  // 1-sheet: X^2/R1^2 + Y^2/R1^2 - Z^2/R2^2 - 1 = 0
  //   => A1_loc=1/R1^2, A2_loc=1/R1^2, A3_loc=-1/R2^2, B_loc=0, C_loc=0, D_loc=-1
  // 2-sheet: X^2/R2^2 + Y^2/R2^2 - Z^2/R1^2 + 1 = 0
  //   => A1_loc=1/R2^2, A2_loc=1/R2^2, A3_loc=-1/R1^2, B_loc=0, C_loc=0, D_loc=+1

  double aA1Loc, aA2Loc, aA3Loc, aDLoc;
  if (myMode == SheetMode::OneSheet)
  {
    aA1Loc = 1.0 / (myR1 * myR1);
    aA2Loc = 1.0 / (myR1 * myR1);
    aA3Loc = -1.0 / (myR2 * myR2);
    aDLoc  = -1.0;
  }
  else
  {
    aA1Loc = 1.0 / (myR2 * myR2);
    aA2Loc = 1.0 / (myR2 * myR2);
    aA3Loc = -1.0 / (myR1 * myR1);
    aDLoc  = 1.0;
  }

  // Transform to global coordinates using the same pattern as Geom_SphericalSurface.
  // The transformation from local to global uses T = SetTransformation(pos).
  // Local equation: aA1Loc*X'^2 + aA2Loc*Y'^2 + aA3Loc*Z'^2 + aDLoc = 0
  // where (X',Y',Z') = T * (X,Y,Z)
  gp_Trsf aTrsf;
  aTrsf.SetTransformation(pos);
  const double T11 = aTrsf.Value(1, 1);
  const double T12 = aTrsf.Value(1, 2);
  const double T13 = aTrsf.Value(1, 3);
  const double T14 = aTrsf.Value(1, 4);
  const double T21 = aTrsf.Value(2, 1);
  const double T22 = aTrsf.Value(2, 2);
  const double T23 = aTrsf.Value(2, 3);
  const double T24 = aTrsf.Value(2, 4);
  const double T31 = aTrsf.Value(3, 1);
  const double T32 = aTrsf.Value(3, 2);
  const double T33 = aTrsf.Value(3, 3);
  const double T34 = aTrsf.Value(3, 4);

  // General quadric: sum over local axes of aCoeff[i] * (Ti1*X + Ti2*Y + Ti3*Z + Ti4)^2 + aDLoc
  // Expanding for diagonal local coefficients aA1Loc, aA2Loc, aA3Loc:
  A1 = aA1Loc * T11 * T11 + aA2Loc * T21 * T21 + aA3Loc * T31 * T31;
  A2 = aA1Loc * T12 * T12 + aA2Loc * T22 * T22 + aA3Loc * T32 * T32;
  A3 = aA1Loc * T13 * T13 + aA2Loc * T23 * T23 + aA3Loc * T33 * T33;
  B1 = aA1Loc * T11 * T12 + aA2Loc * T21 * T22 + aA3Loc * T31 * T32;
  B2 = aA1Loc * T11 * T13 + aA2Loc * T21 * T23 + aA3Loc * T31 * T33;
  B3 = aA1Loc * T12 * T13 + aA2Loc * T22 * T23 + aA3Loc * T32 * T33;
  C1 = aA1Loc * T11 * T14 + aA2Loc * T21 * T24 + aA3Loc * T31 * T34;
  C2 = aA1Loc * T12 * T14 + aA2Loc * T22 * T24 + aA3Loc * T32 * T34;
  C3 = aA1Loc * T13 * T14 + aA2Loc * T23 * T24 + aA3Loc * T33 * T34;
  D  = aA1Loc * T14 * T14 + aA2Loc * T24 * T24 + aA3Loc * T34 * T34 + aDLoc;
}
