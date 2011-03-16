// File:	Geom_Vector.cxx
// Created:	Wed Mar 10 11:03:05 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Vector.cxx, JCV 16/01/91
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



