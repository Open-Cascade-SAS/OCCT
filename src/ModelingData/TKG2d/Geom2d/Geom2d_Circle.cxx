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

Handle(Geom2d_Geometry) Geom2d_Circle::Copy() const
{
  Handle(Geom2d_Circle) C;
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

Geom2d_Circle::Geom2d_Circle(const Ax2d&            A,
                             const Standard_Real    Radius,
                             const Standard_Boolean Sense)
    : radius(Radius)
{

  if (Radius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = gp_Ax22d(A, Sense);
}

//=================================================================================================

Geom2d_Circle::Geom2d_Circle(const gp_Ax22d& A, const Standard_Real Radius)

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

void Geom2d_Circle::SetRadius(const Standard_Real R)
{
  if (R < 0.0)
  {
    throw Standard_ConstructionError();
  }
  radius = R;
}

//=================================================================================================

Standard_Real Geom2d_Circle::Radius() const
{
  return radius;
}

//=================================================================================================

gp_Circ2d Geom2d_Circle::Circ2d() const
{
  return gp_Circ2d(pos, radius);
}

//=================================================================================================

Standard_Real Geom2d_Circle::ReversedParameter(const Standard_Real U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

Standard_Real Geom2d_Circle::Eccentricity() const
{
  return 0.0;
}

//=================================================================================================

Standard_Real Geom2d_Circle::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

Standard_Real Geom2d_Circle::LastParameter() const
{
  return 2.0 * M_PI;
}

//=================================================================================================

Standard_Boolean Geom2d_Circle::IsClosed() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Boolean Geom2d_Circle::IsPeriodic() const
{
  return Standard_True;
}

//=================================================================================================

void Geom2d_Circle::D0(const Standard_Real U, Pnt2d& P) const
{
  P = ElCLib::CircleValue(U, pos, radius);
}

//=================================================================================================

void Geom2d_Circle::D1(const Standard_Real U, Pnt2d& P, Vec2d& V1) const
{
  ElCLib::CircleD1(U, pos, radius, P, V1);
}

//=================================================================================================

void Geom2d_Circle::D2(const Standard_Real U, Pnt2d& P, Vec2d& V1, Vec2d& V2) const
{
  ElCLib::CircleD2(U, pos, radius, P, V1, V2);
}

//=================================================================================================

void Geom2d_Circle::D3(const Standard_Real U, Pnt2d& P, Vec2d& V1, Vec2d& V2, Vec2d& V3) const
{
  ElCLib::CircleD3(U, pos, radius, P, V1, V2, V3);
}

//=================================================================================================

Vec2d Geom2d_Circle::DN(const Standard_Real U, const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::CircleDN(U, pos, radius, N);
}

//=================================================================================================

void Geom2d_Circle::Transform(const Trsf2d& T)
{
  radius = radius * Abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom2d_Circle::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, radius)
}
