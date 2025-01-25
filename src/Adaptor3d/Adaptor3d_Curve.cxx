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
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_Curve, Standard_Transient)

//=================================================================================================

Adaptor3d_Curve::~Adaptor3d_Curve() {}

//=================================================================================================

Handle(Adaptor3d_Curve) Adaptor3d_Curve::ShallowCopy() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::ShallowCopy");
}

//=================================================================================================

Standard_Real Adaptor3d_Curve::FirstParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::FirstParameter");
}

//=================================================================================================

Standard_Real Adaptor3d_Curve::LastParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::LastParameter");
}

//=================================================================================================

GeomAbs_Shape Adaptor3d_Curve::Continuity() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Continuity");
}

//=================================================================================================

Standard_Integer Adaptor3d_Curve::NbIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::NbIntervals");
}

//=================================================================================================

void Adaptor3d_Curve::Intervals(TColStd_Array1OfReal&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Intervals");
}

//=================================================================================================

// Handle(Adaptor3d_Curve) Adaptor3d_Curve::Trim(const Standard_Real First, const Standard_Real
// Last, const Standard_Real Tol) const
Handle(Adaptor3d_Curve) Adaptor3d_Curve::Trim(const Standard_Real,
                                              const Standard_Real,
                                              const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Trim");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Curve::IsClosed() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::IsClosed");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Curve::IsPeriodic() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::IsPeriodic");
}

//=================================================================================================

Standard_Real Adaptor3d_Curve::Period() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Period");
}

//=================================================================================================

// gp_Pnt Adaptor3d_Curve::Value(const Standard_Real U) const
gp_Pnt Adaptor3d_Curve::Value(const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Value");
}

//=================================================================================================

// void Adaptor3d_Curve::D0(const Standard_Real U, gp_Pnt& P) const
void Adaptor3d_Curve::D0(const Standard_Real, gp_Pnt&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D0");
}

//=================================================================================================

// void Adaptor3d_Curve::D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const
void Adaptor3d_Curve::D1(const Standard_Real, gp_Pnt&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D1");
}

//=================================================================================================

// void Adaptor3d_Curve::D2(const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
void Adaptor3d_Curve::D2(const Standard_Real, gp_Pnt&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D2");
}

//=================================================================================================

// void Adaptor3d_Curve::D3(const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3)
// const
void Adaptor3d_Curve::D3(const Standard_Real, gp_Pnt&, gp_Vec&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::D3");
}

//=================================================================================================

// gp_Vec Adaptor3d_Curve::DN(const Standard_Real U, const Standard_Integer N) const
gp_Vec Adaptor3d_Curve::DN(const Standard_Real, const Standard_Integer) const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::DN");
}

//=================================================================================================

// Standard_Real Adaptor3d_Curve::Resolution(const Standard_Real R3d) const
Standard_Real Adaptor3d_Curve::Resolution(const Standard_Real) const
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

Standard_Integer Adaptor3d_Curve::Degree() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Degree");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Curve::IsRational() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::IsRational");
}

//=================================================================================================

Standard_Integer Adaptor3d_Curve::NbPoles() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::NbPoles");
}

//=================================================================================================

Standard_Integer Adaptor3d_Curve::NbKnots() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::NbKnots");
}

//=================================================================================================

Handle(Geom_BezierCurve) Adaptor3d_Curve::Bezier() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::Bezier");
}

//=================================================================================================

Handle(Geom_BSplineCurve) Adaptor3d_Curve::BSpline() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::BSpline");
}

//=================================================================================================

Handle(Geom_OffsetCurve) Adaptor3d_Curve::OffsetCurve() const
{
  throw Standard_NotImplemented("Adaptor3d_Curve::OffsetCurve");
}
