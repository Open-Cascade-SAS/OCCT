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

#include <ElSLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_ConicalSurface, Geom_ElementarySurface)

typedef Geom_ConicalSurface ConicalSurface;
typedef gp_Ax1              Ax1;
typedef gp_Ax2              Ax2;
typedef gp_Ax3              Ax3;
typedef gp_Circ             Circ;
typedef gp_Dir              Dir;
typedef gp_Lin              Lin;
typedef gp_Pnt              Pnt;
typedef gp_Trsf             Trsf;
typedef gp_Vec              Vec;
typedef gp_XYZ              XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_ConicalSurface::Copy() const
{

  occ::handle<Geom_ConicalSurface> Cs;
  Cs = new ConicalSurface(pos, semiAngle, radius);
  return Cs;
}

//=================================================================================================

Geom_ConicalSurface::Geom_ConicalSurface(const Ax3& A3, const double Ang, const double R)
    : radius(R),
      semiAngle(Ang)
{

  if (R < 0.0 || std::abs(Ang) <= gp::Resolution()
      || std::abs(Ang) >= M_PI / 2.0 - gp::Resolution())
    throw Standard_ConstructionError();

  pos = A3;
}

//=================================================================================================

Geom_ConicalSurface::Geom_ConicalSurface(const gp_Cone& C)
    : radius(C.RefRadius()),
      semiAngle(C.SemiAngle())
{
  pos = C.Position();
}

//=================================================================================================

double Geom_ConicalSurface::UReversedParameter(const double U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

double Geom_ConicalSurface::VReversedParameter(const double V) const
{
  return (-V);
}

//=================================================================================================

void Geom_ConicalSurface::VReverse()
{
  semiAngle = -semiAngle;
  pos.ZReverse();
}

//=================================================================================================

double Geom_ConicalSurface::RefRadius() const
{
  return radius;
}

//=================================================================================================

double Geom_ConicalSurface::SemiAngle() const
{
  return semiAngle;
}

//=================================================================================================

bool Geom_ConicalSurface::IsUClosed() const
{
  return true;
}

//=================================================================================================

bool Geom_ConicalSurface::IsVClosed() const
{
  return false;
}

//=================================================================================================

bool Geom_ConicalSurface::IsUPeriodic() const
{
  return true;
}

//=================================================================================================

bool Geom_ConicalSurface::IsVPeriodic() const
{
  return false;
}

//=================================================================================================

gp_Cone Geom_ConicalSurface::Cone() const
{

  return gp_Cone(pos, semiAngle, radius);
}

//=================================================================================================

void Geom_ConicalSurface::SetCone(const gp_Cone& C)
{

  radius    = C.RefRadius();
  semiAngle = C.SemiAngle();
  pos       = C.Position();
}

//=================================================================================================

void Geom_ConicalSurface::SetRadius(const double R)
{

  if (R < 0.0)
    throw Standard_ConstructionError();
  radius = R;
}

//=================================================================================================

void Geom_ConicalSurface::SetSemiAngle(const double Ang)
{

  if (std::abs(Ang) <= gp::Resolution() || std::abs(Ang) >= M_PI / 2.0 - gp::Resolution())
  {
    throw Standard_ConstructionError();
  }
  semiAngle = Ang;
}

//=================================================================================================

Pnt Geom_ConicalSurface::Apex() const
{

  XYZ Coord = Position().Direction().XYZ();
  Coord.Multiply(-radius / std::tan(semiAngle));
  Coord.Add(Position().Location().XYZ());
  return Pnt(Coord);
}

//=================================================================================================

void Geom_ConicalSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
}

//=================================================================================================

void Geom_ConicalSurface::Coefficients(double& A1,
                                       double& A2,
                                       double& A3,
                                       double& B1,
                                       double& B2,
                                       double& B3,
                                       double& C1,
                                       double& C2,
                                       double& C3,
                                       double& D) const
{
  // Dans le repere du cone :
  // X**2 + Y**2 - (Myradius - Z * std::tan(semiAngle))**2 = 0.0

  Trsf T;
  T.SetTransformation(pos);
  double KAng = std::tan(semiAngle);
  double T11  = T.Value(1, 1);
  double T12  = T.Value(1, 2);
  double T13  = T.Value(1, 3);
  double T14  = T.Value(1, 4);
  double T21  = T.Value(2, 1);
  double T22  = T.Value(2, 2);
  double T23  = T.Value(2, 3);
  double T24  = T.Value(2, 4);
  double T31  = T.Value(3, 1) * KAng;
  double T32  = T.Value(3, 2) * KAng;
  double T33  = T.Value(3, 3) * KAng;
  double T34  = T.Value(3, 4) * KAng;
  A1          = T11 * T11 + T21 * T21 - T31 * T31;
  A2          = T12 * T12 + T22 * T22 - T32 * T32;
  A3          = T13 * T13 + T23 * T23 - T33 * T33;
  B1          = T11 * T12 + T21 * T22 - T31 * T32;
  B2          = T11 * T13 + T21 * T23 - T31 * T33;
  B3          = T12 * T13 + T22 * T23 - T32 * T33;
  C1          = T11 * T14 + T21 * T24 + radius * T31;
  C2          = T12 * T14 + T22 * T24 + radius * T32;
  C3          = T13 * T14 + T23 * T24 + radius * T33;
  D           = T14 * T14 + T24 * T24 - radius * radius - T34 * T34 + 2.0 * radius * T34;
}

//=================================================================================================

std::optional<gp_Pnt> Geom_ConicalSurface::EvalD0(const double U, const double V) const
{
  return ElSLib::ConeValue(U, V, pos, radius, semiAngle);
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> Geom_ConicalSurface::EvalD1(const double U, const double V) const
{
  std::optional<Geom_Surface::ResD1> aResult{std::in_place};
  ElSLib::ConeD1(U, V, pos, radius, semiAngle, aResult->Point, aResult->D1U, aResult->D1V);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> Geom_ConicalSurface::EvalD2(const double U, const double V) const
{
  std::optional<Geom_Surface::ResD2> aResult{std::in_place};
  ElSLib::ConeD2(U,
                 V,
                 pos,
                 radius,
                 semiAngle,
                 aResult->Point,
                 aResult->D1U,
                 aResult->D1V,
                 aResult->D2U,
                 aResult->D2V,
                 aResult->D2UV);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD3> Geom_ConicalSurface::EvalD3(const double U, const double V) const
{
  std::optional<Geom_Surface::ResD3> aResult{std::in_place};
  ElSLib::ConeD3(U,
                 V,
                 pos,
                 radius,
                 semiAngle,
                 aResult->Point,
                 aResult->D1U,
                 aResult->D1V,
                 aResult->D2U,
                 aResult->D2V,
                 aResult->D2UV,
                 aResult->D3U,
                 aResult->D3V,
                 aResult->D3UUV,
                 aResult->D3UVV);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec> Geom_ConicalSurface::EvalDN(const double U,
                                                  const double V,
                                                  const int    Nu,
                                                  const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    return std::nullopt;
  if (Nv > 1)
  {
    return Vec(0.0, 0.0, 0.0);
  }
  else
  {
    return ElSLib::ConeDN(U, V, pos, radius, semiAngle, Nu, Nv);
  }
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_ConicalSurface::UIso(const double U) const
{
  occ::handle<Geom_Line> GL = new Geom_Line(ElSLib::ConeUIso(pos, radius, semiAngle, U));
  return GL;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_ConicalSurface::VIso(const double V) const
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(ElSLib::ConeVIso(pos, radius, semiAngle, V));
  return GC;
}

//=================================================================================================

void Geom_ConicalSurface::Transform(const Trsf& T)
{
  radius = radius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_ConicalSurface::TransformParameters(double&, double& V, const gp_Trsf& T) const
{
  if (!Precision::IsInfinite(V))
    V *= std::abs(T.ScaleFactor());
}

//=================================================================================================

gp_GTrsf2d Geom_ConicalSurface::ParametricTransformation(const gp_Trsf& T) const
{
  gp_GTrsf2d T2;
  gp_Ax2d    Axis(gp::Origin2d(), gp::DX2d());
  T2.SetAffinity(Axis, std::abs(T.ScaleFactor()));
  return T2;
}

//=================================================================================================

void Geom_ConicalSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, radius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, semiAngle)
}
