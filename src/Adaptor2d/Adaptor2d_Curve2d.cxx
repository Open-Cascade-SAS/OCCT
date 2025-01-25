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
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor2d_Curve2d, Standard_Transient)

//=================================================================================================

Adaptor2d_Curve2d::~Adaptor2d_Curve2d() {}

//=================================================================================================

Handle(Adaptor2d_Curve2d) Adaptor2d_Curve2d::ShallowCopy() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::ShallowCopy");
}

//=================================================================================================

Standard_Real Adaptor2d_Curve2d::FirstParameter() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::FirstParameter");
}

//=================================================================================================

Standard_Real Adaptor2d_Curve2d::LastParameter() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::LastParameter");
}

//=================================================================================================

GeomAbs_Shape Adaptor2d_Curve2d::Continuity() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Continuity");
}

//=================================================================================================

// Standard_Integer Adaptor2d_Curve2d::NbIntervals(const GeomAbs_Shape S) const
Standard_Integer Adaptor2d_Curve2d::NbIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::NbIntervals");
}

//=================================================================================================

// void Adaptor2d_Curve2d::Intervals(TColStd_Array1OfReal& T,
//				const GeomAbs_Shape S) const
void Adaptor2d_Curve2d::Intervals(TColStd_Array1OfReal&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Intervals");
}

//=================================================================================================

// Handle(Adaptor2d_Curve2d) Adaptor2d_Curve2d::Trim(const Standard_Real First,
//					       const Standard_Real Last ,
//					       const Standard_Real Tol) const
Handle(Adaptor2d_Curve2d) Adaptor2d_Curve2d::Trim(const Standard_Real,
                                                  const Standard_Real,
                                                  const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Trim");
}

//=================================================================================================

Standard_Boolean Adaptor2d_Curve2d::IsClosed() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::IsClosed");
}

//=================================================================================================

Standard_Boolean Adaptor2d_Curve2d::IsPeriodic() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::IsPeriodic");
}

//=================================================================================================

Standard_Real Adaptor2d_Curve2d::Period() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Period");
}

//=================================================================================================

// gp_Pnt2d Adaptor2d_Curve2d::Value(const Standard_Real U) const
gp_Pnt2d Adaptor2d_Curve2d::Value(const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Value");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D0(const Standard_Real U, gp_Pnt2d& P) const
void Adaptor2d_Curve2d::D0(const Standard_Real, gp_Pnt2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D0");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D1(const Standard_Real U,
//			 gp_Pnt2d& P, gp_Vec2d& V) const
void Adaptor2d_Curve2d::D1(const Standard_Real, gp_Pnt2d&, gp_Vec2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D1");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D2(const Standard_Real U,
//			 gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
void Adaptor2d_Curve2d::D2(const Standard_Real, gp_Pnt2d&, gp_Vec2d&, gp_Vec2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D2");
}

//=================================================================================================

// void Adaptor2d_Curve2d::D3(const Standard_Real U,
//			 gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3)
void Adaptor2d_Curve2d::D3(const Standard_Real, gp_Pnt2d&, gp_Vec2d&, gp_Vec2d&, gp_Vec2d&) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::D3");
}

//=================================================================================================

// gp_Vec2d Adaptor2d_Curve2d::DN(const Standard_Real U,
//			     const Standard_Integer N) const
gp_Vec2d Adaptor2d_Curve2d::DN(const Standard_Real, const Standard_Integer) const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::DN");
}

//=================================================================================================

// Standard_Real Adaptor2d_Curve2d::Resolution(const Standard_Real R3d) const
Standard_Real Adaptor2d_Curve2d::Resolution(const Standard_Real) const
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

Standard_Integer Adaptor2d_Curve2d::Degree() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Degree");
}

//=================================================================================================

Standard_Boolean Adaptor2d_Curve2d::IsRational() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::IsRational");
}

//=================================================================================================

Standard_Integer Adaptor2d_Curve2d::NbPoles() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::NbPole");
}

//=================================================================================================

Standard_Integer Adaptor2d_Curve2d::NbKnots() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::NbKnots");
}

//=================================================================================================

Handle(Geom2d_BezierCurve) Adaptor2d_Curve2d::Bezier() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::Bezier");
}

//=================================================================================================

Handle(Geom2d_BSplineCurve) Adaptor2d_Curve2d::BSpline() const
{
  throw Standard_NotImplemented("Adaptor2d_Curve2d::BSpline");
}

//=================================================================================================

Standard_Integer Adaptor2d_Curve2d::NbSamples() const
{
  return 20;
}
