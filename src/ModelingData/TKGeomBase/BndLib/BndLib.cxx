// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <BndLib.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <ElSLib.hxx>

static Standard_Integer ComputeBox(const gp_Hypr&      aHypr,
                                   const Standard_Real aT1,
                                   const Standard_Real aT2,
                                   Bnd_Box&            aBox);

namespace
{
//! Cosine of M_PI/8 (22.5 degrees) - used for 8-point polygon approximation.
constexpr Standard_Real THE_COS_PI8 = 0.92387953251128674;

//! Cosine (and sine) of M_PI/4 (45 degrees) - used for diagonal points.
constexpr Standard_Real THE_COS_PI4 = 0.70710678118654746;

//! Compute method
template <class PointType, class BndBoxType>
void Compute(const Standard_Real theP1,
             const Standard_Real theP2,
             const Standard_Real theRa,
             const Standard_Real theRb,
             const PointType&    theXd,
             const PointType&    theYd,
             const PointType&    theO,
             BndBoxType&         theB)
{
  Standard_Real aTeta1;
  Standard_Real aTeta2;
  if (theP2 < theP1)
  {
    aTeta1 = theP2;
    aTeta2 = theP1;
  }
  else
  {
    aTeta1 = theP1;
    aTeta2 = theP2;
  }

  Standard_Real aDelta = std::abs(aTeta2 - aTeta1);
  if (aDelta > 2. * M_PI)
  {
    aTeta1 = 0.;
    aTeta2 = 2. * M_PI;
  }
  else
  {
    // Normalize aTeta1 to [0, 2*PI) range
    aTeta1 = std::fmod(aTeta1, 2. * M_PI);
    if (aTeta1 < 0.)
    {
      aTeta1 += 2. * M_PI;
    }
    aTeta2 = aTeta1 + aDelta;
  }

  // One places already both ends
  Standard_Real aCn1, aSn1, aCn2, aSn2;
  aCn1 = std::cos(aTeta1);
  aSn1 = std::sin(aTeta1);
  aCn2 = std::cos(aTeta2);
  aSn2 = std::sin(aTeta2);
  theB.Add(PointType(theO.Coord() + theRa * aCn1 * theXd.Coord() + theRb * aSn1 * theYd.Coord()));
  theB.Add(PointType(theO.Coord() + theRa * aCn2 * theXd.Coord() + theRb * aSn2 * theYd.Coord()));

  Standard_Real aRam, aRbm;
  if (aDelta > M_PI / 8.)
  {
    // Main radiuses to take into account only 8 points (/cos(Pi/8.))
    aRam = theRa / THE_COS_PI8;
    aRbm = theRb / THE_COS_PI8;
  }
  else
  {
    // Main radiuses to take into account the arrow
    Standard_Real aTc = std::cos(aDelta / 2);
    aRam              = theRa / aTc;
    aRbm              = theRb / aTc;
  }
  theB.Add(PointType(theO.Coord() + aRam * aCn1 * theXd.Coord() + aRbm * aSn1 * theYd.Coord()));
  theB.Add(PointType(theO.Coord() + aRam * aCn2 * theXd.Coord() + aRbm * aSn2 * theYd.Coord()));

  // X and Y multipliers for 8 polygon points at 45-degree intervals (0, 45, 90, ..., 315 degrees).
  // Point i corresponds to angle i * 45 degrees.
  constexpr Standard_Real aXMult[8] =
    {1., THE_COS_PI4, 0., -THE_COS_PI4, -1., -THE_COS_PI4, 0., THE_COS_PI4};
  constexpr Standard_Real aYMult[8] =
    {0., THE_COS_PI4, 1., THE_COS_PI4, 0., -THE_COS_PI4, -1., -THE_COS_PI4};

  // Lambda to add polygon point by index (0-7).
  const auto addPoint = [&](Standard_Integer theIdx) {
    theB.Add(PointType(theO.Coord() + aRam * aXMult[theIdx] * theXd.Coord()
                       + aRbm * aYMult[theIdx] * theYd.Coord()));
  };

  Standard_Integer aDeb = static_cast<Standard_Integer>(aTeta1 / (M_PI / 4.));
  Standard_Integer aFin = static_cast<Standard_Integer>(aTeta2 / (M_PI / 4.));
  aDeb++;

  if (aDeb > aFin)
  {
    return;
  }

  // Add polygon points from aDeb to aFin, wrapping around using modulo 8.
  for (Standard_Integer i = aDeb; i <= aFin; ++i)
  {
    addPoint(i % 8);
  }
}
} // end namespace

static void OpenMin(const gp_Dir& V, Bnd_Box& B)
{
  // OpenMin opens the box in the direction of decreasing parameter.
  // For a line P(t) = Origin + t*V, as t -> -Inf:
  //   - If V.X() > 0, x-coordinate -> -Inf, so open Xmin
  //   - If V.X() < 0, x-coordinate -> +Inf, so open Xmax
  if (V.IsParallel(gp::DX(), Precision::Angular()))
  {
    if (V.X() > 0.)
      B.OpenXmin();
    else
      B.OpenXmax();
  }
  else if (V.IsParallel(gp::DY(), Precision::Angular()))
  {
    if (V.Y() > 0.)
      B.OpenYmin();
    else
      B.OpenYmax();
  }
  else if (V.IsParallel(gp::DZ(), Precision::Angular()))
  {
    if (V.Z() > 0.)
      B.OpenZmin();
    else
      B.OpenZmax();
  }
  else
  {
    B.OpenXmin();
    B.OpenYmin();
    B.OpenZmin();
  }
}

static void OpenMax(const gp_Dir& V, Bnd_Box& B)
{
  // OpenMax opens the box in the direction of increasing parameter.
  // For a line P(t) = Origin + t*V, as t -> +Inf:
  //   - If V.X() > 0, x-coordinate -> +Inf, so open Xmax
  //   - If V.X() < 0, x-coordinate -> -Inf, so open Xmin
  if (V.IsParallel(gp::DX(), Precision::Angular()))
  {
    if (V.X() > 0.)
      B.OpenXmax();
    else
      B.OpenXmin();
  }
  else if (V.IsParallel(gp::DY(), Precision::Angular()))
  {
    if (V.Y() > 0.)
      B.OpenYmax();
    else
      B.OpenYmin();
  }
  else if (V.IsParallel(gp::DZ(), Precision::Angular()))
  {
    if (V.Z() > 0.)
      B.OpenZmax();
    else
      B.OpenZmin();
  }
  else
  {
    B.OpenXmax();
    B.OpenYmax();
    B.OpenZmax();
  }
}

static void OpenMinMax(const gp_Dir& V, Bnd_Box& B)
{
  if (V.IsParallel(gp::DX(), Precision::Angular()))
  {
    B.OpenXmax();
    B.OpenXmin();
  }
  else if (V.IsParallel(gp::DY(), Precision::Angular()))
  {
    B.OpenYmax();
    B.OpenYmin();
  }
  else if (V.IsParallel(gp::DZ(), Precision::Angular()))
  {
    B.OpenZmax();
    B.OpenZmin();
  }
  else
  {
    B.OpenXmin();
    B.OpenYmin();
    B.OpenZmin();
    B.OpenXmax();
    B.OpenYmax();
    B.OpenZmax();
  }
}

static void OpenMin(const gp_Dir2d& V, Bnd_Box2d& B)
{
  // OpenMin opens the box in the direction of decreasing parameter.
  // For a line P(t) = Origin + t*V, as t -> -Inf:
  //   - If V.X() > 0, x-coordinate -> -Inf, so open Xmin
  //   - If V.X() < 0, x-coordinate -> +Inf, so open Xmax
  if (V.IsParallel(gp::DX2d(), Precision::Angular()))
  {
    if (V.X() > 0.)
      B.OpenXmin();
    else
      B.OpenXmax();
  }
  else if (V.IsParallel(gp::DY2d(), Precision::Angular()))
  {
    if (V.Y() > 0.)
      B.OpenYmin();
    else
      B.OpenYmax();
  }
  else
  {
    B.OpenXmin();
    B.OpenYmin();
  }
}

static void OpenMax(const gp_Dir2d& V, Bnd_Box2d& B)
{
  // OpenMax opens the box in the direction of increasing parameter.
  // For a line P(t) = Origin + t*V, as t -> +Inf:
  //   - If V.X() > 0, x-coordinate -> +Inf, so open Xmax
  //   - If V.X() < 0, x-coordinate -> -Inf, so open Xmin
  if (V.IsParallel(gp::DX2d(), Precision::Angular()))
  {
    if (V.X() > 0.)
      B.OpenXmax();
    else
      B.OpenXmin();
  }
  else if (V.IsParallel(gp::DY2d(), Precision::Angular()))
  {
    if (V.Y() > 0.)
      B.OpenYmax();
    else
      B.OpenYmin();
  }
  else
  {
    B.OpenXmax();
    B.OpenYmax();
  }
}

static void OpenMinMax(const gp_Dir2d& V, Bnd_Box2d& B)
{
  if (V.IsParallel(gp::DX2d(), Precision::Angular()))
  {
    B.OpenXmax();
    B.OpenXmin();
  }
  else if (V.IsParallel(gp::DY2d(), Precision::Angular()))
  {
    B.OpenYmax();
    B.OpenYmin();
  }
  else
  {
    B.OpenXmin();
    B.OpenYmin();
    B.OpenXmax();
    B.OpenYmax();
  }
}

void BndLib::Add(const gp_Lin&       L,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{

  if (Precision::IsNegativeInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      OpenMinMax(L.Direction(), B);
      B.Add(ElCLib::Value(0., L));
    }
    else
    {
      OpenMin(L.Direction(), B);
      B.Add(ElCLib::Value(P2, L));
    }
  }
  else if (Precision::IsPositiveInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      OpenMinMax(L.Direction(), B);
      B.Add(ElCLib::Value(0., L));
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      OpenMax(L.Direction(), B);
      B.Add(ElCLib::Value(P2, L));
    }
  }
  else
  {
    B.Add(ElCLib::Value(P1, L));
    if (Precision::IsNegativeInfinite(P2))
    {
      OpenMin(L.Direction(), B);
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      OpenMax(L.Direction(), B);
    }
    else
    {
      B.Add(ElCLib::Value(P2, L));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Lin2d&     L,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box2d&          B)
{

  if (Precision::IsNegativeInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      OpenMinMax(L.Direction(), B);
      B.Add(ElCLib::Value(0., L));
    }
    else
    {
      OpenMin(L.Direction(), B);
      B.Add(ElCLib::Value(P2, L));
    }
  }
  else if (Precision::IsPositiveInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      OpenMinMax(L.Direction(), B);
      B.Add(ElCLib::Value(0., L));
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      OpenMax(L.Direction(), B);
      B.Add(ElCLib::Value(P2, L));
    }
  }
  else
  {
    B.Add(ElCLib::Value(P1, L));
    if (Precision::IsNegativeInfinite(P2))
    {
      OpenMin(L.Direction(), B);
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      OpenMax(L.Direction(), B);
    }
    else
    {
      B.Add(ElCLib::Value(P2, L));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Circ& C, const Standard_Real Tol, Bnd_Box& B)
{
  Standard_Real U1 = 0., U2 = 2. * M_PI;
  Add(C, U1, U2, Tol, B);
}

void BndLib::Add(const gp_Circ&      C,
                 const Standard_Real U1,
                 const Standard_Real U2,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{
  Standard_Real period = 2. * M_PI - Epsilon(2. * M_PI);

  Standard_Real utrim1 = U1, utrim2 = U2;
  if (U2 - U1 > period)
  {
    utrim1 = 0.;
    utrim2 = 2. * M_PI;
  }
  else
  {
    Standard_Real tol = Epsilon(1.);
    ElCLib::AdjustPeriodic(0., 2. * M_PI, tol, utrim1, utrim2);
  }
  Standard_Real R   = C.Radius();
  gp_XYZ        O   = C.Location().XYZ();
  gp_XYZ        Xd  = C.XAxis().Direction().XYZ();
  gp_XYZ        Yd  = C.YAxis().Direction().XYZ();
  const gp_Ax2& pos = C.Position();
  //
  Standard_Real tt;
  Standard_Real xmin, xmax, txmin, txmax;
  if (std::abs(Xd.X()) > gp::Resolution())
  {
    txmin = std::atan(Yd.X() / Xd.X());
    txmin = ElCLib::InPeriod(txmin, 0., 2. * M_PI);
  }
  else
  {
    txmin = M_PI / 2.;
  }
  txmax = txmin <= M_PI ? txmin + M_PI : txmin - M_PI;
  xmin  = R * std::cos(txmin) * Xd.X() + R * std::sin(txmin) * Yd.X() + O.X();
  xmax  = R * std::cos(txmax) * Xd.X() + R * std::sin(txmax) * Yd.X() + O.X();
  if (xmin > xmax)
  {
    tt    = xmin;
    xmin  = xmax;
    xmax  = tt;
    tt    = txmin;
    txmin = txmax;
    txmax = tt;
  }
  //
  Standard_Real ymin, ymax, tymin, tymax;
  if (std::abs(Xd.Y()) > gp::Resolution())
  {
    tymin = std::atan(Yd.Y() / Xd.Y());
    tymin = ElCLib::InPeriod(tymin, 0., 2. * M_PI);
  }
  else
  {
    tymin = M_PI / 2.;
  }
  tymax = tymin <= M_PI ? tymin + M_PI : tymin - M_PI;
  ymin  = R * std::cos(tymin) * Xd.Y() + R * std::sin(tymin) * Yd.Y() + O.Y();
  ymax  = R * std::cos(tymax) * Xd.Y() + R * std::sin(tymax) * Yd.Y() + O.Y();
  if (ymin > ymax)
  {
    tt    = ymin;
    ymin  = ymax;
    ymax  = tt;
    tt    = tymin;
    tymin = tymax;
    tymax = tt;
  }
  //
  Standard_Real zmin, zmax, tzmin, tzmax;
  if (std::abs(Xd.Z()) > gp::Resolution())
  {
    tzmin = std::atan(Yd.Z() / Xd.Z());
    tzmin = ElCLib::InPeriod(tzmin, 0., 2. * M_PI);
  }
  else
  {
    tzmin = M_PI / 2.;
  }
  tzmax = tzmin <= M_PI ? tzmin + M_PI : tzmin - M_PI;
  zmin  = R * std::cos(tzmin) * Xd.Z() + R * std::sin(tzmin) * Yd.Z() + O.Z();
  zmax  = R * std::cos(tzmax) * Xd.Z() + R * std::sin(tzmax) * Yd.Z() + O.Z();
  if (zmin > zmax)
  {
    tt    = zmin;
    zmin  = zmax;
    zmax  = tt;
    tt    = tzmin;
    tzmin = tzmax;
    tzmax = tt;
  }
  //
  if (utrim2 - utrim1 >= period)
  {
    B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
  else
  {
    gp_Pnt P = ElCLib::CircleValue(utrim1, pos, R);
    B.Add(P);
    P = ElCLib::CircleValue(utrim2, pos, R);
    B.Add(P);
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    B.FinitePart().Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    Standard_Real gap = B.GetGap();
    Xmin += gap;
    Ymin += gap;
    Zmin += gap;
    Xmax -= gap;
    Ymax -= gap;
    Zmax -= gap;
    //
    txmin = ElCLib::InPeriod(txmin, utrim1, utrim1 + 2. * M_PI);
    if (txmin >= utrim1 && txmin <= utrim2)
    {
      Xmin = std::min(xmin, Xmin);
    }
    txmax = ElCLib::InPeriod(txmax, utrim1, utrim1 + 2. * M_PI);
    if (txmax >= utrim1 && txmax <= utrim2)
    {
      Xmax = std::max(xmax, Xmax);
    }
    //
    tymin = ElCLib::InPeriod(tymin, utrim1, utrim1 + 2. * M_PI);
    if (tymin >= utrim1 && tymin <= utrim2)
    {
      Ymin = std::min(ymin, Ymin);
    }
    tymax = ElCLib::InPeriod(tymax, utrim1, utrim1 + 2. * M_PI);
    if (tymax >= utrim1 && tymax <= utrim2)
    {
      Ymax = std::max(ymax, Ymax);
    }
    //
    tzmin = ElCLib::InPeriod(tzmin, utrim1, utrim1 + 2. * M_PI);
    if (tzmin >= utrim1 && tzmin <= utrim2)
    {
      Zmin = std::min(zmin, Zmin);
    }
    tzmax = ElCLib::InPeriod(tzmax, utrim1, utrim1 + 2. * M_PI);
    if (tzmax >= utrim1 && tzmax <= utrim2)
    {
      Zmax = std::max(zmax, Zmax);
    }
    //
    B.Update(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  }
  //
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Circ2d& C, const Standard_Real Tol, Bnd_Box2d& B)
{

  Standard_Real R  = C.Radius();
  gp_XY         O  = C.Location().XY();
  gp_XY         Xd = C.XAxis().Direction().XY();
  gp_XY         Yd = C.YAxis().Direction().XY();
  B.Add(gp_Pnt2d(O - R * Xd - R * Yd));
  B.Add(gp_Pnt2d(O - R * Xd + R * Yd));
  B.Add(gp_Pnt2d(O + R * Xd - R * Yd));
  B.Add(gp_Pnt2d(O + R * Xd + R * Yd));
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Circ2d&    C,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box2d&          B)
{

  Compute(P1,
          P2,
          C.Radius(),
          C.Radius(),
          gp_Pnt2d(C.XAxis().Direction().XY()),
          gp_Pnt2d(C.YAxis().Direction().XY()),
          C.Location(),
          B);
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Elips& C, const Standard_Real Tol, Bnd_Box& B)
{
  Standard_Real U1 = 0., U2 = 2. * M_PI;
  Add(C, U1, U2, Tol, B);
}

void BndLib::Add(const gp_Elips&     C,
                 const Standard_Real U1,
                 const Standard_Real U2,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{
  Standard_Real period = 2. * M_PI - Epsilon(2. * M_PI);

  Standard_Real utrim1 = U1, utrim2 = U2;
  if (U2 - U1 > period)
  {
    utrim1 = 0.;
    utrim2 = 2. * M_PI;
  }
  else
  {
    Standard_Real tol = Epsilon(1.);
    ElCLib::AdjustPeriodic(0., 2. * M_PI, tol, utrim1, utrim2);
  }
  Standard_Real MajR = C.MajorRadius();
  Standard_Real MinR = C.MinorRadius();
  gp_XYZ        O    = C.Location().XYZ();
  gp_XYZ        Xd   = C.XAxis().Direction().XYZ();
  gp_XYZ        Yd   = C.YAxis().Direction().XYZ();
  const gp_Ax2& pos  = C.Position();
  //
  Standard_Real tt;
  Standard_Real xmin, xmax, txmin, txmax;
  if (std::abs(Xd.X()) > gp::Resolution())
  {
    txmin = std::atan((MinR * Yd.X()) / (MajR * Xd.X()));
    txmin = ElCLib::InPeriod(txmin, 0., 2. * M_PI);
  }
  else
  {
    txmin = M_PI / 2.;
  }
  txmax = txmin <= M_PI ? txmin + M_PI : txmin - M_PI;
  xmin  = MajR * std::cos(txmin) * Xd.X() + MinR * std::sin(txmin) * Yd.X() + O.X();
  xmax  = MajR * std::cos(txmax) * Xd.X() + MinR * std::sin(txmax) * Yd.X() + O.X();
  if (xmin > xmax)
  {
    tt    = xmin;
    xmin  = xmax;
    xmax  = tt;
    tt    = txmin;
    txmin = txmax;
    txmax = tt;
  }
  //
  Standard_Real ymin, ymax, tymin, tymax;
  if (std::abs(Xd.Y()) > gp::Resolution())
  {
    tymin = std::atan((MinR * Yd.Y()) / (MajR * Xd.Y()));
    tymin = ElCLib::InPeriod(tymin, 0., 2. * M_PI);
  }
  else
  {
    tymin = M_PI / 2.;
  }
  tymax = tymin <= M_PI ? tymin + M_PI : tymin - M_PI;
  ymin  = MajR * std::cos(tymin) * Xd.Y() + MinR * std::sin(tymin) * Yd.Y() + O.Y();
  ymax  = MajR * std::cos(tymax) * Xd.Y() + MinR * std::sin(tymax) * Yd.Y() + O.Y();
  if (ymin > ymax)
  {
    tt    = ymin;
    ymin  = ymax;
    ymax  = tt;
    tt    = tymin;
    tymin = tymax;
    tymax = tt;
  }
  //
  Standard_Real zmin, zmax, tzmin, tzmax;
  if (std::abs(Xd.Z()) > gp::Resolution())
  {
    tzmin = std::atan((MinR * Yd.Z()) / (MajR * Xd.Z()));
    tzmin = ElCLib::InPeriod(tzmin, 0., 2. * M_PI);
  }
  else
  {
    tzmin = M_PI / 2.;
  }
  tzmax = tzmin <= M_PI ? tzmin + M_PI : tzmin - M_PI;
  zmin  = MajR * std::cos(tzmin) * Xd.Z() + MinR * std::sin(tzmin) * Yd.Z() + O.Z();
  zmax  = MajR * std::cos(tzmax) * Xd.Z() + MinR * std::sin(tzmax) * Yd.Z() + O.Z();
  if (zmin > zmax)
  {
    tt    = zmin;
    zmin  = zmax;
    zmax  = tt;
    tt    = tzmin;
    tzmin = tzmax;
    tzmax = tt;
  }
  //
  if (utrim2 - utrim1 >= period)
  {
    B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
  else
  {
    gp_Pnt P = ElCLib::EllipseValue(utrim1, pos, MajR, MinR);
    B.Add(P);
    P = ElCLib::EllipseValue(utrim2, pos, MajR, MinR);
    B.Add(P);
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    B.FinitePart().Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    Standard_Real gap = B.GetGap();
    Xmin += gap;
    Ymin += gap;
    Zmin += gap;
    Xmax -= gap;
    Ymax -= gap;
    Zmax -= gap;
    //
    txmin = ElCLib::InPeriod(txmin, utrim1, utrim1 + 2. * M_PI);
    if (txmin >= utrim1 && txmin <= utrim2)
    {
      Xmin = std::min(xmin, Xmin);
    }
    txmax = ElCLib::InPeriod(txmax, utrim1, utrim1 + 2. * M_PI);
    if (txmax >= utrim1 && txmax <= utrim2)
    {
      Xmax = std::max(xmax, Xmax);
    }
    //
    tymin = ElCLib::InPeriod(tymin, utrim1, utrim1 + 2. * M_PI);
    if (tymin >= utrim1 && tymin <= utrim2)
    {
      Ymin = std::min(ymin, Ymin);
    }
    tymax = ElCLib::InPeriod(tymax, utrim1, utrim1 + 2. * M_PI);
    if (tymax >= utrim1 && tymax <= utrim2)
    {
      Ymax = std::max(ymax, Ymax);
    }
    //
    tzmin = ElCLib::InPeriod(tzmin, utrim1, utrim1 + 2. * M_PI);
    if (tzmin >= utrim1 && tzmin <= utrim2)
    {
      Zmin = std::min(zmin, Zmin);
    }
    tzmax = ElCLib::InPeriod(tzmax, utrim1, utrim1 + 2. * M_PI);
    if (tzmax >= utrim1 && tzmax <= utrim2)
    {
      Zmax = std::max(zmax, Zmax);
    }
    //
    B.Update(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  }
  //
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Elips2d& C, const Standard_Real Tol, Bnd_Box2d& B)
{

  Standard_Real Ra = C.MajorRadius();
  Standard_Real Rb = C.MinorRadius();
  gp_XY         Xd = C.XAxis().Direction().XY();
  gp_XY         Yd = C.YAxis().Direction().XY();
  gp_XY         O  = C.Location().XY();
  B.Add(gp_Pnt2d(O + Ra * Xd + Rb * Yd));
  B.Add(gp_Pnt2d(O - Ra * Xd + Rb * Yd));
  B.Add(gp_Pnt2d(O - Ra * Xd - Rb * Yd));
  B.Add(gp_Pnt2d(O + Ra * Xd - Rb * Yd));
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Elips2d&   C,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box2d&          B)
{

  Compute(P1,
          P2,
          C.MajorRadius(),
          C.MinorRadius(),
          gp_Pnt2d(C.XAxis().Direction().XY()),
          gp_Pnt2d(C.YAxis().Direction().XY()),
          C.Location(),
          B);
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Parab&     P,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{

  if (Precision::IsNegativeInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
      B.OpenZmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, P));
    }
    B.OpenXmin();
    B.OpenYmin();
    B.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
      B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      B.Add(ElCLib::Value(P2, P));
    }
    B.OpenXmax();
    B.OpenYmax();
    B.OpenZmax();
  }
  else
  {
    B.Add(ElCLib::Value(P1, P));
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
      B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
      B.OpenZmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, P));
      if (P1 * P2 < 0)
        B.Add(ElCLib::Value(0., P));
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Parab2d&   P,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box2d&          B)
{

  if (Precision::IsNegativeInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, P));
    }
    B.OpenXmin();
    B.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      B.Add(ElCLib::Value(P2, P));
    }
    B.OpenXmax();
    B.OpenYmax();
  }
  else
  {
    B.Add(ElCLib::Value(P1, P));
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, P));
      if (P1 * P2 < 0)
        B.Add(ElCLib::Value(0., P));
    }
  }
  B.Enlarge(Tol);
}

//=================================================================================================

void BndLib::Add(const gp_Hypr&      H,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{
  if (Precision::IsNegativeInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
      B.OpenZmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, H));
    }
    B.OpenXmin();
    B.OpenYmin();
    B.OpenZmin();
  }
  else if (Precision::IsPositiveInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
      B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      B.Add(ElCLib::Value(P2, H));
    }
    B.OpenXmax();
    B.OpenYmax();
    B.OpenZmax();
  }
  else
  {
    B.Add(ElCLib::Value(P1, H));
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
      B.OpenZmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
      B.OpenZmax();
    }
    else
    {
      ComputeBox(H, P1, P2, B);
    }
  }
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Hypr2d&    H,
                 const Standard_Real P1,
                 const Standard_Real P2,
                 const Standard_Real Tol,
                 Bnd_Box2d&          B)
{

  if (Precision::IsNegativeInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, H));
    }
    B.OpenXmin();
    B.OpenYmin();
  }
  else if (Precision::IsPositiveInfinite(P1))
  {
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      B.Add(ElCLib::Value(P2, H));
    }
    B.OpenXmax();
    B.OpenYmax();
  }
  else
  {
    B.Add(ElCLib::Value(P1, H));
    if (Precision::IsNegativeInfinite(P2))
    {
      B.OpenXmin();
      B.OpenYmin();
    }
    else if (Precision::IsPositiveInfinite(P2))
    {
      B.OpenXmax();
      B.OpenYmax();
    }
    else
    {
      B.Add(ElCLib::Value(P2, H));
      if (P1 * P2 < 0)
        B.Add(ElCLib::Value(0., H));
    }
  }
  B.Enlarge(Tol);
}

static void ComputeCyl(const gp_Cylinder&  Cyl,
                       const Standard_Real UMin,
                       const Standard_Real UMax,
                       const Standard_Real VMin,
                       const Standard_Real VMax,
                       Bnd_Box&            B)
{
  gp_Circ aC = ElSLib::CylinderVIso(Cyl.Position(), Cyl.Radius(), VMin);
  BndLib::Add(aC, UMin, UMax, 0., B);
  //
  gp_Vec aT = (VMax - VMin) * Cyl.Axis().Direction();
  aC.Translate(aT);
  BndLib::Add(aC, UMin, UMax, 0., B);
}

void BndLib::Add(const gp_Cylinder&  S,
                 const Standard_Real UMin,
                 const Standard_Real UMax,
                 const Standard_Real VMin,
                 const Standard_Real VMax,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{
  // Cache axis direction for infinite cases.
  const gp_Dir& aDir = S.Axis().Direction();

  if (Precision::IsNegativeInfinite(VMin))
  {
    if (Precision::IsNegativeInfinite(VMax))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(VMax))
    {
      OpenMinMax(aDir, B);
    }
    else
    {
      ComputeCyl(S, UMin, UMax, 0., VMax, B);
      OpenMin(aDir, B);
    }
  }
  else if (Precision::IsPositiveInfinite(VMin))
  {
    if (Precision::IsNegativeInfinite(VMax))
    {
      OpenMinMax(aDir, B);
    }
    else if (Precision::IsPositiveInfinite(VMax))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      ComputeCyl(S, UMin, UMax, 0., VMax, B);
      OpenMax(aDir, B);
    }
  }
  else
  {
    if (Precision::IsNegativeInfinite(VMax))
    {
      ComputeCyl(S, UMin, UMax, VMin, 0., B);
      OpenMin(aDir, B);
    }
    else if (Precision::IsPositiveInfinite(VMax))
    {
      ComputeCyl(S, UMin, UMax, VMin, 0., B);
      OpenMax(aDir, B);
    }
    else
    {
      ComputeCyl(S, UMin, UMax, VMin, VMax, B);
    }
  }

  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Cylinder&  S,
                 const Standard_Real VMin,
                 const Standard_Real VMax,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{

  BndLib::Add(S, 0., 2. * M_PI, VMin, VMax, Tol, B);
}

static void ComputeCone(const gp_Cone&      Cone,
                        const Standard_Real UMin,
                        const Standard_Real UMax,
                        const Standard_Real VMin,
                        const Standard_Real VMax,
                        Bnd_Box&            B)
{
  const gp_Ax3& aPos = Cone.Position();
  Standard_Real R    = Cone.RefRadius();
  Standard_Real sang = Cone.SemiAngle();
  gp_Circ       aC   = ElSLib::ConeVIso(aPos, R, sang, VMin);
  if (aC.Radius() > Precision::Confusion())
  {
    BndLib::Add(aC, UMin, UMax, 0., B);
  }
  else
  {
    B.Add(aC.Location());
  }
  //
  aC = ElSLib::ConeVIso(aPos, R, sang, VMax);
  if (aC.Radius() > Precision::Confusion())
  {
    BndLib::Add(aC, UMin, UMax, 0., B);
  }
  else
  {
    B.Add(aC.Location());
  }
}

void BndLib::Add(const gp_Cone&      S,
                 const Standard_Real UMin,
                 const Standard_Real UMax,
                 const Standard_Real VMin,
                 const Standard_Real VMax,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{
  // Cache axis direction for infinite cases.
  const gp_Dir& aD = S.Axis().Direction();

  if (Precision::IsNegativeInfinite(VMin))
  {
    if (Precision::IsNegativeInfinite(VMax))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else if (Precision::IsPositiveInfinite(VMax))
    {
      OpenMinMax(aD, B);
    }
    else
    {
      ComputeCone(S, UMin, UMax, 0., VMax, B);
      OpenMin(aD, B);
    }
  }
  else if (Precision::IsPositiveInfinite(VMin))
  {
    if (Precision::IsNegativeInfinite(VMax))
    {
      OpenMinMax(aD, B);
    }
    else if (Precision::IsPositiveInfinite(VMax))
    {
      throw Standard_Failure("BndLib::bad parameter");
    }
    else
    {
      ComputeCone(S, UMin, UMax, 0., VMax, B);
      OpenMax(aD, B);
    }
  }
  else
  {
    if (Precision::IsNegativeInfinite(VMax))
    {
      ComputeCone(S, UMin, UMax, VMin, 0., B);
      OpenMin(aD, B);
    }
    else if (Precision::IsPositiveInfinite(VMax))
    {
      ComputeCone(S, UMin, UMax, VMin, 0., B);
      OpenMax(aD, B);
    }
    else
    {
      ComputeCone(S, UMin, UMax, VMin, VMax, B);
    }
  }

  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Cone&      S,
                 const Standard_Real VMin,
                 const Standard_Real VMax,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{

  BndLib::Add(S, 0., 2. * M_PI, VMin, VMax, Tol, B);
}

static void ComputeSphere(const gp_Sphere&    Sphere,
                          const Standard_Real UMin,
                          const Standard_Real UMax,
                          const Standard_Real VMin,
                          const Standard_Real VMax,
                          Bnd_Box&            B)
{
  gp_Pnt        P = Sphere.Location();
  Standard_Real R = Sphere.Radius();
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  xmin = P.X() - R;
  xmax = P.X() + R;
  ymin = P.Y() - R;
  ymax = P.Y() + R;
  zmin = P.Z() - R;
  zmax = P.Z() + R;

  constexpr Standard_Real uper = 2. * M_PI - Precision::PConfusion();
  constexpr Standard_Real vper = M_PI - Precision::PConfusion();
  if (UMax - UMin >= uper && VMax - VMin >= vper)
  {
    // a whole sphere
    B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  }
  else
  {
    Standard_Real u, v;
    Standard_Real umax = UMin + 2. * M_PI;
    const gp_Ax3& Pos  = Sphere.Position();
    gp_Pnt        PExt = P;
    PExt.SetX(xmin);
    ElSLib::SphereParameters(Pos, R, PExt, u, v);
    u = ElCLib::InPeriod(u, UMin, umax);
    if (u >= UMin && u <= UMax && v >= VMin && v <= VMax)
    {
      B.Add(PExt);
    }
    //
    PExt.SetX(xmax);
    ElSLib::SphereParameters(Pos, R, PExt, u, v);
    u = ElCLib::InPeriod(u, UMin, umax);
    if (u >= UMin && u <= UMax && v >= VMin && v <= VMax)
    {
      B.Add(PExt);
    }
    PExt.SetX(P.X());
    //
    PExt.SetY(ymin);
    ElSLib::SphereParameters(Pos, R, PExt, u, v);
    u = ElCLib::InPeriod(u, UMin, umax);
    if (u >= UMin && u <= UMax && v >= VMin && v <= VMax)
    {
      B.Add(PExt);
    }
    //
    PExt.SetY(ymax);
    ElSLib::SphereParameters(Pos, R, PExt, u, v);
    u = ElCLib::InPeriod(u, UMin, umax);
    if (u >= UMin && u <= UMax && v >= VMin && v <= VMax)
    {
      B.Add(PExt);
    }
    PExt.SetY(P.Y());
    //
    PExt.SetZ(zmin);
    ElSLib::SphereParameters(Pos, R, PExt, u, v);
    u = ElCLib::InPeriod(u, UMin, umax);
    if (u >= UMin && u <= UMax && v >= VMin && v <= VMax)
    {
      B.Add(PExt);
    }
    //
    PExt.SetZ(zmax);
    ElSLib::SphereParameters(Pos, R, PExt, u, v);
    u = ElCLib::InPeriod(u, UMin, umax);
    if (u >= UMin && u <= UMax && v >= VMin && v <= VMax)
    {
      B.Add(PExt);
    }
    //
    // Add boundaries of patch
    // UMin, UMax
    gp_Circ aC = ElSLib::SphereUIso(Pos, R, UMin);
    BndLib::Add(aC, VMin, VMax, 0., B);
    aC = ElSLib::SphereUIso(Pos, R, UMax);
    BndLib::Add(aC, VMin, VMax, 0., B);
    // VMin, VMax
    aC = ElSLib::SphereVIso(Pos, R, VMin);
    BndLib::Add(aC, UMin, UMax, 0., B);
    aC = ElSLib::SphereVIso(Pos, R, VMax);
    BndLib::Add(aC, UMin, UMax, 0., B);
  }
}

//=======================================================================
// function : computeDegeneratedTorus
// purpose  : compute bounding box for degenerated torus
//=======================================================================

static void computeDegeneratedTorus(const gp_Torus&     theTorus,
                                    const Standard_Real theUMin,
                                    const Standard_Real theUMax,
                                    const Standard_Real theVMin,
                                    const Standard_Real theVMax,
                                    Bnd_Box&            theB)
{
  gp_Pnt        aP  = theTorus.Location();
  Standard_Real aRa = theTorus.MajorRadius();
  Standard_Real aRi = theTorus.MinorRadius();
  Standard_Real aXmin, anYmin, aZmin, aXmax, anYmax, aZmax;
  aXmin  = aP.X() - aRa - aRi;
  aXmax  = aP.X() + aRa + aRi;
  anYmin = aP.Y() - aRa - aRi;
  anYmax = aP.Y() + aRa + aRi;
  aZmin  = aP.Z() - aRi;
  aZmax  = aP.Z() + aRi;

  Standard_Real aPhi = std::acos(-aRa / aRi);

  constexpr Standard_Real anUper = 2. * M_PI - Precision::PConfusion();
  Standard_Real           aVper  = 2. * aPhi - Precision::PConfusion();
  if (theUMax - theUMin >= anUper && theVMax - theVMin >= aVper)
  {
    // a whole torus
    theB.Update(aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);
    return;
  }

  Standard_Real anU, aV;
  Standard_Real anUmax = theUMin + 2. * M_PI;
  const gp_Ax3& aPos   = theTorus.Position();
  gp_Pnt        aPExt  = aP;
  aPExt.SetX(aXmin);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theB.Add(aPExt);
  }
  //
  aPExt.SetX(aXmax);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theB.Add(aPExt);
  }
  aPExt.SetX(aP.X());
  //
  aPExt.SetY(anYmin);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theB.Add(aPExt);
  }
  //
  aPExt.SetY(anYmax);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theB.Add(aPExt);
  }
  aPExt.SetY(aP.Y());
  //
  aPExt.SetZ(aZmin);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theB.Add(aPExt);
  }
  //
  aPExt.SetZ(aZmax);
  ElSLib::TorusParameters(aPos, aRa, aRi, aPExt, anU, aV);
  anU = ElCLib::InPeriod(anU, theUMin, anUmax);
  if (anU >= theUMin && anU <= theUMax && aV >= theVMin && aV <= theVMax)
  {
    theB.Add(aPExt);
  }
  //
  // Add boundaries of patch
  // UMin, UMax
  gp_Circ aC = ElSLib::TorusUIso(aPos, aRa, aRi, theUMin);
  BndLib::Add(aC, theVMin, theVMax, 0., theB);
  aC = ElSLib::TorusUIso(aPos, aRa, aRi, theUMax);
  BndLib::Add(aC, theVMin, theVMax, 0., theB);
  // VMin, VMax
  aC = ElSLib::TorusVIso(aPos, aRa, aRi, theVMin);
  BndLib::Add(aC, theUMin, theUMax, 0., theB);
  aC = ElSLib::TorusVIso(aPos, aRa, aRi, theVMax);
  BndLib::Add(aC, theUMin, theUMax, 0., theB);
}

void BndLib::Add(const gp_Sphere&    S,
                 const Standard_Real UMin,
                 const Standard_Real UMax,
                 const Standard_Real VMin,
                 const Standard_Real VMax,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{
  ComputeSphere(S, UMin, UMax, VMin, VMax, B);
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Sphere& S, const Standard_Real Tol, Bnd_Box& B)
{
  gp_Pnt        P = S.Location();
  Standard_Real R = S.Radius();
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  xmin = P.X() - R;
  xmax = P.X() + R;
  ymin = P.Y() - R;
  ymax = P.Y() + R;
  zmin = P.Z() - R;
  zmax = P.Z() + R;
  B.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Torus&     S,
                 const Standard_Real UMin,
                 const Standard_Real UMax,
                 const Standard_Real VMin,
                 const Standard_Real VMax,
                 const Standard_Real Tol,
                 Bnd_Box&            B)
{

  Standard_Integer Fi1;
  Standard_Integer Fi2;
  if (VMax < VMin)
  {
    Fi1 = static_cast<Standard_Integer>(VMax / (M_PI / 4.));
    Fi2 = static_cast<Standard_Integer>(VMin / (M_PI / 4.));
  }
  else
  {
    Fi1 = static_cast<Standard_Integer>(VMin / (M_PI / 4.));
    Fi2 = static_cast<Standard_Integer>(VMax / (M_PI / 4.));
  }
  Fi2++;

  const Standard_Real Ra = S.MajorRadius();
  const Standard_Real Ri = S.MinorRadius();

  if (Fi2 < Fi1)
    return;

  if (Ra < Ri)
  {
    computeDegeneratedTorus(S, UMin, UMax, VMin, VMax, B);
    B.Enlarge(Tol);
    return;
  }

  // Cache direction vectors.
  const gp_XYZ aZDir   = S.Axis().Direction().XYZ();
  const gp_XYZ aLocXYZ = S.Location().XYZ();
  const gp_Pnt aXd(S.XAxis().Direction().XYZ());
  const gp_Pnt aYd(S.YAxis().Direction().XYZ());

  // Multipliers for torus cross-section points at 45-degree intervals.
  // radiusMult[i]: multiplier for Ri in radius calculation (Ra + Ri * radiusMult[i])
  // zMult[i]: multiplier for Ri in Z offset calculation (Ri * zMult[i])
  // THE_COS_PI4 = cos(45 deg) = sin(45 deg) = 0.707...
  constexpr Standard_Real aRadiusMult[8] =
    {1., THE_COS_PI4, 0., -THE_COS_PI4, -1., -THE_COS_PI4, 0., THE_COS_PI4};
  constexpr Standard_Real aZMult[8] =
    {0., THE_COS_PI4, 1., THE_COS_PI4, 0., -THE_COS_PI4, -1., -THE_COS_PI4};

  // Lambda to add torus cross-section point by index (0-7).
  const auto addTorusPoint = [&](Standard_Integer theIdx) {
    const Standard_Real aRadius = Ra + Ri * aRadiusMult[theIdx];
    const gp_Pnt        aCenter(aLocXYZ + (Ri * aZMult[theIdx]) * aZDir);
    Compute(UMin, UMax, aRadius, aRadius, aXd, aYd, aCenter, B);
  };

  // Add points from Fi1 to Fi2, handling wrap-around for indices.
  // Use ((i % 8) + 8) % 8 to handle negative indices correctly
  // (C++ modulo can return negative values for negative dividends).
  for (Standard_Integer i = Fi1; i <= Fi2; ++i)
  {
    addTorusPoint(((i % 8) + 8) % 8);
  }

  B.Enlarge(Tol);
}

void BndLib::Add(const gp_Torus& S, const Standard_Real Tol, Bnd_Box& B)
{
  const Standard_Real aRMa = S.MajorRadius();
  const Standard_Real aRmi = S.MinorRadius();
  const Standard_Real aR   = aRMa + aRmi;
  const gp_XYZ        aO   = S.Location().XYZ();
  const gp_XYZ        aXd  = S.XAxis().Direction().XYZ();
  const gp_XYZ        aYd  = S.YAxis().Direction().XYZ();
  const gp_XYZ        aZd  = S.Axis().Direction().XYZ();
  // Precompute scaled direction vectors.
  const gp_XYZ aRXd  = aR * aXd;
  const gp_XYZ aRYd  = aR * aYd;
  const gp_XYZ aRiZd = aRmi * aZd;
  // Add 8 corner points of torus bounding box.
  B.Add(gp_Pnt(aO - aRXd - aRYd + aRiZd));
  B.Add(gp_Pnt(aO - aRXd - aRYd - aRiZd));
  B.Add(gp_Pnt(aO + aRXd - aRYd + aRiZd));
  B.Add(gp_Pnt(aO + aRXd - aRYd - aRiZd));
  B.Add(gp_Pnt(aO - aRXd + aRYd + aRiZd));
  B.Add(gp_Pnt(aO - aRXd + aRYd - aRiZd));
  B.Add(gp_Pnt(aO + aRXd + aRYd + aRiZd));
  B.Add(gp_Pnt(aO + aRXd + aRYd - aRiZd));
  B.Enlarge(Tol);
}

//=================================================================================================

Standard_Integer ComputeBox(const gp_Hypr&      aHypr,
                            const Standard_Real aT1,
                            const Standard_Real aT2,
                            Bnd_Box&            aBox)
{
  Standard_Integer i, iErr;
  Standard_Real    aRmaj, aRmin, aA, aB, aABP, aBAM, aT3, aCf, aEps;
  gp_Pnt           aP1, aP2, aP3, aP0;
  //
  //
  aP1 = ElCLib::Value(aT1, aHypr);
  aP2 = ElCLib::Value(aT2, aHypr);
  //
  aBox.Add(aP1);
  aBox.Add(aP2);
  //
  if (aT1 * aT2 < 0.)
  {
    aP0 = ElCLib::Value(0., aHypr);
    aBox.Add(aP0);
  }
  //
  aEps = Epsilon(1.);
  iErr = 1;
  //
  const gp_Ax2& aPos  = aHypr.Position();
  const gp_XYZ& aXDir = aPos.XDirection().XYZ();
  const gp_XYZ& aYDir = aPos.YDirection().XYZ();
  aRmaj               = aHypr.MajorRadius();
  aRmin               = aHypr.MinorRadius();
  //
  // Find extrema for each coordinate (X, Y, Z) independently.
  // Each coordinate can have its extremum at a different parameter value.
  for (i = 1; i <= 3; ++i)
  {
    aA = aRmin * aYDir.Coord(i);
    aB = aRmaj * aXDir.Coord(i);
    //
    aABP = aA + aB;
    aBAM = aB - aA;
    //
    aABP = std::abs(aABP);
    aBAM = std::abs(aBAM);
    //
    if (aABP < aEps || aBAM < aEps)
    {
      continue;
    }
    //
    aCf = aBAM / aABP;
    aT3 = 0.5 * std::log(aCf);
    //
    if (aT3 < aT1 || aT3 > aT2)
    {
      continue;
    }
    // Add extremum point for this coordinate.
    aP3 = ElCLib::Value(aT3, aHypr);
    aBox.Add(aP3);
    iErr = 0;
  }
  //
  return iErr;
}
