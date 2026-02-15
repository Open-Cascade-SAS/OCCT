// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

#include <ElCLib.hxx>
#include <Geom_Parabola.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Parabola, Geom_Conic)

typedef Geom_Parabola Parabola;
typedef gp_Ax1        Ax1;
typedef gp_Ax2        Ax2;
typedef gp_Pnt        Pnt;
typedef gp_Trsf       Trsf;
typedef gp_Vec        Vec;
typedef gp_XYZ        XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_Parabola::Copy() const
{

  occ::handle<Geom_Parabola> Prb;
  Prb = new Parabola(pos, focalLength);
  return Prb;
}

//=================================================================================================

Geom_Parabola::Geom_Parabola(const gp_Parab& Prb)
    : focalLength(Prb.Focal())
{
  pos = Prb.Position();
}

//=================================================================================================

Geom_Parabola::Geom_Parabola(const Ax2& A2, const double Focal)
    : focalLength(Focal)
{

  if (Focal < 0.0)
    throw Standard_ConstructionError();
  pos = A2;
}

//=================================================================================================

Geom_Parabola::Geom_Parabola(const Ax1& D, const Pnt& F)
{

  gp_Parab Prb(D, F);
  pos         = Prb.Position();
  focalLength = Prb.Focal();
}

//=================================================================================================

double Geom_Parabola::ReversedParameter(const double U) const
{
  return (-U);
}

//=================================================================================================

bool Geom_Parabola::IsClosed() const
{
  return false;
}

//=================================================================================================

bool Geom_Parabola::IsPeriodic() const
{
  return false;
}

//=================================================================================================

double Geom_Parabola::Eccentricity() const
{
  return 1.0;
}

//=================================================================================================

double Geom_Parabola::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

double Geom_Parabola::Focal() const
{
  return focalLength;
}

//=================================================================================================

double Geom_Parabola::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

double Geom_Parabola::Parameter() const
{
  return 2.0 * focalLength;
}

//=================================================================================================

void Geom_Parabola::SetFocal(const double Focal)
{

  if (Focal < 0.0)
    throw Standard_ConstructionError();
  focalLength = Focal;
}

//=================================================================================================

void Geom_Parabola::SetParab(const gp_Parab& Prb)
{

  focalLength = Prb.Focal();
  pos         = Prb.Position();
}

//=================================================================================================

Ax1 Geom_Parabola::Directrix() const
{

  gp_Parab Prb(pos, focalLength);
  return Prb.Directrix();
}

//=================================================================================================

gp_Pnt Geom_Parabola::EvalD0(const double U) const
{
  return ElCLib::ParabolaValue(U, pos, focalLength);
}

//=================================================================================================

Geom_Curve::ResD1 Geom_Parabola::EvalD1(const double U) const
{
  Geom_Curve::ResD1 aResult;
  ElCLib::ParabolaD1(U, pos, focalLength, aResult.Point, aResult.D1);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD2 Geom_Parabola::EvalD2(const double U) const
{
  Geom_Curve::ResD2 aResult;
  ElCLib::ParabolaD2(U, pos, focalLength, aResult.Point, aResult.D1, aResult.D2);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD3 Geom_Parabola::EvalD3(const double U) const
{
  Geom_Curve::ResD3 aResult;
  ElCLib::ParabolaD2(U, pos, focalLength, aResult.Point, aResult.D1, aResult.D2);
  aResult.D3.SetCoord(0.0, 0.0, 0.0);
  return aResult;
}

//=================================================================================================

gp_Vec Geom_Parabola::EvalDN(const double U, const int N) const
{
  if (N < 1)
    throw Geom_UndefinedDerivative();
  return ElCLib::ParabolaDN(U, pos, focalLength, N);
}

//=================================================================================================

Pnt Geom_Parabola::Focus() const
{

  double Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord(Xp, Yp, Zp);
  pos.XDirection().Coord(Xd, Yd, Zd);
  return Pnt(Xp + focalLength * Xd, Yp + focalLength * Yd, Zp + focalLength * Zd);
}

//=================================================================================================

gp_Parab Geom_Parabola::Parab() const
{

  return gp_Parab(pos, focalLength);
}

//=================================================================================================

void Geom_Parabola::Transform(const Trsf& T)
{

  focalLength *= std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

double Geom_Parabola::TransformedParameter(const double U, const gp_Trsf& T) const
{
  if (Precision::IsInfinite(U))
    return U;
  return U * std::abs(T.ScaleFactor());
}

//=================================================================================================

double Geom_Parabola::ParametricTransformation(const gp_Trsf& T) const
{
  return std::abs(T.ScaleFactor());
}

//=================================================================================================

void Geom_Parabola::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, focalLength)
}
