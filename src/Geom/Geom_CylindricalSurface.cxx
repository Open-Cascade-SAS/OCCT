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
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <GeomAbs_UVSense.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_CylindricalSurface,Geom_ElementarySurface)

typedef Geom_CylindricalSurface         CylindricalSurface;

typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Ax3  Ax3;
typedef gp_Circ Circ;
typedef gp_Dir  Dir;
typedef gp_Lin  Lin;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_CylindricalSurface::Copy () const {

   Handle(Geom_CylindricalSurface) Cs;
   Cs = new CylindricalSurface (pos, radius);
   return Cs;
}


//=======================================================================
//function : Geom_CylindricalSurface
//purpose  : 
//=======================================================================

Geom_CylindricalSurface::Geom_CylindricalSurface (const gp_Cylinder& C) 
: radius (C.Radius()) {

  pos = C.Position (); 
}


//=======================================================================
//function : Geom_CylindricalSurface
//purpose  : 
//=======================================================================

Geom_CylindricalSurface::Geom_CylindricalSurface ( const Ax3& A3, 
						   const Standard_Real R) 
: radius (R) {

  if (R < 0.0) throw Standard_ConstructionError();
  pos = A3;
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_CylindricalSurface::UReversedParameter( const Standard_Real U) const
{
  return (2.*M_PI - U);
}

//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_CylindricalSurface::VReversedParameter( const Standard_Real V) const 
{
  return (-V);
}

//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real Geom_CylindricalSurface::Radius () const             { return radius; }

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_CylindricalSurface::IsUClosed () const       { return Standard_True; }

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_CylindricalSurface::IsVClosed () const       { return Standard_False; }

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_CylindricalSurface::IsUPeriodic () const     { return Standard_True; }

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_CylindricalSurface::IsVPeriodic () const     { return Standard_False; }

//=======================================================================
//function : SetCylinder
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::SetCylinder (const gp_Cylinder& C) {

   radius = C.Radius();
   pos = C.Position();
}


//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::SetRadius (const Standard_Real R) {

  if (R < 0.0) { throw Standard_ConstructionError(); }
  radius = R;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::Bounds (Standard_Real& U1, Standard_Real& U2, 
				      Standard_Real& V1, Standard_Real& V2) const {

   U1 = 0.0;  U2 = 2.0 * M_PI;
   V1 = - Precision::Infinite();   V2 = Precision::Infinite();
}


//=======================================================================
//function : Coefficients
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::Coefficients (Standard_Real& A1, Standard_Real& A2, Standard_Real& A3, 
					    Standard_Real& B1, Standard_Real& B2, Standard_Real& B3, 
					    Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, 
					    Standard_Real& D) const {
// Dans le repere local du cylindre :
// X**2 + Y**2 - radius = 0.0

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
  A1 = T11 * T11 + T21 * T21;
  A2 = T12 * T12 + T22 * T22;
  A3 = T13 * T13 + T23 * T23;
  B1 = T11 * T12 + T21 * T22;
  B2 = T11 * T13 + T21 * T23;
  B3 = T12 * T13 + T22 * T23;
  C1 = T11 * T14 + T21 * T24;
  C2 = T12 * T14 + T22 * T24;
  C3 = T13 * T14 + T23 * T24;
  D = T14 * T14 + T24 * T24 - radius * radius;  
}



//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder Geom_CylindricalSurface::Cylinder () const {
  
  return gp_Cylinder (pos, radius);
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::D0 (const Standard_Real U, 
				  const Standard_Real V,
				        Pnt& P ) const 
{
  ElSLib::CylinderD0 (U, V, pos, radius, P);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::D1 (const Standard_Real U, const Standard_Real V , 
				        Pnt& P, 
				        Vec& D1U,     Vec& D1V) const 
{
  ElSLib::CylinderD1 (U, V, pos, radius, P, D1U, D1V);
}



//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::D2 (const Standard_Real U, const Standard_Real V, 
				  Pnt& P,
				  Vec& D1U, Vec& D1V,
				  Vec& D2U, Vec& D2V, Vec& D2UV) const 
{
  ElSLib::CylinderD2 (U, V, pos, radius, P, D1U, D1V, D2U, D2V, D2UV);
}



//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::D3 (const Standard_Real U, const Standard_Real V,
				  Pnt& P, 
				  Vec& D1U, Vec& D1V, 
				  Vec& D2U, Vec& D2V, Vec& D2UV,
				  Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV) 
                                  const 
{
  ElSLib::CylinderD3 (U, V, pos, radius, P, D1U, D1V, D2U, D2V,
		      D2UV, D3U, D3V, D3UUV, D3UVV);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_CylindricalSurface::DN (const Standard_Real    U , const Standard_Real    V , 
				 const Standard_Integer Nu, const Standard_Integer Nv ) const 
{
  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv <0, " ");

  if (Nv > 1) { return Vec (0.0, 0.0, 0.0); }
  else {
    return ElSLib::CylinderDN (U, V, pos, radius, Nu, Nv);
  }
}




//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_CylindricalSurface::UIso (const Standard_Real U) const 
{
  Handle(Geom_Line) GL = new Geom_Line(ElSLib::CylinderUIso(pos,radius,U));
  return GL;
}



//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_CylindricalSurface::VIso (const Standard_Real V) const 
{
  Handle(Geom_Circle) GC = new Geom_Circle(ElSLib::CylinderVIso(pos,radius,V));
  return GC;
}



//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::Transform (const Trsf& T) {

   radius = radius * Abs(T.ScaleFactor());
   pos.Transform (T);
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_CylindricalSurface::TransformParameters(Standard_Real& ,
						  Standard_Real& V,
						  const gp_Trsf& T) 
const
{
  if (!Precision::IsInfinite(V)) V *= Abs(T.ScaleFactor());
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_CylindricalSurface::ParametricTransformation(const gp_Trsf& T)
const
{
  gp_GTrsf2d T2;
  gp_Ax2d Axis(gp::Origin2d(),gp::DX2d());
  T2.SetAffinity(Axis, Abs(T.ScaleFactor()));
  return T2;
}
