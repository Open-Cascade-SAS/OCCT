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
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Adaptor3d_Surface, Standard_Transient)

//=================================================================================================

Adaptor3d_Surface::~Adaptor3d_Surface() {}

//=================================================================================================

Handle(Adaptor3d_Surface) Adaptor3d_Surface::ShallowCopy() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::ShallowCopy");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::FirstUParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::FirstUParameter");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::LastUParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::LastUParameter");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::FirstVParameter() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::FirstVParameter");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::LastVParameter() const
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

// Standard_Integer Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape S) const
Standard_Integer Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUIntervals");
}

//=================================================================================================

// Standard_Integer Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape S) const
Standard_Integer Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVIntervals");
}

//=================================================================================================

// void Adaptor3d_Surface::UIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
void Adaptor3d_Surface::UIntervals(TColStd_Array1OfReal&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UIntervals");
}

//=================================================================================================

// void Adaptor3d_Surface::VIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
void Adaptor3d_Surface::VIntervals(TColStd_Array1OfReal&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VIntervals");
}

//=================================================================================================

// Handle(Adaptor3d_Surface) Adaptor3d_Surface::UTrim(const Standard_Real First, const Standard_Real
// Last, const Standard_Real Tol) const
Handle(Adaptor3d_Surface) Adaptor3d_Surface::UTrim(const Standard_Real,
                                                   const Standard_Real,
                                                   const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UTrim");
}

//=================================================================================================

// Handle(Adaptor3d_Surface) Adaptor3d_Surface::VTrim(const Standard_Real First, const Standard_Real
// Last, const Standard_Real Tol) const
Handle(Adaptor3d_Surface) Adaptor3d_Surface::VTrim(const Standard_Real,
                                                   const Standard_Real,
                                                   const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VTrim");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Surface::IsUClosed() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsUClosed");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Surface::IsVClosed() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVClosed");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Surface::IsUPeriodic() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsUPeriodic");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::UPeriod() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UPeriod");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Surface::IsVPeriodic() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVPeriodic");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::VPeriod() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VPeriod");
}

//=================================================================================================

// gp_Pnt Adaptor3d_Surface::Value(const Standard_Real U, const Standard_Real V) const
gp_Pnt Adaptor3d_Surface::Value(const Standard_Real, const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Value");
}

//=================================================================================================

// void Adaptor3d_Surface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const
void Adaptor3d_Surface::D0(const Standard_Real, const Standard_Real, gp_Pnt&) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D0");
}

//=================================================================================================

// void Adaptor3d_Surface::D1(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V) const
void Adaptor3d_Surface::D1(const Standard_Real,
                           const Standard_Real,
                           gp_Pnt&,
                           gp_Vec&,
                           gp_Vec&) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D1");
}

//=================================================================================================

// void Adaptor3d_Surface::D2(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const
void Adaptor3d_Surface::D2(const Standard_Real,
                           const Standard_Real,
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

// void Adaptor3d_Surface::D3(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U,
// gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV,
// gp_Vec& D3UVV) const
void Adaptor3d_Surface::D3(const Standard_Real,
                           const Standard_Real,
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

// gp_Vec Adaptor3d_Surface::DN(const Standard_Real U, const Standard_Real V, const Standard_Integer
// Nu, const Standard_Integer Nv) const
gp_Vec Adaptor3d_Surface::DN(const Standard_Real,
                             const Standard_Real,
                             const Standard_Integer,
                             const Standard_Integer) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::DN");
}

//=================================================================================================

// Standard_Real Adaptor3d_Surface::UResolution(const Standard_Real R3d) const
Standard_Real Adaptor3d_Surface::UResolution(const Standard_Real) const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UResolution");
}

//=================================================================================================

// Standard_Real Adaptor3d_Surface::VResolution(const Standard_Real R3d) const
Standard_Real Adaptor3d_Surface::VResolution(const Standard_Real) const
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

Standard_Integer Adaptor3d_Surface::UDegree() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UDegree");
}

//=================================================================================================

Standard_Integer Adaptor3d_Surface::NbUPoles() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUPoles");
}

//=================================================================================================

Standard_Integer Adaptor3d_Surface::VDegree() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VDegree");
}

//=================================================================================================

Standard_Integer Adaptor3d_Surface::NbVPoles() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVPoles");
}

//=================================================================================================

Standard_Integer Adaptor3d_Surface::NbUKnots() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUKnots");
}

//=================================================================================================

Standard_Integer Adaptor3d_Surface::NbVKnots() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVKnots");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Surface::IsURational() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsURational");
}

//=================================================================================================

Standard_Boolean Adaptor3d_Surface::IsVRational() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVRational");
}

//=================================================================================================

Handle(Geom_BezierSurface) Adaptor3d_Surface::Bezier() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Bezier");
}

//=================================================================================================

Handle(Geom_BSplineSurface) Adaptor3d_Surface::BSpline() const
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

Handle(Adaptor3d_Curve) Adaptor3d_Surface::BasisCurve() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BasisCurve");
}

//=================================================================================================

Handle(Adaptor3d_Surface) Adaptor3d_Surface::BasisSurface() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BasisSurface");
}

//=================================================================================================

Standard_Real Adaptor3d_Surface::OffsetValue() const
{
  throw Standard_NotImplemented("Adaptor3d_Surface::OffsetValue");
}
