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

#include <Geom2dEval_SineWaveCurve.hxx>

#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEval_SineWaveCurve, Geom2d_Curve)

namespace
{
inline double powInt(const double theBase, const int theExp)
{
  double aRes = 1.0;
  for (int i = 0; i < theExp; ++i)
  {
    aRes *= theBase;
  }
  return aRes;
}

inline double sinShiftByQuarterTurns(const double theSin, const double theCos, const int theN)
{
  switch (theN & 3)
  {
    case 0:
      return theSin;
    case 1:
      return theCos;
    case 2:
      return -theSin;
    default:
      return -theCos;
  }
}
} // namespace

//==================================================================================================

Geom2dEval_SineWaveCurve::Geom2dEval_SineWaveCurve(const gp_Ax2d& thePosition,
                                            double         theAmplitude,
                                            double         theOmega,
                                            double         thePhase)
    : myPosition(thePosition),
      myAmplitude(theAmplitude),
      myOmega(theOmega),
      myPhase(thePhase)
{
  if (theAmplitude <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_SineWaveCurve: amplitude must be > 0");
  }
  if (theOmega <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_SineWaveCurve: omega must be > 0");
  }
}

//==================================================================================================

const gp_Ax2d& Geom2dEval_SineWaveCurve::Position() const
{
  return myPosition;
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::Amplitude() const
{
  return myAmplitude;
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::Omega() const
{
  return myOmega;
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::Phase() const
{
  return myPhase;
}

//==================================================================================================

void Geom2dEval_SineWaveCurve::Reverse()
{
  throw Standard_NotImplemented("Geom2dEval_SineWaveCurve::Reverse");
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("Geom2dEval_SineWaveCurve::ReversedParameter");
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::FirstParameter() const
{
  return -Precision::Infinite();
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::LastParameter() const
{
  return Precision::Infinite();
}

//==================================================================================================

bool Geom2dEval_SineWaveCurve::IsClosed() const
{
  return false;
}

//==================================================================================================

bool Geom2dEval_SineWaveCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

double Geom2dEval_SineWaveCurve::Period() const
{
  return Geom2d_Curve::Period();
}

//==================================================================================================

GeomAbs_Shape Geom2dEval_SineWaveCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool Geom2dEval_SineWaveCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt2d Geom2dEval_SineWaveCurve::EvalD0(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aArg = myOmega * U + myPhase;

  return gp_Pnt2d(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2dEval_SineWaveCurve::EvalD1(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aArg = myOmega * U + myPhase;

  Geom2d_Curve::ResD1 aResult;
  aResult.Point = gp_Pnt2d(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
  aResult.D1 = gp_Vec2d(aXD + myAmplitude * myOmega * std::cos(aArg) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2dEval_SineWaveCurve::EvalD2(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aArg = myOmega * U + myPhase;
  const double aOm2 = myOmega * myOmega;

  Geom2d_Curve::ResD2 aResult;
  aResult.Point = gp_Pnt2d(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
  aResult.D1 = gp_Vec2d(aXD + myAmplitude * myOmega * std::cos(aArg) * aYD);
  aResult.D2 = gp_Vec2d(myAmplitude * (-aOm2) * std::sin(aArg) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2dEval_SineWaveCurve::EvalD3(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aArg = myOmega * U + myPhase;
  const double aOm2 = myOmega * myOmega;
  const double aOm3 = aOm2 * myOmega;

  Geom2d_Curve::ResD3 aResult;
  aResult.Point = gp_Pnt2d(anO + U * aXD + myAmplitude * std::sin(aArg) * aYD);
  aResult.D1 = gp_Vec2d(aXD + myAmplitude * myOmega * std::cos(aArg) * aYD);
  aResult.D2 = gp_Vec2d(myAmplitude * (-aOm2) * std::sin(aArg) * aYD);
  aResult.D3 = gp_Vec2d(myAmplitude * (-aOm3) * std::cos(aArg) * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec2d Geom2dEval_SineWaveCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError("Geom2dEval_SineWaveCurve::EvalDN: N must be >= 1");
  }

  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aArg  = myOmega * U + myPhase;
  const double aSin  = std::sin(aArg);
  const double aCos  = std::cos(aArg);
  const double aOmN  = powInt(myOmega, N);
  const double aSinN = sinShiftByQuarterTurns(aSin, aCos, N);

  gp_XY aResult = myAmplitude * aOmN * aSinN * aYD;
  if (N == 1)
  {
    aResult += aXD;
  }

  return gp_Vec2d(aResult);
}

//==================================================================================================

void Geom2dEval_SineWaveCurve::Transform(const gp_Trsf2d& T)
{
  (void)T;
  throw Standard_NotImplemented("Geom2dEval_SineWaveCurve::Transform");
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2dEval_SineWaveCurve::Copy() const
{
  return new Geom2dEval_SineWaveCurve(myPosition, myAmplitude, myOmega, myPhase);
}

//==================================================================================================

void Geom2dEval_SineWaveCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAmplitude)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOmega)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPhase)
}
