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

#include <GeomEval_CircularHelicoidSurface.hxx>

#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_CircularHelicoidSurface, Geom_ElementarySurface)

//==================================================================================================

GeomEval_CircularHelicoidSurface::GeomEval_CircularHelicoidSurface(const gp_Ax3& thePosition,
                                                                   double        thePitch)
    : myPitch(thePitch)
{
  if (thePitch == 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_CircularHelicoidSurface: pitch must be != 0");
  }
  pos = thePosition;
}

//==================================================================================================

double GeomEval_CircularHelicoidSurface::Pitch() const
{
  return myPitch;
}

//==================================================================================================

void GeomEval_CircularHelicoidSurface::SetPitch(double thePitch)
{
  if (thePitch == 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_CircularHelicoidSurface::SetPitch: pitch must be != 0");
  }
  myPitch = thePitch;
}

//==================================================================================================

void GeomEval_CircularHelicoidSurface::UReverse()
{
  throw Standard_NotImplemented("GeomEval_CircularHelicoidSurface::UReverse");
}

//==================================================================================================

void GeomEval_CircularHelicoidSurface::VReverse()
{
  throw Standard_NotImplemented("GeomEval_CircularHelicoidSurface::VReverse");
}

//==================================================================================================

double GeomEval_CircularHelicoidSurface::UReversedParameter(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_CircularHelicoidSurface::UReversedParameter");
}

//==================================================================================================

double GeomEval_CircularHelicoidSurface::VReversedParameter(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_CircularHelicoidSurface::VReversedParameter");
}

//==================================================================================================

void GeomEval_CircularHelicoidSurface::Bounds(double& U1,
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

bool GeomEval_CircularHelicoidSurface::IsUClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_CircularHelicoidSurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_CircularHelicoidSurface::IsUPeriodic() const
{
  return false;
}

//==================================================================================================

bool GeomEval_CircularHelicoidSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_CircularHelicoidSurface::UIso(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_CircularHelicoidSurface::UIso");
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_CircularHelicoidSurface::VIso(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_CircularHelicoidSurface::VIso");
}

//==================================================================================================

gp_Pnt GeomEval_CircularHelicoidSurface::EvalD0(const double U, const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);

  return gp_Pnt(anO + V * aCosU * aXD
                     + V * aSinU * aYD
                     + aZComp * aZD);
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_CircularHelicoidSurface::EvalD1(const double U,
                                                             const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);
  const double aZRate = myPitch / (2.0 * M_PI);

  Geom_Surface::ResD1 aResult;
  aResult.Point = gp_Pnt(anO + V * aCosU * aXD
                              + V * aSinU * aYD
                              + aZComp * aZD);

  // dS/du = -v*sin(u)*XDir + v*cos(u)*YDir + P/(2*Pi)*ZDir
  aResult.D1U = gp_Vec(V * (-aSinU) * aXD
                      + V * aCosU * aYD
                      + aZRate * aZD);

  // dS/dv = cos(u)*XDir + sin(u)*YDir
  aResult.D1V = gp_Vec(aCosU * aXD + aSinU * aYD);

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_CircularHelicoidSurface::EvalD2(const double U,
                                                             const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);
  const double aZRate = myPitch / (2.0 * M_PI);

  Geom_Surface::ResD2 aResult;
  aResult.Point = gp_Pnt(anO + V * aCosU * aXD
                              + V * aSinU * aYD
                              + aZComp * aZD);

  aResult.D1U = gp_Vec(V * (-aSinU) * aXD
                      + V * aCosU * aYD
                      + aZRate * aZD);

  aResult.D1V = gp_Vec(aCosU * aXD + aSinU * aYD);

  // d2S/dudu = -v*cos(u)*XDir - v*sin(u)*YDir
  aResult.D2U = gp_Vec(V * (-aCosU) * aXD
                      + V * (-aSinU) * aYD);

  // d2S/dvdv = 0 (ruled surface)
  aResult.D2V = gp_Vec(0.0, 0.0, 0.0);

  // d2S/dudv = -sin(u)*XDir + cos(u)*YDir
  aResult.D2UV = gp_Vec((-aSinU) * aXD + aCosU * aYD);

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_CircularHelicoidSurface::EvalD3(const double U,
                                                             const double V) const
{
  const double aCosU = std::cos(U);
  const double aSinU = std::sin(U);

  const gp_XYZ& anO = pos.Location().XYZ();
  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  const double aZComp = myPitch * U / (2.0 * M_PI);
  const double aZRate = myPitch / (2.0 * M_PI);

  Geom_Surface::ResD3 aResult;

  aResult.Point = gp_Pnt(anO + V * aCosU * aXD
                              + V * aSinU * aYD
                              + aZComp * aZD);

  aResult.D1U = gp_Vec(V * (-aSinU) * aXD
                      + V * aCosU * aYD
                      + aZRate * aZD);

  aResult.D1V = gp_Vec(aCosU * aXD + aSinU * aYD);

  aResult.D2U = gp_Vec(V * (-aCosU) * aXD
                      + V * (-aSinU) * aYD);

  aResult.D2V = gp_Vec(0.0, 0.0, 0.0);

  aResult.D2UV = gp_Vec((-aSinU) * aXD + aCosU * aYD);

  // d3S/du3 = v*sin(u)*XDir - v*cos(u)*YDir
  aResult.D3U = gp_Vec(V * aSinU * aXD
                      + V * (-aCosU) * aYD);

  // d3S/dv3 = 0
  aResult.D3V = gp_Vec(0.0, 0.0, 0.0);

  // d3S/du2dv = -cos(u)*XDir - sin(u)*YDir
  aResult.D3UUV = gp_Vec((-aCosU) * aXD + (-aSinU) * aYD);

  // d3S/dudv2 = 0
  aResult.D3UVV = gp_Vec(0.0, 0.0, 0.0);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_CircularHelicoidSurface::EvalDN(const double U,
                                                const double V,
                                                const int    Nu,
                                                const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative(
      "GeomEval_CircularHelicoidSurface::EvalDN: invalid derivative order");
  }

  // S(u,v) = O + v*cos(u)*XDir + v*sin(u)*YDir + P*u/(2*Pi)*ZDir
  // The v-dependence is linear, so Nv >= 2 => 0.
  // The u-dependence of the Z term is linear, so Nu >= 2 on Z => 0.
  if (Nv >= 2)
  {
    return gp_Vec(0.0, 0.0, 0.0);
  }

  const gp_XYZ& aXD = pos.XDirection().XYZ();
  const gp_XYZ& aYD = pos.YDirection().XYZ();
  const gp_XYZ& aZD = pos.Direction().XYZ();

  // d^Nu/du^Nu[cos(u)] = cos(u + Nu*Pi/2)
  // d^Nu/du^Nu[sin(u)] = sin(u + Nu*Pi/2)
  const double aPhaseU = Nu * M_PI / 2.0;

  if (Nv == 0)
  {
    // d^Nu/du^Nu of [v*cos(u)*XD + v*sin(u)*YD + P*u/(2*Pi)*ZD]
    const double aCoeffX = V * std::cos(U + aPhaseU);
    const double aCoeffY = V * std::sin(U + aPhaseU);

    gp_XYZ aResult = aCoeffX * aXD + aCoeffY * aYD;

    // Z term: d^Nu/du^Nu[P*u/(2*Pi)] = P/(2*Pi) if Nu=1, 0 if Nu>=2
    if (Nu == 1)
    {
      aResult += (myPitch / (2.0 * M_PI)) * aZD;
    }

    return gp_Vec(aResult);
  }

  // Nv == 1: d/dv of d^Nu/du^Nu [v*cos(u)*XD + v*sin(u)*YD]
  // = d^Nu/du^Nu[cos(u)]*XD + d^Nu/du^Nu[sin(u)]*YD
  const double aCoeffX = std::cos(U + aPhaseU);
  const double aCoeffY = std::sin(U + aPhaseU);

  return gp_Vec(aCoeffX * aXD + aCoeffY * aYD);
}

//==================================================================================================

void GeomEval_CircularHelicoidSurface::Transform(const gp_Trsf& T)
{
  myPitch *= T.ScaleFactor();
  pos.Transform(T);
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_CircularHelicoidSurface::Copy() const
{
  return new GeomEval_CircularHelicoidSurface(pos, myPitch);
}

//==================================================================================================

void GeomEval_CircularHelicoidSurface::DumpJson(Standard_OStream& theOStream,
                                                int               theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPitch)
}
