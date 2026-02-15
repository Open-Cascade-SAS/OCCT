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

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor2d_Curve2d, Standard_Transient)

//=================================================================================================

Adaptor2d_Curve2d::~Adaptor2d_Curve2d() = default;

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> Adaptor2d_Curve2d::ShallowCopy() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::ShallowCopy");
}

//=================================================================================================

double Adaptor2d_Curve2d::FirstParameter() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::FirstParameter");
}

//=================================================================================================

double Adaptor2d_Curve2d::LastParameter() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::LastParameter");
}

//=================================================================================================

GeomAbs_Shape Adaptor2d_Curve2d::Continuity() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Continuity");
}

//=================================================================================================

// int Adaptor2d_Curve2d::NbIntervals(const GeomAbs_Shape S) const
int Adaptor2d_Curve2d::NbIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::NbIntervals");
}

//=================================================================================================

// void Adaptor2d_Curve2d::Intervals(NCollection_Array1<double>& T,
//				const GeomAbs_Shape S) const
void Adaptor2d_Curve2d::Intervals(NCollection_Array1<double>&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Intervals");
}

//=================================================================================================

// occ::handle<Adaptor2d_Curve2d> Adaptor2d_Curve2d::Trim(const double First,
//					       const double Last ,
//					       const double Tol) const
occ::handle<Adaptor2d_Curve2d> Adaptor2d_Curve2d::Trim(const double,
                                                       const double,
                                                       const double) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Trim");
}

//=================================================================================================

bool Adaptor2d_Curve2d::IsClosed() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::IsClosed");
}

//=================================================================================================

bool Adaptor2d_Curve2d::IsPeriodic() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::IsPeriodic");
}

//=================================================================================================

double Adaptor2d_Curve2d::Period() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Period");
}

//=================================================================================================

// gp_Pnt2d Adaptor2d_Curve2d::Value(const double U) const
gp_Pnt2d Adaptor2d_Curve2d::Value(const double) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Value");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D0(const double U, gp_Pnt2d& P) const
void Adaptor2d_Curve2d::D0(const double, gp_Pnt2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D0");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D1(const double U,
//			 gp_Pnt2d& P, gp_Vec2d& V) const
void Adaptor2d_Curve2d::D1(const double, gp_Pnt2d&, gp_Vec2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D1");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D2(const double U,
//			 gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
void Adaptor2d_Curve2d::D2(const double, gp_Pnt2d&, gp_Vec2d&, gp_Vec2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D2");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D3(const double U,
//			 gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3)
void Adaptor2d_Curve2d::D3(const double, gp_Pnt2d&, gp_Vec2d&, gp_Vec2d&, gp_Vec2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D3");
}

//=================================================================================================

// gp_Vec2d Adaptor2d_Curve2d::DN(const double U,
//			     const int N) const
gp_Vec2d Adaptor2d_Curve2d::DN(const double, const int) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::DN");
}

//=================================================================================================

// double Adaptor2d_Curve2d::Resolution(const double R3d) const
double Adaptor2d_Curve2d::Resolution(const double) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Resolution");
}

//=================================================================================================

GeomAbs_CurveType Adaptor2d_Curve2d::GetType() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::GetType");
}

//=================================================================================================

gp_Lin2d Adaptor2d_Curve2d::Line() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Line");
}

//=================================================================================================

gp_Circ2d Adaptor2d_Curve2d::Circle() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Circle");
}

//=================================================================================================

gp_Elips2d Adaptor2d_Curve2d::Ellipse() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Ellipse");
}

//=================================================================================================

gp_Hypr2d Adaptor2d_Curve2d::Hyperbola() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Hyperbola");
}

//=================================================================================================

gp_Parab2d Adaptor2d_Curve2d::Parabola() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Parabola");
}

//=================================================================================================

int Adaptor2d_Curve2d::Degree() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Degree");
}

//=================================================================================================

bool Adaptor2d_Curve2d::IsRational() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::IsRational");
}

//=================================================================================================

int Adaptor2d_Curve2d::NbPoles() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::NbPole");
}

//=================================================================================================

int Adaptor2d_Curve2d::NbKnots() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::NbKnots");
}

//=================================================================================================

occ::handle<Geom2d_BezierCurve> Adaptor2d_Curve2d::Bezier() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Bezier");
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Adaptor2d_Curve2d::BSpline() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::BSpline");
}

//=================================================================================================

int Adaptor2d_Curve2d::NbSamples() const
{
  return 20;
}

//=================================================================================================

gp_Pnt2d Adaptor2d_Curve2d::EvalD0(double theU) const
{
  gp_Pnt2d aP;
  D0(theU, aP);
  return aP;
}

//=================================================================================================

Geom2d_Curve::ResD1 Adaptor2d_Curve2d::EvalD1(double theU) const
{
  Geom2d_Curve::ResD1 aResult;
  D1(theU, aResult.Point, aResult.D1);
  return aResult;
}

//=================================================================================================

Geom2d_Curve::ResD2 Adaptor2d_Curve2d::EvalD2(double theU) const
{
  Geom2d_Curve::ResD2 aResult;
  D2(theU, aResult.Point, aResult.D1, aResult.D2);
  return aResult;
}

//=================================================================================================

Geom2d_Curve::ResD3 Adaptor2d_Curve2d::EvalD3(double theU) const
{
  Geom2d_Curve::ResD3 aResult;
  D3(theU, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
  return aResult;
}

//=================================================================================================

gp_Vec2d Adaptor2d_Curve2d::EvalDN(double theU, int theN) const
{
  return DN(theU, theN);
}
