// File:	Geom2d_Vector.cxx
// Created:	Wed Mar 24 19:31:58 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_Vector.cxx, JCV 23/06/91

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
