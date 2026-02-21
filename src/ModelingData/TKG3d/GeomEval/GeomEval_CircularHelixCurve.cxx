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

#include <GeomEval_CircularHelixCurve.hxx>

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_CircularHelixCurve, Geom_Curve)

//==================================================================================================

GeomEval_CircularHelixCurve::GeomEval_CircularHelixCurve(const gp_Ax2& thePosition,
                                                         double        theRadius,
                                                         double        thePitch)
    : myPosition(thePosition),
      myRadius(theRadius),
      myPitch(thePitch)
{
  if (theRadius <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_CircularHelixCurve: radius must be > 0");
  }
}

//==================================================================================================

const gp_Ax2& GeomEval_CircularHelixCurve::Position() const
{
  return myPosition;
}

//==================================================================================================

double GeomEval_CircularHelixCurve::Radius() const
{
  return myRadius;
}

//==================================================================================================

double GeomEval_CircularHelixCurve::Pitch() const
{
  return myPitch;
}

//==================================================================================================

void GeomEval_CircularHelixCurve::Reverse()
{
  throw Standard_NotImplemented("GeomEval_CircularHelixCurve::Reverse");
}

//==================================================================================================

double GeomEval_CircularHelixCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("GeomEval_CircularHelixCurve::ReversedParameter");
}

//==================================================================================================

double GeomEval_CircularHelixCurve::FirstParameter() const
{
  return -Precision::Infinite();
}

//==================================================================================================

double GeomEval_CircularHelixCurve::LastParameter() const
{
  return Precision::Infinite();
}

//==================================================================================================

bool GeomEval_CircularHelixCurve::IsClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_CircularHelixCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape GeomEval_CircularHelixCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool GeomEval_CircularHelixCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt GeomEval_CircularHelixCurve::EvalD0(const double U) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();
  const gp_XYZ& aZD = myPosition.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);

  return gp_Pnt(anO + myRadius * aCosU * aXD
                     + myRadius * aSinU * aYD
                     + aZComp * aZD);
}

//==================================================================================================

Geom_Curve::ResD1 GeomEval_CircularHelixCurve::EvalD1(const double U) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();
  const gp_XYZ& aZD = myPosition.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);
  const double aZRate = myPitch / (2.0 * M_PI);

  Geom_Curve::ResD1 aResult;
  aResult.Point = gp_Pnt(anO + myRadius * aCosU * aXD
                              + myRadius * aSinU * aYD
                              + aZComp * aZD);

  // D1 = -R*sin(t)*XDir + R*cos(t)*YDir + P/(2*Pi)*ZDir
  aResult.D1 = gp_Vec(myRadius * (-aSinU) * aXD
                     + myRadius * aCosU * aYD
                     + aZRate * aZD);

  return aResult;
}

//==================================================================================================

Geom_Curve::ResD2 GeomEval_CircularHelixCurve::EvalD2(const double U) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();
  const gp_XYZ& aZD = myPosition.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);
  const double aZRate = myPitch / (2.0 * M_PI);

  Geom_Curve::ResD2 aResult;
  aResult.Point = gp_Pnt(anO + myRadius * aCosU * aXD
                              + myRadius * aSinU * aYD
                              + aZComp * aZD);

  aResult.D1 = gp_Vec(myRadius * (-aSinU) * aXD
                     + myRadius * aCosU * aYD
                     + aZRate * aZD);

  // D2 = -R*cos(t)*XDir - R*sin(t)*YDir
  aResult.D2 = gp_Vec(myRadius * (-aCosU) * aXD
                     + myRadius * (-aSinU) * aYD);

  return aResult;
}

//==================================================================================================

Geom_Curve::ResD3 GeomEval_CircularHelixCurve::EvalD3(const double U) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();
  const gp_XYZ& aZD = myPosition.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);
  const double aZRate = myPitch / (2.0 * M_PI);

  Geom_Curve::ResD3 aResult;
  aResult.Point = gp_Pnt(anO + myRadius * aCosU * aXD
                              + myRadius * aSinU * aYD
                              + aZComp * aZD);

  aResult.D1 = gp_Vec(myRadius * (-aSinU) * aXD
                     + myRadius * aCosU * aYD
                     + aZRate * aZD);

  aResult.D2 = gp_Vec(myRadius * (-aCosU) * aXD
                     + myRadius * (-aSinU) * aYD);

  // D3 = R*sin(t)*XDir - R*cos(t)*YDir
  aResult.D3 = gp_Vec(myRadius * aSinU * aXD
                     + myRadius * (-aCosU) * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_CircularHelixCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError("GeomEval_CircularHelixCurve::EvalDN: N must be >= 1");
  }

  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();
  const gp_XYZ& aZD = myPosition.Direction().XYZ();

  // DN cycles with period 4 in the XY plane:
  // d^N/dt^N[cos(t)] = cos(t + N*Pi/2)
  // d^N/dt^N[sin(t)] = sin(t + N*Pi/2)
  // Z component is P/(2*Pi) for N=1, 0 otherwise.
  const double aPhase = N * M_PI / 2.0;
  const double aCoeffX = myRadius * std::cos(U + aPhase);
  const double aCoeffY = myRadius * std::sin(U + aPhase);

  gp_XYZ aResult = aCoeffX * aXD + aCoeffY * aYD;
  if (N == 1)
  {
    aResult += (myPitch / (2.0 * M_PI)) * aZD;
  }

  return gp_Vec(aResult);
}

//==================================================================================================

void GeomEval_CircularHelixCurve::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_CircularHelixCurve::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_CircularHelixCurve::Copy() const
{
  return new GeomEval_CircularHelixCurve(myPosition, myRadius, myPitch);
}

//==================================================================================================

void GeomEval_CircularHelixCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPitch)
}
