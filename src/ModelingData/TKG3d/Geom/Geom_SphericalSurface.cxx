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
#include <Geom_Geometry.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_SphericalSurface, Geom_ElementarySurface)

typedef Geom_Circle           Circle;
typedef Geom_SphericalSurface SphericalSurface;
typedef gp_Ax2                Ax2;
typedef gp_Ax3                Ax3;
typedef gp_Circ               Circ;
typedef gp_Dir                Dir;
typedef gp_Pnt                Pnt;
typedef gp_Trsf               Trsf;
typedef gp_XYZ                XYZ;
typedef gp_Vec                Vec;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_SphericalSurface::Copy() const
{

  occ::handle<Geom_SphericalSurface> Cs;
  Cs = new SphericalSurface(pos, radius);
  return Cs;
}

//=================================================================================================

Geom_SphericalSurface::Geom_SphericalSurface(const Ax3& A, const double R)
    : radius(R)
{

  if (R < 0.0)
    throw Standard_ConstructionError();
  pos = A;
}

//=================================================================================================

Geom_SphericalSurface::Geom_SphericalSurface(const gp_Sphere& S)
    : radius(S.Radius())
{

  pos = S.Position();
}

//=================================================================================================

double Geom_SphericalSurface::UReversedParameter(const double U) const
{
  return (2. * M_PI - U);
}

//=================================================================================================

double Geom_SphericalSurface::VReversedParameter(const double V) const
{
  return (-V);
}

//=================================================================================================

double Geom_SphericalSurface::Area() const
{
  return 4.0 * M_PI * radius * radius;
}

//=================================================================================================

double Geom_SphericalSurface::Radius() const
{
  return radius;
}

//=================================================================================================

bool Geom_SphericalSurface::IsUClosed() const
{
  return true;
}

//=================================================================================================

bool Geom_SphericalSurface::IsVClosed() const
{
  return false;
}

//=================================================================================================

bool Geom_SphericalSurface::IsUPeriodic() const
{
  return true;
}

//=================================================================================================

bool Geom_SphericalSurface::IsVPeriodic() const
{
  return false;
}

//=================================================================================================

void Geom_SphericalSurface::SetRadius(const double R)
{

  if (R < 0.0)
  {
    throw Standard_ConstructionError();
  }
  radius = R;
}

//=================================================================================================

void Geom_SphericalSurface::SetSphere(const gp_Sphere& S)
{

  radius = S.Radius();
  pos    = S.Position();
}

//=================================================================================================

void Geom_SphericalSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = 0.0;
  U2 = M_PI * 2.0;
  V1 = -M_PI / 2.0;
  V2 = M_PI / 2.0;
}

//=================================================================================================

void Geom_SphericalSurface::Coefficients(double& A1,
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

  // Dans le repere local de la sphere :
  // X*X + Y*Y + Z*Z - radius * radius = 0

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
  double T31 = T.Value(3, 1);
  double T32 = T.Value(3, 2);
  double T33 = T.Value(3, 3);
  double T34 = T.Value(3, 4);
  A1         = T11 * T11 + T21 * T21 + T31 * T31;
  A2         = T12 * T12 + T22 * T22 + T32 * T32;
  A3         = T13 * T13 + T23 * T23 + T33 * T33;
  B1         = T11 * T12 + T21 * T22 + T31 * T32;
  B2         = T11 * T13 + T21 * T23 + T31 * T33;
  B3         = T12 * T13 + T22 * T23 + T32 * T33;
  C1         = T11 * T14 + T21 * T24 + T31 * T34;
  C2         = T12 * T14 + T22 * T24 + T32 * T34;
  C3         = T13 * T14 + T23 * T24 + T33 * T34;
  D          = T14 * T14 + T24 * T24 + T34 * T34 - radius * radius;
}

//=================================================================================================

std::optional<gp_Pnt> Geom_SphericalSurface::EvalD0(const double U, const double V) const
{
  gp_Pnt aP;
  ElSLib::SphereD0(U, V, pos, radius, aP);
  return aP;
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> Geom_SphericalSurface::EvalD1(const double U,
                                                                 const double V) const
{
  std::optional<Geom_Surface::ResD1> aResult{std::in_place};
  ElSLib::SphereD1(U, V, pos, radius, aResult->Point, aResult->D1U, aResult->D1V);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> Geom_SphericalSurface::EvalD2(const double U,
                                                                 const double V) const
{
  std::optional<Geom_Surface::ResD2> aResult{std::in_place};
  ElSLib::SphereD2(U,
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

std::optional<Geom_Surface::ResD3> Geom_SphericalSurface::EvalD3(const double U,
                                                                 const double V) const
{
  std::optional<Geom_Surface::ResD3> aResult{std::in_place};
  ElSLib::SphereD3(U,
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

std::optional<gp_Vec> Geom_SphericalSurface::EvalDN(const double U,
                                                    const double V,
                                                    const int    Nu,
                                                    const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    return std::nullopt;
  return ElSLib::SphereDN(U, V, pos, radius, Nu, Nv);
}

//=================================================================================================

gp_Sphere Geom_SphericalSurface::Sphere() const
{

  return gp_Sphere(pos, radius);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_SphericalSurface::UIso(const double U) const
{
  occ::handle<Geom_Circle>       GC  = new Geom_Circle(ElSLib::SphereUIso(pos, radius, U));
  occ::handle<Geom_TrimmedCurve> iso = new Geom_TrimmedCurve(GC, -M_PI / 2., M_PI / 2);
  return iso;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_SphericalSurface::VIso(const double V) const
{
  occ::handle<Geom_Circle> GC = new Geom_Circle(ElSLib::SphereVIso(pos, radius, V));
  return GC;
}

//=================================================================================================

double Geom_SphericalSurface::Volume() const
{

  return (4.0 * M_PI * radius * radius * radius) / 3.0;
}

//=================================================================================================

void Geom_SphericalSurface::Transform(const Trsf& T)
{

  radius = radius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_SphericalSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, radius)
}
