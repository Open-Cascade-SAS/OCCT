// File:	Geom2d_CartesianPoint.cxx
// Created:	Wed Mar 24 19:19:34 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_CartesianPoint.cxx, JCV 23/06/91

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
