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
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAbs_UVSense.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_ToroidalSurface,Geom_ElementarySurface)

typedef Geom_ToroidalSurface         ToroidalSurface;
typedef TColStd_Array1OfReal      Array1OfReal;
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Ax3  Ax3;
typedef gp_Circ Circ;
typedef gp_Dir  Dir;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_ToroidalSurface::Copy () const {
 
   Handle(Geom_ToroidalSurface) Cs;
   Cs = new ToroidalSurface (pos, majorRadius, minorRadius);
   return Cs;
}



//=======================================================================
//function : Geom_ToroidalSurface
//purpose  : 
//=======================================================================

Geom_ToroidalSurface::Geom_ToroidalSurface 
  ( const Ax3& A3, 
    const Standard_Real MajorRadius, 
    const Standard_Real MinorRadius )
 
: majorRadius (MajorRadius), minorRadius (MinorRadius) {

  if (MinorRadius < 0.0 || MajorRadius < 0.0) {
    throw Standard_ConstructionError();
  }
  else {
    pos = A3;
  }
}


//=======================================================================
//function : Geom_ToroidalSurface
//purpose  : 
//=======================================================================

Geom_ToroidalSurface::Geom_ToroidalSurface (const gp_Torus& T)  
: majorRadius (T.MajorRadius()), minorRadius (T.MinorRadius()) {

   pos = T.Position();
}



//=======================================================================
//function : MajorRadius
//purpose  : 
//=======================================================================

Standard_Real Geom_ToroidalSurface::MajorRadius () const { 

  return majorRadius; 
}

//=======================================================================

//function : MinorRadius
//purpose  : 
//=======================================================================

Standard_Real Geom_ToroidalSurface::MinorRadius () const {
  
  return minorRadius; 
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_ToroidalSurface::UReversedParameter( const Standard_Real U) const {

  return (2.*M_PI - U);
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_ToroidalSurface::VReversedParameter( const Standard_Real V) const {

  return (2.*M_PI - V);
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ToroidalSurface::IsUClosed () const {

  return Standard_True; 
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ToroidalSurface::IsVClosed () const {

  return Standard_True; 
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ToroidalSurface::IsUPeriodic () const {

  return Standard_True; 
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ToroidalSurface::IsVPeriodic () const {

 return Standard_True; 
}


//=======================================================================
//function : SetMajorRadius
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::SetMajorRadius (const Standard_Real MajorRadius) {

  if (MajorRadius - minorRadius <= gp::Resolution())
    throw Standard_ConstructionError();
  else 
    majorRadius = MajorRadius;
}


//=======================================================================
//function : SetMinorRadius
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::SetMinorRadius (const Standard_Real MinorRadius) {

  if (MinorRadius < 0.0 || majorRadius - MinorRadius <= gp::Resolution())
    throw Standard_ConstructionError();
  else 
    minorRadius = MinorRadius;
}


//=======================================================================
//function : SetTorus
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::SetTorus (const gp_Torus& T) {
  
   minorRadius = T.MinorRadius();
   majorRadius = T.MajorRadius();
   pos = T.Position();
}


//=======================================================================
//function : Area
//purpose  : 
//=======================================================================

Standard_Real Geom_ToroidalSurface::Area () const {
  return 4.0 * M_PI * M_PI * minorRadius * majorRadius;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::Bounds (Standard_Real& U1,
				   Standard_Real& U2, 
				   Standard_Real& V1, 
				   Standard_Real& V2 ) const {
 
  U1 = 0.0;  
  V1 = 0.0;  
  U2 = 2*M_PI;  
  V2 = 2*M_PI;
}


//=======================================================================
//function : Coefficients
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::Coefficients (Array1OfReal& Coef) const {

  gp_Torus Tor (pos, majorRadius, minorRadius);
  Tor.Coefficients (Coef);
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::D0 (const Standard_Real U, const Standard_Real V, Pnt& P) const 
{

  ElSLib::TorusD0 (U, V, pos, majorRadius, minorRadius,P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::D1 
  (const Standard_Real U, const Standard_Real V, 
         Pnt& P, 
         Vec& D1U, Vec& D1V) const 
{
  ElSLib::TorusD1 (U, V, pos, majorRadius, minorRadius, P, D1U, D1V);  
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::D2 
  (const Standard_Real U  , const Standard_Real V, 
         Pnt& P  , 
         Vec& D1U, Vec& D1V, 
         Vec& D2U, Vec& D2V, Vec& D2UV ) const 
{
  ElSLib::TorusD2 (U, V, pos, majorRadius, minorRadius, P, D1U, D1V,
		   D2U, D2V, D2UV);  
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::D3 
  (const Standard_Real U, const Standard_Real V,
         Pnt& P, 
         Vec& D1U, Vec& D1V,
         Vec& D2U, Vec& D2V, Vec& D2UV,
         Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV ) const 
{

  ElSLib::TorusD3 (U, V, pos, majorRadius, minorRadius, P, D1U, D1V,
		   D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);  
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_ToroidalSurface::DN 
  (const Standard_Real    U , const Standard_Real    V, 
   const Standard_Integer Nu, const Standard_Integer Nv ) const {

  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv <0, "  ");
  return ElSLib::TorusDN (U, V, pos, majorRadius, minorRadius, Nu, Nv);
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus Geom_ToroidalSurface::Torus () const {

   return gp_Torus (pos, majorRadius, minorRadius);
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_ToroidalSurface::UIso (const Standard_Real U) const 
{
  Handle(Geom_Circle) 
    GC = new Geom_Circle(ElSLib::TorusUIso(pos,majorRadius,minorRadius,U));
  return GC;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_ToroidalSurface::VIso (const Standard_Real V) const 
{
  Handle(Geom_Circle) GC = 
    new Geom_Circle(ElSLib::TorusVIso(pos,majorRadius,minorRadius,V));
  return GC;
}


//=======================================================================
//function : Volume
//purpose  : 
//=======================================================================

Standard_Real Geom_ToroidalSurface::Volume () const {

  return (M_PI * minorRadius * minorRadius) * (2.0 * M_PI * majorRadius);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_ToroidalSurface::Transform (const Trsf& T) {

   majorRadius = majorRadius * Abs(T.ScaleFactor());
   minorRadius = minorRadius * Abs(T.ScaleFactor());
   pos.Transform (T);
}
