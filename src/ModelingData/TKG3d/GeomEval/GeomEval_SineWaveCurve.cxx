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

#include <GeomEval_SineWaveCurve.hxx>

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_SineWaveCurve, Geom_Curve)

//==================================================================================================

GeomEval_SineWaveCurve::GeomEval_SineWaveCurve(const gp_Ax2& thePosition,
                                               double        theAmplitude,
                                               double        theOmega,
                                               double        thePhase)
    : myPosition(thePosition),
      myAmplitude(theAmplitude),
      myOmega(theOmega),
      myPhase(thePhase)
{
  if (theAmplitude <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_SineWaveCurve: amplitude must be > 0");
  }
  if (theOmega <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_SineWaveCurve: omega must be > 0");
  }
}

//==================================================================================================

const gp_Ax2& GeomEval_SineWaveCurve::Position() const
{
  return myPosition;
}

//==================================================================================================

double GeomEval_SineWaveCurve::Amplitude() const
{
  return myAmplitude;
}

//==================================================================================================

double GeomEval_SineWaveCurve::Omega() const
{
  return myOmega;
}

//==================================================================================================

double GeomEval_SineWaveCurve::Phase() const
{
  return myPhase;
}

//==================================================================================================

void GeomEval_SineWaveCurve::Reverse()
{
  throw Standard_NotImplemented("GeomEval_SineWaveCurve::Reverse");
}

//==================================================================================================

double GeomEval_SineWaveCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("GeomEval_SineWaveCurve::ReversedParameter");
}

//==================================================================================================

double GeomEval_SineWaveCurve::FirstParameter() const
{
  return -Precision::Infinite();
}

//==================================================================================================

double GeomEval_SineWaveCurve::LastParameter() const
{
  return Precision::Infinite();
}

//==================================================================================================

bool GeomEval_SineWaveCurve::IsClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_SineWaveCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

double GeomEval_SineWaveCurve::Period() const
{
  return Geom_Curve::Period();
}

//==================================================================================================

GeomAbs_Shape GeomEval_SineWaveCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool GeomEval_SineWaveCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt GeomEval_SineWaveCurve::EvalD0(const double U) const
{
  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();

  const double aArg = myOmega * U + myPhase;

  return gp_Pnt(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
}

//==================================================================================================

Geom_Curve::ResD1 GeomEval_SineWaveCurve::EvalD1(const double U) const
{
  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();

  const double aArg = myOmega * U + myPhase;

  Geom_Curve::ResD1 aResult;
  aResult.Point = gp_Pnt(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);

  // D1 = XDir + A*omega*cos(omega*t + phi)*YDir
  aResult.D1 = gp_Vec(aXD + myAmplitude * myOmega * std::cos(aArg) * aYD);

  return aResult;
}

//==================================================================================================

Geom_Curve::ResD2 GeomEval_SineWaveCurve::EvalD2(const double U) const
{
  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();

  const double aArg = myOmega * U + myPhase;
  const double aOm2 = myOmega * myOmega;

  Geom_Curve::ResD2 aResult;
  aResult.Point = gp_Pnt(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
  aResult.D1    = gp_Vec(aXD + myAmplitude * myOmega * std::cos(aArg) * aYD);

  // D2 = -A*omega^2*sin(omega*t + phi)*YDir
  aResult.D2 = gp_Vec(myAmplitude * (-aOm2) * std::sin(aArg) * aYD);

  return aResult;
}

//==================================================================================================

Geom_Curve::ResD3 GeomEval_SineWaveCurve::EvalD3(const double U) const
{
  const gp_XYZ& anO = myPosition.Location().XYZ();
  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();

  const double aArg = myOmega * U + myPhase;
  const double aOm2 = myOmega * myOmega;
  const double aOm3 = aOm2 * myOmega;

  Geom_Curve::ResD3 aResult;
  aResult.Point = gp_Pnt(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
  aResult.D1    = gp_Vec(aXD + myAmplitude * myOmega * std::cos(aArg) * aYD);
  aResult.D2    = gp_Vec(myAmplitude * (-aOm2) * std::sin(aArg) * aYD);

  // D3 = -A*omega^3*cos(omega*t + phi)*YDir
  aResult.D3 = gp_Vec(myAmplitude * (-aOm3) * std::cos(aArg) * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_SineWaveCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError("GeomEval_SineWaveCurve::EvalDN: N must be >= 1");
  }

  const gp_XYZ& aXD = myPosition.XDirection().XYZ();
  const gp_XYZ& aYD = myPosition.YDirection().XYZ();

  // d^N/dt^N[t] = 1 if N=1, 0 if N>=2
  // d^N/dt^N[A*sin(omega*t+phi)] = A*omega^N*sin(omega*t+phi+N*Pi/2)
  const double aOmN   = std::pow(myOmega, N);
  const double aPhase = myOmega * U + myPhase + N * M_PI / 2.0;

  gp_XYZ aResult = myAmplitude * aOmN * std::sin(aPhase) * aYD;
  if (N == 1)
  {
    aResult += aXD;
  }

  return gp_Vec(aResult);
}

//==================================================================================================

void GeomEval_SineWaveCurve::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_SineWaveCurve::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_SineWaveCurve::Copy() const
{
  return new GeomEval_SineWaveCurve(myPosition, myAmplitude, myOmega, myPhase);
}

//==================================================================================================

void GeomEval_SineWaveCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAmplitude)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOmega)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPhase)
}
