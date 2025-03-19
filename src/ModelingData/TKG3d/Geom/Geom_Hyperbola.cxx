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

Handle(Geom_Geometry) Geom_Hyperbola::Copy() const
{

  Handle(Geom_Hyperbola) H;
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

Geom_Hyperbola::Geom_Hyperbola(const Ax2&          A,
                               const Standard_Real MajorRadius,
                               const Standard_Real MinorRadius)
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

Standard_Boolean Geom_Hyperbola::IsClosed() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Boolean Geom_Hyperbola::IsPeriodic() const
{
  return Standard_False;
}

//=================================================================================================

Standard_Real Geom_Hyperbola::FirstParameter() const
{
  return -Precision::Infinite();
}

//=================================================================================================

Standard_Real Geom_Hyperbola::LastParameter() const
{
  return Precision::Infinite();
}

//=================================================================================================

Standard_Real Geom_Hyperbola::MajorRadius() const
{
  return majorRadius;
}

//=================================================================================================

Standard_Real Geom_Hyperbola::MinorRadius() const
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

void Geom_Hyperbola::SetMajorRadius(const Standard_Real MajorRadius)
{

  if (MajorRadius < 0.0)
    throw Standard_ConstructionError();
  else
    majorRadius = MajorRadius;
}

//=================================================================================================

void Geom_Hyperbola::SetMinorRadius(const Standard_Real MinorRadius)
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

Standard_Real Geom_Hyperbola::ReversedParameter(const Standard_Real U) const
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

void Geom_Hyperbola::D0(const Standard_Real U, Pnt& P) const
{

  P = ElCLib::HyperbolaValue(U, pos, majorRadius, minorRadius);
}

//=================================================================================================

void Geom_Hyperbola::D1(const Standard_Real U, Pnt& P, Vec& V1) const
{

  ElCLib::HyperbolaD1(U, pos, majorRadius, minorRadius, P, V1);
}

//=================================================================================================

void Geom_Hyperbola::D2(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const
{

  ElCLib::HyperbolaD2(U, pos, majorRadius, minorRadius, P, V1, V2);
}

//=================================================================================================

void Geom_Hyperbola::D3(const Standard_Real U, Pnt& P, Vec& V1, Vec& V2, Vec& V3) const
{

  ElCLib::HyperbolaD3(U, pos, majorRadius, minorRadius, P, V1, V2, V3);
}

//=================================================================================================

Vec Geom_Hyperbola::DN(const Standard_Real U, const Standard_Integer N) const
{

  Standard_RangeError_Raise_if(N < 1, " ");
  return ElCLib::HyperbolaDN(U, pos, majorRadius, minorRadius, N);
}

//=================================================================================================

Standard_Real Geom_Hyperbola::Eccentricity() const
{

  Standard_ConstructionError_Raise_if(majorRadius == 0.0, " ") return (
    Sqrt(majorRadius * majorRadius + minorRadius * minorRadius))
    / majorRadius;
}

//=================================================================================================

Standard_Real Geom_Hyperbola::Focal() const
{

  return 2.0 * Sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
}

//=================================================================================================

Pnt Geom_Hyperbola::Focus1() const
{

  Standard_Real C = Sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  Standard_Real Xp, Yp, Zp, Xd, Yd, Zd;
  pos.Location().Coord(Xp, Yp, Zp);
  pos.XDirection().Coord(Xd, Yd, Zd);
  return Pnt(Xp + C * Xd, Yp + C * Yd, Zp + C * Zd);
}

//=================================================================================================

Pnt Geom_Hyperbola::Focus2() const
{

  Standard_Real C = Sqrt(majorRadius * majorRadius + minorRadius * minorRadius);
  Standard_Real Xp, Yp, Zp, Xd, Yd, Zd;
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

Standard_Real Geom_Hyperbola::Parameter() const
{

  Standard_ConstructionError_Raise_if(majorRadius == 0.0, " ");
  return (minorRadius * minorRadius) / majorRadius;
}

//=================================================================================================

void Geom_Hyperbola::Transform(const Trsf& T)
{

  majorRadius = majorRadius * Abs(T.ScaleFactor());
  minorRadius = minorRadius * Abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_Hyperbola::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Conic)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, majorRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, minorRadius)
}
