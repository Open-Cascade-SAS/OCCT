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



#include <Geom2d_Direction.ixx>
#include <gp.hxx>
#include <Standard_ConstructionError.hxx>

typedef Geom2d_Direction         Direction;
typedef Handle(Geom2d_Direction) Handle(Direction);
typedef Handle(Geom2d_Vector)    Handle(Vector);
typedef gp_Ax2d   Ax2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Trsf2d Trsf2d;





Handle(Geom2d_Geometry) Geom2d_Direction::Copy() const {

  Handle(Direction) D;
  D = new Direction (gpVec2d);
  return D; 
}






Geom2d_Direction::Geom2d_Direction (const Standard_Real X, const Standard_Real Y) {

  Standard_Real D = Sqrt (X * X + Y * Y);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec2d = gp_Vec2d (X/D, Y/D);
}


Geom2d_Direction::Geom2d_Direction (const gp_Dir2d& V) { gpVec2d = V; }


void Geom2d_Direction::SetCoord (const Standard_Real X, const Standard_Real Y) {

  Standard_Real D = Sqrt (X * X + Y * Y);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec2d = gp_Vec2d (X/D, Y/D);
}


void Geom2d_Direction::SetDir2d (const gp_Dir2d& V) { gpVec2d = V; }


void Geom2d_Direction::SetX (const Standard_Real X) {

  Standard_Real D = Sqrt(X * X + gpVec2d.Y() * gpVec2d.Y());
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec2d = gp_Vec2d (X/D, gpVec2d.Y()/D);
}


void Geom2d_Direction::SetY (const Standard_Real Y) {

  Standard_Real D = Sqrt (gpVec2d.X() * gpVec2d.X() + Y * Y);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec2d = gp_Vec2d (gpVec2d.X()/D, Y/D);
}


gp_Dir2d Geom2d_Direction::Dir2d () const {  return gpVec2d; }


Standard_Real Geom2d_Direction::Magnitude () const { return 1.0; }


Standard_Real Geom2d_Direction::SquareMagnitude () const { return 1.0; }


Standard_Real Geom2d_Direction::Crossed (const Handle(Vector)& Other) const {

   return gpVec2d.Crossed (Other->Vec2d());
}


void Geom2d_Direction::Transform (const gp_Trsf2d& T) { 

  gp_Dir2d dir = gpVec2d;
  dir.Transform (T);
  gpVec2d = dir;
}
