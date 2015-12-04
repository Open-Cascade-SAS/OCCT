// Created on: 1993-03-24
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


#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Geometry.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_CartesianPoint,Geom2d_Point)

typedef Geom2d_CartesianPoint         CartesianPoint;
typedef gp_Ax2d   Ax2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;

Geom2d_CartesianPoint::Geom2d_CartesianPoint (const gp_Pnt2d& P) : gpPnt2d (P)
{}


Geom2d_CartesianPoint::Geom2d_CartesianPoint (const Standard_Real X, const Standard_Real Y)  
: gpPnt2d (X , Y) { }



Handle(Geom2d_Geometry) Geom2d_CartesianPoint::Copy() const {

  Handle(Geom2d_CartesianPoint) P;
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
