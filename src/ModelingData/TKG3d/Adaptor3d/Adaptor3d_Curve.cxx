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

#include <Adaptor3d_Curve.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_Curve, Standard_Transient)

//=================================================================================================

Adaptor3d_Curve::~Adaptor3d_Curve() = default;

//=================================================================================================

occ::handle<Adaptor3d_Curve> Adaptor3d_Curve::ShallowCopy() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::ShallowCopy");
}

//=================================================================================================

double Adaptor3d_Curve::FirstParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::FirstParameter");
}

//=================================================================================================

double Adaptor3d_Curve::LastParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::LastParameter");
}

//=================================================================================================

GeomAbs_Shape Adaptor3d_Curve::Continuity() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Continuity");
}

//=================================================================================================

int Adaptor3d_Curve::NbIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::NbIntervals");
}

//=================================================================================================

void Adaptor3d_Curve::Intervals(NCollection_Array1<double>&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Intervals");
}

//=================================================================================================

// occ::handle<Adaptor3d_Curve> Adaptor3d_Curve::Trim(const double First, const double
// Last, const double Tol) const
occ::handle<Adaptor3d_Curve> Adaptor3d_Curve::Trim(const double, const double, const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Trim");
}

//=================================================================================================

bool Adaptor3d_Curve::IsClosed() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::IsClosed");
}

//=================================================================================================

bool Adaptor3d_Curve::IsPeriodic() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::IsPeriodic");
}

//=================================================================================================

double Adaptor3d_Curve::Period() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Period");
}

//=================================================================================================

// gp_Pnt Adaptor3d_Curve::Value(const double U) const
gp_Pnt Adaptor3d_Curve::Value(const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Value");
}

//=================================================================================================

// void Adaptor3d_Curve::D0(const double U, gp_Pnt& P) const
void Adaptor3d_Curve::D0(const double, gp_Pnt&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D0");
}

//=================================================================================================

// void Adaptor3d_Curve::D1(const double U, gp_Pnt& P, gp_Vec& V) const
void Adaptor3d_Curve::D1(const double, gp_Pnt&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D1");
}

//=================================================================================================

// void Adaptor3d_Curve::D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
void Adaptor3d_Curve::D2(const double, gp_Pnt&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D2");
}

//=================================================================================================

// void Adaptor3d_Curve::D3(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3)
// const
void Adaptor3d_Curve::D3(const double, gp_Pnt&, gp_Vec&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D3");
}

//=================================================================================================

// gp_Vec Adaptor3d_Curve::DN(const double U, const int N) const
gp_Vec Adaptor3d_Curve::DN(const double, const int) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::DN");
}

//=================================================================================================

// double Adaptor3d_Curve::Resolution(const double R3d) const
double Adaptor3d_Curve::Resolution(const double) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Resolution");
}

//=================================================================================================

GeomAbs_CurveType Adaptor3d_Curve::GetType() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::GetType");
}

//=================================================================================================

gp_Lin Adaptor3d_Curve::Line() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Line");
}

//=================================================================================================

gp_Circ Adaptor3d_Curve::Circle() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Circle");
}

//=================================================================================================

gp_Elips Adaptor3d_Curve::Ellipse() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Ellipse");
}

//=================================================================================================

gp_Hypr Adaptor3d_Curve::Hyperbola() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Hyperbola");
}

//=================================================================================================

gp_Parab Adaptor3d_Curve::Parabola() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Parabola");
}

//=================================================================================================

int Adaptor3d_Curve::Degree() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Degree");
}

//=================================================================================================

bool Adaptor3d_Curve::IsRational() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::IsRational");
}

//=================================================================================================

int Adaptor3d_Curve::NbPoles() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::NbPoles");
}

//=================================================================================================

int Adaptor3d_Curve::NbKnots() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::NbKnots");
}

//=================================================================================================

occ::handle<Geom_BezierCurve> Adaptor3d_Curve::Bezier() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Bezier");
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> Adaptor3d_Curve::BSpline() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::BSpline");
}

//=================================================================================================

occ::handle<Geom_OffsetCurve> Adaptor3d_Curve::OffsetCurve() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::OffsetCurve");
}

//=================================================================================================

gp_Pnt Adaptor3d_Curve::EvalD0(double theU) const
{
  gp_Pnt aP;
  D0(theU, aP);
  return aP;
}

//=================================================================================================

Geom_Curve::ResD1 Adaptor3d_Curve::EvalD1(double theU) const
{
  Geom_Curve::ResD1 aResult;
  D1(theU, aResult.Point, aResult.D1);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD2 Adaptor3d_Curve::EvalD2(double theU) const
{
  Geom_Curve::ResD2 aResult;
  D2(theU, aResult.Point, aResult.D1, aResult.D2);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD3 Adaptor3d_Curve::EvalD3(double theU) const
{
  Geom_Curve::ResD3 aResult;
  D3(theU, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
  return aResult;
}

//=================================================================================================

gp_Vec Adaptor3d_Curve::EvalDN(double theU, int theN) const
{
  return DN(theU, theN);
}
