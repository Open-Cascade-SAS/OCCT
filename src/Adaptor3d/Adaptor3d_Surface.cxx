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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_Surface.hxx>
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
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : ~Adaptor3d_Surface
//purpose  : Destructor
//=======================================================================
Adaptor3d_Surface::~Adaptor3d_Surface()
{
}

//=======================================================================
//function : FirstUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::FirstUParameter() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::FirstUParameter");
}


//=======================================================================
//function : LastUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::LastUParameter() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::LastUParameter");
}


//=======================================================================
//function : FirstVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::FirstVParameter() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::FirstVParameter");
}


//=======================================================================
//function : LastVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::LastVParameter() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::LastVParameter");
}


//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_Surface::UContinuity() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UContinuity");
}


//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_Surface::VContinuity() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VContinuity");
}


//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

//Standard_Integer Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape S) const 
Standard_Integer Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUIntervals");
}


//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

//Standard_Integer Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape S) const 
Standard_Integer Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVIntervals");
}


//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::UIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const 
void Adaptor3d_Surface::UIntervals(TColStd_Array1OfReal& , const GeomAbs_Shape ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UIntervals");
}


//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::VIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const 
void Adaptor3d_Surface::VIntervals(TColStd_Array1OfReal& , const GeomAbs_Shape ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VIntervals");
}


//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

//Handle(Adaptor3d_HSurface) Adaptor3d_Surface::UTrim(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const 
Handle(Adaptor3d_HSurface) Adaptor3d_Surface::UTrim(const Standard_Real , const Standard_Real , const Standard_Real ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UTrim");
}


//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

//Handle(Adaptor3d_HSurface) Adaptor3d_Surface::VTrim(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const 
Handle(Adaptor3d_HSurface) Adaptor3d_Surface::VTrim(const Standard_Real , const Standard_Real , const Standard_Real ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VTrim");
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsUClosed() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsUClosed");
}


//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsVClosed() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVClosed");
}


//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsUPeriodic() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsUPeriodic");
}


//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::UPeriod() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UPeriod");
}


//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsVPeriodic() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVPeriodic");
}


//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::VPeriod() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VPeriod");
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

//gp_Pnt Adaptor3d_Surface::Value(const Standard_Real U, const Standard_Real V) const 
gp_Pnt Adaptor3d_Surface::Value(const Standard_Real , const Standard_Real ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Value");
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const 
void Adaptor3d_Surface::D0(const Standard_Real , const Standard_Real , gp_Pnt& ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D0");
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D1(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const 
void Adaptor3d_Surface::D1(const Standard_Real , const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D1");
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D2(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const 
void Adaptor3d_Surface::D2(const Standard_Real , const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D2");
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D3(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const 
void Adaptor3d_Surface::D3(const Standard_Real , const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::D3");
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

//gp_Vec Adaptor3d_Surface::DN(const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const 
gp_Vec Adaptor3d_Surface::DN(const Standard_Real , const Standard_Real , const Standard_Integer , const Standard_Integer ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::DN");
}


//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

//Standard_Real Adaptor3d_Surface::UResolution(const Standard_Real R3d) const 
Standard_Real Adaptor3d_Surface::UResolution(const Standard_Real ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UResolution");
}


//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

//Standard_Real Adaptor3d_Surface::VResolution(const Standard_Real R3d) const 
Standard_Real Adaptor3d_Surface::VResolution(const Standard_Real ) const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VResolution");
}


//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_SurfaceType Adaptor3d_Surface::GetType() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::GetType");
}


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln Adaptor3d_Surface::Plane() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Plane");
}


//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder Adaptor3d_Surface::Cylinder() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Cylinder");
}


//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone Adaptor3d_Surface::Cone() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Cone");
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere Adaptor3d_Surface::Sphere() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Sphere");
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus Adaptor3d_Surface::Torus() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Torus");
}


//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::UDegree() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::UDegree");
}


//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbUPoles() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUPoles");
}


//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::VDegree() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::VDegree");
}


//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbVPoles() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVPoles");
}


//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbUKnots() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbUKnots");
}


//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbVKnots() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::NbVKnots");
}


//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsURational() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsURational");
}


//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsVRational() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::IsVRational");
}


//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) Adaptor3d_Surface::Bezier() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Bezier");
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) Adaptor3d_Surface::BSpline() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BSpline");
}


//=======================================================================
//function : AxeOfRevolution
//purpose  : 
//=======================================================================

gp_Ax1 Adaptor3d_Surface::AxeOfRevolution() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::AxeOfRevolution");
}


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir Adaptor3d_Surface::Direction() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::Direction");
}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) Adaptor3d_Surface::BasisCurve() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BasisCurve");
}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) Adaptor3d_Surface::BasisSurface() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::BasisSurface");
}


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::OffsetValue() const 
{
  throw Standard_NotImplemented("Adaptor3d_Surface::OffsetValue");
}
