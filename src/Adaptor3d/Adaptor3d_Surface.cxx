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
  Standard_NotImplemented::Raise("Adaptor3d_Surface::FirstUParameter");
  return 0.;
}


//=======================================================================
//function : LastUParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::LastUParameter() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::LastUParameter");
  return 0.;
}


//=======================================================================
//function : FirstVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::FirstVParameter() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::FirstVParameter");
  return 0.;
}


//=======================================================================
//function : LastVParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::LastVParameter() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::LastVParameter");
  return 0.;
}


//=======================================================================
//function : UContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_Surface::UContinuity() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::UContinuity");
  return GeomAbs_C0;
}


//=======================================================================
//function : VContinuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_Surface::VContinuity() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::VContinuity");
  return GeomAbs_C0;
}


//=======================================================================
//function : NbUIntervals
//purpose  : 
//=======================================================================

//Standard_Integer Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape S) const 
Standard_Integer Adaptor3d_Surface::NbUIntervals(const GeomAbs_Shape ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::NbUIntervals");
  return 0;
}


//=======================================================================
//function : NbVIntervals
//purpose  : 
//=======================================================================

//Standard_Integer Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape S) const 
Standard_Integer Adaptor3d_Surface::NbVIntervals(const GeomAbs_Shape ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::NbVIntervals");
  return 0;
}


//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::UIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const 
void Adaptor3d_Surface::UIntervals(TColStd_Array1OfReal& , const GeomAbs_Shape ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::UIntervals");
}


//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::VIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const 
void Adaptor3d_Surface::VIntervals(TColStd_Array1OfReal& , const GeomAbs_Shape ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::VIntervals");
}


//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

//Handle(Adaptor3d_HSurface) Adaptor3d_Surface::UTrim(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const 
Handle(Adaptor3d_HSurface) Adaptor3d_Surface::UTrim(const Standard_Real , const Standard_Real , const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::UTrim");
  return Handle(Adaptor3d_HSurface)();
}


//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

//Handle(Adaptor3d_HSurface) Adaptor3d_Surface::VTrim(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const 
Handle(Adaptor3d_HSurface) Adaptor3d_Surface::VTrim(const Standard_Real , const Standard_Real , const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::VTrim");
  return Handle(Adaptor3d_HSurface)();
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsUClosed() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::IsUClosed");
  return 0;
}


//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsVClosed() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::IsVClosed");
  return 0;
}


//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsUPeriodic() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::IsUPeriodic");
  return 0;
}


//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::UPeriod() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::UPeriod");
  return 0.;
}


//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsVPeriodic() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::IsVPeriodic");
  return 0;
}


//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::VPeriod() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::VPeriod");
  return 0.;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

//gp_Pnt Adaptor3d_Surface::Value(const Standard_Real U, const Standard_Real V) const 
gp_Pnt Adaptor3d_Surface::Value(const Standard_Real , const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Value");
  return gp_Pnt();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const 
void Adaptor3d_Surface::D0(const Standard_Real , const Standard_Real , gp_Pnt& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::D0");
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D1(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const 
void Adaptor3d_Surface::D1(const Standard_Real , const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::D1");
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D2(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const 
void Adaptor3d_Surface::D2(const Standard_Real , const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::D2");
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

//void Adaptor3d_Surface::D3(const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const 
void Adaptor3d_Surface::D3(const Standard_Real , const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& , gp_Vec& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::D3");
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

//gp_Vec Adaptor3d_Surface::DN(const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const 
gp_Vec Adaptor3d_Surface::DN(const Standard_Real , const Standard_Real , const Standard_Integer , const Standard_Integer ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::DN");
  return gp_Vec();
}


//=======================================================================
//function : UResolution
//purpose  : 
//=======================================================================

//Standard_Real Adaptor3d_Surface::UResolution(const Standard_Real R3d) const 
Standard_Real Adaptor3d_Surface::UResolution(const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::UResolution");
  return 0.;
}


//=======================================================================
//function : VResolution
//purpose  : 
//=======================================================================

//Standard_Real Adaptor3d_Surface::VResolution(const Standard_Real R3d) const 
Standard_Real Adaptor3d_Surface::VResolution(const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::VResolution");
  return 0.;
}


//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_SurfaceType Adaptor3d_Surface::GetType() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::GetType");
  return GeomAbs_OtherSurface;
}


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln Adaptor3d_Surface::Plane() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Plane");
  return gp_Pln();
}


//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder Adaptor3d_Surface::Cylinder() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Cylinder");
  return gp_Cylinder();
}


//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone Adaptor3d_Surface::Cone() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Cone");
  return gp_Cone();
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere Adaptor3d_Surface::Sphere() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Sphere");
  return gp_Sphere();
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus Adaptor3d_Surface::Torus() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Torus");
  return gp_Torus();
}


//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::UDegree() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::UDegree");
  return 0;
}


//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbUPoles() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::NbUPoles");
  return 0;
}


//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::VDegree() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::VDegree");
  return 0;
}


//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbVPoles() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::NbVPoles");
  return 0;
}


//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbUKnots() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::NbUKnots");
  return 0;
}


//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Surface::NbVKnots() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::NbVKnots");
  return 0;
}


//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsURational() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::IsURational");
  return 0;
}


//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Surface::IsVRational() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::IsVRational");
  return 0;
}


//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) Adaptor3d_Surface::Bezier() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Bezier");
  return Handle(Geom_BezierSurface)();
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) Adaptor3d_Surface::BSpline() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::BSpline");
  return Handle(Geom_BSplineSurface)();
}


//=======================================================================
//function : AxeOfRevolution
//purpose  : 
//=======================================================================

gp_Ax1 Adaptor3d_Surface::AxeOfRevolution() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::AxeOfRevolution");
  return gp_Ax1();
}


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir Adaptor3d_Surface::Direction() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::Direction");
  return gp_Dir();
}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) Adaptor3d_Surface::BasisCurve() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::BasisCurve");
  return Handle(Adaptor3d_HCurve)();
}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) Adaptor3d_Surface::BasisSurface() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::BasisSurface");
  return Handle(Adaptor3d_HSurface)();
}


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Surface::OffsetValue() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Surface::OffsetValue");
  return 0.;
}
