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
#include <Geom_Hyperbola.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Hyperbola, Geom_Conic)

typedef Geom_Hyperbola Hyperbola;
typedef gp_Ax1         Ax1;
typedef gp_Ax2         Ax2;
typedef gp_Pnt         Pnt;
typedef gp_Vec         Vec;
typedef gp_Trsf        Trsf;
typedef gp_XYZ         XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_Hyperbola::Copy() const
{

  occ::handle<Geom_Hyperbola> H;
  H = new Hyperbola(pos, majorRadius, minorRadius);
  return H;
}

//=================================================================================================

Geom_Hyperbola::Geom_Hyperbola(const gp_Hypr& H)
    : majorRadius(H.MajorRadius()),
      minorRadius(H.MinorRadius())
{

  pos = H.Position();
}

//=================================================================================================

Geom_Hyperbola::Geom_Hyperbola(const Ax2& A, const double MajorRadius, const double MinorRadius)
    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{

  if (MajorRadius < 0.0 || MinorRadius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  pos = A;
}

//=================================================================================================

bool Geom_Hyperbola::IsClosed() const
{
  return false;
}

//=================================================================================================

bool Geom_Hyperbola::IsPeriodic() const
{
  return false;
}

//=================================================================================================

double Geom_Hyperbola::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

double Geom_Hyperbola::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

double Geom_Hyperbola::MajorRadius() const
{
  return majorRadius;
}

//=================================================================================================

double Geom_Hyperbola::MinorRadius() const
{
  return minorRadius;
}

//=================================================================================================

void Geom_Hyperbola::SetHypr(const gp_Hypr& H)
{

  majorRadius = H.MajorRadius();
  minorRadius = H.MinorRadius();
  pos         = H.Position();
}

//=================================================================================================

void Geom_Hyperbola::SetMajorRadius(const double MajorRadius)
{

  if (MajorRadius < 0.0)
    throw Standard_ConstructionError();
  else
    majorRadius = MajorRadius;
}

//=================================================================================================

void Geom_Hyperbola::SetMinorRadius(const double MinorRadius)
{

  if (MinorRadius < 0.0)
    throw Standard_ConstructionError();
  else
    minorRadius = MinorRadius;
}

//=================================================================================================

gp_Hypr Geom_Hyperbola::Hypr() const
{
  return gp_Hypr(pos, majorRadius, minorRadius);
}

//=================================================================================================

double Geom_Hyperbola::ReversedParameter(const double U) const
{
  return (-U);
}

//=================================================================================================

Ax1 Geom_Hyperbola::Asymptote1() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.Asymptote1();
}

//=================================================================================================

Ax1 Geom_Hyperbola::Asymptote2() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.Asymptote2();
}

//=================================================================================================

gp_Hypr Geom_Hyperbola::ConjugateBranch1() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.ConjugateBranch1();
}

//=================================================================================================

gp_Hypr Geom_Hyperbola::ConjugateBranch2() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.ConjugateBranch2();
}

//=================================================================================================

Ax1 Geom_Hyperbola::Directrix1() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.Directrix1();
}

//=================================================================================================

Ax1 Geom_Hyperbola::Directrix2() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.Directrix2();
}

//=================================================================================================

gp_Pnt Geom_Hyperbola::EvalD0(const double U) const
{
  return ElCLib::HyperbolaValue(U, pos, majorRadius, minorRadius);
}

//=================================================================================================

Geom_Curve::ResD1 Geom_Hyperbola::EvalD1(const double U) const
{
  Geom_Curve::ResD1 aResult;
  ElCLib::HyperbolaD1(U, pos, majorRadius, minorRadius, aResult.Point, aResult.D1);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD2 Geom_Hyperbola::EvalD2(const double U) const
{
  Geom_Curve::ResD2 aResult;
  ElCLib::HyperbolaD2(U, pos, majorRadius, minorRadius, aResult.Point, aResult.D1, aResult.D2);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD3 Geom_Hyperbola::EvalD3(const double U) const
{
  Geom_Curve::ResD3 aResult;
  ElCLib::HyperbolaD3(U,
                      pos,
                      majorRadius,
                      minorRadius,
                      aResult.Point,
                      aResult.D1,
                      aResult.D2,
                      aResult.D3);
  return aResult;
}

//=================================================================================================

gp_Vec Geom_Hyperbola::EvalDN(const double U, const int N) const
{
  if (N < 1)
    throw Geom_UndefinedDerivative();
  return ElCLib::HyperbolaDN(U, pos, majorRadius, minorRadius, N);
}

//=================================================================================================

double Geom_Hyperbola::Eccentricity() const
{

  Standard_ConstructionError_Raise_if(majorRadius == 0.0, " ") return (
    std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius))
    / majorRadius;
}

//=================================================================================================

double Geom_Hyperbola::Focal() const
{

  return 2.0 * std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
}

//=================================================================================================

Pnt Geom_Hyperbola::Focus1() const
{

  double C = std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  double Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord(Xp, Yp, Zp);
  pos.XDirection().Coord(Xd, Yd, Zd);
  return Pnt(Xp + C * Xd, Yp + C * Yd, Zp + C * Zd);
}

//=================================================================================================

Pnt Geom_Hyperbola::Focus2() const
{

  double C = std::sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  double Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord(Xp, Yp, Zp);
  pos.XDirection().Coord(Xd, Yd, Zd);
  return Pnt(Xp - C * Xd, Yp - C * Yd, Zp - C * Zd);
}

//=================================================================================================

gp_Hypr Geom_Hyperbola::OtherBranch() const
{

  gp_Hypr Hv(pos, majorRadius, minorRadius);
  return Hv.OtherBranch();
}

//=================================================================================================

double Geom_Hyperbola::Parameter() const
{

  Standard_ConstructionError_Raise_if(majorRadius == 0.0, " ");
  return (minorRadius * minorRadius) / majorRadius;
}

//=================================================================================================

void Geom_Hyperbola::Transform(const Trsf& T)
{

  majorRadius = majorRadius * std::abs(T.ScaleFactor());
  minorRadius = minorRadius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_Hyperbola::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, majorRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, minorRadius)
}
