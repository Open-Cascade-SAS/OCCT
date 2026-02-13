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
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_CylindricalSurface, Geom_ElementarySurface)

typedef Geom_CylindricalSurface CylindricalSurface;

typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Ax3  Ax3;
typedef gp_Circ Circ;
typedef gp_Dir  Dir;
typedef gp_Lin  Lin;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_CylindricalSurface::Copy() const
{

  occ::handle<Geom_CylindricalSurface> Cs;
  Cs = new CylindricalSurface(pos, radius);
  return Cs;
}

//=================================================================================================

Geom_CylindricalSurface::Geom_CylindricalSurface(const gp_Cylinder& C)
    : radius(C.Radius())
{

  pos = C.Position();
}

//=================================================================================================

Geom_CylindricalSurface::Geom_CylindricalSurface(const Ax3& A3, const double R)
    : radius(R)
{

  if (R < 0.0)
    throw Standard_ConstructionError();
  pos = A3;
}

//=================================================================================================

double Geom_CylindricalSurface::UReversedParameter(const double U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

double Geom_CylindricalSurface::VReversedParameter(const double V) const
{
  return (-V);
}

//=================================================================================================

double Geom_CylindricalSurface::Radius() const
{
  return radius;
}

//=================================================================================================

bool Geom_CylindricalSurface::IsUClosed() const
{
  return true;
}

//=================================================================================================

bool Geom_CylindricalSurface::IsVClosed() const
{
  return false;
}

//=================================================================================================

bool Geom_CylindricalSurface::IsUPeriodic() const
{
  return true;
}

//=================================================================================================

bool Geom_CylindricalSurface::IsVPeriodic() const
{
  return false;
}

//=================================================================================================

void Geom_CylindricalSurface::SetCylinder(const gp_Cylinder& C)
{

  radius = C.Radius();
  pos    = C.Position();
}

//=================================================================================================

void Geom_CylindricalSurface::SetRadius(const double R)
{

  if (R < 0.0)
  {
    throw Standard_ConstructionError();
  }
  radius = R;
}

//=================================================================================================

void Geom_CylindricalSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
}

//=================================================================================================

void Geom_CylindricalSurface::Coefficients(double& A1,
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
  // Dans le repere local du cylindre :
  // X**2 + Y**2 - radius = 0.0

  Trsf T;
  T.SetTransformation(pos);
  double T11 = T.Value(1, 1);
  double T12 = T.Value(1, 2);
  double T13 = T.Value(1, 3);
  double T14 = T.Value(1, 4);
  double T21 = T.Value(2, 1);
  double T22 = T.Value(2, 2);
  double T23 = T.Value(2, 3);
  double T24 = T.Value(2, 4);
  A1         = T11 * T11 + T21 * T21;
  A2         = T12 * T12 + T22 * T22;
  A3         = T13 * T13 + T23 * T23;
  B1         = T11 * T12 + T21 * T22;
  B2         = T11 * T13 + T21 * T23;
  B3         = T12 * T13 + T22 * T23;
  C1         = T11 * T14 + T21 * T24;
  C2         = T12 * T14 + T22 * T24;
  C3         = T13 * T14 + T23 * T24;
  D          = T14 * T14 + T24 * T24 - radius * radius;
}

//=================================================================================================

gp_Cylinder Geom_CylindricalSurface::Cylinder() const
{

  return gp_Cylinder(pos, radius);
}

//=================================================================================================

std::optional<gp_Pnt> Geom_CylindricalSurface::EvalD0(const double U, const double V) const
{
  gp_Pnt aP;
  ElSLib::CylinderD0(U, V, pos, radius, aP);
  return aP;
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> Geom_CylindricalSurface::EvalD1(const double U,
                                                                   const double V) const
{
  std::optional<Geom_Surface::ResD1> aResult{std::in_place};
  ElSLib::CylinderD1(U, V, pos, radius, aResult->Point, aResult->D1U, aResult->D1V);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> Geom_CylindricalSurface::EvalD2(const double U,
                                                                   const double V) const
{
  std::optional<Geom_Surface::ResD2> aResult{std::in_place};
  ElSLib::CylinderD2(U,
                     V,
                     pos,
                     radius,
                     aResult->Point,
                     aResult->D1U,
                     aResult->D1V,
                     aResult->D2U,
                     aResult->D2V,
                     aResult->D2UV);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD3> Geom_CylindricalSurface::EvalD3(const double U,
                                                                   const double V) const
{
  std::optional<Geom_Surface::ResD3> aResult{std::in_place};
  ElSLib::CylinderD3(U,
                     V,
                     pos,
                     radius,
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

std::optional<gp_Vec> Geom_CylindricalSurface::EvalDN(const double U,
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
    return ElSLib::CylinderDN(U, V, pos, radius, Nu, Nv);
  }
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_CylindricalSurface::UIso(const double U) const
{
  occ::handle<Geom_Line> GL = new Geom_Line(ElSLib::CylinderUIso(pos, radius, U));
  return GL;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_CylindricalSurface::VIso(const double V) const
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(ElSLib::CylinderVIso(pos, radius, V));
  return GC;
}

//=================================================================================================

void Geom_CylindricalSurface::Transform(const Trsf& T)
{

  radius = radius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_CylindricalSurface::TransformParameters(double&, double& V, const gp_Trsf& T) const
{
  if (!Precision::IsInfinite(V))
    V *= std::abs(T.ScaleFactor());
}

//=================================================================================================

gp_GTrsf2d Geom_CylindricalSurface::ParametricTransformation(const gp_Trsf& T) const
{
  gp_GTrsf2d T2;
  gp_Ax2d    Axis(gp::Origin2d(), gp::DX2d());
  T2.SetAffinity(Axis, std::abs(T.ScaleFactor()));
  return T2;
}

//=================================================================================================

void Geom_CylindricalSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, radius)
}
