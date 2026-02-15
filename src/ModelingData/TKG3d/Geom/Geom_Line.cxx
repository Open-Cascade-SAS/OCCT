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
#include <Geom_Line.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Line, Geom_Curve)

//=================================================================================================

occ::handle<Geom_Geometry> Geom_Line::Copy() const
{

  occ::handle<Geom_Line> L;
  L = new Geom_Line(pos);
  return L;
}

//=================================================================================================

Geom_Line::Geom_Line(const gp_Ax1& A)
    : pos(A)
{
}

//=================================================================================================

Geom_Line::Geom_Line(const gp_Lin& L)
    : pos(L.Position())
{
}

//=================================================================================================

Geom_Line::Geom_Line(const gp_Pnt& P, const gp_Dir& V)
    : pos(P, V)
{
}

//=================================================================================================

void Geom_Line::Reverse()
{
  pos.Reverse();
}

//=================================================================================================

double Geom_Line::ReversedParameter(const double U) const
{
  return (-U);
}

//=================================================================================================

void Geom_Line::SetDirection(const gp_Dir& V)
{
  pos.SetDirection(V);
}

//=================================================================================================

void Geom_Line::SetLin(const gp_Lin& L)
{
  pos = L.Position();
}

//=================================================================================================

void Geom_Line::SetLocation(const gp_Pnt& P)
{
  pos.SetLocation(P);
}

//=================================================================================================

void Geom_Line::SetPosition(const gp_Ax1& A1)
{
  pos = A1;
}

//=================================================================================================

const gp_Ax1& Geom_Line::Position() const
{
  return pos;
}

//=================================================================================================

bool Geom_Line::IsClosed() const
{
  return false;
}

//=================================================================================================

bool Geom_Line::IsPeriodic() const
{
  return false;
}

//=================================================================================================

GeomAbs_Shape Geom_Line::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

double Geom_Line::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

double Geom_Line::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

gp_Lin Geom_Line::Lin() const
{
  return gp_Lin(pos);
}

//=================================================================================================

bool Geom_Line::IsCN(const int) const
{
  return true;
}

//=================================================================================================

void Geom_Line::Transform(const gp_Trsf& T)
{
  pos.Transform(T);
}

//=================================================================================================

gp_Pnt Geom_Line::EvalD0(const double U) const
{
  return ElCLib::LineValue(U, pos);
}

//=================================================================================================

Geom_Curve::ResD1 Geom_Line::EvalD1(const double U) const
{
  Geom_Curve::ResD1 aResult;
  ElCLib::LineD1(U, pos, aResult.Point, aResult.D1);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD2 Geom_Line::EvalD2(const double U) const
{
  Geom_Curve::ResD2 aResult;
  ElCLib::LineD1(U, pos, aResult.Point, aResult.D1);
  aResult.D2.SetCoord(0.0, 0.0, 0.0);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD3 Geom_Line::EvalD3(const double U) const
{
  Geom_Curve::ResD3 aResult;
  ElCLib::LineD1(U, pos, aResult.Point, aResult.D1);
  aResult.D2.SetCoord(0.0, 0.0, 0.0);
  aResult.D3.SetCoord(0.0, 0.0, 0.0);
  return aResult;
}

//=================================================================================================

gp_Vec Geom_Line::EvalDN(const double, const int N) const
{
  if (N < 1)
    throw Geom_UndefinedDerivative();
  if (N == 1)
    return gp_Vec(pos.Direction());
  else
    return gp_Vec(0.0, 0.0, 0.0);
}

//=================================================================================================

double Geom_Line::TransformedParameter(const double U, const gp_Trsf& T) const
{
  if (Precision::IsInfinite(U))
    return U;
  return U * std::abs(T.ScaleFactor());
}

//=================================================================================================

double Geom_Line::ParametricTransformation(const gp_Trsf& T) const
{
  return std::abs(T.ScaleFactor());
}

//=================================================================================================

void Geom_Line::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &pos)
}
