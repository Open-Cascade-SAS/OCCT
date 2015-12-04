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
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAbs_UVSense.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Plane,Geom_ElementarySurface)

typedef Geom_Plane         Plane;
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Ax3  Ax3;
typedef gp_Dir  Dir;
typedef gp_Lin  Lin;
typedef gp_Pln  Pln;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Plane::Copy () const {
 
  Handle(Geom_Plane) Pl = new Plane ( pos);
  return Pl;
}




//=======================================================================
//function : Geom_Plane
//purpose  : 
//=======================================================================

Geom_Plane::Geom_Plane (const gp_Ax3& A3)  {

  pos = A3;
}


//=======================================================================
//function : Geom_Plane
//purpose  : 
//=======================================================================

Geom_Plane::Geom_Plane (const gp_Pln& Pl)   {

  pos = Pl.Position(); 
}


//=======================================================================
//function : Geom_Plane
//purpose  : 
//=======================================================================

Geom_Plane::Geom_Plane (const Pnt& P, const Dir& V) {

  gp_Pln Pl (P, V);
  pos = Pl.Position();
}


//=======================================================================
//function : Geom_Plane
//purpose  : 
//=======================================================================

Geom_Plane::Geom_Plane ( const Standard_Real A, 
			 const Standard_Real B, 
			 const Standard_Real C, 
			 const Standard_Real D) {

  gp_Pln Pl (A, B, C, D);
  pos = Pl.Position ();
}


//=======================================================================
//function : SetPln
//purpose  : 
//=======================================================================

void Geom_Plane::SetPln (const gp_Pln& Pl)    
{ pos = Pl.Position(); }

//=======================================================================
//function : Pln
//purpose  : 
//=======================================================================

gp_Pln Geom_Plane::Pln () const { 

  return gp_Pln (Position()); 
}


//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_Plane::UReverse() {
  pos.XReverse();
}

//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Plane::UReversedParameter( const Standard_Real U) const {

  return (-U); 
}


//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_Plane::VReverse() {

  pos.YReverse();
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Plane::VReversedParameter( const Standard_Real V) const {

  return (-V); 
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_Plane::Transform (const Trsf& T)           
{ pos.Transform (T); }

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Plane::IsUClosed () const {

  return Standard_False; 
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Plane::IsVClosed () const {

  return Standard_False; 
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Plane::IsUPeriodic () const {

  return Standard_False; 
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Plane::IsVPeriodic () const {

  return Standard_False; 
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_Plane::Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const {
  
  U1 = -Precision::Infinite();  
  U2 = Precision::Infinite();  
  V1 = -Precision::Infinite(); 
  V2 = Precision::Infinite();
}


//=======================================================================
//function : Coefficients
//purpose  : 
//=======================================================================

void Geom_Plane::Coefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D) const {

   gp_Pln Pl (Position());
   Pl.Coefficients (A, B, C, D);
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_Plane::D0 (const Standard_Real U, const Standard_Real V, Pnt& P) const {
  
  P = ElSLib::PlaneValue (U, V, pos);
}



//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_Plane::D1 ( const Standard_Real U, const Standard_Real V, 
		            Pnt& P,       Vec& D1U, Vec& D1V) const 
{

  ElSLib::PlaneD1 (U, V, pos, P, D1U, D1V);   
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_Plane::D2 ( const Standard_Real U  , const Standard_Real V, 
		            Pnt& P  , 
		            Vec& D1U, Vec& D1V, 
		            Vec& D2U, Vec& D2V, Vec& D2UV) const 
{

  ElSLib::PlaneD1 (U, V, pos, P, D1U, D1V);
    D2U.SetCoord (0.0, 0.0, 0.0);
    D2V.SetCoord (0.0, 0.0, 0.0);
    D2UV.SetCoord (0.0, 0.0, 0.0);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_Plane::D3 ( const Standard_Real U, const Standard_Real V, 
		      Pnt& P, 
		      Vec& D1U, Vec& D1V, 
                      Vec& D2U, Vec& D2V, Vec& D2UV, 
                      Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV) const 
{
  ElSLib::PlaneD1 (U, V, pos, P, D1U, D1V);
   D2U.SetCoord (0.0, 0.0, 0.0);
   D2V.SetCoord (0.0, 0.0, 0.0);
   D2UV.SetCoord (0.0, 0.0, 0.0);
   D3U.SetCoord (0.0, 0.0, 0.0);
   D3V.SetCoord (0.0, 0.0, 0.0);
   D3UUV.SetCoord (0.0, 0.0, 0.0);
   D3UVV.SetCoord (0.0, 0.0, 0.0);
}



//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_Plane::DN ( const Standard_Real      , const Standard_Real      , 
                     const Standard_Integer Nu, const Standard_Integer Nv ) const {

   Standard_RangeError_Raise_if (Nu < 0 || Nv < 0 || Nu + Nv < 1, " ");
   if (Nu == 0 && Nv == 1) {
     return Vec (pos.YDirection());
   }
   else if (Nu == 1 && Nv == 0) {
     return Vec (pos.XDirection());
   }
   return Vec (0.0, 0.0, 0.0);
}


//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_Plane::UIso (const Standard_Real U) const 
{
  Handle(Geom_Line) GL = new Geom_Line (ElSLib::PlaneUIso(pos,U));
  return GL;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_Plane::VIso (const Standard_Real V) const 
{
  Handle(Geom_Line) GL = new Geom_Line (ElSLib::PlaneVIso(pos,V));
  return GL;
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_Plane::TransformParameters(Standard_Real& U,
				     Standard_Real& V,
				     const gp_Trsf& T) 
const
{
  if (!Precision::IsInfinite(U)) U *= Abs(T.ScaleFactor());
  if (!Precision::IsInfinite(V)) V *= Abs(T.ScaleFactor());
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_Plane::ParametricTransformation(const gp_Trsf& T) const
{
  gp_Trsf2d T2;
  T2.SetScale(gp::Origin2d(), Abs(T.ScaleFactor()));
  return gp_GTrsf2d(T2);
}
