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
#include <Geom2d_Hyperbola.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Hyperbola, Geom2d_Conic)

typedef Geom2d_Hyperbola Hyperbola;
typedef gp_Ax2d          Ax2d;
typedef gp_Dir2d         Dir2d;
typedef gp_Pnt2d         Pnt2d;
typedef gp_Vec2d         Vec2d;
typedef gp_Trsf2d        Trsf2d;
typedef gp_XY            XY;

//=================================================================================================

occ::handle<Geom2d_Geometry> Geom2d_Hyperbola::Copy() const
{
  occ::handle<Geom2d_Hyperbola> H;
  H = new Hyperbola(pos, majorRadius, minorRadius);
  return H;
}

//=================================================================================================

Geom2d_Hyperbola::Geom2d_Hyperbola(const gp_Hypr2d& H)
{
  majorRadius = H.MajorRadius();
  minorRadius = H.MinorRadius();
  pos         = H.Axis();
}

//=================================================================================================

Geom2d_Hyperbola::Geom2d_Hyperbola(const Ax2d&  A,
                                   const double MajorRadius,
                                   const double MinorRadius,
                                   const bool   Sense)
    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0)
    throw Standard_ConstructionError();
  pos = gp_Ax22d(A, Sense);
}

//=================================================================================================

Geom2d_Hyperbola::Geom2d_Hyperbola(const gp_Ax22d& Axis,
                                   const double    MajorRadius,
                                   const double    MinorRadius)
    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0)
    throw Standard_ConstructionError();
  pos = Axis;
}

//=================================================================================================

void Geom2d_Hyperbola::SetHypr2d(const gp_Hypr2d& H)
{
  majorRadius = H.MajorRadius();
  minorRadius = H.MinorRadius();
  pos         = H.Axis();
}

//=================================================================================================

void Geom2d_Hyperbola::SetMajorRadius(const double MajorRadius)
{
  if (MajorRadius < 0.0)
    throw Standard_ConstructionError();
  else
    majorRadius = MajorRadius;
}

//=================================================================================================

void Geom2d_Hyperbola::SetMinorRadius(const double MinorRadius)
{
  if (MinorRadius < 0.0)
    throw Standard_ConstructionError();
  else
    minorRadius = MinorRadius;
}

//=================================================================================================

gp_Hypr2d Geom2d_Hyperbola::Hypr2d() const
{
  return gp_Hypr2d(pos, majorRadius, minorRadius);
}

//=================================================================================================

double Geom2d_Hyperbola::ReversedParameter(const double U) const
{
  return (-U);
}

//=================================================================================================

double Geom2d_Hyperbola::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

double Geom2d_Hyperbola::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

bool Geom2d_Hyperbola::IsClosed() const
{
  return false;
}

//=================================================================================================

bool Geom2d_Hyperbola::IsPeriodic() const
{
  return false;
}

//=================================================================================================

Ax2d Geom2d_Hyperbola::Asymptote1() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.Asymptote1();
}

//=================================================================================================

Ax2d Geom2d_Hyperbola::Asymptote2() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.Asymptote2();
}

//=================================================================================================

gp_Hypr2d Geom2d_Hyperbola::ConjugateBranch1() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.ConjugateBranch1();
}

//=================================================================================================

gp_Hypr2d Geom2d_Hyperbola::ConjugateBranch2() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.ConjugateBranch2();
}

//=================================================================================================

Ax2d Geom2d_Hyperbola::Directrix1() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.Directrix1();
}

//=================================================================================================

Ax2d Geom2d_Hyperbola::Directrix2() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.Directrix2();
}

//=================================================================================================

double Geom2d_Hyperbola::Eccentricity() const
{
  Standard_DomainError_Raise_if(majorRadius <= gp::Resolution(), " ");
  return (std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius)) / majorRadius;
}

//=================================================================================================

double Geom2d_Hyperbola::Focal() const
{
  return 2.0 * std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
}

//=================================================================================================

Pnt2d Geom2d_Hyperbola::Focus1() const
{
  double C   = std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  XY     Pxy = pos.XDirection().XY();
  Pxy.Multiply(C);
  Pxy.Add(pos.Location().XY());
  return Pnt2d(Pxy);
}

//=================================================================================================

Pnt2d Geom2d_Hyperbola::Focus2() const
{
  double C   = std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  XY     Pxy = pos.XDirection().XY();
  Pxy.Multiply(-C);
  Pxy.Add(pos.Location().XY());
  return Pnt2d(Pxy);
}

//=================================================================================================

double Geom2d_Hyperbola::MajorRadius() const
{
  return majorRadius;
}

//=================================================================================================

double Geom2d_Hyperbola::MinorRadius() const
{
  return minorRadius;
}

//=================================================================================================

gp_Hypr2d Geom2d_Hyperbola::OtherBranch() const
{
  gp_Hypr2d Hv(pos, majorRadius, minorRadius);
  return Hv.OtherBranch();
}

//=================================================================================================

double Geom2d_Hyperbola::Parameter() const
{
  Standard_DomainError_Raise_if(majorRadius <= gp::Resolution(), " ");
  return (minorRadius * minorRadius) / majorRadius;
}

//=================================================================================================

std::optional<gp_Pnt2d> Geom2d_Hyperbola::EvalD0(const double U) const
{
  return ElCLib::HyperbolaValue(U, pos, majorRadius, minorRadius);
}

//=================================================================================================

std::optional<Geom2d_CurveD1> Geom2d_Hyperbola::EvalD1(const double U) const
{
  std::optional<Geom2d_CurveD1> aResult{std::in_place};
  ElCLib::HyperbolaD1(U, pos, majorRadius, minorRadius, aResult->Point, aResult->D1);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD2> Geom2d_Hyperbola::EvalD2(const double U) const
{
  std::optional<Geom2d_CurveD2> aResult{std::in_place};
  ElCLib::HyperbolaD2(U, pos, majorRadius, minorRadius, aResult->Point, aResult->D1, aResult->D2);
  return aResult;
}

//=================================================================================================

std::optional<Geom2d_CurveD3> Geom2d_Hyperbola::EvalD3(const double U) const
{
  std::optional<Geom2d_CurveD3> aResult{std::in_place};
  ElCLib::HyperbolaD3(U,
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

std::optional<gp_Vec2d> Geom2d_Hyperbola::EvalDN(const double U, const int N) const
{
  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::HyperbolaDN(U, pos, majorRadius, minorRadius, N);
}

//=================================================================================================

void Geom2d_Hyperbola::Transform(const Trsf2d& T)
{
  majorRadius = majorRadius * std::abs(T.ScaleFactor());
  minorRadius = minorRadius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom2d_Hyperbola::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, majorRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, minorRadius)
}
