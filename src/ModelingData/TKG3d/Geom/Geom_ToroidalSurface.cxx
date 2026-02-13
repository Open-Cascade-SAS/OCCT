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
#include <Geom_ToroidalSurface.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_ToroidalSurface, Geom_ElementarySurface)

typedef Geom_ToroidalSurface       ToroidalSurface;
typedef NCollection_Array1<double> Array1OfReal;
typedef gp_Ax1                     Ax1;
typedef gp_Ax2                     Ax2;
typedef gp_Ax3                     Ax3;
typedef gp_Circ                    Circ;
typedef gp_Dir                     Dir;
typedef gp_Pnt                     Pnt;
typedef gp_Trsf                    Trsf;
typedef gp_Vec                     Vec;
typedef gp_XYZ                     XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_ToroidalSurface::Copy() const
{

  occ::handle<Geom_ToroidalSurface> Cs;
  Cs = new ToroidalSurface(pos, majorRadius, minorRadius);
  return Cs;
}

//=================================================================================================

Geom_ToroidalSurface::Geom_ToroidalSurface(const Ax3&   A3,
                                           const double MajorRadius,
                                           const double MinorRadius)

    : majorRadius(MajorRadius),
      minorRadius(MinorRadius)
{

  if (MinorRadius < 0.0 || MajorRadius < 0.0)
  {
    throw Standard_ConstructionError();
  }
  else
  {
    pos = A3;
  }
}

//=================================================================================================

Geom_ToroidalSurface::Geom_ToroidalSurface(const gp_Torus& T)
    : majorRadius(T.MajorRadius()),
      minorRadius(T.MinorRadius())
{

  pos = T.Position();
}

//=================================================================================================

double Geom_ToroidalSurface::MajorRadius() const
{

  return majorRadius;
}

//=================================================================================================

double Geom_ToroidalSurface::MinorRadius() const
{

  return minorRadius;
}

//=================================================================================================

double Geom_ToroidalSurface::UReversedParameter(const double U) const
{

  return (2. * M_PI - U);
}

//=================================================================================================

double Geom_ToroidalSurface::VReversedParameter(const double V) const
{

  return (2. * M_PI - V);
}

//=================================================================================================

bool Geom_ToroidalSurface::IsUClosed() const
{

  return true;
}

//=================================================================================================

bool Geom_ToroidalSurface::IsVClosed() const
{

  return true;
}

//=================================================================================================

bool Geom_ToroidalSurface::IsUPeriodic() const
{

  return true;
}

//=================================================================================================

bool Geom_ToroidalSurface::IsVPeriodic() const
{

  return true;
}

//=================================================================================================

void Geom_ToroidalSurface::SetMajorRadius(const double MajorRadius)
{

  if (MajorRadius - minorRadius <= gp::Resolution())
    throw Standard_ConstructionError();
  else
    majorRadius = MajorRadius;
}

//=================================================================================================

void Geom_ToroidalSurface::SetMinorRadius(const double MinorRadius)
{

  if (MinorRadius < 0.0 || majorRadius - MinorRadius <= gp::Resolution())
    throw Standard_ConstructionError();
  else
    minorRadius = MinorRadius;
}

//=================================================================================================

void Geom_ToroidalSurface::SetTorus(const gp_Torus& T)
{

  minorRadius = T.MinorRadius();
  majorRadius = T.MajorRadius();
  pos         = T.Position();
}

//=================================================================================================

double Geom_ToroidalSurface::Area() const
{
  return 4.0 * M_PI * M_PI * minorRadius * majorRadius;
}

//=================================================================================================

void Geom_ToroidalSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = 0.0;
  V1 = 0.0;
  U2 = 2 * M_PI;
  V2 = 2 * M_PI;
}

//=================================================================================================

void Geom_ToroidalSurface::Coefficients(Array1OfReal& Coef) const
{

  gp_Torus Tor(pos, majorRadius, minorRadius);
  Tor.Coefficients(Coef);
}

//=================================================================================================

std::optional<gp_Pnt> Geom_ToroidalSurface::EvalD0(const double U, const double V) const
{
  gp_Pnt aP;
  ElSLib::TorusD0(U, V, pos, majorRadius, minorRadius, aP);
  return aP;
}

//=================================================================================================

std::optional<Geom_SurfD1> Geom_ToroidalSurface::EvalD1(const double U, const double V) const
{
  std::optional<Geom_SurfD1> aResult{std::in_place};
  ElSLib::TorusD1(U, V, pos, majorRadius, minorRadius, aResult->Point, aResult->D1U, aResult->D1V);
  return aResult;
}

//=================================================================================================

std::optional<Geom_SurfD2> Geom_ToroidalSurface::EvalD2(const double U, const double V) const
{
  std::optional<Geom_SurfD2> aResult{std::in_place};
  ElSLib::TorusD2(U, V, pos, majorRadius, minorRadius, aResult->Point, aResult->D1U, aResult->D1V, aResult->D2U, aResult->D2V, aResult->D2UV);
  return aResult;
}

//=================================================================================================

std::optional<Geom_SurfD3> Geom_ToroidalSurface::EvalD3(const double U, const double V) const
{
  std::optional<Geom_SurfD3> aResult{std::in_place};
  ElSLib::TorusD3(U,
                  V,
                  pos,
                  majorRadius,
                  minorRadius,
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

std::optional<gp_Vec> Geom_ToroidalSurface::EvalDN(const double U, const double V, const int Nu, const int Nv) const
{
  Standard_RangeError_Raise_if(Nu + Nv < 1 || Nu < 0 || Nv < 0, "  ");
  return ElSLib::TorusDN(U, V, pos, majorRadius, minorRadius, Nu, Nv);
}

//=================================================================================================

gp_Torus Geom_ToroidalSurface::Torus() const
{

  return gp_Torus(pos, majorRadius, minorRadius);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_ToroidalSurface::UIso(const double U) const
{
  occ::handle<Geom_Circle> GC =
    new Geom_Circle(ElSLib::TorusUIso(pos, majorRadius, minorRadius, U));
  return GC;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_ToroidalSurface::VIso(const double V) const
{
  occ::handle<Geom_Circle> GC =
    new Geom_Circle(ElSLib::TorusVIso(pos, majorRadius, minorRadius, V));
  return GC;
}

//=================================================================================================

double Geom_ToroidalSurface::Volume() const
{

  return (M_PI * minorRadius * minorRadius) * (2.0 * M_PI * majorRadius);
}

//=================================================================================================

void Geom_ToroidalSurface::Transform(const Trsf& T)
{

  majorRadius = majorRadius * std::abs(T.ScaleFactor());
  minorRadius = minorRadius * std::abs(T.ScaleFactor());
  pos.Transform(T);
}

//=================================================================================================

void Geom_ToroidalSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, majorRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, minorRadius)
}
