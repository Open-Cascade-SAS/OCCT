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


#include <Geom2d_Geometry.hxx>
#include <Geom2d_Vector.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_VectorWithMagnitude,Geom2d_Vector)

typedef Geom2d_Vector Vector;
typedef Geom2d_VectorWithMagnitude         VectorWithMagnitude;
typedef gp_Ax2d   Ax2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Trsf2d Trsf2d;

Geom2d_VectorWithMagnitude::Geom2d_VectorWithMagnitude (const gp_Vec2d& V) 
{ gpVec2d = V; }


Geom2d_VectorWithMagnitude::Geom2d_VectorWithMagnitude (
const Standard_Real X,  const Standard_Real Y) { gpVec2d = gp_Vec2d (X, Y); }


Geom2d_VectorWithMagnitude::Geom2d_VectorWithMagnitude (
const Pnt2d& P1, const Pnt2d& P2) { gpVec2d = gp_Vec2d (P1, P2); }


Handle(Geom2d_Geometry) Geom2d_VectorWithMagnitude::Copy() const {

  Handle(Geom2d_VectorWithMagnitude) V;
  V = new VectorWithMagnitude (gpVec2d);
  return V; 
}


void Geom2d_VectorWithMagnitude::SetCoord (const Standard_Real X, const Standard_Real Y) {

  gpVec2d = gp_Vec2d (X, Y);
}


void Geom2d_VectorWithMagnitude::SetVec2d (const gp_Vec2d& V) { gpVec2d = V; }

void Geom2d_VectorWithMagnitude::SetX (const Standard_Real X) { gpVec2d.SetX (X); }


void Geom2d_VectorWithMagnitude::SetY (const Standard_Real Y) { gpVec2d.SetY (Y); }


Standard_Real Geom2d_VectorWithMagnitude::Magnitude () const { 
  
  return gpVec2d.Magnitude ();
}


Standard_Real Geom2d_VectorWithMagnitude::SquareMagnitude () const { 

  return gpVec2d.SquareMagnitude ();
}


void Geom2d_VectorWithMagnitude::Add (const Handle(Geom2d_Vector)& Other) { 

  gpVec2d.Add (Other->Vec2d());
}


Handle(Geom2d_VectorWithMagnitude) Geom2d_VectorWithMagnitude::Added (

const Handle(Geom2d_Vector)& Other) const { 
     
 gp_Vec2d Temp = Other->Vec2d();
 Temp.Add (gpVec2d);  
 return new VectorWithMagnitude (Temp);
}


Standard_Real Geom2d_VectorWithMagnitude::Crossed (const Handle(Geom2d_Vector)& Other) const{ 

  return gpVec2d.Crossed (Other->Vec2d());
}


void Geom2d_VectorWithMagnitude::Divide (const Standard_Real Scalar) { 

   gpVec2d.Divide (Scalar);
}


Handle(Geom2d_VectorWithMagnitude) Geom2d_VectorWithMagnitude::Divided (
const Standard_Real Scalar) const { 

  gp_Vec2d V (gpVec2d);
  V.Divide (Scalar);
  return new VectorWithMagnitude (V);
}


Handle(Geom2d_VectorWithMagnitude) Geom2d_VectorWithMagnitude::Multiplied (
const Standard_Real Scalar) const { 

  gp_Vec2d V(gpVec2d);
  V.Multiply (Scalar);
  return new VectorWithMagnitude (V);
}


void Geom2d_VectorWithMagnitude::Multiply (const Standard_Real Scalar) { 

   gpVec2d.Multiply (Scalar);
}


void Geom2d_VectorWithMagnitude::Normalize () { gpVec2d.Normalize (); }


Handle(Geom2d_VectorWithMagnitude) Geom2d_VectorWithMagnitude::Normalized () const { 

   gp_Vec2d V = gpVec2d;
   V.Normalized ();
   return new VectorWithMagnitude (V);
}


void Geom2d_VectorWithMagnitude::Subtract (const Handle(Geom2d_Vector)& Other) { 

  gpVec2d.Subtract (Other->Vec2d());
}


Handle(Geom2d_VectorWithMagnitude) Geom2d_VectorWithMagnitude::Subtracted (
const Handle(Geom2d_Vector)& Other) const { 

  gp_Vec2d V = gpVec2d;
  V.Subtract (Other->Vec2d());
  return new VectorWithMagnitude (V);
}



void Geom2d_VectorWithMagnitude::Transform (const Trsf2d& T) { 

  gpVec2d.Transform (T);
}
