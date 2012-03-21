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



#include <Geom_CartesianPoint.ixx>

typedef Geom_CartesianPoint         CartesianPoint;
typedef Handle(Geom_CartesianPoint) Handle(CartesianPoint);
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Vec  Vec;
typedef gp_Trsf Trsf;



//=======================================================================
//function : Geom_CartesianPoint
//purpose  : 
//=======================================================================

Geom_CartesianPoint::Geom_CartesianPoint (const gp_Pnt& P) : gpPnt(P) { }


//=======================================================================
//function : Geom_CartesianPoint
//purpose  : 
//=======================================================================

Geom_CartesianPoint::Geom_CartesianPoint (
const Standard_Real X, const Standard_Real Y, const Standard_Real Z) : gpPnt (X, Y ,Z) { }


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_CartesianPoint::Copy() const {

  Handle(CartesianPoint) P;
  P = new CartesianPoint (gpPnt);
  return P; 
}


//=======================================================================
//function : SetCoord
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::SetCoord (const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  gpPnt.SetCoord (X, Y, Z);
}


//=======================================================================
//function : SetPnt
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::SetPnt (const gp_Pnt& P) {  gpPnt = P; }

//=======================================================================
//function : SetX
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::SetX (const Standard_Real X) { gpPnt.SetX (X); }

//=======================================================================
//function : SetY
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::SetY (const Standard_Real Y) { gpPnt.SetY (Y); }

//=======================================================================
//function : SetZ
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::SetZ (const Standard_Real Z) { gpPnt.SetZ (Z); }


//=======================================================================
//function : Coord
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::Coord (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

  gpPnt.Coord (X, Y, Z);
}


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

gp_Pnt Geom_CartesianPoint::Pnt () const { return gpPnt; }

//=======================================================================
//function : X
//purpose  : 
//=======================================================================

Standard_Real Geom_CartesianPoint::X () const { return gpPnt.X(); }

//=======================================================================
//function : Y
//purpose  : 
//=======================================================================

Standard_Real Geom_CartesianPoint::Y () const { return gpPnt.Y(); }

//=======================================================================
//function : Z
//purpose  : 
//=======================================================================

Standard_Real Geom_CartesianPoint::Z () const { return gpPnt.Z(); }

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_CartesianPoint::Transform (const Trsf& T) { gpPnt.Transform (T); }






