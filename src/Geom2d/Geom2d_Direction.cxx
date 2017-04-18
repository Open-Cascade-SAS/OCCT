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


#include <Geom2d_Direction.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_Vector.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Direction,Geom2d_Vector)

typedef Geom2d_Direction         Direction;
typedef gp_Ax2d   Ax2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Trsf2d Trsf2d;





Handle(Geom2d_Geometry) Geom2d_Direction::Copy() const {

  Handle(Geom2d_Direction) D;
  D = new Direction (gpVec2d);
  return D; 
}






Geom2d_Direction::Geom2d_Direction (const Standard_Real X, const Standard_Real Y) {

  Standard_Real D = Sqrt (X * X + Y * Y);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom2d_Direction() - input vector has zero length");
  gpVec2d = gp_Vec2d (X/D, Y/D);
}


Geom2d_Direction::Geom2d_Direction (const gp_Dir2d& V) { gpVec2d = V; }


void Geom2d_Direction::SetCoord (const Standard_Real X, const Standard_Real Y) {

  Standard_Real D = Sqrt (X * X + Y * Y);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom2d_Direction::SetCoord() - input vector has zero length");
  gpVec2d = gp_Vec2d (X/D, Y/D);
}


void Geom2d_Direction::SetDir2d (const gp_Dir2d& V) { gpVec2d = V; }


void Geom2d_Direction::SetX (const Standard_Real X) {

  Standard_Real D = Sqrt(X * X + gpVec2d.Y() * gpVec2d.Y());
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom2d_Direction::SetX() - input vector has zero length");
  gpVec2d = gp_Vec2d (X/D, gpVec2d.Y()/D);
}


void Geom2d_Direction::SetY (const Standard_Real Y) {

  Standard_Real D = Sqrt (gpVec2d.X() * gpVec2d.X() + Y * Y);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom2d_Direction::SetY() - input vector has zero length");
  gpVec2d = gp_Vec2d (gpVec2d.X()/D, Y/D);
}


gp_Dir2d Geom2d_Direction::Dir2d () const {  return gpVec2d; }


Standard_Real Geom2d_Direction::Magnitude () const { return 1.0; }


Standard_Real Geom2d_Direction::SquareMagnitude () const { return 1.0; }


Standard_Real Geom2d_Direction::Crossed (const Handle(Geom2d_Vector)& Other) const {

   return gpVec2d.Crossed (Other->Vec2d());
}


void Geom2d_Direction::Transform (const gp_Trsf2d& T) { 

  gp_Dir2d dir = gpVec2d;
  dir.Transform (T);
  gpVec2d = dir;
}
