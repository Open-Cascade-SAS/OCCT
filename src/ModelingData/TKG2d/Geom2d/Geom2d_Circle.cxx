// Created on: 1993-03-24
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
#include <Geom2d_Circle.hxx>
#include <Geom2d_Geometry.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Circle, Geom2d_Conic)

typedef Geom2d_Circle Circle;
typedef gp_Ax2d       Ax2d;
typedef gp_Dir2d      Dir2d;
typedef gp_Pnt2d      Pnt2d;
typedef gp_Trsf2d     Trsf2d;
typedef gp_Vec2d      Vec2d;
typedef gp_XY         XY;

//=================================================================================================

occ::handle<Geom2d_Geometry> Geom2d_Circle::Copy() const
{
  occ::handle<Geom2d_Circle> C;
  C = new Circle(pos, radius);
  return C;
}

//=================================================================================================

Geom2d_Circle::Geom2d_Circle(const gp_Circ2d& C)
    : radius(C.Radius())
{

  pos = C.Axis();
}

//=================================================================================================

Geom2d_Circle::Geom2d_Circle(const Ax2d& A, const double Radius, const bool Sense)
    : radius(Radius)
{

  if (Radius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = gp_Ax22d(A, Sense);
}

//=================================================================================================

Geom2d_Circle::Geom2d_Circle(const gp_Ax22d& A, const double Radius)

    : radius(Radius)
{

  if (Radius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = A;
}

//=================================================================================================

void Geom2d_Circle::SetCirc2d(const gp_Circ2d& C)
{

  radius = C.Radius();
  pos    = C.Axis();
}

//=================================================================================================

void Geom2d_Circle::SetRadius(const double R)
{
  if (R < 0.0)
  {
    throw Standard_ConstructionError();
  }
  radius = R;
}

//=================================================================================================

double Geom2d_Circle::Radius() const
{
  return radius;
}

//=================================================================================================

gp_Circ2d Geom2d_Circle::Circ2d() const
{
  return gp_Circ2d(pos, radius);
}

//=================================================================================================

double Geom2d_Circle::ReversedParameter(const double U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

double Geom2d_Circle::Eccentricity() const
{
  return 0.0;
}

//=================================================================================================

double Geom2d_Circle::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

double Geom2d_Circle::LastParameter() const
{
  return 2.0 * M_PI;
}

//=================================================================================================

bool Geom2d_Circle::IsClosed() const
{
  return true;
}

//=================================================================================================

bool Geom2d_Circle::IsPeriodic() const
{
  return true;
}

//=================================================================================================

std::optional<gp_Pnt2d> Geom2d_Circle::EvalD0(const double U) const
{
  return ElCLib::CircleValue(U, pos, radius);
}

//=================================================================================================

std::optional<Geom2d_CurveD1> Geom2d_Circle::EvalD1(const double U) const
{
  std::optional<Geom2d_CurveD1> aResult{std::in_place};
  ElCLib::CircleD1(U, pos, radius, aResult->Point, aResult->D1);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD2> Geom2d_Circle::EvalD2(const double U) const
{
  std::optional<Geom2d_CurveD2> aResult{std::in_place};
  ElCLib::CircleD2(U, pos, radius, aResult->Point, aResult->D1, aResult->D2);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD3> Geom2d_Circle::EvalD3(const double U) const
{
  std::optional<Geom2d_CurveD3> aResult{std::in_place};
  ElCLib::CircleD3(U, pos, radius, aResult->Point, aResult->D1, aResult->D2, aResult->D3);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec2d> Geom2d_Circle::EvalDN(const double U, const int N) const
{
  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::CircleDN(U, pos, radius, N);
}

//=================================================================================================

void Geom2d_Circle::Transform(const Trsf2d& T)
{
  radius = radius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom2d_Circle::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, radius)
}
