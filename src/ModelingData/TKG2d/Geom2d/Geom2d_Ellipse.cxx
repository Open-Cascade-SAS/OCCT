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
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Geometry.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Ellipse, Geom2d_Conic)

typedef gp_Ax2d   Ax2d;
typedef gp_Dir2d  Dir2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;
typedef gp_XY     XY;

//=================================================================================================

occ::handle<Geom2d_Geometry> Geom2d_Ellipse::Copy() const
{
  occ::handle<Geom2d_Ellipse> E;
  E = new Geom2d_Ellipse(pos, majorRadius, minorRadius);
  return E;
}

//=================================================================================================

Geom2d_Ellipse::Geom2d_Ellipse(const gp_Elips2d& E)
{

  majorRadius = E.MajorRadius();
  minorRadius = E.MinorRadius();
  pos         = E.Axis();
}

//=================================================================================================

Geom2d_Ellipse::Geom2d_Ellipse(const Ax2d&  MajorAxis,
                               const double MajorRadius,
                               const double MinorRadius,
                               const bool   Sense)
    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{

  if (MajorRadius < MinorRadius || MinorRadius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = gp_Ax22d(MajorAxis, Sense);
}

//=================================================================================================

Geom2d_Ellipse::Geom2d_Ellipse(const gp_Ax22d& Axis,
                               const double    MajorRadius,
                               const double    MinorRadius)
    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{
  if (MajorRadius < MinorRadius || MinorRadius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = Axis;
}

//=================================================================================================

void Geom2d_Ellipse::SetElips2d(const gp_Elips2d& E)
{
  majorRadius = E.MajorRadius();
  minorRadius = E.MinorRadius();
  pos         = E.Axis();
}

//=================================================================================================

void Geom2d_Ellipse::SetMajorRadius(const double MajorRadius)
{
  if (MajorRadius < minorRadius)
    throw Standard_ConstructionError();
  else
    majorRadius = MajorRadius;
}

//=================================================================================================

void Geom2d_Ellipse::SetMinorRadius(const double MinorRadius)
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

gp_Elips2d Geom2d_Ellipse::Elips2d() const
{
  return gp_Elips2d(pos, majorRadius, minorRadius);
}

//=================================================================================================

double Geom2d_Ellipse::ReversedParameter(const double U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

Ax2d Geom2d_Ellipse::Directrix1() const
{
  gp_Elips2d Ev(pos, majorRadius, minorRadius);
  return Ev.Directrix1();
}

//=================================================================================================

Ax2d Geom2d_Ellipse::Directrix2() const
{
  gp_Elips2d Ev(pos, majorRadius, minorRadius);
  return Ev.Directrix2();
}

//=================================================================================================

double Geom2d_Ellipse::Eccentricity() const
{
  if (majorRadius == 0.0)
  {
    return 0.0;
  }
  else
  {
    return (std::sqrt(majorRadius * majorRadius - minorRadius * minorRadius)) / majorRadius;
  }
}

//=================================================================================================

double Geom2d_Ellipse::Focal() const
{
  return 2.0 * std::sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
}

//=================================================================================================

Pnt2d Geom2d_Ellipse::Focus1() const
{
  double C = std::sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  return Pnt2d(pos.Location().X() + C * pos.XDirection().X(),
               pos.Location().Y() + C * pos.XDirection().Y());
}

//=================================================================================================

Pnt2d Geom2d_Ellipse::Focus2() const
{
  double C = std::sqrt(majorRadius * majorRadius - minorRadius * minorRadius);
  return Pnt2d(pos.Location().X() - C * pos.XDirection().X(),
               pos.Location().Y() - C * pos.XDirection().Y());
}

//=================================================================================================

double Geom2d_Ellipse::MajorRadius() const
{
  return majorRadius;
}

//=================================================================================================

double Geom2d_Ellipse::MinorRadius() const
{
  return minorRadius;
}

//=================================================================================================

double Geom2d_Ellipse::Parameter() const
{
  if (majorRadius == 0.0)
    return 0.0;
  else
    return (minorRadius * minorRadius) / majorRadius;
}

//=================================================================================================

double Geom2d_Ellipse::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

double Geom2d_Ellipse::LastParameter() const
{
  return 2.0 * M_PI;
}

//=================================================================================================

bool Geom2d_Ellipse::IsClosed() const
{
  return true;
}

//=================================================================================================

bool Geom2d_Ellipse::IsPeriodic() const
{
  return true;
}

//=================================================================================================

std::optional<gp_Pnt2d> Geom2d_Ellipse::EvalD0(const double U) const
{
  return ElCLib::EllipseValue(U, pos, majorRadius, minorRadius);
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD1> Geom2d_Ellipse::EvalD1(const double U) const
{
  std::optional<Geom2d_Curve::ResD1> aResult{std::in_place};
  ElCLib::EllipseD1(U, pos, majorRadius, minorRadius, aResult->Point, aResult->D1);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD2> Geom2d_Ellipse::EvalD2(const double U) const
{
  std::optional<Geom2d_Curve::ResD2> aResult{std::in_place};
  ElCLib::EllipseD2(U, pos, majorRadius, minorRadius, aResult->Point, aResult->D1, aResult->D2);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD3> Geom2d_Ellipse::EvalD3(const double U) const
{
  std::optional<Geom2d_Curve::ResD3> aResult{std::in_place};
  ElCLib::EllipseD3(U,
                    pos,
                    majorRadius,
                    minorRadius,
                    aResult->Point,
                    aResult->D1,
                    aResult->D2,
                    aResult->D3);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec2d> Geom2d_Ellipse::EvalDN(const double U, const int N) const
{
  if (N < 1)
    return std::nullopt;
  return ElCLib::EllipseDN(U, pos, majorRadius, minorRadius, N);
}

//=================================================================================================

void Geom2d_Ellipse::Transform(const Trsf2d& T)
{
  majorRadius = majorRadius * std::abs(T.ScaleFactor());
  minorRadius = minorRadius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom2d_Ellipse::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, majorRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, minorRadius)
}
