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
#include <Geom2d_Line.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Line, Geom2d_Curve)

typedef Geom2d_Line Line;
typedef gp_Ax2d     Ax2d;
typedef gp_Dir2d    Dir2d;
typedef gp_Pnt2d    Pnt2d;
typedef gp_Vec2d    Vec2d;
typedef gp_Trsf2d   Trsf2d;
typedef gp_XY       XY;

//=================================================================================================

occ::handle<Geom2d_Geometry> Geom2d_Line::Copy() const
{
  occ::handle<Geom2d_Line> L;
  L = new Line(pos);
  return L;
}

//=================================================================================================

Geom2d_Line::Geom2d_Line(const Ax2d& A)
    : pos(A)
{
}

//=================================================================================================

Geom2d_Line::Geom2d_Line(const gp_Lin2d& L)
    : pos(L.Position())
{
}

//=================================================================================================

Geom2d_Line::Geom2d_Line(const Pnt2d& P, const Dir2d& V)
    : pos(P, V)
{
}

//=================================================================================================

void Geom2d_Line::SetDirection(const Dir2d& V)
{
  pos.SetDirection(V);
}

//=================================================================================================

const gp_Dir2d& Geom2d_Line::Direction() const
{
  return pos.Direction();
}

//=================================================================================================

void Geom2d_Line::SetLin2d(const gp_Lin2d& L)
{
  pos = L.Position();
}

//=================================================================================================

void Geom2d_Line::SetLocation(const Pnt2d& P)
{
  pos.SetLocation(P);
}

//=================================================================================================

const gp_Pnt2d& Geom2d_Line::Location() const
{
  return pos.Location();
}

//=================================================================================================

void Geom2d_Line::SetPosition(const Ax2d& A)
{
  pos = A;
}

//=================================================================================================

const gp_Ax2d& Geom2d_Line::Position() const
{
  return pos;
}

//=================================================================================================

gp_Lin2d Geom2d_Line::Lin2d() const
{
  return gp_Lin2d(pos);
}

//=================================================================================================

void Geom2d_Line::Reverse()
{
  pos.Reverse();
}

//=================================================================================================

double Geom2d_Line::ReversedParameter(const double U) const
{
  return (-U);
}

//=================================================================================================

double Geom2d_Line::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

double Geom2d_Line::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

bool Geom2d_Line::IsClosed() const
{
  return false;
}

//=================================================================================================

bool Geom2d_Line::IsPeriodic() const
{
  return false;
}

//=================================================================================================

GeomAbs_Shape Geom2d_Line::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

bool Geom2d_Line::IsCN(const int) const
{
  return true;
}

//=================================================================================================

std::optional<gp_Pnt2d> Geom2d_Line::EvalD0(const double U) const
{
  return ElCLib::LineValue(U, pos);
}

//=================================================================================================

std::optional<Geom2d_CurveD1> Geom2d_Line::EvalD1(const double U) const
{
  std::optional<Geom2d_CurveD1> aResult{std::in_place};
  ElCLib::LineD1(U, pos, aResult->Point, aResult->D1);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD2> Geom2d_Line::EvalD2(const double U) const
{
  std::optional<Geom2d_CurveD2> aResult{std::in_place};
  ElCLib::LineD1(U, pos, aResult->Point, aResult->D1);
  aResult->D2.SetCoord(0.0, 0.0);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD3> Geom2d_Line::EvalD3(const double U) const
{
  std::optional<Geom2d_CurveD3> aResult{std::in_place};
  ElCLib::LineD1(U, pos, aResult->Point, aResult->D1);
  aResult->D2.SetCoord(0.0, 0.0);
  aResult->D3.SetCoord(0.0, 0.0);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec2d> Geom2d_Line::EvalDN(const double, const int N) const
{
  Standard_RangeError_Raise_if(N <= 0, " ");
  if (N == 1)
    return gp_Vec2d(pos.Direction());
  else
    return gp_Vec2d(0.0, 0.0);
}

//=================================================================================================

void Geom2d_Line::Transform(const Trsf2d& T)
{
  pos.Transform(T);
}

//=================================================================================================

double Geom2d_Line::TransformedParameter(const double U, const gp_Trsf2d& T) const
{
  if (Precision::IsInfinite(U))
    return U;
  return U * std::abs(T.ScaleFactor());
}

//=================================================================================================

double Geom2d_Line::ParametricTransformation(const gp_Trsf2d& T) const
{
  return std::abs(T.ScaleFactor());
}

//=================================================================================================

double Geom2d_Line::Distance(const gp_Pnt2d& P) const
{

  gp_Lin2d L(pos);
  return L.Distance(P);
}

//=================================================================================================

void Geom2d_Line::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &pos)
}
