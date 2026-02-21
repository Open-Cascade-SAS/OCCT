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

#include <Geom2dEval_LogarithmicSpiralCurve.hxx>

#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEval_LogarithmicSpiralCurve, Geom2d_Curve)

//==================================================================================================

Geom2dEval_LogarithmicSpiralCurve::Geom2dEval_LogarithmicSpiralCurve(const gp_Ax2d& thePosition,
                                                                     double         theScale,
                                                                     double theGrowthExponent)
    : myPosition(thePosition),
      myScale(theScale),
      myGrowthExponent(theGrowthExponent)
{
  if (theScale <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_LogarithmicSpiralCurve: scale must be > 0");
  }
  if (theGrowthExponent <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_LogarithmicSpiralCurve: growth exponent must be > 0");
  }
}

//==================================================================================================

const gp_Ax2d& Geom2dEval_LogarithmicSpiralCurve::Position() const
{
  return myPosition;
}

//==================================================================================================

double Geom2dEval_LogarithmicSpiralCurve::Scale() const
{
  return myScale;
}

//==================================================================================================

double Geom2dEval_LogarithmicSpiralCurve::GrowthExponent() const
{
  return myGrowthExponent;
}

//==================================================================================================

void Geom2dEval_LogarithmicSpiralCurve::Reverse()
{
  gp_Dir2d aDir = myPosition.Direction();
  aDir.Reverse();
  myPosition.SetDirection(aDir);
}

//==================================================================================================

double Geom2dEval_LogarithmicSpiralCurve::ReversedParameter(const double U) const
{
  return -U;
}

//==================================================================================================

double Geom2dEval_LogarithmicSpiralCurve::FirstParameter() const
{
  return -Precision::Infinite();
}

//==================================================================================================

double Geom2dEval_LogarithmicSpiralCurve::LastParameter() const
{
  return Precision::Infinite();
}

//==================================================================================================

bool Geom2dEval_LogarithmicSpiralCurve::IsClosed() const
{
  return false;
}

//==================================================================================================

bool Geom2dEval_LogarithmicSpiralCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape Geom2dEval_LogarithmicSpiralCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool Geom2dEval_LogarithmicSpiralCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt2d Geom2dEval_LogarithmicSpiralCurve::EvalD0(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aExp = myScale * std::exp(myGrowthExponent * U);
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  return gp_Pnt2d(anO + aExp * aCosU * aXD + aExp * aSinU * aYD);
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2dEval_LogarithmicSpiralCurve::EvalD1(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aExp = myScale * std::exp(myGrowthExponent * U);
  const double aB = myGrowthExponent;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD1 aResult;
  aResult.Point = gp_Pnt2d(anO + aExp * aCosU * aXD + aExp * aSinU * aYD);

  // D1 = a*e^(b*t)*[(b*cos(t)-sin(t))*XDir + (b*sin(t)+cos(t))*YDir]
  aResult.D1 = gp_Vec2d(aExp * (aB * aCosU - aSinU) * aXD
                       + aExp * (aB * aSinU + aCosU) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2dEval_LogarithmicSpiralCurve::EvalD2(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aExp = myScale * std::exp(myGrowthExponent * U);
  const double aB = myGrowthExponent;
  const double aB2 = aB * aB;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD2 aResult;
  aResult.Point = gp_Pnt2d(anO + aExp * aCosU * aXD + aExp * aSinU * aYD);
  aResult.D1 = gp_Vec2d(aExp * (aB * aCosU - aSinU) * aXD
                       + aExp * (aB * aSinU + aCosU) * aYD);

  // D2 = a*e^(b*t)*[((b^2-1)*cos(t) - 2*b*sin(t))*XDir
  //                + ((b^2-1)*sin(t) + 2*b*cos(t))*YDir]
  aResult.D2 = gp_Vec2d(aExp * ((aB2 - 1.0) * aCosU - 2.0 * aB * aSinU) * aXD
                       + aExp * ((aB2 - 1.0) * aSinU + 2.0 * aB * aCosU) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2dEval_LogarithmicSpiralCurve::EvalD3(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aExp = myScale * std::exp(myGrowthExponent * U);
  const double aB = myGrowthExponent;
  const double aB2 = aB * aB;
  const double aB3 = aB2 * aB;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD3 aResult;
  aResult.Point = gp_Pnt2d(anO + aExp * aCosU * aXD + aExp * aSinU * aYD);
  aResult.D1 = gp_Vec2d(aExp * (aB * aCosU - aSinU) * aXD
                       + aExp * (aB * aSinU + aCosU) * aYD);
  aResult.D2 = gp_Vec2d(aExp * ((aB2 - 1.0) * aCosU - 2.0 * aB * aSinU) * aXD
                       + aExp * ((aB2 - 1.0) * aSinU + 2.0 * aB * aCosU) * aYD);

  // D3 = a*e^(b*t)*[((b^3-3*b)*cos(t) - (3*b^2-1)*sin(t))*XDir
  //                + ((b^3-3*b)*sin(t) + (3*b^2-1)*cos(t))*YDir]
  aResult.D3 = gp_Vec2d(aExp * ((aB3 - 3.0 * aB) * aCosU - (3.0 * aB2 - 1.0) * aSinU) * aXD
                       + aExp * ((aB3 - 3.0 * aB) * aSinU + (3.0 * aB2 - 1.0) * aCosU) * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec2d Geom2dEval_LogarithmicSpiralCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError(
      "Geom2dEval_LogarithmicSpiralCurve::EvalDN: N must be >= 1");
  }

  // For low orders, delegate to explicit implementations.
  if (N == 1)
  {
    return EvalD1(U).D1;
  }
  if (N == 2)
  {
    return EvalD2(U).D2;
  }
  if (N == 3)
  {
    return EvalD3(U).D3;
  }

  // General case using the fact that C(t) = a*e^(b*t)*e^(i*t)
  // in complex representation. The N-th derivative is:
  // a*(b+i)^N*e^((b+i)*t)
  // where (b+i)^N = |b+i|^N * e^(i*N*atan2(1,b))
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aB = myGrowthExponent;
  const double aExp = myScale * std::exp(aB * U);

  // (b+i)^N in polar form: magnitude = (b^2+1)^(N/2), angle = N*atan2(1,b)
  const double aMag = std::pow(aB * aB + 1.0, N / 2.0);
  const double aAngle = N * std::atan2(1.0, aB);

  // Result = a*e^(b*t) * magnitude * [cos(t + angle)*XDir + sin(t + angle)*YDir]
  const double aCoeff = aExp * aMag;
  const double aCos = std::cos(U + aAngle);
  const double aSin = std::sin(U + aAngle);

  return gp_Vec2d(aCoeff * aCos * aXD + aCoeff * aSin * aYD);
}

//==================================================================================================

void Geom2dEval_LogarithmicSpiralCurve::Transform(const gp_Trsf2d& T)
{
  myPosition.Transform(T);
  myScale *= std::abs(T.ScaleFactor());
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2dEval_LogarithmicSpiralCurve::Copy() const
{
  return new Geom2dEval_LogarithmicSpiralCurve(myPosition, myScale, myGrowthExponent);
}

//==================================================================================================

void Geom2dEval_LogarithmicSpiralCurve::DumpJson(Standard_OStream& theOStream,
                                                  int               theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myScale)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGrowthExponent)
}
