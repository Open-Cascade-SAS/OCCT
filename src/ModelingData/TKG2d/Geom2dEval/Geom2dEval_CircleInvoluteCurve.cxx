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

#include <Geom2dEval_CircleInvoluteCurve.hxx>

#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEval_CircleInvoluteCurve, Geom2d_Curve)

namespace
{
inline void trigShiftByQuarterTurns(const double theSin,
                                    const double theCos,
                                    const int    theN,
                                    double&      theSinShift,
                                    double&      theCosShift)
{
  switch (theN & 3)
  {
    case 0:
      theSinShift = theSin;
      theCosShift = theCos;
      break;
    case 1:
      theSinShift = theCos;
      theCosShift = -theSin;
      break;
    case 2:
      theSinShift = -theSin;
      theCosShift = -theCos;
      break;
    default:
      theSinShift = -theCos;
      theCosShift = theSin;
      break;
  }
}
} // namespace

//==================================================================================================

Geom2dEval_CircleInvoluteCurve::Geom2dEval_CircleInvoluteCurve(const gp_Ax2d& thePosition,
                                                               double         theRadius)
    : myPosition(thePosition),
      myRadius(theRadius)
{
  if (theRadius <= 0.0)
  {
    throw Standard_ConstructionError("Geom2dEval_CircleInvoluteCurve: radius must be > 0");
  }
}

//==================================================================================================

const gp_Ax2d& Geom2dEval_CircleInvoluteCurve::Position() const
{
  return myPosition;
}

//==================================================================================================

double Geom2dEval_CircleInvoluteCurve::Radius() const
{
  return myRadius;
}

//==================================================================================================

void Geom2dEval_CircleInvoluteCurve::Reverse()
{
  throw Standard_NotImplemented("Geom2dEval_CircleInvoluteCurve::Reverse");
}

//==================================================================================================

double Geom2dEval_CircleInvoluteCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("Geom2dEval_CircleInvoluteCurve::ReversedParameter");
}

//==================================================================================================

double Geom2dEval_CircleInvoluteCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double Geom2dEval_CircleInvoluteCurve::LastParameter() const
{
  return Precision::Infinite();
}

//==================================================================================================

bool Geom2dEval_CircleInvoluteCurve::IsClosed() const
{
  return false;
}

//==================================================================================================

bool Geom2dEval_CircleInvoluteCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape Geom2dEval_CircleInvoluteCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool Geom2dEval_CircleInvoluteCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt2d Geom2dEval_CircleInvoluteCurve::EvalD0(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  // C(t) = O + R*(cos(t) + t*sin(t))*XDir + R*(sin(t) - t*cos(t))*YDir
  return gp_Pnt2d(anO + myRadius * (aCosU + U * aSinU) * aXD
                  + myRadius * (aSinU - U * aCosU) * aYD);
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2dEval_CircleInvoluteCurve::EvalD1(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD1 aResult;
  aResult.Point =
    gp_Pnt2d(anO + myRadius * (aCosU + U * aSinU) * aXD + myRadius * (aSinU - U * aCosU) * aYD);

  // D1 = R*t*cos(t)*XDir + R*t*sin(t)*YDir
  aResult.D1 = gp_Vec2d(myRadius * U * aCosU * aXD + myRadius * U * aSinU * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2dEval_CircleInvoluteCurve::EvalD2(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD2 aResult;
  aResult.Point =
    gp_Pnt2d(anO + myRadius * (aCosU + U * aSinU) * aXD + myRadius * (aSinU - U * aCosU) * aYD);

  aResult.D1 = gp_Vec2d(myRadius * U * aCosU * aXD + myRadius * U * aSinU * aYD);

  // D2 = R*(cos(t) - t*sin(t))*XDir + R*(sin(t) + t*cos(t))*YDir
  aResult.D2 =
    gp_Vec2d(myRadius * (aCosU - U * aSinU) * aXD + myRadius * (aSinU + U * aCosU) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2dEval_CircleInvoluteCurve::EvalD3(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD3 aResult;
  aResult.Point =
    gp_Pnt2d(anO + myRadius * (aCosU + U * aSinU) * aXD + myRadius * (aSinU - U * aCosU) * aYD);

  aResult.D1 = gp_Vec2d(myRadius * U * aCosU * aXD + myRadius * U * aSinU * aYD);

  aResult.D2 =
    gp_Vec2d(myRadius * (aCosU - U * aSinU) * aXD + myRadius * (aSinU + U * aCosU) * aYD);

  // D3 = R*(-2*sin(t) - t*cos(t))*XDir + R*(2*cos(t) - t*sin(t))*YDir
  aResult.D3 = gp_Vec2d(myRadius * (-2.0 * aSinU - U * aCosU) * aXD
                        + myRadius * (2.0 * aCosU - U * aSinU) * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec2d Geom2dEval_CircleInvoluteCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError("Geom2dEval_CircleInvoluteCurve::EvalDN: N must be >= 1");
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

  // General: C(t) = R*(cos(t) + t*sin(t))*XD + R*(sin(t) - t*cos(t))*YD
  // The X component is R*f(t) where f(t) = cos(t) + t*sin(t)
  // The Y component is R*g(t) where g(t) = sin(t) - t*cos(t)
  // By Leibniz rule on the t*trig(t) terms:
  // f^(N)(t) = cos(t+N*Pi/2) + t*sin(t+N*Pi/2) + N*sin(t+(N-1)*Pi/2)
  // g^(N)(t) = sin(t+N*Pi/2) - t*cos(t+N*Pi/2) - N*cos(t+(N-1)*Pi/2)
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aSinU   = std::sin(U);
  const double aCosU   = std::cos(U);
  double       aSinN   = 0.0;
  double       aCosN   = 0.0;
  double       aSinNm1 = 0.0;
  double       aCosNm1 = 0.0;
  trigShiftByQuarterTurns(aSinU, aCosU, N, aSinN, aCosN);
  trigShiftByQuarterTurns(aSinU, aCosU, N - 1, aSinNm1, aCosNm1);

  const double aFN = aCosN + U * aSinN + N * aSinNm1;
  const double aGN = aSinN - U * aCosN - N * aCosNm1;

  return gp_Vec2d(myRadius * aFN * aXD + myRadius * aGN * aYD);
}

//==================================================================================================

void Geom2dEval_CircleInvoluteCurve::Transform(const gp_Trsf2d& T)
{
  (void)T;
  throw Standard_NotImplemented("Geom2dEval_CircleInvoluteCurve::Transform");
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2dEval_CircleInvoluteCurve::Copy() const
{
  return new Geom2dEval_CircleInvoluteCurve(myPosition, myRadius);
}

//==================================================================================================

void Geom2dEval_CircleInvoluteCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRadius)
}
