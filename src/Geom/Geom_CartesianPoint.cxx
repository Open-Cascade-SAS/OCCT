// File:	Geom_CartesianPoint.cxx
// Created:	Wed Mar 10 09:28:38 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_CartesianPoint.cxx, JCV 16/01/91

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






