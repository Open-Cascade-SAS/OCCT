// Created on: 1993-03-24
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




#include <Geom2d_Circle.ixx>
#include <ElCLib.hxx>
#include <gp_XY.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_ConstructionError.hxx> 


typedef Geom2d_Circle         Circle;
typedef Handle(Geom2d_Circle) Handle(Circle);
typedef gp_Ax2d   Ax2d;
typedef gp_Dir2d  Dir2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Trsf2d Trsf2d;
typedef gp_Vec2d  Vec2d;
typedef gp_XY     XY;








//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom2d_Geometry) Geom2d_Circle::Copy() const 
{
  Handle(Circle) C;
  C = new Circle (pos, radius);
  return C;
}


//=======================================================================
//function : Geom2d_Circle
//purpose  : 
//=======================================================================

Geom2d_Circle::Geom2d_Circle (const gp_Circ2d& C) : radius (C.Radius()) { 

  pos = C.Axis();
}


//=======================================================================
//function : Geom2d_Circle
//purpose  : 
//=======================================================================

Geom2d_Circle::Geom2d_Circle (const Ax2d& A, const Standard_Real Radius,
			      const Standard_Boolean Sense)
: radius(Radius) {
  
   if (Radius < 0.0) { Standard_ConstructionError::Raise(); }
   pos = gp_Ax22d(A, Sense);
}

//=======================================================================
//function : Geom2d_Circle
//purpose  : 
//=======================================================================

Geom2d_Circle::Geom2d_Circle (const gp_Ax22d& A, const Standard_Real Radius)

: radius (Radius) {

   if (Radius < 0.0) { Standard_ConstructionError::Raise(); }
   pos = A;
}

//=======================================================================
//function : SetCirc2d
//purpose  : 
//=======================================================================

void Geom2d_Circle::SetCirc2d (const gp_Circ2d& C) {

   radius = C.Radius();
   pos    = C.Axis();
}


//=======================================================================
//function : SetRadius
//purpose  : 
//=======================================================================

void Geom2d_Circle::SetRadius (const Standard_Real R) 
{ 
  if (R < 0.0) { Standard_ConstructionError::Raise(); }
  radius = R;
}

//=======================================================================
//function : Radius
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Circle::Radius () const
{ 
  return radius;
}

//=======================================================================
//function : Circ2d
//purpose  : 
//=======================================================================

gp_Circ2d Geom2d_Circle::Circ2d () const    
{
  return gp_Circ2d (pos, radius); 
}

//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Circle::ReversedParameter( const Standard_Real U) const 
{
  return (2. * M_PI - U);
}

//=======================================================================
//function : Eccentricity
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Circle::Eccentricity () const       
{
  return 0.0; 
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Circle::FirstParameter () const     
{
  return 0.0; 
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Circle::LastParameter () const      
{
  return 2.0 * M_PI; 
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Circle::IsClosed () const        
{
  return Standard_True; 
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Circle::IsPeriodic () const      
{
  return Standard_True; 
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom2d_Circle::D0 (const Standard_Real   U, 
			      Pnt2d& P) const 
{
  P= ElCLib::CircleValue (U, pos, radius);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom2d_Circle::D1 (const Standard_Real U, Pnt2d& P, Vec2d& V1) const 
{
  ElCLib::CircleD1 (U, pos, radius, P, V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom2d_Circle::D2 (const Standard_Real U, 
			      Pnt2d& P, 
			      Vec2d& V1, Vec2d& V2) const 
{
  ElCLib::CircleD2 (U, pos, radius, P, V1, V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom2d_Circle::D3 (const Standard_Real U, 
			      Pnt2d& P, 
			      Vec2d& V1, Vec2d& V2, Vec2d& V3) const 
{
  ElCLib::CircleD3 (U, pos, radius, P, V1, V2, V3);
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec2d Geom2d_Circle::DN (const Standard_Real U, const Standard_Integer N) const 
{
  Standard_RangeError_Raise_if (N < 1," ");
  return ElCLib::CircleDN (U, pos, radius, N);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom2d_Circle::Transform (const Trsf2d& T) 
{
   radius = radius * Abs(T.ScaleFactor());
   pos.Transform (T);
}
