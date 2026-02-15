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
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Plane, Geom_ElementarySurface)

typedef Geom_Plane Plane;
typedef gp_Ax1     Ax1;
typedef gp_Ax2     Ax2;
typedef gp_Ax3     Ax3;
typedef gp_Dir     Dir;
typedef gp_Lin     Lin;
typedef gp_Pln     Pln;
typedef gp_Pnt     Pnt;
typedef gp_Trsf    Trsf;
typedef gp_Vec     Vec;
typedef gp_XYZ     XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_Plane::Copy() const
{

  occ::handle<Geom_Plane> Pl = new Plane(pos);
  return Pl;
}

//=================================================================================================

Geom_Plane::Geom_Plane(const gp_Ax3& A3)
{

  pos = A3;
}

//=================================================================================================

Geom_Plane::Geom_Plane(const gp_Pln& Pl)
{

  pos = Pl.Position();
}

//=================================================================================================

Geom_Plane::Geom_Plane(const Pnt& P, const Dir& V)
{

  gp_Pln Pl(P, V);
  pos = Pl.Position();
}

//=================================================================================================

Geom_Plane::Geom_Plane(const double A, const double B, const double C, const double D)
{

  gp_Pln Pl(A, B, C, D);
  pos = Pl.Position();
}

//=================================================================================================

void Geom_Plane::SetPln(const gp_Pln& Pl)
{
  pos = Pl.Position();
}

//=================================================================================================

gp_Pln Geom_Plane::Pln() const
{

  return gp_Pln(Position());
}

//=================================================================================================

void Geom_Plane::UReverse()
{
  pos.XReverse();
}

//=================================================================================================

double Geom_Plane::UReversedParameter(const double U) const
{

  return (-U);
}

//=================================================================================================

void Geom_Plane::VReverse()
{

  pos.YReverse();
}

//=================================================================================================

double Geom_Plane::VReversedParameter(const double V) const
{

  return (-V);
}

//=================================================================================================

void Geom_Plane::Transform(const Trsf& T)
{
  pos.Transform(T);
}

//=================================================================================================

bool Geom_Plane::IsUClosed() const
{

  return false;
}

//=================================================================================================

bool Geom_Plane::IsVClosed() const
{

  return false;
}

//=================================================================================================

bool Geom_Plane::IsUPeriodic() const
{

  return false;
}

//=================================================================================================

bool Geom_Plane::IsVPeriodic() const
{

  return false;
}

//=================================================================================================

void Geom_Plane::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = -Precision::Infinite();
  U2 = Precision::Infinite();
  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
}

//=================================================================================================

void Geom_Plane::Coefficients(double& A, double& B, double& C, double& D) const
{

  gp_Pln Pl(Position());
  Pl.Coefficients(A, B, C, D);
}

//=================================================================================================

gp_Pnt Geom_Plane::EvalD0(const double U, const double V) const
{
  return ElSLib::PlaneValue(U, V, pos);
}

//=================================================================================================

Geom_Surface::ResD1 Geom_Plane::EvalD1(const double U, const double V) const
{
  Geom_Surface::ResD1 aResult;
  ElSLib::PlaneD1(U, V, pos, aResult.Point, aResult.D1U, aResult.D1V);
  return aResult;
}

//=================================================================================================

Geom_Surface::ResD2 Geom_Plane::EvalD2(const double U, const double V) const
{
  Geom_Surface::ResD2 aResult;
  ElSLib::PlaneD1(U, V, pos, aResult.Point, aResult.D1U, aResult.D1V);
  aResult.D2U.SetCoord(0.0, 0.0, 0.0);
  aResult.D2V.SetCoord(0.0, 0.0, 0.0);
  aResult.D2UV.SetCoord(0.0, 0.0, 0.0);
  return aResult;
}

//=================================================================================================

Geom_Surface::ResD3 Geom_Plane::EvalD3(const double U, const double V) const
{
  Geom_Surface::ResD3 aResult;
  ElSLib::PlaneD1(U, V, pos, aResult.Point, aResult.D1U, aResult.D1V);
  aResult.D2U.SetCoord(0.0, 0.0, 0.0);
  aResult.D2V.SetCoord(0.0, 0.0, 0.0);
  aResult.D2UV.SetCoord(0.0, 0.0, 0.0);
  aResult.D3U.SetCoord(0.0, 0.0, 0.0);
  aResult.D3V.SetCoord(0.0, 0.0, 0.0);
  aResult.D3UUV.SetCoord(0.0, 0.0, 0.0);
  aResult.D3UVV.SetCoord(0.0, 0.0, 0.0);
  return aResult;
}

//=================================================================================================

gp_Vec Geom_Plane::EvalDN(const double,
                                         const double,
                                         const int Nu,
                                         const int Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    throw Geom_UndefinedDerivative();
  if (Nu == 0 && Nv == 1)
  {
    return Vec(pos.YDirection());
  }
  else if (Nu == 1 && Nv == 0)
  {
    return Vec(pos.XDirection());
  }
  return Vec(0.0, 0.0, 0.0);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_Plane::UIso(const double U) const
{
  occ::handle<Geom_Line> GL = new Geom_Line(ElSLib::PlaneUIso(pos, U));
  return GL;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_Plane::VIso(const double V) const
{
  occ::handle<Geom_Line> GL = new Geom_Line(ElSLib::PlaneVIso(pos, V));
  return GL;
}

//=================================================================================================

void Geom_Plane::TransformParameters(double& U, double& V, const gp_Trsf& T) const
{
  if (!Precision::IsInfinite(U))
    U *= std::abs(T.ScaleFactor());
  if (!Precision::IsInfinite(V))
    V *= std::abs(T.ScaleFactor());
}

//=================================================================================================

gp_GTrsf2d Geom_Plane::ParametricTransformation(const gp_Trsf& T) const
{
  gp_Trsf2d T2;
  T2.SetScale(gp::Origin2d(), std::abs(T.ScaleFactor()));
  return gp_GTrsf2d(T2);
}

//=================================================================================================

void Geom_Plane::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_ElementarySurface)
}
