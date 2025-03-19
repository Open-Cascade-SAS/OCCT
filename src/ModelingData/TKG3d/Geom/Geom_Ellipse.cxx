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
#include <Geom_Ellipse.hxx>
#include <Geom_Geometry.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Ellipse, Geom_Conic)

typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Vec  Vec;
typedef gp_Trsf Trsf;
typedef gp_XYZ  XYZ;

//=================================================================================================

Handle(Geom_Geometry) Geom_Ellipse::Copy() const
{
  Handle(Geom_Ellipse) E;
  E = new Geom_Ellipse(pos, majorRadius, minorRadius);
  return E;
}

//=================================================================================================

Geom_Ellipse::Geom_Ellipse(const gp_Elips& E)
    : majorRadius(E.MajorRadius()),
      minorRadius(E.MinorRadius())
{
  pos = E.Position();
}

//=================================================================================================

Geom_Ellipse::Geom_Ellipse(const Ax2&          A,
                           const Standard_Real MajorRadius,
                           const Standard_Real MinorRadius)
    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{

  if (MajorRadius < MinorRadius || MinorRadius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = A;
}

//=================================================================================================

Standard_Boolean Geom_Ellipse::IsClosed() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Boolean Geom_Ellipse::IsPeriodic() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Real Geom_Ellipse::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

Standard_Real Geom_Ellipse::LastParameter() const
{
  return 2.0 * M_PI;
}

//=================================================================================================

Standard_Real Geom_Ellipse::MajorRadius() const
{
  return majorRadius;
}

//=================================================================================================

Standard_Real Geom_Ellipse::MinorRadius() const
{
  return minorRadius;
}

//=================================================================================================

void Geom_Ellipse::SetElips(const gp_Elips& E)
{

  majorRadius = E.MajorRadius();
  minorRadius = E.MinorRadius();
  pos         = E.Position();
}

//=================================================================================================

void Geom_Ellipse::SetMajorRadius(const Standard_Real MajorRadius)
{

  if (MajorRadius < minorRadius)
    throw Standard_ConstructionError();
  else
    majorRadius = MajorRadius;
}

//=================================================================================================

void Geom_Ellipse::SetMinorRadius(const Standard_Real MinorRadius)
{

  if (MinorRadius < 0 || majorRadius < MinorRadius)
  {
    throw Standard_ConstructionError();
  }
  else
  {
    minorRadius = MinorRadius;
  }
}

//=================================================================================================

gp_Elips Geom_Ellipse::Elips() const
{

  return gp_Elips(pos, majorRadius, minorRadius);
}

//=================================================================================================

Standard_Real Geom_Ellipse::ReversedParameter(const Standard_Real U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

Ax1 Geom_Ellipse::Directrix1() const
{

  gp_Elips Ev(pos, majorRadius, minorRadius);
  return Ev.Directrix1();
}

//=================================================================================================

Ax1 Geom_Ellipse::Directrix2() const
{

  gp_Elips Ev(pos, majorRadius, minorRadius);
  return Ev.Directrix2();
}

//=================================================================================================

void Geom_Ellipse::D0(const Standard_Real U, gp_Pnt& P) const
{

  P = ElCLib::EllipseValue(U, pos, majorRadius, minorRadius);
}

//=================================================================================================

void Geom_Ellipse::D1(const Standard_Real U, Pnt& P, Vec& V1) const
{

  ElCLib::EllipseD1(U, pos, majorRadius, minorRadius, P, V1);
}

//=================================================================================================

void Geom_Ellipse::D2(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const
{

  ElCLib::EllipseD2(U, pos, majorRadius, minorRadius, P, V1, V2);
}

//=================================================================================================

void Geom_Ellipse::D3(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2, Vec& V3) const
{

  ElCLib::EllipseD3(U, pos, majorRadius, minorRadius, P, V1, V2, V3);
}

//=================================================================================================

Vec Geom_Ellipse::DN(const Standard_Real U, const Standard_Integer N) const
{

  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::EllipseDN(U, pos, majorRadius, minorRadius, N);
}

//=================================================================================================

Standard_Real Geom_Ellipse::Eccentricity() const
{

  if (majorRadius == 0.0)
  {
    return 0.0;
  }
  else
  {
    return (Sqrt(majorRadius * majorRadius - minorRadius * minorRadius)) / majorRadius;
  }
}

//=================================================================================================

Standard_Real Geom_Ellipse::Focal() const
{

  return 2.0 * Sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
}

//=================================================================================================

Pnt Geom_Ellipse::Focus1() const
{

  Standard_Real C = Sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  Standard_Real Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord(Xp, Yp, Zp);
  pos.XDirection().Coord(Xd, Yd, Zd);
  return Pnt(Xp + C * Xd, Yp + C * Yd, Zp + C * Zd);
}

//=================================================================================================

Pnt Geom_Ellipse::Focus2() const
{

  Standard_Real C = Sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  Standard_Real Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord(Xp, Yp, Zp);
  pos.XDirection().Coord(Xd, Yd, Zd);
  return Pnt(Xp - C * Xd, Yp - C * Yd, Zp - C * Zd);
}

//=================================================================================================

Standard_Real Geom_Ellipse::Parameter() const
{

  if (majorRadius == 0.0)
    return 0.0;
  else
    return (minorRadius * minorRadius) / majorRadius;
}

//=================================================================================================

void Geom_Ellipse::Transform(const Trsf& T)
{

  majorRadius = majorRadius * Abs(T.ScaleFactor());
  minorRadius = minorRadius * Abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_Ellipse::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, majorRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, minorRadius)
}
