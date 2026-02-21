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

#include <Geom2dEval_ArchimedeanSpiralCurve.hxx>

#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEval_ArchimedeanSpiralCurve, Geom2d_Curve)

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

Geom2dEval_ArchimedeanSpiralCurve::Geom2dEval_ArchimedeanSpiralCurve(const gp_Ax2d& thePosition,
                                                                     double         theInitialRadius,
                                                                     double         theGrowthRate)
    : myPosition(thePosition),
      myInitialRadius(theInitialRadius),
      myGrowthRate(theGrowthRate)
{
  if (theInitialRadius < 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_ArchimedeanSpiralCurve: initial radius must be >= 0");
  }
  if (theGrowthRate <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_ArchimedeanSpiralCurve: growth rate must be > 0");
  }
}

//==================================================================================================

const gp_Ax2d& Geom2dEval_ArchimedeanSpiralCurve::Position() const
{
  return myPosition;
}

//==================================================================================================

double Geom2dEval_ArchimedeanSpiralCurve::InitialRadius() const
{
  return myInitialRadius;
}

//==================================================================================================

double Geom2dEval_ArchimedeanSpiralCurve::GrowthRate() const
{
  return myGrowthRate;
}

//==================================================================================================

void Geom2dEval_ArchimedeanSpiralCurve::Reverse()
{
  throw Standard_NotImplemented("Geom2dEval_ArchimedeanSpiralCurve::Reverse");
}

//==================================================================================================

double Geom2dEval_ArchimedeanSpiralCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("Geom2dEval_ArchimedeanSpiralCurve::ReversedParameter");
}

//==================================================================================================

double Geom2dEval_ArchimedeanSpiralCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double Geom2dEval_ArchimedeanSpiralCurve::LastParameter() const
{
  return Precision::Infinite();
}

//==================================================================================================

bool Geom2dEval_ArchimedeanSpiralCurve::IsClosed() const
{
  return false;
}

//==================================================================================================

bool Geom2dEval_ArchimedeanSpiralCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape Geom2dEval_ArchimedeanSpiralCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool Geom2dEval_ArchimedeanSpiralCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt2d Geom2dEval_ArchimedeanSpiralCurve::EvalD0(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aR = myInitialRadius + myGrowthRate * U;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  return gp_Pnt2d(anO + aR * aCosU * aXD + aR * aSinU * aYD);
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2dEval_ArchimedeanSpiralCurve::EvalD1(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aR = myInitialRadius + myGrowthRate * U;
  const double aB = myGrowthRate;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD1 aResult;
  aResult.Point = gp_Pnt2d(anO + aR * aCosU * aXD + aR * aSinU * aYD);

  // D1 = (b*cos(t) - r*sin(t))*XDir + (b*sin(t) + r*cos(t))*YDir
  aResult.D1 = gp_Vec2d((aB * aCosU - aR * aSinU) * aXD
                       + (aB * aSinU + aR * aCosU) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2dEval_ArchimedeanSpiralCurve::EvalD2(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aR = myInitialRadius + myGrowthRate * U;
  const double aB = myGrowthRate;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD2 aResult;
  aResult.Point = gp_Pnt2d(anO + aR * aCosU * aXD + aR * aSinU * aYD);
  aResult.D1 = gp_Vec2d((aB * aCosU - aR * aSinU) * aXD
                       + (aB * aSinU + aR * aCosU) * aYD);

  // D2 = (-2*b*sin(t) - r*cos(t))*XDir + (2*b*cos(t) - r*sin(t))*YDir
  aResult.D2 = gp_Vec2d((-2.0 * aB * aSinU - aR * aCosU) * aXD
                       + (2.0 * aB * aCosU - aR * aSinU) * aYD);

  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2dEval_ArchimedeanSpiralCurve::EvalD3(const double U) const
{
  const gp_XY& anO = myPosition.Location().XY();
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aR = myInitialRadius + myGrowthRate * U;
  const double aB = myGrowthRate;
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  Geom2d_Curve::ResD3 aResult;
  aResult.Point = gp_Pnt2d(anO + aR * aCosU * aXD + aR * aSinU * aYD);
  aResult.D1 = gp_Vec2d((aB * aCosU - aR * aSinU) * aXD
                       + (aB * aSinU + aR * aCosU) * aYD);
  aResult.D2 = gp_Vec2d((-2.0 * aB * aSinU - aR * aCosU) * aXD
                       + (2.0 * aB * aCosU - aR * aSinU) * aYD);

  // D3 = (-3*b*cos(t) + r*sin(t))*XDir + (-3*b*sin(t) - r*cos(t))*YDir
  aResult.D3 = gp_Vec2d((-3.0 * aB * aCosU + aR * aSinU) * aXD
                       + (-3.0 * aB * aSinU - aR * aCosU) * aYD);

  return aResult;
}

//==================================================================================================

gp_Vec2d Geom2dEval_ArchimedeanSpiralCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError(
      "Geom2dEval_ArchimedeanSpiralCurve::EvalDN: N must be >= 1");
  }

  // Use the general formula for d^N/dt^N[(a+b*t)*cos(t)] and [(a+b*t)*sin(t)].
  // By Leibniz rule: d^N/dt^N[r(t)*f(t)] = sum_{k=0}^{N} C(N,k)*r^(k)*f^(N-k)
  // where r(t) = a + b*t, r'(t) = b, r''(t) = 0 for k >= 2.
  // So only k=0 and k=1 contribute:
  // = r(t)*f^(N)(t) + N*b*f^(N-1)(t)
  const gp_XY& aXD = myPosition.Direction().XY();
  const gp_XY  aYD(-aXD.Y(), aXD.X());

  const double aR = myInitialRadius + myGrowthRate * U;
  const double aB = myGrowthRate;

  const double aSinU = std::sin(U);
  const double aCosU = std::cos(U);
  double       aSinN = 0.0;
  double       aCosN = 0.0;
  double       aSinNm1 = 0.0;
  double       aCosNm1 = 0.0;
  trigShiftByQuarterTurns(aSinU, aCosU, N, aSinN, aCosN);
  trigShiftByQuarterTurns(aSinU, aCosU, N - 1, aSinNm1, aCosNm1);

  // X component: r*cos^(N)(t) + N*b*cos^(N-1)(t)
  const double aCoeffX = aR * aCosN + N * aB * aCosNm1;
  // Y component: r*sin^(N)(t) + N*b*sin^(N-1)(t)
  const double aCoeffY = aR * aSinN + N * aB * aSinNm1;

  return gp_Vec2d(aCoeffX * aXD + aCoeffY * aYD);
}

//==================================================================================================

void Geom2dEval_ArchimedeanSpiralCurve::Transform(const gp_Trsf2d& T)
{
  myPosition.Transform(T);
  const double aScale = std::abs(T.ScaleFactor());
  myInitialRadius *= aScale;
  myGrowthRate *= aScale;
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2dEval_ArchimedeanSpiralCurve::Copy() const
{
  return new Geom2dEval_ArchimedeanSpiralCurve(myPosition, myInitialRadius, myGrowthRate);
}

//==================================================================================================

void Geom2dEval_ArchimedeanSpiralCurve::DumpJson(Standard_OStream& theOStream,
                                                  int               theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myInitialRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGrowthRate)
}
