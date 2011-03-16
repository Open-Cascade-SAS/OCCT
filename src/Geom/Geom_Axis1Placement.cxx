// File:	Geom_Axis1Placement.cxx
// Created:	Tue Mar  9 19:25:37 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

//File Geom_Axis1Placement.cxx, JCV 17/01/91

#include <Geom_Axis1Placement.ixx>

typedef Geom_Axis1Placement         Axis1Placement;
typedef Handle(Geom_Axis1Placement) Handle(Axis1Placement);
typedef gp_Ax2  Ax2;
typedef gp_Dir  Dir;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Axis1Placement::Copy() const {

  Handle(Axis1Placement) A1;
  A1 = new Axis1Placement (axis);
  return A1;
}





//=======================================================================
//function : Geom_Axis1Placement
//purpose  : 
//=======================================================================

Geom_Axis1Placement::Geom_Axis1Placement (const gp_Ax1& A1) 
{
 axis = A1;
}

 
Geom_Axis1Placement::Geom_Axis1Placement (const Pnt& P, const Dir& V) {

 axis = gp_Ax1 (P, V);
}



void Geom_Axis1Placement::SetDirection (const Dir& V) {axis.SetDirection (V);}

const gp_Ax1& Geom_Axis1Placement::Ax1 () const { return Axis(); }

void Geom_Axis1Placement::Reverse()  { axis.Reverse(); }

void Geom_Axis1Placement::Transform (const Trsf& T) { axis.Transform (T); }

Handle(Axis1Placement) Geom_Axis1Placement::Reversed() const {

   gp_Ax1 A1 = axis;
   A1.Reverse();
   Handle (Axis1Placement) Temp = new Axis1Placement (A1);
   return Temp;
}


