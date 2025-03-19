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

Handle(Geom_Geometry) Geom_Parabola::Copy() const
{

  Handle(Geom_Parabola) Prb;
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

Geom_Parabola::Geom_Parabola(const Ax2& A2, const Standard_Real Focal)
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

Standard_Real Geom_Parabola::ReversedParameter(const Standard_Real U) const
{
  return (-U);
}

//=================================================================================================

Standard_Boolean Geom_Parabola::IsClosed() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Geom_Parabola::IsPeriodic() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Real Geom_Parabola::Eccentricity() const
{
  return 1.0;
}

//=================================================================================================

Standard_Real Geom_Parabola::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

Standard_Real Geom_Parabola::Focal() const
{
  return focalLength;
}

//=================================================================================================

Standard_Real Geom_Parabola::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

Standard_Real Geom_Parabola::Parameter() const
{
  return 2.0 * focalLength;
}

//=================================================================================================

void Geom_Parabola::SetFocal(const Standard_Real Focal)
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

void Geom_Parabola::D0(const Standard_Real U, Pnt& P) const
{

  P = ElCLib::ParabolaValue(U, pos, focalLength);
}

//=================================================================================================

void Geom_Parabola::D1(const Standard_Real U, Pnt& P, Vec& V1) const
{

  ElCLib::ParabolaD1(U, pos, focalLength, P, V1);
}

//=================================================================================================

void Geom_Parabola::D2(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const
{

  ElCLib::ParabolaD2(U, pos, focalLength, P, V1, V2);
}

//=================================================================================================

void Geom_Parabola::D3(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2, Vec& V3) const
{

  ElCLib::ParabolaD2(U, pos, focalLength, P, V1, V2);
  V3.SetCoord(0.0, 0.0, 0.0);
}

//=================================================================================================

Vec Geom_Parabola::DN(const Standard_Real U, const Standard_Integer N) const
{

  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::ParabolaDN(U, pos, focalLength, N);
}

//=================================================================================================

Pnt Geom_Parabola::Focus() const
{

  Standard_Real Xp, Yp, Zp, Xd, Yd, Zd;
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

  focalLength *= Abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

Standard_Real Geom_Parabola::TransformedParameter(const Standard_Real U, const gp_Trsf& T) const
{
  if (Precision::IsInfinite(U))
    return U;
  return U * Abs(T.ScaleFactor());
}

//=================================================================================================

Standard_Real Geom_Parabola::ParametricTransformation(const gp_Trsf& T) const
{
  return Abs(T.ScaleFactor());
}

//=================================================================================================

void Geom_Parabola::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, focalLength)
}
