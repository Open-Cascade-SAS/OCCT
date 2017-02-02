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
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <GeomAbs_UVSense.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
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

IMPLEMENT_STANDARD_RTTIEXT(Geom_ConicalSurface,Geom_ElementarySurface)

typedef Geom_ConicalSurface         ConicalSurface;
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

Handle(Geom_Geometry) Geom_ConicalSurface::Copy () const {
 
   Handle(Geom_ConicalSurface) Cs;
   Cs = new ConicalSurface (pos, semiAngle, radius);
   return Cs;
}

//=======================================================================
//function : Geom_ConicalSurface
//purpose  : 
//=======================================================================

Geom_ConicalSurface::Geom_ConicalSurface ( const Ax3& A3 , 
					   const Standard_Real Ang, 
					   const Standard_Real R) :
       radius(R), semiAngle (Ang) 
{

  if (R < 0.0 || Abs(Ang) <= gp::Resolution() || Abs(Ang) >= M_PI/2.0 - gp::Resolution()) 
    throw Standard_ConstructionError();
  
  pos = A3;
}


//=======================================================================
//function : Geom_ConicalSurface
//purpose  : 
//=======================================================================

Geom_ConicalSurface::Geom_ConicalSurface ( const gp_Cone& C ) 
: radius (C.RefRadius()), semiAngle (C.SemiAngle()) 
{
   pos = C.Position();
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_ConicalSurface::UReversedParameter( const Standard_Real U) const
{
  return ( 2.*M_PI - U);
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_ConicalSurface::VReversedParameter( const Standard_Real V) const
{
  return ( -V);
}

//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::VReverse()
{
  semiAngle = -semiAngle;
  pos.ZReverse();
}

//=======================================================================
//function : RefRadius
//purpose  : 
//=======================================================================

Standard_Real Geom_ConicalSurface::RefRadius () const               
{ return radius; }

//=======================================================================
//function : SemiAngle
//purpose  : 
//=======================================================================

Standard_Real Geom_ConicalSurface::SemiAngle () const               
{ return semiAngle;}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ConicalSurface::IsUClosed () const            
{ return Standard_True; }

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ConicalSurface::IsVClosed () const            
{ return Standard_False; }

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ConicalSurface::IsUPeriodic () const          
{ return Standard_True; }

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_ConicalSurface::IsVPeriodic () const          
{ return Standard_False; }

//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone Geom_ConicalSurface::Cone () const {

  return gp_Cone (pos, semiAngle, radius);
}


//=======================================================================
//function : SetCone
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::SetCone (const gp_Cone& C) {

  radius = C.RefRadius ();
  semiAngle   = C.SemiAngle ();
  pos    = C.Position  ();
}


//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::SetRadius (const Standard_Real R) {

  if (R < 0.0)  throw Standard_ConstructionError();
  radius = R;
}


//=======================================================================
//function : SetSemiAngle
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::SetSemiAngle (const Standard_Real Ang) {

  if (Abs(Ang) <= gp::Resolution() || Abs(Ang) >= M_PI/2.0 - gp::Resolution()) {
    throw Standard_ConstructionError();
  }
  semiAngle = Ang;
}


//=======================================================================
//function : Apex
//purpose  : 
//=======================================================================

Pnt Geom_ConicalSurface::Apex () const 
{

   XYZ Coord = Position().Direction().XYZ();
   Coord.Multiply (-radius / Tan (semiAngle));
   Coord.Add      (Position().Location().XYZ());
   return Pnt     (Coord);
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::Bounds (Standard_Real& U1, Standard_Real& U2, 
				  Standard_Real& V1, Standard_Real& V2) const {

   U1 = 0.0;  U2 = 2.0 * M_PI;  
   V1 = -Precision::Infinite();  V2 = Precision::Infinite();
}


//=======================================================================
//function : Coefficients
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::Coefficients (Standard_Real& A1, Standard_Real& A2, Standard_Real& A3,
					Standard_Real& B1, Standard_Real& B2, Standard_Real& B3,
					Standard_Real& C1, Standard_Real& C2, Standard_Real& C3, 
					Standard_Real& D)  const 
{
   // Dans le repere du cone :
   // X**2 + Y**2 - (Myradius - Z * Tan(semiAngle))**2 = 0.0

      Trsf T;
      T.SetTransformation (pos);
      Standard_Real KAng = Tan (semiAngle);
      Standard_Real T11 = T.Value (1, 1);
      Standard_Real T12 = T.Value (1, 2);
      Standard_Real T13 = T.Value (1, 3);
      Standard_Real T14 = T.Value (1, 4);
      Standard_Real T21 = T.Value (2, 1);
      Standard_Real T22 = T.Value (2, 2);
      Standard_Real T23 = T.Value (2, 3);
      Standard_Real T24 = T.Value (2, 4);
      Standard_Real T31 = T.Value (3, 1) * KAng;
      Standard_Real T32 = T.Value (3, 2) * KAng;
      Standard_Real T33 = T.Value (3, 3) * KAng;
      Standard_Real T34 = T.Value (3, 4) * KAng;
      A1 = T11 * T11 + T21 * T21 - T31 * T31;
      A2 = T12 * T12 + T22 * T22 - T32 * T32;
      A3 = T13 * T13 + T23 * T23 - T33 * T33;
      B1 = T11 * T12 + T21 * T22 - T31 * T32;
      B2 = T11 * T13 + T21 * T23 - T31 * T33;
      B3 = T12 * T13 + T22 * T23 - T32 * T33;
      C1 = T11 * T14 + T21 * T24 + radius * T31;
      C2 = T12 * T14 + T22 * T24 + radius * T32;
      C3 = T13 * T14 + T23 * T24 + radius * T33;
      D = T14 * T14 + T24 * T24 - radius * radius - T34 * T34 +
          2.0 * radius * T34;  
}



//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::D0 (const Standard_Real U, const Standard_Real V, Pnt& P) const 
{

  P = ElSLib::ConeValue (U, V, pos, radius, semiAngle);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::D1 (const Standard_Real U  , const Standard_Real V, 
			            Pnt& P  , 
			            Vec& D1U, Vec& D1V     ) const 
{
  ElSLib::ConeD1 (U, V, pos, radius, semiAngle, P, D1U, D1V);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::D2 ( const Standard_Real U  , const Standard_Real V, 
                                     Pnt& P  ,  
			             Vec& D1U, Vec& D1V, 
			             Vec& D2U, Vec& D2V, Vec& D2UV) const 
{
  ElSLib::ConeD2 (U, V, pos, radius, semiAngle, P, D1U, D1V, 
		  D2U, D2V, D2UV);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::D3 (const Standard_Real U, const Standard_Real V, 
			      Pnt& P      , 
			      Vec& D1U    , Vec& D1V,
			      Vec& D2U    , Vec& D2V, Vec& D2UV,
			      Vec& D3U    , Vec& D3V, Vec& D3UUV, Vec& D3UVV
			      ) const
{
  ElSLib::ConeD3 (U, V, pos, radius, semiAngle, P, D1U, D1V, D2U, D2V,
		  D2UV, D3U, D3V, D3UUV, D3UVV);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_ConicalSurface::DN (const Standard_Real    U , const Standard_Real     V, 
			     const Standard_Integer Nu, const Standard_Integer Nv ) const
{
  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv < 0, " ");
  if (Nv > 1) { return Vec (0.0, 0.0, 0.0); }
  else {
    return ElSLib::ConeDN (U, V, pos, radius, semiAngle, Nu, Nv);
  }
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_ConicalSurface::UIso (const Standard_Real U) const 
{
  Handle(Geom_Line) 
    GL = new Geom_Line(ElSLib::ConeUIso(pos,radius,semiAngle,U));
  return GL;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_ConicalSurface::VIso (const Standard_Real V) const 
{
  Handle(Geom_Circle) 
    GC = new Geom_Circle(ElSLib::ConeVIso(pos,radius,semiAngle,V));
  return GC;
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::Transform (const Trsf& T) 
{
  radius = radius * Abs(T.ScaleFactor());
  pos.Transform (T);
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_ConicalSurface::TransformParameters(Standard_Real& ,
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

gp_GTrsf2d Geom_ConicalSurface::ParametricTransformation(const gp_Trsf& T)
const
{
  gp_GTrsf2d T2;
  gp_Ax2d Axis(gp::Origin2d(),gp::DX2d());
  T2.SetAffinity(Axis, Abs(T.ScaleFactor()));
  return T2;
}

