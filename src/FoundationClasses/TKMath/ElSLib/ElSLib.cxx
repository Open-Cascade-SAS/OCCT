// Created on: 1991-09-09
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2026 OPEN CASCADE SAS
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

#ifndef No_Exception
  #define No_Exception
#endif

#include <ElSLib.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{
static constexpr double PIPI = M_PI + M_PI;
// Threshold for angle normalization to avoid discontinuity near zero
static constexpr double NEGATIVE_RESOLUTION = -Precision::Computational();

// Normalize angle to [0, 2*PI] range, with special handling
// for values very close to zero to avoid discontinuity.
// Preserves values at exactly 2*PI for proper seam handling.
static inline void normalizeAngle(double& theAngle)
{
  while (theAngle < NEGATIVE_RESOLUTION)
  {
    theAngle += PIPI;
  }
  // Only normalize angles strictly greater than 2*PI (with small tolerance)
  // to preserve the closing seam value of exactly 2*PI
  while (theAngle > PIPI * (1.0 + gp::Resolution()))
  {
    theAngle -= PIPI;
  }
  if (theAngle < 0.)
  {
    theAngle = 0.;
  }
}

//! Sine/cosine pair used by torus evaluators.
struct TorusSinCos
{
  double Sin;
  double Cos;
};

//! Returns a machine-level bound for canonicalizing a trigonometric zero.
//! The bound is expressed in parameter space and is capped by one torus period,
//! so it does not grow for periodically equivalent large parameters.
static double torusTrigResolution(const double theParameter)
{
  const double aMagnitude = std::abs(theParameter);
  const double aPeriodScale = aMagnitude < PIPI ? aMagnitude : PIPI;
  return Precision::Computational() * (1.0 + aPeriodScale);
}

//! Canonicalizes a sine/cosine pair at numerically exact quadrantal parameters.
//! The companion component is restored to exactly +/-1 to preserve an orthonormal frame.
static TorusSinCos torusSinCos(const double theParameter)
{
  if (theParameter == 0.0)
  {
    return {0.0, 1.0};
  }

  TorusSinCos aResult = {std::sin(theParameter), std::cos(theParameter)};
  const double aResolution = torusTrigResolution(theParameter);

  if (std::abs(aResult.Sin) <= aResolution)
  {
    aResult.Sin = 0.0;
    aResult.Cos = std::copysign(1.0, aResult.Cos);
  }
  else if (std::abs(aResult.Cos) <= aResolution)
  {
    aResult.Cos = 0.0;
    aResult.Sin = std::copysign(1.0, aResult.Sin);
  }
  return aResult;
}

//! Evaluates only sine when cosine is not required.
static double torusSin(const double theParameter)
{
  if (theParameter == 0.0)
  {
    return 0.0;
  }

  const double aValue = std::sin(theParameter);
  return std::abs(aValue) <= torusTrigResolution(theParameter) ? 0.0 : aValue;
}

//! Evaluates only cosine when sine is not required.
static double torusCos(const double theParameter)
{
  if (theParameter == 0.0)
  {
    return 1.0;
  }

  const double aValue = std::cos(theParameter);
  return std::abs(aValue) <= torusTrigResolution(theParameter) ? 0.0 : aValue;
}

struct TorusSection
{
  double Radius;
  double MinorCos;
};

//! Computes r*cos(v) and R + r*cos(v).  The section radius uses one rounded
//! multiply-add; a result below the accumulated input roundoff represents a
//! horn/spindle singular circle and is canonicalized to exact zero.
static TorusSection torusSection(const double theMajorRadius,
                                 const double theMinorRadius,
                                 const double theCosV)
{
  TorusSection aResult = {
    std::fma(theMinorRadius, theCosV, theMajorRadius), theMinorRadius * theCosV};

  if (!std::isfinite(aResult.Radius) || !std::isfinite(aResult.MinorCos))
  {
    return aResult;
  }

  const double aScale = std::abs(theMajorRadius) + std::abs(aResult.MinorCos);
  if (std::isfinite(aScale)
      && std::abs(aResult.Radius) <= Precision::Computational() * aScale)
  {
    aResult.Radius = 0.0;
  }
  return aResult;
}

//! Returns a scaled copy of an XYZ vector.
static gp_XYZ scaledXYZ(const gp_XYZ& theVector, const double theScale)
{
  gp_XYZ aResult = theVector;
  aResult.Multiply(theScale);
  return aResult;
}

//! Returns A * theScaleA + B * theScaleB.
static gp_XYZ combinedXYZ(const gp_XYZ& theA,
                          const double  theScaleA,
                          const gp_XYZ& theB,
                          const double  theScaleB)
{
  gp_XYZ aResult = scaledXYZ(theA, theScaleA);
  aResult.Add(scaledXYZ(theB, theScaleB));
  return aResult;
}

//! Returns d^theOrder/du^theOrder (cos(u) * X + sin(u) * Y).
static gp_XYZ torusRadialDerivative(const gp_XYZ&      theXDirection,
                                    const gp_XYZ&      theYDirection,
                                    const TorusSinCos& theTrig,
                                    const int          theOrder)
{
  double aXCoeff = 0.0;
  double aYCoeff = 0.0;
  switch (theOrder & 3)
  {
    case 0:
      aXCoeff = theTrig.Cos;
      aYCoeff = theTrig.Sin;
      break;
    case 1:
      aXCoeff = -theTrig.Sin;
      aYCoeff = theTrig.Cos;
      break;
    case 2:
      aXCoeff = -theTrig.Cos;
      aYCoeff = -theTrig.Sin;
      break;
    default:
      aXCoeff = theTrig.Sin;
      aYCoeff = -theTrig.Cos;
      break;
  }

  gp_XYZ aResult = theXDirection;
  aResult.Multiply(aXCoeff);
  gp_XYZ aYPart = theYDirection;
  aYPart.Multiply(aYCoeff);
  aResult.Add(aYPart);
  return aResult;
}

//! Returns the requested derivative of cos(parameter) from a previously evaluated pair.
static double torusCosDerivative(const TorusSinCos& theTrig, const int theOrder)
{
  switch (theOrder & 3)
  {
    case 0:
      return theTrig.Cos;
    case 1:
      return -theTrig.Sin;
    case 2:
      return -theTrig.Cos;
    default:
      return theTrig.Sin;
  }
}

//! Returns the requested derivative of sin(parameter) from a previously evaluated pair.
static double torusSinDerivative(const TorusSinCos& theTrig, const int theOrder)
{
  switch (theOrder & 3)
  {
    case 0:
      return theTrig.Sin;
    case 1:
      return theTrig.Cos;
    case 2:
      return -theTrig.Sin;
    default:
      return -theTrig.Cos;
  }
}

//! Returns the requested derivative of cos(parameter), evaluating only the
//! trigonometric component required by the derivative order.
static double torusCosDerivative(const double theParameter, const int theOrder)
{
  switch (theOrder & 3)
  {
    case 0:
      return torusCos(theParameter);
    case 1:
      return -torusSin(theParameter);
    case 2:
      return -torusCos(theParameter);
    default:
      return torusSin(theParameter);
  }
}

struct TorusEvaluation
{
  gp_XYZ Radial;
  gp_XYZ Tangent;
  double Radius;
  double MinorCosV;
  double MinorSinV;
};

//! Computes the common torus frame and scalar terms used by D1-D3.
static TorusEvaluation torusEvaluation(const double  theU,
                                       const double  theV,
                                       const gp_Ax3& thePosition,
                                       const double  theMajorRadius,
                                       const double  theMinorRadius)
{
  const TorusSinCos aUTrig = torusSinCos(theU);
  const TorusSinCos aVTrig = torusSinCos(theV);

  const TorusSection aSection = torusSection(theMajorRadius, theMinorRadius, aVTrig.Cos);
  return {
    torusRadialDerivative(thePosition.XDirection().XYZ(), thePosition.YDirection().XYZ(), aUTrig, 0),
    torusRadialDerivative(thePosition.XDirection().XYZ(), thePosition.YDirection().XYZ(), aUTrig, 1),
    aSection.Radius,
    aSection.MinorCos,
    theMinorRadius * aVTrig.Sin};
}

//! Computes a torus point without constructing the U-tangent required by derivative evaluators.
static gp_XYZ torusPoint(const double  theU,
                         const double  theV,
                         const gp_Ax3& thePosition,
                         const double  theMajorRadius,
                         const double  theMinorRadius)
{
  const TorusSinCos aUTrig = torusSinCos(theU);
  const TorusSinCos aVTrig = torusSinCos(theV);
  const gp_XYZ aRadial =
    torusRadialDerivative(thePosition.XDirection().XYZ(), thePosition.YDirection().XYZ(), aUTrig, 0);

  const TorusSection aSection = torusSection(theMajorRadius, theMinorRadius, aVTrig.Cos);
  gp_XYZ aPoint = combinedXYZ(aRadial,
                              aSection.Radius,
                              thePosition.Direction().XYZ(),
                              theMinorRadius * aVTrig.Sin);
  aPoint.Add(thePosition.Location().XYZ());
  return aPoint;
}
} // namespace

gp_Pnt ElSLib::PlaneValue(const double U, const double V, const gp_Ax3& Pos)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  return gp_Pnt(U * XDir.X() + V * YDir.X() + PLoc.X(),
                U * XDir.Y() + V * YDir.Y() + PLoc.Y(),
                U * XDir.Z() + V * YDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::ConeValue(const double  U,
                         const double  V,
                         const gp_Ax3& Pos,
                         const double  Radius,
                         const double  SAngle)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        R    = Radius + V * sin(SAngle);
  double        A3   = V * cos(SAngle);
  double        A1   = R * cos(U);
  double        A2   = R * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X(),
                A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y(),
                A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::CylinderValue(const double U, const double V, const gp_Ax3& Pos, const double Radius)
{
  // M(u,v) = C + Radius * ( Xdir * std::cos(u) + Ydir * std::sin(u)) + V * Zdir
  // where C is the location point of the Axis2placement
  // Xdir, Ydir ,Zdir are the directions of the local coordinates system

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        A1   = Radius * cos(U);
  double        A2   = Radius * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + V * ZDir.X() + PLoc.X(),
                A1 * XDir.Y() + A2 * YDir.Y() + V * ZDir.Y() + PLoc.Y(),
                A1 * XDir.Z() + A2 * YDir.Z() + V * ZDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::SphereValue(const double U, const double V, const gp_Ax3& Pos, const double Radius)
{
  // M(U,V) = Location +
  //          R * CosV (CosU * XDirection + SinU * YDirection) +
  //          R * SinV * Direction

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        R    = Radius * cos(V);
  double        A3   = Radius * sin(V);
  double        A1   = R * cos(U);
  double        A2   = R * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X(),
                A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y(),
                A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::TorusValue(const double  U,
                          const double  V,
                          const gp_Ax3& Pos,
                          const double  MajorRadius,
                          const double  MinorRadius)
{
  return gp_Pnt(torusPoint(U, V, Pos, MajorRadius, MinorRadius));
}

gp_Vec ElSLib::PlaneDN(const double, const double, const gp_Ax3& Pos, const int Nu, const int Nv)
{
  if (Nu == 0 && Nv == 1)
  {
    return gp_Vec(Pos.YDirection());
  }
  else if (Nu == 1 && Nv == 0)
  {
    return gp_Vec(Pos.XDirection());
  }
  return gp_Vec(0., 0., 0.);
}

gp_Vec ElSLib::ConeDN(const double  U,
                      const double  V,
                      const gp_Ax3& Pos,
                      const double  Radius,
                      const double  SAngle,
                      const int     Nu,
                      const int     Nv)
{
  gp_XYZ Xdir = Pos.XDirection().XYZ();
  gp_XYZ Ydir = Pos.YDirection().XYZ();
  double Um   = U + Nu * M_PI_2; // M_PI * 0.5
  Xdir.Multiply(cos(Um));
  Ydir.Multiply(sin(Um));
  Xdir.Add(Ydir);
  if (Nv == 0)
  {
    Xdir.Multiply(Radius + V * sin(SAngle));
    if (Nu == 0)
    {
      Xdir.Add(Pos.Location().XYZ());
    }
    return gp_Vec(Xdir);
  }
  else if (Nv == 1)
  {
    Xdir.Multiply(sin(SAngle));
    if (Nu == 0)
    {
      Xdir.Add(Pos.Direction().XYZ() * cos(SAngle));
    }
    return gp_Vec(Xdir);
  }
  return gp_Vec(0.0, 0.0, 0.0);
}

gp_Vec ElSLib::CylinderDN(const double U,
                          const double,
                          const gp_Ax3& Pos,
                          const double  Radius,
                          const int     Nu,
                          const int     Nv)
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    return gp_Vec();
  }
  if (Nv == 0)
  {
    double RCosU = Radius * cos(U);
    double RSinU = Radius * sin(U);
    gp_XYZ Xdir  = Pos.XDirection().XYZ();
    gp_XYZ Ydir  = Pos.YDirection().XYZ();
    if ((Nu + 6) % 4 == 0)
    {
      Xdir.Multiply(-RCosU);
      Ydir.Multiply(-RSinU);
    }
    else if ((Nu + 5) % 4 == 0)
    {
      Xdir.Multiply(RSinU);
      Ydir.Multiply(-RCosU);
    }
    else if ((Nu + 3) % 4 == 0)
    {
      Xdir.Multiply(-RSinU);
      Ydir.Multiply(RCosU);
    }
    else if (Nu % 4 == 0)
    {
      Xdir.Multiply(RCosU);
      Ydir.Multiply(RSinU);
    }
    Xdir.Add(Ydir);
    return gp_Vec(Xdir);
  }
  else if (Nv == 1 && Nu == 0)
  {
    return gp_Vec(Pos.Direction());
  }
  else
  {
    return gp_Vec(0.0, 0.0, 0.0);
  }
}

gp_Vec ElSLib::SphereDN(const double  U,
                        const double  V,
                        const gp_Ax3& Pos,
                        const double  Radius,
                        const int     Nu,
                        const int     Nv)
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    return gp_Vec();
  }
  double        CosU  = cos(U);
  double        SinU  = sin(U);
  double        RCosV = Radius * cos(V);
  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  double        A1, A2, A3, X, Y, Z;
  if (Nu == 0)
  {
    double RSinV = Radius * sin(V);
    if (IsOdd(Nv))
    {
      A1 = -RSinV * CosU;
      A2 = -RSinV * SinU;
      A3 = RCosV;
    }
    else
    {
      A1 = -RCosV * CosU;
      A2 = -RCosV * SinU;
      A3 = -RSinV;
    }
    X = A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X();
    Y = A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y();
    Z = A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z();
    if ((Nv + 2) % 4 != 0 && (Nv + 3) % 4 != 0)
    {
      X = -X;
      Y = -Y;
      Z = -Z;
    }
  }
  else if (Nv == 0)
  {
    if (IsOdd(Nu))
    {
      A1 = -RCosV * SinU;
      A2 = RCosV * CosU;
    }
    else
    {
      A1 = RCosV * CosU;
      A2 = RCosV * SinU;
    }
    X = A1 * XDir.X() + A2 * YDir.X();
    Y = A1 * XDir.Y() + A2 * YDir.Y();
    Z = A1 * XDir.Z() + A2 * YDir.Z();
    if ((Nu + 2) % 4 == 0 || (Nu + 1) % 4 == 0)
    {
      X = -X;
      Y = -Y;
      Z = -Z;
    }
  }
  else
  {
    double RSinV = Radius * sin(V);
    if (IsOdd(Nu))
    {
      A1 = -SinU;
      A2 = CosU;
    }
    else
    {
      A1 = -CosU;
      A2 = -SinU;
    }
    if (IsOdd(Nv))
    {
      A3 = -RSinV;
    }
    else
    {
      A3 = -RCosV;
    }
    X = (A1 * XDir.X() + A2 * YDir.X()) * A3;
    Y = (A1 * XDir.Y() + A2 * YDir.Y()) * A3;
    Z = (A1 * XDir.Z() + A2 * YDir.Z()) * A3;
    if (((Nu + 2) % 4 != 0 && (Nu + 3) % 4 != 0 && ((Nv + 2) % 4 == 0 || (Nv + 3) % 4 == 0))
        || (((Nu + 2) % 4 == 0 || (Nu + 3) % 4 == 0) && (Nv + 2) % 4 != 0 && (Nv + 3) % 4 != 0))
    {
      X = -X;
      Y = -Y;
      Z = -Z;
    }
  }
  return gp_Vec(X, Y, Z);
}

gp_Vec ElSLib::TorusDN(const double  U,
                       const double  V,
                       const gp_Ax3& Pos,
                       const double  MajorRadius,
                       const double  MinorRadius,
                       const int     Nu,
                       const int     Nv)
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    return gp_Vec();
  }

  const TorusSinCos aUTrig = torusSinCos(U);
  const gp_XYZ aUDerivative =
    torusRadialDerivative(Pos.XDirection().XYZ(), Pos.YDirection().XYZ(), aUTrig, Nu);

  if (Nv == 0)
  {
    // Pure U derivative requires only cos(V).
    const double aRadius = torusSection(MajorRadius, MinorRadius, torusCos(V)).Radius;
    return gp_Vec(scaledXYZ(aUDerivative, aRadius));
  }

  if (Nu > 0)
  {
    // Mixed derivatives depend only on a derivative of cos(V).
    const double aCosDerivative = MinorRadius * torusCosDerivative(V, Nv);
    return gp_Vec(scaledXYZ(aUDerivative, aCosDerivative));
  }

  // Pure V derivatives require both section components; evaluate the pair once.
  const TorusSinCos aVTrig          = torusSinCos(V);
  const double aCosDerivative       = MinorRadius * torusCosDerivative(aVTrig, Nv);
  const double aSinDerivative       = MinorRadius * torusSinDerivative(aVTrig, Nv);
  return gp_Vec(combinedXYZ(aUDerivative,
                            aCosDerivative,
                            Pos.Direction().XYZ(),
                            aSinDerivative));
}

void ElSLib::PlaneD0(const double U, const double V, const gp_Ax3& Pos, gp_Pnt& P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  P.SetX(U * XDir.X() + V * YDir.X() + PLoc.X());
  P.SetY(U * XDir.Y() + V * YDir.Y() + PLoc.Y());
  P.SetZ(U * XDir.Z() + V * YDir.Z() + PLoc.Z());
}

void ElSLib::ConeD0(const double  U,
                    const double  V,
                    const gp_Ax3& Pos,
                    const double  Radius,
                    const double  SAngle,
                    gp_Pnt&       P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        R    = Radius + V * sin(SAngle);
  double        A3   = V * cos(SAngle);
  double        A1   = R * cos(U);
  double        A2   = R * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

void ElSLib::CylinderD0(const double  U,
                        const double  V,
                        const gp_Ax3& Pos,
                        const double  Radius,
                        gp_Pnt&       P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        A1   = Radius * cos(U);
  double        A2   = Radius * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + V * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + V * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + V * ZDir.Z() + PLoc.Z());
}

void ElSLib::SphereD0(const double  U,
                      const double  V,
                      const gp_Ax3& Pos,
                      const double  Radius,
                      gp_Pnt&       P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        R    = Radius * cos(V);
  double        A3   = Radius * sin(V);
  double        A1   = R * cos(U);
  double        A2   = R * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

void ElSLib::TorusD0(const double  U,
                     const double  V,
                     const gp_Ax3& Pos,
                     const double  MajorRadius,
                     const double  MinorRadius,
                     gp_Pnt&       P)
{
  P.SetXYZ(torusPoint(U, V, Pos, MajorRadius, MinorRadius));
}

void ElSLib::PlaneD1(const double  U,
                     const double  V,
                     const gp_Ax3& Pos,
                     gp_Pnt&       P,
                     gp_Vec&       Vu,
                     gp_Vec&       Vv)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  P.SetX(U * XDir.X() + V * YDir.X() + PLoc.X());
  P.SetY(U * XDir.Y() + V * YDir.Y() + PLoc.Y());
  P.SetZ(U * XDir.Z() + V * YDir.Z() + PLoc.Z());
  Vu.SetX(XDir.X());
  Vu.SetY(XDir.Y());
  Vu.SetZ(XDir.Z());
  Vv.SetX(YDir.X());
  Vv.SetY(YDir.Y());
  Vv.SetZ(YDir.Z());
}

void ElSLib::ConeD1(const double  U,
                    const double  V,
                    const gp_Ax3& Pos,
                    const double  Radius,
                    const double  SAngle,
                    gp_Pnt&       P,
                    gp_Vec&       Vu,
                    gp_Vec&       Vv)
{
  // Z = V * std::cos(SAngle)
  // M(U,V) = Location() + V * std::cos(SAngle) * ZDirection() +
  // (Radius + V*Sin(SAng)) * (std::cos(U) * XDirection() + std::sin(U) * YDirection())

  // D1U =
  //(Radius + V*Sin(SAng)) * (-std::sin(U) * XDirection() + std::cos(U) * YDirection())

  // D1V =
  // Direction() *std::cos(SAngle) + std::sin(SAng) * (std::cos(U) * XDirection() +
  // std::sin(U) * YDirection())

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        CosU = cos(U);
  double        SinU = sin(U);
  double        CosA = cos(SAngle);
  double        SinA = sin(SAngle);
  double        R    = Radius + V * SinA;
  double        A3   = V * CosA;
  double        A1   = R * CosU;
  double        A2   = R * SinU;
  double        R1   = SinA * CosU;
  double        R2   = SinA * SinU;
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
  Vu.SetX(-A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(-A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(-A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(R1 * XDir.X() + R2 * YDir.X() + CosA * ZDir.X());
  Vv.SetY(R1 * XDir.Y() + R2 * YDir.Y() + CosA * ZDir.Y());
  Vv.SetZ(R1 * XDir.Z() + R2 * YDir.Z() + CosA * ZDir.Z());
}

void ElSLib::CylinderD1(const double  U,
                        const double  V,
                        const gp_Ax3& Pos,
                        const double  Radius,
                        gp_Pnt&       P,
                        gp_Vec&       Vu,
                        gp_Vec&       Vv)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        A1   = Radius * cos(U);
  double        A2   = Radius * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + V * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + V * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + V * ZDir.Z() + PLoc.Z());
  Vu.SetX(-A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(-A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(-A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(ZDir.X());
  Vv.SetY(ZDir.Y());
  Vv.SetZ(ZDir.Z());
}

void ElSLib::SphereD1(const double  U,
                      const double  V,
                      const gp_Ax3& Pos,
                      const double  Radius,
                      gp_Pnt&       P,
                      gp_Vec&       Vu,
                      gp_Vec&       Vv)
{
  // Vxy = CosU * XDirection + SinU * YDirection
  // DVxy = -SinU * XDirection + CosU * YDirection

  // P(U,V) = Location +  R * CosV * Vxy  +   R * SinV * Direction

  // Vu = R * CosV * DVxy

  // Vv = -R * SinV * Vxy + R * CosV * Direction

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location().XYZ();
  double        CosU = cos(U);
  double        SinU = sin(U);
  double        R1   = Radius * cos(V);
  double        R2   = Radius * sin(V);
  double        A1   = R1 * CosU;
  double        A2   = R1 * SinU;
  double        A3   = R2 * CosU;
  double        A4   = R2 * SinU;
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + R2 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + R2 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + R2 * ZDir.Z() + PLoc.Z());
  Vu.SetX(-A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(-A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(-A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(-A3 * XDir.X() - A4 * YDir.X() + R1 * ZDir.X());
  Vv.SetY(-A3 * XDir.Y() - A4 * YDir.Y() + R1 * ZDir.Y());
  Vv.SetZ(-A3 * XDir.Z() - A4 * YDir.Z() + R1 * ZDir.Z());
}

void ElSLib::TorusD1(const double  U,
                     const double  V,
                     const gp_Ax3& Pos,
                     const double  MajorRadius,
                     const double  MinorRadius,
                     gp_Pnt&       P,
                     gp_Vec&       Vu,
                     gp_Vec&       Vv)
{
  const TorusEvaluation aData = torusEvaluation(U, V, Pos, MajorRadius, MinorRadius);

  gp_XYZ aPoint = combinedXYZ(aData.Radial, aData.Radius, Pos.Direction().XYZ(), aData.MinorSinV);
  aPoint.Add(Pos.Location().XYZ());
  P  = gp_Pnt(aPoint);
  Vu = gp_Vec(scaledXYZ(aData.Tangent, aData.Radius));
  Vv = gp_Vec(combinedXYZ(aData.Radial,
                          -aData.MinorSinV,
                          Pos.Direction().XYZ(),
                          aData.MinorCosV));
}

void ElSLib::ConeD2(const double  U,
                    const double  V,
                    const gp_Ax3& Pos,
                    const double  Radius,
                    const double  SAngle,
                    gp_Pnt&       P,
                    gp_Vec&       Vu,
                    gp_Vec&       Vv,
                    gp_Vec&       Vuu,
                    gp_Vec&       Vvv,
                    gp_Vec&       Vuv)
{
  // Z = V * std::cos(SAngle)
  // M(U,V) = Location() + V * std::cos(SAngle) * Direction() +
  // (Radius + V*Sin(SAng)) * (std::cos(U) * XDirection() + std::sin(U) * YDirection())

  // DU =
  //(Radius + V*Sin(SAng)) * (-std::sin(U) * XDirection() + std::cos(U) * YDirection())

  // DV =
  // Direction() *std::cos(SAngle) + std::sin(SAng) * (std::cos(U) * XDirection() +
  // std::sin(U) * YDirection())

  // D2U =
  //(Radius + V*Sin(SAng)) * (-std::cos(U) * XDirection() - std::sin(U) * YDirection())

  // D2V = 0.0

  // DUV =
  // std::sin(SAng) * (-std::sin(U) * XDirection() + std::cos(U) * YDirection())

  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  const gp_XYZ& PLoc  = Pos.Location().XYZ();
  double        CosU  = cos(U);
  double        SinU  = sin(U);
  double        CosA  = cos(SAngle);
  double        SinA  = sin(SAngle);
  double        R     = Radius + V * SinA;
  double        A3    = V * CosA;
  double        A1    = R * CosU;
  double        A2    = R * SinU;
  double        R1    = SinA * CosU;
  double        R2    = SinA * SinU;
  double        Som1X = A1 * XDir.X() + A2 * YDir.X();
  double        Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  double        Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  P.SetX(Som1X + A3 * ZDir.X() + PLoc.X());
  P.SetY(Som1Y + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(Som1Z + A3 * ZDir.Z() + PLoc.Z());
  Vu.SetX(-A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(-A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(-A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(R1 * XDir.X() + R2 * YDir.X() + CosA * ZDir.X());
  Vv.SetY(R1 * XDir.Y() + R2 * YDir.Y() + CosA * ZDir.Y());
  Vv.SetZ(R1 * XDir.Z() + R2 * YDir.Z() + CosA * ZDir.Z());
  Vuu.SetX(-Som1X);
  Vuu.SetY(-Som1Y);
  Vuu.SetZ(-Som1Z);
  Vvv.SetX(0.0);
  Vvv.SetY(0.0);
  Vvv.SetZ(0.0);
  Vuv.SetX(-R2 * XDir.X() + R1 * YDir.X());
  Vuv.SetY(-R2 * XDir.Y() + R1 * YDir.Y());
  Vuv.SetZ(-R2 * XDir.Z() + R1 * YDir.Z());
}

void ElSLib::CylinderD2(const double  U,
                        const double  V,
                        const gp_Ax3& Pos,
                        const double  Radius,
                        gp_Pnt&       P,
                        gp_Vec&       Vu,
                        gp_Vec&       Vv,
                        gp_Vec&       Vuu,
                        gp_Vec&       Vvv,
                        gp_Vec&       Vuv)
{
  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  const gp_XYZ& PLoc  = Pos.Location().XYZ();
  double        A1    = Radius * cos(U);
  double        A2    = Radius * sin(U);
  double        Som1X = A1 * XDir.X() + A2 * YDir.X();
  double        Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  double        Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  P.SetX(Som1X + V * ZDir.X() + PLoc.X());
  P.SetY(Som1Y + V * ZDir.Y() + PLoc.Y());
  P.SetZ(Som1Z + V * ZDir.Z() + PLoc.Z());
  Vu.SetX(-A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(-A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(-A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(ZDir.X());
  Vv.SetY(ZDir.Y());
  Vv.SetZ(ZDir.Z());
  Vuu.SetX(-Som1X);
  Vuu.SetY(-Som1Y);
  Vuu.SetZ(-Som1Z);
  Vvv.SetX(0.0);
  Vvv.SetY(0.0);
  Vvv.SetZ(0.0);
  Vuv.SetX(0.0);
  Vuv.SetY(0.0);
  Vuv.SetZ(0.0);
}

void ElSLib::SphereD2(const double  U,
                      const double  V,
                      const gp_Ax3& Pos,
                      const double  Radius,
                      gp_Pnt&       P,
                      gp_Vec&       Vu,
                      gp_Vec&       Vv,
                      gp_Vec&       Vuu,
                      gp_Vec&       Vvv,
                      gp_Vec&       Vuv)
{
  // Vxy = CosU * XDirection + SinU * YDirection
  // DVxy = -SinU * XDirection + CosU * YDirection

  // P(U,V) = Location +  R * CosV * Vxy  +   R * SinV * Direction

  // Vu = R * CosV * DVxy

  // Vuu = - R * CosV * Vxy

  // Vv = -R * SinV * Vxy + R * CosV * Direction

  // Vvv = -R * CosV * Vxy - R * SinV * Direction

  // Vuv = - R * SinV * DVxy

  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  const gp_XYZ& PLoc  = Pos.Location().XYZ();
  double        CosU  = cos(U);
  double        SinU  = sin(U);
  double        R1    = Radius * cos(V);
  double        R2    = Radius * sin(V);
  double        A1    = R1 * CosU;
  double        A2    = R1 * SinU;
  double        A3    = R2 * CosU;
  double        A4    = R2 * SinU;
  double        Som1X = A1 * XDir.X() + A2 * YDir.X();
  double        Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  double        Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  double        R2ZX  = R2 * ZDir.X();
  double        R2ZY  = R2 * ZDir.Y();
  double        R2ZZ  = R2 * ZDir.Z();
  P.SetX(Som1X + R2ZX + PLoc.X());
  P.SetY(Som1Y + R2ZY + PLoc.Y());
  P.SetZ(Som1Z + R2ZZ + PLoc.Z());
  Vu.SetX(-A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(-A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(-A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(-A3 * XDir.X() - A4 * YDir.X() + R1 * ZDir.X());
  Vv.SetY(-A3 * XDir.Y() - A4 * YDir.Y() + R1 * ZDir.Y());
  Vv.SetZ(-A3 * XDir.Z() - A4 * YDir.Z() + R1 * ZDir.Z());
  Vuu.SetX(-Som1X);
  Vuu.SetY(-Som1Y);
  Vuu.SetZ(-Som1Z);
  Vvv.SetX(-Som1X - R2ZX);
  Vvv.SetY(-Som1Y - R2ZY);
  Vvv.SetZ(-Som1Z - R2ZZ);
  Vuv.SetX(A4 * XDir.X() - A3 * YDir.X());
  Vuv.SetY(A4 * XDir.Y() - A3 * YDir.Y());
  Vuv.SetZ(A4 * XDir.Z() - A3 * YDir.Z());
}

void ElSLib::TorusD2(const double  U,
                     const double  V,
                     const gp_Ax3& Pos,
                     const double  MajorRadius,
                     const double  MinorRadius,
                     gp_Pnt&       P,
                     gp_Vec&       Vu,
                     gp_Vec&       Vv,
                     gp_Vec&       Vuu,
                     gp_Vec&       Vvv,
                     gp_Vec&       Vuv)
{
  const TorusEvaluation aData = torusEvaluation(U, V, Pos, MajorRadius, MinorRadius);

  gp_XYZ aPoint = combinedXYZ(aData.Radial, aData.Radius, Pos.Direction().XYZ(), aData.MinorSinV);
  aPoint.Add(Pos.Location().XYZ());
  P   = gp_Pnt(aPoint);
  Vu  = gp_Vec(scaledXYZ(aData.Tangent, aData.Radius));
  Vv  = gp_Vec(combinedXYZ(aData.Radial,
                           -aData.MinorSinV,
                           Pos.Direction().XYZ(),
                           aData.MinorCosV));
  Vuu = gp_Vec(scaledXYZ(aData.Radial, -aData.Radius));
  Vvv = gp_Vec(combinedXYZ(aData.Radial,
                           -aData.MinorCosV,
                           Pos.Direction().XYZ(),
                           -aData.MinorSinV));
  Vuv = gp_Vec(scaledXYZ(aData.Tangent, -aData.MinorSinV));
}

void ElSLib::ConeD3(const double  U,
                    const double  V,
                    const gp_Ax3& Pos,
                    const double  Radius,
                    const double  SAngle,
                    gp_Pnt&       P,
                    gp_Vec&       Vu,
                    gp_Vec&       Vv,
                    gp_Vec&       Vuu,
                    gp_Vec&       Vvv,
                    gp_Vec&       Vuv,
                    gp_Vec&       Vuuu,
                    gp_Vec&       Vvvv,
                    gp_Vec&       Vuuv,
                    gp_Vec&       Vuvv)
{
  // Z = V * std::cos(SAngle)
  // M(U,V) = Location() + V * std::cos(SAngle) * Direction() +
  // (Radius + V*Sin(SAng)) * (std::cos(U) * XDirection() + std::sin(U) * YDirection())

  // DU =
  //(Radius + V*Sin(SAng)) * (-std::sin(U) * XDirection() + std::cos(U) * YDirection())

  // DV =
  // Direction() *std::cos(SAngle) + std::sin(SAng) * (std::cos(U) * XDirection() +
  // std::sin(U) * YDirection())

  // D2U =
  //(Radius + V*Sin(SAng)) * (-std::cos(U) * XDirection() - std::sin(U) * YDirection())

  // D2V = 0.0

  // DUV =
  // std::sin(SAng) * (-std::sin(U) * XDirection() + std::cos(U) * YDirection())

  // D3U =
  //(Radius + V*Sin(SAng)) * (std::sin(U) * XDirection() - std::cos(U) * YDirection())

  // DUVV = 0.0

  // D3V = 0.0

  // DUUV =  std::sin(SAng) * (-std::cos(U)*XDirection()-std::sin(U) * YDirection()) +

  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  const gp_XYZ& PLoc  = Pos.Location().XYZ();
  double        CosU  = cos(U);
  double        SinU  = sin(U);
  double        CosA  = cos(SAngle);
  double        SinA  = sin(SAngle);
  double        R     = Radius + V * SinA;
  double        A3    = V * CosA;
  double        A1    = R * CosU;
  double        A2    = R * SinU;
  double        R1    = SinA * CosU;
  double        R2    = SinA * SinU;
  double        Som1X = A1 * XDir.X() + A2 * YDir.X();
  double        Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  double        Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  double        Som2X = R1 * XDir.X() + R2 * YDir.X();
  double        Som2Y = R1 * XDir.Y() + R2 * YDir.Y();
  double        Som2Z = R1 * XDir.Z() + R2 * YDir.Z();
  double        Dif1X = A2 * XDir.X() - A1 * YDir.X();
  double        Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  double        Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  P.SetX(Som1X + A3 * ZDir.X() + PLoc.X());
  P.SetY(Som1Y + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(Som1Z + A3 * ZDir.Z() + PLoc.Z());
  Vu.SetX(-Dif1X);
  Vu.SetY(-Dif1Y);
  Vu.SetZ(-Dif1Z);
  Vv.SetX(Som2X + CosA * ZDir.X());
  Vv.SetY(Som2Y + CosA * ZDir.Y());
  Vv.SetZ(Som2Z + CosA * ZDir.Z());
  Vuu.SetX(-Som1X);
  Vuu.SetY(-Som1Y);
  Vuu.SetZ(-Som1Z);
  Vvv.SetX(0.0);
  Vvv.SetY(0.0);
  Vvv.SetZ(0.0);
  Vuv.SetX(-R2 * XDir.X() + R1 * YDir.X());
  Vuv.SetY(-R2 * XDir.Y() + R1 * YDir.Y());
  Vuv.SetZ(-R2 * XDir.Z() + R1 * YDir.Z());
  Vuuu.SetX(Dif1X);
  Vuuu.SetY(Dif1Y);
  Vuuu.SetZ(Dif1Z);
  Vvvv.SetX(0.0);
  Vvvv.SetY(0.0);
  Vvvv.SetZ(0.0);
  Vuvv.SetX(0.0);
  Vuvv.SetY(0.0);
  Vuvv.SetZ(0.0);
  Vuuv.SetX(-Som2X);
  Vuuv.SetY(-Som2Y);
  Vuuv.SetZ(-Som2Z);
}

void ElSLib::CylinderD3(const double  U,
                        const double  V,
                        const gp_Ax3& Pos,
                        const double  Radius,
                        gp_Pnt&       P,
                        gp_Vec&       Vu,
                        gp_Vec&       Vv,
                        gp_Vec&       Vuu,
                        gp_Vec&       Vvv,
                        gp_Vec&       Vuv,
                        gp_Vec&       Vuuu,
                        gp_Vec&       Vvvv,
                        gp_Vec&       Vuuv,
                        gp_Vec&       Vuvv)
{
  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  const gp_XYZ& PLoc  = Pos.Location().XYZ();
  double        A1    = Radius * cos(U);
  double        A2    = Radius * sin(U);
  double        Som1X = A1 * XDir.X() + A2 * YDir.X();
  double        Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  double        Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  double        Dif1X = A2 * XDir.X() - A1 * YDir.X();
  double        Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  double        Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  P.SetX(Som1X + V * ZDir.X() + PLoc.X());
  P.SetY(Som1Y + V * ZDir.Y() + PLoc.Y());
  P.SetZ(Som1Z + V * ZDir.Z() + PLoc.Z());
  Vu.SetX(-Dif1X);
  Vu.SetY(-Dif1Y);
  Vu.SetZ(-Dif1Z);
  Vv.SetX(ZDir.X());
  Vv.SetY(ZDir.Y());
  Vv.SetZ(ZDir.Z());
  Vuu.SetX(-Som1X);
  Vuu.SetY(-Som1Y);
  Vuu.SetZ(-Som1Z);
  Vvv.SetX(0.0);
  Vvv.SetY(0.0);
  Vvv.SetZ(0.0);
  Vuv.SetX(0.0);
  Vuv.SetY(0.0);
  Vuv.SetZ(0.0);
  Vuuu.SetX(Dif1X);
  Vuuu.SetY(Dif1Y);
  Vuuu.SetZ(Dif1Z);
  Vvvv.SetX(0.0);
  Vvvv.SetY(0.0);
  Vvvv.SetZ(0.0);
  Vuvv.SetX(0.0);
  Vuvv.SetY(0.0);
  Vuvv.SetZ(0.0);
  Vuuv.SetX(0.0);
  Vuuv.SetY(0.0);
  Vuuv.SetZ(0.0);
}

void ElSLib::SphereD3(const double  U,
                      const double  V,
                      const gp_Ax3& Pos,
                      const double  Radius,
                      gp_Pnt&       P,
                      gp_Vec&       Vu,
                      gp_Vec&       Vv,
                      gp_Vec&       Vuu,
                      gp_Vec&       Vvv,
                      gp_Vec&       Vuv,
                      gp_Vec&       Vuuu,
                      gp_Vec&       Vvvv,
                      gp_Vec&       Vuuv,
                      gp_Vec&       Vuvv)
{

  // Vxy = CosU * XDirection + SinU * YDirection
  // DVxy = -SinU * XDirection + CosU * YDirection

  // P(U,V) = Location +  R * CosV * Vxy  +   R * SinV * Direction

  // Vu = R * CosV * DVxy

  // Vuu = - R * CosV * Vxy

  // Vuuu = - Vu

  // Vv = -R * SinV * Vxy + R * CosV * Direction

  // Vvv = -R * CosV * Vxy - R * SinV * Direction

  // Vvvv = -Vv

  // Vuv = - R * SinV * DVxy

  // Vuuv = R * SinV * Vxy

  // Vuvv = - R * CosV * DVxy = Vuuu = -Vu

  const gp_XYZ& XDir  = Pos.XDirection().XYZ();
  const gp_XYZ& YDir  = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir  = Pos.Direction().XYZ();
  const gp_XYZ& PLoc  = Pos.Location().XYZ();
  double        CosU  = cos(U);
  double        SinU  = sin(U);
  double        R1    = Radius * cos(V);
  double        R2    = Radius * sin(V);
  double        A1    = R1 * CosU;
  double        A2    = R1 * SinU;
  double        A3    = R2 * CosU;
  double        A4    = R2 * SinU;
  double        Som1X = A1 * XDir.X() + A2 * YDir.X();
  double        Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  double        Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  double        Som3X = A3 * XDir.X() + A4 * YDir.X();
  double        Som3Y = A3 * XDir.Y() + A4 * YDir.Y();
  double        Som3Z = A3 * XDir.Z() + A4 * YDir.Z();
  double        Dif1X = A2 * XDir.X() - A1 * YDir.X();
  double        Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  double        Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  double        R1ZX  = R1 * ZDir.X();
  double        R1ZY  = R1 * ZDir.Y();
  double        R1ZZ  = R1 * ZDir.Z();
  double        R2ZX  = R2 * ZDir.X();
  double        R2ZY  = R2 * ZDir.Y();
  double        R2ZZ  = R2 * ZDir.Z();
  P.SetX(Som1X + R2ZX + PLoc.X());
  P.SetY(Som1Y + R2ZY + PLoc.Y());
  P.SetZ(Som1Z + R2ZZ + PLoc.Z());
  Vu.SetX(-Dif1X);
  Vu.SetY(-Dif1Y);
  Vu.SetZ(-Dif1Z);
  Vv.SetX(-Som3X + R1ZX);
  Vv.SetY(-Som3Y + R1ZY);
  Vv.SetZ(-Som3Z + R1ZZ);
  Vuu.SetX(-Som1X);
  Vuu.SetY(-Som1Y);
  Vuu.SetZ(-Som1Z);
  Vvv.SetX(-Som1X - R2ZX);
  Vvv.SetY(-Som1Y - R2ZY);
  Vvv.SetZ(-Som1Z - R2ZZ);
  Vuv.SetX(A4 * XDir.X() - A3 * YDir.X());
  Vuv.SetY(A4 * XDir.Y() - A3 * YDir.Y());
  Vuv.SetZ(A4 * XDir.Z() - A3 * YDir.Z());
  Vuuu.SetX(Dif1X);
  Vuuu.SetY(Dif1Y);
  Vuuu.SetZ(Dif1Z);
  Vvvv.SetX(Som3X - R1ZX);
  Vvvv.SetY(Som3Y - R1ZY);
  Vvvv.SetZ(Som3Z - R1ZZ);
  Vuvv.SetX(Dif1X);
  Vuvv.SetY(Dif1Y);
  Vuvv.SetZ(Dif1Z);
  Vuuv.SetX(Som3X);
  Vuuv.SetY(Som3Y);
  Vuuv.SetZ(Som3Z);
}

void ElSLib::TorusD3(const double  U,
                     const double  V,
                     const gp_Ax3& Pos,
                     const double  MajorRadius,
                     const double  MinorRadius,
                     gp_Pnt&       P,
                     gp_Vec&       Vu,
                     gp_Vec&       Vv,
                     gp_Vec&       Vuu,
                     gp_Vec&       Vvv,
                     gp_Vec&       Vuv,
                     gp_Vec&       Vuuu,
                     gp_Vec&       Vvvv,
                     gp_Vec&       Vuuv,
                     gp_Vec&       Vuvv)
{
  const TorusEvaluation aData = torusEvaluation(U, V, Pos, MajorRadius, MinorRadius);

  gp_XYZ aPoint = combinedXYZ(aData.Radial, aData.Radius, Pos.Direction().XYZ(), aData.MinorSinV);
  aPoint.Add(Pos.Location().XYZ());
  P    = gp_Pnt(aPoint);
  Vu   = gp_Vec(scaledXYZ(aData.Tangent, aData.Radius));
  Vv   = gp_Vec(combinedXYZ(aData.Radial,
                            -aData.MinorSinV,
                            Pos.Direction().XYZ(),
                            aData.MinorCosV));
  Vuu  = gp_Vec(scaledXYZ(aData.Radial, -aData.Radius));
  Vvv  = gp_Vec(combinedXYZ(aData.Radial,
                            -aData.MinorCosV,
                            Pos.Direction().XYZ(),
                            -aData.MinorSinV));
  Vuv  = gp_Vec(scaledXYZ(aData.Tangent, -aData.MinorSinV));
  Vuuu = gp_Vec(scaledXYZ(aData.Tangent, -aData.Radius));
  Vvvv = gp_Vec(combinedXYZ(aData.Radial,
                            aData.MinorSinV,
                            Pos.Direction().XYZ(),
                            -aData.MinorCosV));
  Vuuv = gp_Vec(scaledXYZ(aData.Radial, aData.MinorSinV));
  Vuvv = gp_Vec(scaledXYZ(aData.Tangent, -aData.MinorCosV));
}

//=================================================================================================

void ElSLib::PlaneParameters(const gp_Ax3& Pos, const gp_Pnt& P, double& U, double& V)
{
  gp_Trsf T;
  T.SetTransformation(Pos);
  gp_Pnt Ploc = P.Transformed(T);
  U           = Ploc.X();
  V           = Ploc.Y();
}

//=================================================================================================

void ElSLib::CylinderParameters(const gp_Ax3& Pos,
                                const double,
                                const gp_Pnt& P,
                                double&       U,
                                double&       V)
{
  gp_Trsf T;
  T.SetTransformation(Pos);
  gp_Pnt Ploc = P.Transformed(T);
  U           = atan2(Ploc.Y(), Ploc.X());
  normalizeAngle(U);
  V = Ploc.Z();
}

//=================================================================================================

void ElSLib::ConeParameters(const gp_Ax3& Pos,
                            const double  Radius,
                            const double  SAngle,
                            const gp_Pnt& P,
                            double&       U,
                            double&       V)
{
  gp_Trsf T;
  T.SetTransformation(Pos);
  gp_Pnt Ploc = P.Transformed(T);

  // Check if point is at the apex
  if (std::abs(Ploc.X()) < gp::Resolution() && std::abs(Ploc.Y()) < gp::Resolution())
  {
    U = 0.0;
  }
  else if (-Radius > Ploc.Z() * std::tan(SAngle))
  {
    // the point is at the wrong side of the apex
    U = atan2(-Ploc.Y(), -Ploc.X());
  }
  else
  {
    U = atan2(Ploc.Y(), Ploc.X());
  }
  normalizeAngle(U);
  // Evaluate V as follows :
  // P0 = Cone.Value(U,0)
  // P1 = Cone.Value(U,1)
  // V = P0 P1 . P0 Ploc
  // After simplification obtain:
  // V = std::sin(Sang) * ( x cosU + y SinU - R) + z * std::cos(Sang)
  // Method that permits to find V of the projected point if the point
  // is not actually on the cone.

  V = sin(SAngle) * (Ploc.X() * cos(U) + Ploc.Y() * sin(U) - Radius) + cos(SAngle) * Ploc.Z();
}

//=================================================================================================

void ElSLib::SphereParameters(const gp_Ax3& Pos,
                              const double,
                              const gp_Pnt& P,
                              double&       U,
                              double&       V)
{
  gp_Trsf T;
  T.SetTransformation(Pos);
  gp_Pnt Ploc = P.Transformed(T);
  double x, y, z;
  Ploc.Coord(x, y, z);
  double l = sqrt(x * x + y * y);
  if (l < gp::Resolution())
  { // point on axis Z of the sphere
    if (z > 0.)
    {
      V = M_PI_2; // PI * 0.5
    }
    else
    {
      V = -M_PI_2; // PI * 0.5
    }
    U = 0.;
  }
  else
  {
    V = atan(z / l);
    U = atan2(y, x);
    normalizeAngle(U);
  }
}

//=================================================================================================

void ElSLib::TorusParameters(const gp_Ax3& Pos,
                             const double  MajorRadius,
                             const double  MinorRadius,
                             const gp_Pnt& P,
                             double&       U,
                             double&       V)
{
  gp_Trsf aTransformation;
  aTransformation.SetTransformation(Pos);
  const gp_Pnt aLocalPoint = P.Transformed(aTransformation);
  const double aX          = aLocalPoint.X();
  const double aY          = aLocalPoint.Y();
  const double aZ          = aLocalPoint.Z();
  const double aRadial     = std::hypot(aX, aY);

  U = std::atan2(aY, aX);

  // A spindle torus has two possible meridian branches for the direction returned by atan2().
  // Select the branch whose tube-circle equation has the smaller residual.
  double aSignedRadial = aRadial;
  if (MajorRadius < MinorRadius)
  {
    const double aNearRadius   = aRadial - MajorRadius;
    const double aFarRadius    = aRadial + MajorRadius;
    const double aNearResidual = std::abs(std::hypot(aNearRadius, aZ) - MinorRadius);
    const double aFarResidual  = std::abs(std::hypot(aFarRadius, aZ) - MinorRadius);
    if (aFarResidual < aNearResidual)
    {
      U += M_PI;
      aSignedRadial = -aRadial;
    }
  }
  normalizeAngle(U);

  const double aSectionX    = aSignedRadial - MajorRadius;
  const double aSectionNorm = std::hypot(aSectionX, aZ);
  const double aScale       = std::abs(MajorRadius) + aRadial + std::abs(aZ);
  if (aSectionNorm == 0.0
      || (std::isfinite(aScale)
          && aSectionNorm <= Precision::Computational() * aScale))
  {
    V = 0.0;
  }
  else
  {
    V = std::atan2(aZ, aSectionX);
    normalizeAngle(V);
  }
}

//=================================================================================================

gp_Lin ElSLib::PlaneUIso(const gp_Ax3& Pos, const double U)
{
  gp_Lin L(Pos.Location(), Pos.YDirection());
  gp_Vec Ve(Pos.XDirection());
  Ve *= U;
  L.Translate(Ve);
  return L;
}

//=================================================================================================

gp_Lin ElSLib::CylinderUIso(const gp_Ax3& Pos, const double Radius, const double U)
{
  gp_Pnt P;
  gp_Vec DU, DV;
  CylinderD1(U, 0., Pos, Radius, P, DU, DV);
  gp_Lin L(P, DV);
  return L;
}

//=================================================================================================

gp_Lin ElSLib::ConeUIso(const gp_Ax3& Pos, const double Radius, const double SAngle, const double U)
{
  gp_Pnt P;
  gp_Vec DU, DV;
  ConeD1(U, 0, Pos, Radius, SAngle, P, DU, DV);
  gp_Lin L(P, DV);
  return L;
}

//=================================================================================================

gp_Circ ElSLib::SphereUIso(const gp_Ax3& Pos, const double Radius, const double U)
{
  gp_Vec  dx = Pos.XDirection();
  gp_Vec  dy = Pos.YDirection();
  gp_Dir  dz = Pos.Direction();
  gp_Dir  cx = cos(U) * dx + sin(U) * dy;
  gp_Ax2  axes(Pos.Location(), cx.Crossed(dz), cx);
  gp_Circ Circ(axes, Radius);
  return Circ;
}

//=================================================================================================

gp_Circ ElSLib::TorusUIso(const gp_Ax3& Pos,
                          const double  MajorRadius,
                          const double  MinorRadius,
                          const double  U)
{
  const TorusSinCos aTrig = torusSinCos(U);
  const gp_XYZ aRadial =
    torusRadialDerivative(Pos.XDirection().XYZ(), Pos.YDirection().XYZ(), aTrig, 0);
  const gp_Dir aRadialDirection(aRadial);
  gp_Ax2 anAxes(Pos.Location(), aRadialDirection.Crossed(Pos.Direction()), aRadialDirection);
  anAxes.Translate(gp_Vec(scaledXYZ(aRadial, MajorRadius)));
  return gp_Circ(anAxes, MinorRadius);
}

//=================================================================================================

gp_Lin ElSLib::PlaneVIso(const gp_Ax3& Pos, const double V)
{
  gp_Lin L(Pos.Location(), Pos.XDirection());
  gp_Vec Ve(Pos.YDirection());
  Ve *= V;
  L.Translate(Ve);
  return L;
}

//=================================================================================================

gp_Circ ElSLib::CylinderVIso(const gp_Ax3& Pos, const double Radius, const double V)
{
  gp_Ax2 axes = Pos.Ax2();
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(V);
  axes.Translate(Ve);
  gp_Circ C(axes, Radius);
  return C;
}

//=================================================================================================

gp_Circ ElSLib::ConeVIso(const gp_Ax3& Pos,
                         const double  Radius,
                         const double  SAngle,
                         const double  V)
{
  gp_Ax3 axes(Pos);
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(V * cos(SAngle));
  axes.Translate(Ve);
  double R = Radius + V * sin(SAngle);
  if (R < 0)
  {
    axes.XReverse();
    axes.YReverse();
    R = -R;
  }
  gp_Circ C(axes.Ax2(), R);
  return C;
}

//=================================================================================================

gp_Circ ElSLib::SphereVIso(const gp_Ax3& Pos, const double Radius, const double V)
{
  gp_Ax2 axes = Pos.Ax2();
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(Radius * sin(V));
  axes.Translate(Ve);
  double radius = Radius * cos(V);
  // #23170: if V is even slightly (e.g. by double epsilon) greater than PI/2,
  // radius will become negative and constructor of gp_Circ will raise exception.
  // Lets try to create correct isoline even on analytical continuation for |V| > PI/2...
  if (radius < 0.)
  {
    axes.SetDirection(-axes.Direction());
    radius = -radius;
  }
  gp_Circ Circ(axes, radius);
  return Circ;
}

//=================================================================================================

gp_Circ ElSLib::TorusVIso(const gp_Ax3& Pos,
                          const double  MajorRadius,
                          const double  MinorRadius,
                          const double  V)
{
  const TorusSinCos aTrig = torusSinCos(V);
  gp_Ax3 anAxes = Pos.Ax2();
  anAxes.Translate(gp_Vec(scaledXYZ(Pos.Direction().XYZ(), MinorRadius * aTrig.Sin)));

  double aRadius = torusSection(MajorRadius, MinorRadius, aTrig.Cos).Radius;
  if (aRadius < 0.0)
  {
    anAxes.XReverse();
    anAxes.YReverse();
    aRadius = -aRadius;
  }
  return gp_Circ(anAxes.Ax2(), aRadius);
}
