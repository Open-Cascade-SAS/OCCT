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



#include <Geom2d_Vector.ixx>

typedef Geom2d_Vector         Vector;
typedef Handle(Geom2d_Vector) Handle(Vector);
typedef gp_Ax2d  Ax2d;
typedef gp_Pnt2d Pnt2d;



Standard_Real Geom2d_Vector::Angle (
const Handle(Geom2d_Vector)& Other) const { 

  return gpVec2d.Angle (Other->Vec2d());
}


void Geom2d_Vector::Reverse () { gpVec2d.Reverse(); }

gp_Vec2d Geom2d_Vector::Vec2d () const      { return gpVec2d; }

Standard_Real Geom2d_Vector::X () const              { return gpVec2d.X(); }

Standard_Real Geom2d_Vector::Y () const              { return gpVec2d.Y(); }


Handle(Vector) Geom2d_Vector::Reversed () const {

  Handle(Vector) V = Handle(Vector)::DownCast(Copy());
  V->Reverse();
  return V;
}


void Geom2d_Vector::Coord (Standard_Real& X, Standard_Real& Y) const {

  gpVec2d.Coord (X, Y);
}
 

Standard_Real Geom2d_Vector::Dot (const Handle(Vector)& Other) const  { 

  return gpVec2d.Dot (Other->Vec2d());
}
