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
#include <Geom2d_Parabola.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Parabola, Geom2d_Conic)

typedef Geom2d_Parabola Parabola;
typedef gp_Ax2d         Ax2d;
typedef gp_Dir2d        Dir2d;
typedef gp_Pnt2d        Pnt2d;
typedef gp_Vec2d        Vec2d;
typedef gp_Trsf2d       Trsf2d;
typedef gp_XY           XY;

//=================================================================================================

Handle(Geom2d_Geometry) Geom2d_Parabola::Copy() const
{
  Handle(Geom2d_Parabola) Prb;
  Prb = new Parabola(pos, focalLength);
  return Prb;
}

//=================================================================================================

Geom2d_Parabola::Geom2d_Parabola(const gp_Parab2d& Prb)
{
  focalLength = Prb.Focal();
  pos         = Prb.Axis();
}

//=================================================================================================

Geom2d_Parabola::Geom2d_Parabola(const Ax2d&            MirrorAxis,
                                 const Standard_Real    Focal,
                                 const Standard_Boolean Sense)
    : focalLength(Focal)
{
  if (Focal < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = gp_Ax22d(MirrorAxis, Sense);
}

//=================================================================================================

Geom2d_Parabola::Geom2d_Parabola(const gp_Ax22d& Axis, const Standard_Real Focal)
    : focalLength(Focal)
{
  if (Focal < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = Axis;
}

//=================================================================================================

Geom2d_Parabola::Geom2d_Parabola(const Ax2d& D, const Pnt2d& F)
{

  gp_Parab2d Prb(D, F);
  pos         = Prb.Axis();
  focalLength = Prb.Focal();
}

//=================================================================================================

void Geom2d_Parabola::SetFocal(const Standard_Real Focal)
{
  if (Focal < 0.0)
    throw Standard_ConstructionError();
  focalLength = Focal;
}

//=================================================================================================

void Geom2d_Parabola::SetParab2d(const gp_Parab2d& Prb)
{
  focalLength = Prb.Focal();
  pos         = Prb.Axis();
}

//=================================================================================================

gp_Parab2d Geom2d_Parabola::Parab2d() const
{
  return gp_Parab2d(pos, focalLength);
}

//=================================================================================================

Standard_Real Geom2d_Parabola::ReversedParameter(const Standard_Real U) const
{
  return (-U);
}

//=================================================================================================

Standard_Real Geom2d_Parabola::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

Standard_Real Geom2d_Parabola::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

Standard_Boolean Geom2d_Parabola::IsClosed() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Geom2d_Parabola::IsPeriodic() const
{
  return Standard_False;
}

//=================================================================================================

Ax2d Geom2d_Parabola::Directrix() const
{
  gp_Parab2d Prb(pos, focalLength);
  return Prb.Directrix();
}

//=================================================================================================

Standard_Real Geom2d_Parabola::Eccentricity() const
{
  return 1.0;
}

//=================================================================================================

Pnt2d Geom2d_Parabola::Focus() const
{
  XY Pxy = pos.XDirection().XY();
  Pxy.Multiply(focalLength);
  Pxy.Add(pos.Location().XY());
  return Pnt2d(Pxy);
}

//=================================================================================================

Standard_Real Geom2d_Parabola::Focal() const
{
  return focalLength;
}

//=================================================================================================

Standard_Real Geom2d_Parabola::Parameter() const
{
  return 2.0 * focalLength;
}

//=================================================================================================

void Geom2d_Parabola::D0(const Standard_Real U, Pnt2d& P) const
{
  P = ElCLib::ParabolaValue(U, pos, focalLength);
}

//=================================================================================================

void Geom2d_Parabola::D1(const Standard_Real U, Pnt2d& P, Vec2d& V1) const
{
  ElCLib::ParabolaD1(U, pos, focalLength, P, V1);
}

//=================================================================================================

void Geom2d_Parabola::D2(const Standard_Real U, Pnt2d& P, Vec2d& V1, Vec2d& V2) const
{
  ElCLib::ParabolaD2(U, pos, focalLength, P, V1, V2);
}

//=================================================================================================

void Geom2d_Parabola::D3(const Standard_Real U, Pnt2d& P, Vec2d& V1, Vec2d& V2, Vec2d& V3) const
{
  ElCLib::ParabolaD2(U, pos, focalLength, P, V1, V2);
  V3.SetCoord(0.0, 0.0);
}

//=================================================================================================

Vec2d Geom2d_Parabola::DN(const Standard_Real U, const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::ParabolaDN(U, pos, focalLength, N);
}

//=================================================================================================

void Geom2d_Parabola::Transform(const Trsf2d& T)
{
  focalLength *= Abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

Standard_Real Geom2d_Parabola::TransformedParameter(const Standard_Real U, const gp_Trsf2d& T) const
{
  if (Precision::IsInfinite(U))
    return U;
  return U * Abs(T.ScaleFactor());
}

//=================================================================================================

Standard_Real Geom2d_Parabola::ParametricTransformation(const gp_Trsf2d& T) const
{
  return Abs(T.ScaleFactor());
}

//=================================================================================================

void Geom2d_Parabola::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, focalLength)
}
