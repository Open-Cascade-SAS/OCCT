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



#include <Geom2d_VectorWithMagnitude.ixx>


typedef Geom2d_Vector Vector;
typedef Geom2d_VectorWithMagnitude         VectorWithMagnitude;
typedef Handle(Geom2d_VectorWithMagnitude) Handle(VectorWithMagnitude);
typedef Handle(Geom2d_Vector)              Handle(Vector);

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

  Handle(VectorWithMagnitude) V;
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


void Geom2d_VectorWithMagnitude::Add (const Handle(Vector)& Other) { 

  gpVec2d.Add (Other->Vec2d());
}


Handle(VectorWithMagnitude) Geom2d_VectorWithMagnitude::Added (

const Handle(Vector)& Other) const { 
     
 gp_Vec2d Temp = Other->Vec2d();
 Temp.Add (gpVec2d);  
 return new VectorWithMagnitude (Temp);
}


Standard_Real Geom2d_VectorWithMagnitude::Crossed (const Handle(Vector)& Other) const{ 

  return gpVec2d.Crossed (Other->Vec2d());
}


void Geom2d_VectorWithMagnitude::Divide (const Standard_Real Scalar) { 

   gpVec2d.Divide (Scalar);
}


Handle(VectorWithMagnitude) Geom2d_VectorWithMagnitude::Divided (
const Standard_Real Scalar) const { 

  gp_Vec2d V (gpVec2d);
  V.Divide (Scalar);
  return new VectorWithMagnitude (V);
}


Handle(VectorWithMagnitude) Geom2d_VectorWithMagnitude::Multiplied (
const Standard_Real Scalar) const { 

  gp_Vec2d V(gpVec2d);
  V.Multiply (Scalar);
  return new VectorWithMagnitude (V);
}


void Geom2d_VectorWithMagnitude::Multiply (const Standard_Real Scalar) { 

   gpVec2d.Multiply (Scalar);
}


void Geom2d_VectorWithMagnitude::Normalize () { gpVec2d.Normalize (); }


Handle(VectorWithMagnitude) Geom2d_VectorWithMagnitude::Normalized () const { 

   gp_Vec2d V = gpVec2d;
   V.Normalized ();
   return new VectorWithMagnitude (V);
}


void Geom2d_VectorWithMagnitude::Subtract (const Handle(Vector)& Other) { 

  gpVec2d.Subtract (Other->Vec2d());
}


Handle(VectorWithMagnitude) Geom2d_VectorWithMagnitude::Subtracted (
const Handle(Vector)& Other) const { 

  gp_Vec2d V = gpVec2d;
  V.Subtract (Other->Vec2d());
  return new VectorWithMagnitude (V);
}



void Geom2d_VectorWithMagnitude::Transform (const Trsf2d& T) { 

  gpVec2d.Transform (T);
}
