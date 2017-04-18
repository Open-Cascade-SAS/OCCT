// Created on: 1993-03-10
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


#include <Geom_Direction.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Vector.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Direction,Geom_Vector)

typedef Geom_Direction         Direction;
typedef Geom_Vector            Vector;
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Direction::Copy() const {

  Handle(Geom_Direction) D;
  D = new Direction (gpVec);
  return D; 
}

//=======================================================================
//function : Geom_Direction
//purpose  : 
//=======================================================================

Geom_Direction::Geom_Direction (const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

   Standard_Real D = sqrt (X * X + Y * Y + Z * Z);
   Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                        "Geom_Direction() - input vector has zero length");
   gpVec = gp_Vec (X/D, Y/D, Z/D);
}

Geom_Direction::Geom_Direction (const gp_Dir& V)  { gpVec = V; }

void Geom_Direction::SetDir (const gp_Dir& V)     { gpVec = V; }

gp_Dir Geom_Direction::Dir () const               {  return gpVec; }

Standard_Real Geom_Direction::Magnitude () const           { return 1.0; }

Standard_Real Geom_Direction::SquareMagnitude () const     { return 1.0; }

void Geom_Direction::SetCoord (const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  Standard_Real D = Sqrt (X * X + Y * Y + Z * Z);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom_Direction::SetCoord() - input vector has zero length");
  gpVec = gp_Vec(X/D, Y/D, Z/D);
}


void Geom_Direction::SetX (const Standard_Real X) {

  Standard_Real D = Sqrt (X * X + gpVec.Y() * gpVec.Y() + gpVec.Z() * gpVec.Z());
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom_Direction::SetX() - input vector has zero length");
  gpVec = gp_Vec (X/D, gpVec.Y()/D, gpVec.Z()/D);
}


void Geom_Direction::SetY (const Standard_Real Y) {

  Standard_Real D = Sqrt (gpVec.X() * gpVec.X() + Y * Y + gpVec.Z() * gpVec.Z());
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom_Direction::SetY() - input vector has zero length");
  gpVec = gp_Vec (gpVec.X()/D, Y/D, gpVec.Z()/D);
}


void Geom_Direction::SetZ (const Standard_Real Z) {

  Standard_Real D = Sqrt (gpVec.X() * gpVec.X() + gpVec.Y() * gpVec.Y() + Z * Z);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(),
                                       "Geom_Direction::SetZ() - input vector has zero length");
  gpVec = gp_Vec (gpVec.X()/D, gpVec.Y()/D, Z/D);
}


void Geom_Direction::Cross (const Handle(Geom_Vector)& Other) {

  gp_Dir V (gpVec.Crossed(Other->Vec()));
  gpVec = V;
}


void Geom_Direction::CrossCross (
const Handle(Geom_Vector)& V1, const Handle(Geom_Vector)& V2) {

  gp_Dir V (gpVec.CrossCrossed (V1->Vec(), V2->Vec()));
  gpVec = V;
}


Handle(Geom_Vector) Geom_Direction::Crossed (const Handle(Geom_Vector)& Other)
const {

   gp_Dir V (gpVec.Crossed (Other->Vec()));
   return new Direction (V);
}


Handle(Geom_Vector) Geom_Direction::CrossCrossed (
const Handle(Geom_Vector)& V1, const Handle(Geom_Vector)& V2) const {

  gp_Dir V (gpVec.CrossCrossed (V1->Vec(), V2->Vec()));
  return new Direction (V);
}


void Geom_Direction::Transform (const gp_Trsf& T) { 

  gp_Dir V (gpVec);
  V.Transform (T);
  gpVec = V;
}
