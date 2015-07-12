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
#include <Adaptor3d_HCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : ~Adaptor3d_Curve
//purpose  : Destructor
//=======================================================================
Adaptor3d_Curve::~Adaptor3d_Curve()
{
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Curve::FirstParameter() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::FirstParameter");
  return 0.;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Curve::LastParameter() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::LastParameter");
  return 0.;
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_Curve::Continuity() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Continuity");
  return GeomAbs_C0;
}


//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Curve::NbIntervals(const GeomAbs_Shape ) const
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::NbIntervals");
  return 0;
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void Adaptor3d_Curve::Intervals(TColStd_Array1OfReal& , const GeomAbs_Shape ) const
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Intervals");
}


//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

//Handle(Adaptor3d_HCurve) Adaptor3d_Curve::Trim(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const 
Handle(Adaptor3d_HCurve) Adaptor3d_Curve::Trim(const Standard_Real , const Standard_Real , const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Trim");
  return Handle(Adaptor3d_HCurve)();
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Curve::IsClosed() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::IsClosed");
  return 0;
}


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Curve::IsPeriodic() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::IsPeriodic");
  return 0;
}


//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_Curve::Period() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Period");
  return 0.;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

//gp_Pnt Adaptor3d_Curve::Value(const Standard_Real U) const 
gp_Pnt Adaptor3d_Curve::Value(const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Value");
  return gp_Pnt();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

//void Adaptor3d_Curve::D0(const Standard_Real U, gp_Pnt& P) const 
void Adaptor3d_Curve::D0(const Standard_Real , gp_Pnt& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::D0");
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

//void Adaptor3d_Curve::D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const 
void Adaptor3d_Curve::D1(const Standard_Real , gp_Pnt& , gp_Vec& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::D1");
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

//void Adaptor3d_Curve::D2(const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const 
void Adaptor3d_Curve::D2(const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::D2");
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

//void Adaptor3d_Curve::D3(const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const 
void Adaptor3d_Curve::D3(const Standard_Real , gp_Pnt& , gp_Vec& , gp_Vec& , gp_Vec& ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::D3");
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

//gp_Vec Adaptor3d_Curve::DN(const Standard_Real U, const Standard_Integer N) const 
gp_Vec Adaptor3d_Curve::DN(const Standard_Real , const Standard_Integer ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::DN");
  return gp_Vec();
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

//Standard_Real Adaptor3d_Curve::Resolution(const Standard_Real R3d) const 
Standard_Real Adaptor3d_Curve::Resolution(const Standard_Real ) const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Resolution");
  return 0.;
}


//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType Adaptor3d_Curve::GetType() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::GetType");
  return GeomAbs_OtherCurve;
}


//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin Adaptor3d_Curve::Line() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Line");
  return gp_Lin();
}


//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ Adaptor3d_Curve::Circle() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Circle");
  return gp_Circ();
}


//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips Adaptor3d_Curve::Ellipse() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Ellipse");
  return gp_Elips();
}


//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr Adaptor3d_Curve::Hyperbola() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Hyperbola");
  return gp_Hypr();
}


//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab Adaptor3d_Curve::Parabola() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Parabola");
  return gp_Parab();
}


//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Curve::Degree() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Degree");
  return 0;
}


//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_Curve::IsRational() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::IsRational");
  return 0;
}


//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Curve::NbPoles() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::NbPoles");
  return 0;
}


//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_Curve::NbKnots() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::NbKnots");
  return 0;
}


//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) Adaptor3d_Curve::Bezier() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::Bezier");
  return Handle(Geom_BezierCurve)();
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) Adaptor3d_Curve::BSpline() const 
{
  Standard_NotImplemented::Raise("Adaptor3d_Curve::BSpline");
  return Handle(Geom_BSplineCurve)();
}
