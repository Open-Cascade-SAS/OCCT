// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Geom_Vector.ixx>

typedef Geom_Vector         Vector;
typedef Handle(Geom_Vector) Handle(Vector);
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;




void Geom_Vector::Reverse ()                  { gpVec.Reverse(); }

Standard_Real Geom_Vector::X () const                  { return gpVec.X(); }

Standard_Real Geom_Vector::Y () const                  { return gpVec.Y(); }

Standard_Real Geom_Vector::Z () const                  { return gpVec.Z(); }

const gp_Vec& Geom_Vector::Vec () const              { return gpVec; }

Handle(Vector) Geom_Vector::Reversed () const
{
  Handle(Vector) V = Handle(Vector)::DownCast(Copy());
  V->Reverse();
  return V;
}


Standard_Real Geom_Vector::Angle (const Handle(Vector)& Other) const { 

  return gpVec.Angle (Other->Vec());
}


Standard_Real Geom_Vector::AngleWithRef (
const Handle(Vector)& Other, const Handle(Vector)& VRef) const {

   return gpVec.AngleWithRef (Other->Vec(), VRef->Vec());
}


void Geom_Vector::Coord (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

   gpVec.Coord (X, Y, Z);
}
 

Standard_Real Geom_Vector::Dot (const Handle(Vector)& Other) const  { 

  return gpVec.Dot (Other->Vec());
}


Standard_Real Geom_Vector::DotCross (
const Handle(Vector)& V1, const Handle(Vector)& V2) const {

  return gpVec.DotCross (V1->Vec(), V2->Vec());
}



