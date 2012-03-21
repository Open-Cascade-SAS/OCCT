// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Geom_Circle.ixx>
#include <gp_XYZ.hxx>
#include <ElCLib.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ConstructionError.hxx> 

typedef Geom_Circle         Circle;
typedef Handle(Geom_Circle) Handle(Circle);
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;




//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Circle::Copy() const {

  Handle(Circle) C;
  C = new Circle (pos, radius);
  return C;
}


//=======================================================================
//function : Geom_Circle
//purpose  : 
//=======================================================================

Geom_Circle::Geom_Circle (const gp_Circ& C) : radius (C.Radius()) {  

  pos = C.Position(); 
}


//=======================================================================
//function : Geom_Circle
//purpose  : 
//=======================================================================

Geom_Circle::Geom_Circle (const Ax2& A2, const Standard_Real R) : radius (R) {

  if (R < 0.0) Standard_ConstructionError::Raise();
  pos = A2;
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Circle::IsClosed () const        { return Standard_True; }


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_Circle::IsPeriodic () const      { return Standard_True; }


//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Circle::ReversedParameter( const Standard_Real U) const 
{
  return ( 2. * M_PI - U);
}

//=======================================================================
//function : Eccentricity
//purpose  : 
//=======================================================================

Standard_Real Geom_Circle::Eccentricity () const       { return 0.0; }


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Circle::FirstParameter () const     { return 0.0; }


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_Circle::LastParameter () const      { return 2.0 * M_PI; }


//=======================================================================
//function : Circ
//purpose  : 
//=======================================================================

gp_Circ Geom_Circle::Circ () const  { return gp_Circ (pos, radius); }


//=======================================================================
//function : SetCirc
//purpose  : 
//=======================================================================

void Geom_Circle::SetCirc (const gp_Circ& C) {

   radius = C.Radius();
   pos = C.Position();
}


//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void Geom_Circle::SetRadius (const Standard_Real R) { 

   if (R < 0.0)  Standard_ConstructionError::Raise();
   radius = R;
}

//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real Geom_Circle::Radius() const
{
  return radius;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_Circle::D0 (const Standard_Real U, Pnt& P) const {

  P = ElCLib::CircleValue (U, pos, radius);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_Circle::D1 (const Standard_Real U, Pnt& P, Vec& V1) const {

  ElCLib::CircleD1 (U, pos, radius, P, V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_Circle::D2 (const Standard_Real U, Pnt& P, Vec& V1, Vec& V2) const {

   ElCLib::CircleD2 (U, pos, radius, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_Circle::D3 (
const Standard_Real U, Pnt& P, Vec& V1, Vec& V2, Vec& V3) const {

  ElCLib::CircleD3 (U, pos, radius, P, V1, V2, V3);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_Circle::DN (const Standard_Real U, const Standard_Integer N) const {

   Standard_RangeError_Raise_if (N < 1, " ");
   return ElCLib::CircleDN (U, pos, radius, N);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_Circle::Transform (const Trsf& T) {

   radius = radius * Abs(T.ScaleFactor());
   pos.Transform (T);
}
