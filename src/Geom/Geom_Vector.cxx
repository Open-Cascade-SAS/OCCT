// Created on: 1993-03-10
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



