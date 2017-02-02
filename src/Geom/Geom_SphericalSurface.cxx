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
#include <Geom_SphericalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_SphericalSurface,Geom_ElementarySurface)

typedef Geom_Circle                   Circle;
typedef Geom_SphericalSurface         SphericalSurface;
typedef gp_Ax2  Ax2;
typedef gp_Ax3  Ax3;
typedef gp_Circ Circ;
typedef gp_Dir  Dir;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_XYZ  XYZ;
typedef gp_Vec  Vec;

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_SphericalSurface::Copy () const {
 
  Handle(Geom_SphericalSurface) Cs;
  Cs = new SphericalSurface (pos, radius);
  return Cs;
}



//=======================================================================
//function : Geom_SphericalSurface
//purpose  : 
//=======================================================================

Geom_SphericalSurface::Geom_SphericalSurface (const Ax3& A, const Standard_Real R) 
: radius (R) {

  if (R < 0.0) throw Standard_ConstructionError();
  pos = A;
}


//=======================================================================
//function : Geom_SphericalSurface
//purpose  : 
//=======================================================================

Geom_SphericalSurface::Geom_SphericalSurface (const gp_Sphere& S) 
 :radius (S.Radius()) {

  pos = S.Position();
}



//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SphericalSurface::UReversedParameter( const Standard_Real U) const
{
  return (2.*M_PI - U);
}

//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SphericalSurface::VReversedParameter( const Standard_Real V) const
{
  return (-V);
}


//=======================================================================
//function : Area
//purpose  : 
//=======================================================================

Standard_Real Geom_SphericalSurface::Area () const 
{return 4.0 * M_PI * radius * radius;}

//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real Geom_SphericalSurface::Radius () const                  
{ return radius; }

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SphericalSurface::IsUClosed () const           
{ return Standard_True; }

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SphericalSurface::IsVClosed () const            
{ return Standard_False; }

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SphericalSurface::IsUPeriodic () const          
{ return Standard_True; }

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SphericalSurface::IsVPeriodic () const          
{ return Standard_False; }

//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::SetRadius (const Standard_Real R) {

  if (R < 0.0) { throw Standard_ConstructionError(); }
  radius = R;
}


//=======================================================================
//function : SetSphere
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::SetSphere (const gp_Sphere& S) {

  radius = S.Radius();
  pos = S.Position();
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::Bounds (Standard_Real& U1, Standard_Real& U2,
				    Standard_Real& V1, Standard_Real& V2) const {

  U1 =       0.0;  
  U2 =  M_PI * 2.0; 
  V1 = -M_PI / 2.0;
  V2 =  M_PI / 2.0;
}


//=======================================================================
//function : Coefficients
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::Coefficients (Standard_Real& A1, Standard_Real& A2, Standard_Real& A3,
					  Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, 
					  Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, 
					  Standard_Real& D ) const {

   // Dans le repere local de la sphere :
   // X*X + Y*Y + Z*Z - radius * radius = 0

      Trsf T;
      T.SetTransformation (pos);
      Standard_Real T11 = T.Value (1, 1);
      Standard_Real T12 = T.Value (1, 2);
      Standard_Real T13 = T.Value (1, 3);
      Standard_Real T14 = T.Value (1, 4);
      Standard_Real T21 = T.Value (2, 1);
      Standard_Real T22 = T.Value (2, 2);
      Standard_Real T23 = T.Value (2, 3);
      Standard_Real T24 = T.Value (2, 4);
      Standard_Real T31 = T.Value (3, 1);
      Standard_Real T32 = T.Value (3, 2);
      Standard_Real T33 = T.Value (3, 3);
      Standard_Real T34 = T.Value (3, 4);
      A1 = T11 * T11 + T21 * T21 + T31 * T31;
      A2 = T12 * T12 + T22 * T22 + T32 * T32;
      A3 = T13 * T13 + T23 * T23 + T33 * T33;
      B1 = T11 * T12 + T21 * T22 + T31 * T32;
      B2 = T11 * T13 + T21 * T23 + T31 * T33;
      B3 = T12 * T13 + T22 * T23 + T32 * T33;
      C1 = T11 * T14 + T21 * T24 + T31 * T34;
      C2 = T12 * T14 + T22 * T24 + T32 * T34;
      C3 = T13 * T14 + T23 * T24 + T33 * T34;
      D = T14 * T14 + T24 * T24 + T34 * T34 - radius * radius;
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::D0 (const Standard_Real U, const Standard_Real V, Pnt& P) const 
{
  ElSLib::SphereD0(U,V,pos,radius,P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::D1 (const Standard_Real U, const Standard_Real V  , 
				      Pnt& P,       Vec& D1U, Vec& D1V
			       ) const 
{
  ElSLib::SphereD1 (U, V, pos, radius, P ,D1U, D1V);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::D2 (const Standard_Real U, const Standard_Real V,
				Pnt& P,
				Vec& D1U, Vec& D1V, 
				Vec& D2U, Vec& D2V, Vec& D2UV ) const
{
  ElSLib::SphereD2 (U, V, pos, radius, P, D1U, D1V, D2U, D2V, D2UV);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::D3 (const Standard_Real U, const Standard_Real V, 
				Pnt& P,
				Vec& D1U, Vec& D1V, 
				Vec& D2U, Vec& D2V, Vec& D2UV,
				Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV
       			       ) const
{
  ElSLib::SphereD3 (U, V, pos, radius, P, D1U, D1V, D2U, D2V,
		    D2UV, D3U, D3V, D3UUV, D3UVV);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_SphericalSurface::DN (const Standard_Real U, const Standard_Real V, 
			       const Standard_Integer Nu, const Standard_Integer Nv) const {

   Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv <0, " ");
   return  ElSLib::SphereDN (U, V, pos, radius, Nu, Nv);
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere Geom_SphericalSurface::Sphere () const {

  return gp_Sphere (pos, radius);
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SphericalSurface::UIso (const Standard_Real U) const 
{
  Handle(Geom_Circle) GC = new Geom_Circle(ElSLib::SphereUIso(pos,radius,U));
  Handle(Geom_TrimmedCurve) iso = new Geom_TrimmedCurve(GC,-M_PI/2.,M_PI/2);
  return iso;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SphericalSurface::VIso (const Standard_Real V) const 
{
  Handle(Geom_Circle) 
    GC = new Geom_Circle(ElSLib::SphereVIso(pos,radius,V));
  return GC;
}


//=======================================================================
//function : Volume
//purpose  : 
//=======================================================================

Standard_Real Geom_SphericalSurface::Volume () const {

   return (4.0 * M_PI * radius * radius * radius)/3.0;
}



//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_SphericalSurface::Transform (const Trsf& T) {

   radius = radius * Abs(T.ScaleFactor());
   pos.Transform (T);
}
