// Created on: 1993-07-01
// Created by: Bruno DUMORTIER
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

#include <Adaptor3d_Surface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_Surface, Standard_Transient)

//=================================================================================================

Adaptor3d_Surface::~Adaptor3d_Surface() = default;

//=================================================================================================

occ::handle<Adaptor3d_Surface> Adaptor3d_Surface::ShallowCopy() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::ShallowCopy");
}

//=================================================================================================

double Adaptor3d_Surface::FirstUParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::FirstUParameter");
}

//=================================================================================================

double Adaptor3d_Surface::LastUParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::LastUParameter");
}

//=================================================================================================

double Adaptor3d_Surface::FirstVParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::FirstVParameter");
}

//=================================================================================================

double Adaptor3d_Surface::LastVParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::LastVParameter");
}

//=================================================================================================

GeomAbs_Shape Adaptor3d_Surface::UContinuity() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UContinuity");
}

//=================================================================================================

GeomAbs_Shape Adaptor3d_Surface::VContinuity() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VContinuity");
}

//=================================================================================================

// int Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape S) const
int Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUIntervals");
}

//=================================================================================================

// int Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape S) const
int Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVIntervals");
}

//=================================================================================================

// void Adaptor3d_Surface::UIntervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
void Adaptor3d_Surface::UIntervals(NCollection_Array1<double>&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UIntervals");
}

//=================================================================================================

// void Adaptor3d_Surface::VIntervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
void Adaptor3d_Surface::VIntervals(NCollection_Array1<double>&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VIntervals");
}

//=================================================================================================

// occ::handle<Adaptor3d_Surface> Adaptor3d_Surface::UTrim(const double First, const double
// Last, const double Tol) const
occ::handle<Adaptor3d_Surface> Adaptor3d_Surface::UTrim(const double,
                                                        const double,
                                                        const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UTrim");
}

//=================================================================================================

// occ::handle<Adaptor3d_Surface> Adaptor3d_Surface::VTrim(const double First, const double
// Last, const double Tol) const
occ::handle<Adaptor3d_Surface> Adaptor3d_Surface::VTrim(const double,
                                                        const double,
                                                        const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VTrim");
}

//=================================================================================================

bool Adaptor3d_Surface::IsUClosed() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsUClosed");
}

//=================================================================================================

bool Adaptor3d_Surface::IsVClosed() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVClosed");
}

//=================================================================================================

bool Adaptor3d_Surface::IsUPeriodic() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsUPeriodic");
}

//=================================================================================================

double Adaptor3d_Surface::UPeriod() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UPeriod");
}

//=================================================================================================

bool Adaptor3d_Surface::IsVPeriodic() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVPeriodic");
}

//=================================================================================================

double Adaptor3d_Surface::VPeriod() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VPeriod");
}

//=================================================================================================

// gp_Pnt Adaptor3d_Surface::Value(const double U, const double V) const
gp_Pnt Adaptor3d_Surface::Value(const double, const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Value");
}

//=================================================================================================

// void Adaptor3d_Surface::D0(const double U, const double V, gp_Pnt& P) const
void Adaptor3d_Surface::D0(const double, const double, gp_Pnt&) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D0");
}

//=================================================================================================

// void Adaptor3d_Surface::D1(const double U, const double V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V) const
void Adaptor3d_Surface::D1(const double, const double, gp_Pnt&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D1");
}

//=================================================================================================

// void Adaptor3d_Surface::D2(const double U, const double V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const
void Adaptor3d_Surface::D2(const double,
                           const double,
                           gp_Pnt&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D2");
}

//=================================================================================================

// void Adaptor3d_Surface::D3(const double U, const double V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV,
// gp_Vec& D3UVV) const
void Adaptor3d_Surface::D3(const double,
                           const double,
                           gp_Pnt&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&,
                           gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D3");
}

//=================================================================================================

// gp_Vec Adaptor3d_Surface::DN(const double U, const double V, const int
// Nu, const int Nv) const
gp_Vec Adaptor3d_Surface::DN(const double, const double, const int, const int) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::DN");
}

//=================================================================================================

// double Adaptor3d_Surface::UResolution(const double R3d) const
double Adaptor3d_Surface::UResolution(const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UResolution");
}

//=================================================================================================

// double Adaptor3d_Surface::VResolution(const double R3d) const
double Adaptor3d_Surface::VResolution(const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VResolution");
}

//=================================================================================================

GeomAbs_SurfaceType Adaptor3d_Surface::GetType() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::GetType");
}

//=================================================================================================

gp_Pln Adaptor3d_Surface::Plane() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Plane");
}

//=================================================================================================

gp_Cylinder Adaptor3d_Surface::Cylinder() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Cylinder");
}

//=================================================================================================

gp_Cone Adaptor3d_Surface::Cone() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Cone");
}

//=================================================================================================

gp_Sphere Adaptor3d_Surface::Sphere() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Sphere");
}

//=================================================================================================

gp_Torus Adaptor3d_Surface::Torus() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Torus");
}

//=================================================================================================

int Adaptor3d_Surface::UDegree() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UDegree");
}

//=================================================================================================

int Adaptor3d_Surface::NbUPoles() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUPoles");
}

//=================================================================================================

int Adaptor3d_Surface::VDegree() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VDegree");
}

//=================================================================================================

int Adaptor3d_Surface::NbVPoles() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVPoles");
}

//=================================================================================================

int Adaptor3d_Surface::NbUKnots() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUKnots");
}

//=================================================================================================

int Adaptor3d_Surface::NbVKnots() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVKnots");
}

//=================================================================================================

bool Adaptor3d_Surface::IsURational() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsURational");
}

//=================================================================================================

bool Adaptor3d_Surface::IsVRational() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVRational");
}

//=================================================================================================

occ::handle<Geom_BezierSurface> Adaptor3d_Surface::Bezier() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Bezier");
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> Adaptor3d_Surface::BSpline() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BSpline");
}

//=================================================================================================

gp_Ax1 Adaptor3d_Surface::AxeOfRevolution() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::AxeOfRevolution");
}

//=================================================================================================

gp_Dir Adaptor3d_Surface::Direction() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Direction");
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> Adaptor3d_Surface::BasisCurve() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BasisCurve");
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> Adaptor3d_Surface::BasisSurface() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BasisSurface");
}

//=================================================================================================

double Adaptor3d_Surface::OffsetValue() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::OffsetValue");
}

//=================================================================================================

std::optional<gp_Pnt> Adaptor3d_Surface::EvalD0(double theU, double theV) const
{
  try
  {
    gp_Pnt aP;
    D0(theU, theV, aP);
    return aP;
  }
  catch (const Standard_Failure&)
  {
    return std::nullopt;
  }
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> Adaptor3d_Surface::EvalD1(double theU, double theV) const
{
  try
  {
    Geom_Surface::ResD1 aResult;
    D1(theU, theV, aResult.Point, aResult.D1U, aResult.D1V);
    return aResult;
  }
  catch (const Standard_Failure&)
  {
    return std::nullopt;
  }
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> Adaptor3d_Surface::EvalD2(double theU, double theV) const
{
  try
  {
    Geom_Surface::ResD2 aResult;
    D2(theU, theV, aResult.Point, aResult.D1U, aResult.D1V, aResult.D2U, aResult.D2V, aResult.D2UV);
    return aResult;
  }
  catch (const Standard_Failure&)
  {
    return std::nullopt;
  }
}

//=================================================================================================

std::optional<Geom_Surface::ResD3> Adaptor3d_Surface::EvalD3(double theU, double theV) const
{
  try
  {
    Geom_Surface::ResD3 aResult;
    D3(theU,
       theV,
       aResult.Point,
       aResult.D1U,
       aResult.D1V,
       aResult.D2U,
       aResult.D2V,
       aResult.D2UV,
       aResult.D3U,
       aResult.D3V,
       aResult.D3UUV,
       aResult.D3UVV);
    return aResult;
  }
  catch (const Standard_Failure&)
  {
    return std::nullopt;
  }
}

//=================================================================================================

std::optional<gp_Vec> Adaptor3d_Surface::EvalDN(double theU,
                                                double theV,
                                                int    theNu,
                                                int    theNv) const
{
  try
  {
    return DN(theU, theV, theNu, theNv);
  }
  catch (const Standard_Failure&)
  {
    return std::nullopt;
  }
}
