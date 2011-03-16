// File:	Geom_Direction.cxx
// Created:	Wed Mar 10 09:40:32 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Direction.cxx, JCV 16/01/91

#include <Geom_Direction.ixx>
#include <gp.hxx>
#include <Standard_ConstructionError.hxx>

typedef Geom_Direction         Direction;
typedef Handle(Geom_Direction) Handle(Direction);
typedef Geom_Vector            Vector;
typedef Handle(Geom_Vector)    Handle(Vector);
typedef gp_Ax1  Ax1;
typedef gp_Ax2  Ax2;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Direction::Copy() const {

  Handle(Direction) D;
  D = new Direction (gpVec);
  return D; 
}

//=======================================================================
//function : Geom_Direction
//purpose  : 
//=======================================================================

Geom_Direction::Geom_Direction (const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

   Standard_Real D = sqrt (X * X + Y * Y + Z * Z);
   Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
   gpVec = gp_Vec (X/D, Y/D, Z/D);
}

Geom_Direction::Geom_Direction (const gp_Dir& V)  { gpVec = V; }

void Geom_Direction::SetDir (const gp_Dir& V)     { gpVec = V; }

gp_Dir Geom_Direction::Dir () const               {  return gpVec; }

Standard_Real Geom_Direction::Magnitude () const           { return 1.0; }

Standard_Real Geom_Direction::SquareMagnitude () const     { return 1.0; }

void Geom_Direction::SetCoord (const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  Standard_Real D = Sqrt (X * X + Y * Y + Z * Z);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec = gp_Vec(X/D, Y/D, Z/D);
}


void Geom_Direction::SetX (const Standard_Real X) {

  Standard_Real D = Sqrt (X * X + gpVec.Y() * gpVec.Y() + gpVec.Z() * gpVec.Z());
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec = gp_Vec (X/D, gpVec.Y()/D, gpVec.Z()/D);
}


void Geom_Direction::SetY (const Standard_Real Y) {

  Standard_Real D = Sqrt (gpVec.X() * gpVec.X() + Y * Y + gpVec.Z() * gpVec.Z());
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec = gp_Vec (gpVec.X()/D, Y/D, gpVec.Z()/D);
}


void Geom_Direction::SetZ (const Standard_Real Z) {

  Standard_Real D = Sqrt (gpVec.X() * gpVec.X() + gpVec.Y() * gpVec.Y() + Z * Z);
  Standard_ConstructionError_Raise_if (D <= gp::Resolution(), "");
  gpVec = gp_Vec (gpVec.X()/D, gpVec.Y()/D, Z/D);
}


void Geom_Direction::Cross (const Handle(Vector)& Other) {

  gp_Dir V (gpVec.Crossed(Other->Vec()));
  gpVec = V;
}


void Geom_Direction::CrossCross (
const Handle(Vector)& V1, const Handle(Vector)& V2) {

  gp_Dir V (gpVec.CrossCrossed (V1->Vec(), V2->Vec()));
  gpVec = V;
}


Handle(Vector) Geom_Direction::Crossed (const Handle(Vector)& Other)
const {

   gp_Dir V (gpVec.Crossed (Other->Vec()));
   return new Direction (V);
}


Handle(Vector) Geom_Direction::CrossCrossed (
const Handle(Vector)& V1, const Handle(Vector)& V2) const {

  gp_Dir V (gpVec.CrossCrossed (V1->Vec(), V2->Vec()));
  return new Direction (V);
}


void Geom_Direction::Transform (const gp_Trsf& T) { 

  gp_Dir V (gpVec);
  V.Transform (T);
  gpVec = V;
}
