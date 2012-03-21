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



#include <Geom2d_CartesianPoint.ixx>

typedef Geom2d_CartesianPoint         CartesianPoint;
typedef Handle(Geom2d_CartesianPoint) Handle(CartesianPoint);
typedef gp_Ax2d   Ax2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;

Geom2d_CartesianPoint::Geom2d_CartesianPoint (const gp_Pnt2d& P) : gpPnt2d (P)
{}


Geom2d_CartesianPoint::Geom2d_CartesianPoint (const Standard_Real X, const Standard_Real Y)  
: gpPnt2d (X , Y) { }



Handle(Geom2d_Geometry) Geom2d_CartesianPoint::Copy() const {

  Handle(CartesianPoint) P;
  P = new CartesianPoint (gpPnt2d);
  return P; 
}


void Geom2d_CartesianPoint::SetCoord (const Standard_Real X, const Standard_Real Y) {

  gpPnt2d.SetCoord (X, Y);
}

void Geom2d_CartesianPoint::Coord (Standard_Real& X, Standard_Real& Y) const {

   gpPnt2d.Coord (X, Y);
}


void Geom2d_CartesianPoint::SetPnt2d (const gp_Pnt2d& P) {  gpPnt2d = P; }

void Geom2d_CartesianPoint::SetX (const Standard_Real X)   { gpPnt2d.SetX (X); }

void Geom2d_CartesianPoint::SetY (const Standard_Real Y)   { gpPnt2d.SetY (Y); }

gp_Pnt2d Geom2d_CartesianPoint::Pnt2d () const    { return gpPnt2d; }

Standard_Real Geom2d_CartesianPoint::X () const    { return gpPnt2d.X(); }

Standard_Real Geom2d_CartesianPoint::Y () const    { return gpPnt2d.Y(); }

void Geom2d_CartesianPoint::Transform (const Trsf2d& T) { 

  gpPnt2d.Transform (T);
}
