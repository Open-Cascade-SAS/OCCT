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
#include <Geom_Circle.hxx>
#include <Geom_Geometry.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Circle, Geom_Conic)

typedef Geom_Circle Circle;
typedef gp_Ax2      Ax2;
typedef gp_Pnt      Pnt;
typedef gp_Trsf     Trsf;
typedef gp_Vec      Vec;
typedef gp_XYZ      XYZ;

//=================================================================================================

Handle(Geom_Geometry) Geom_Circle::Copy() const
{

  Handle(Geom_Circle) C;
  C = new Circle(pos, radius);
  return C;
}

//=================================================================================================

Geom_Circle::Geom_Circle(const gp_Circ& C)
    : radius(C.Radius())
{

  pos = C.Position();
}

//=================================================================================================

Geom_Circle::Geom_Circle(const Ax2& A2, const Standard_Real R)
    : radius(R)
{

  if (R < 0.0)
    throw Standard_ConstructionError();
  pos = A2;
}

//=================================================================================================

Standard_Boolean Geom_Circle::IsClosed() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Boolean Geom_Circle::IsPeriodic() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Real Geom_Circle::ReversedParameter(const Standard_Real U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

Standard_Real Geom_Circle::Eccentricity() const
{
  return 0.0;
}

//=================================================================================================

Standard_Real Geom_Circle::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

Standard_Real Geom_Circle::LastParameter() const
{
  return 2.0 * M_PI;
}

//=================================================================================================

gp_Circ Geom_Circle::Circ() const
{
  return gp_Circ(pos, radius);
}

//=================================================================================================

void Geom_Circle::SetCirc(const gp_Circ& C)
{

  radius = C.Radius();
  pos    = C.Position();
}

//=================================================================================================

void Geom_Circle::SetRadius(const Standard_Real R)
{

  if (R < 0.0)
    throw Standard_ConstructionError();
  radius = R;
}

//=================================================================================================

Standard_Real Geom_Circle::Radius() const
{
  return radius;
}

//=================================================================================================

void Geom_Circle::D0(const Standard_Real U, Pnt& P) const
{

  P = ElCLib::CircleValue(U, pos, radius);
}

//=================================================================================================

void Geom_Circle::D1(const Standard_Real U, Pnt& P, Vec& V1) const
{

  ElCLib::CircleD1(U, pos, radius, P, V1);
}

//=================================================================================================

void Geom_Circle::D2(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const
{

  ElCLib::CircleD2(U, pos, radius, P, V1, V2);
}

//=================================================================================================

void Geom_Circle::D3(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2, Vec& V3) const
{

  ElCLib::CircleD3(U, pos, radius, P, V1, V2, V3);
}

//=================================================================================================

Vec Geom_Circle::DN(const Standard_Real U, const Standard_Integer N) const
{

  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::CircleDN(U, pos, radius, N);
}

//=================================================================================================

void Geom_Circle::Transform(const Trsf& T)
{

  radius = radius * Abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_Circle::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, radius)
}
