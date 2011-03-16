// File:	Geom2d_AxisPlacement.cxx
// Created:	Wed Mar 24 19:18:35 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom_AxisPlacement.cxx, JCV 25/06/91

#include <Geom2d_AxisPlacement.ixx>



typedef Geom2d_AxisPlacement          AxisPlacement;
typedef Handle(Geom2d_AxisPlacement)  Handle(AxisPlacement);
typedef gp_Dir2d  Dir2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Trsf2d Trsf2d;
typedef gp_Vec2d  Vec2d;




Handle(Geom2d_Geometry) Geom2d_AxisPlacement::Copy() const {

  Handle(AxisPlacement) A;
  A = new AxisPlacement (axis);
  return A;
}






Geom2d_AxisPlacement::Geom2d_AxisPlacement (const gp_Ax2d& A) { axis = A; }


Geom2d_AxisPlacement::Geom2d_AxisPlacement (const Pnt2d& P, const Dir2d& V) {

   axis = gp_Ax2d (P, V);
}

gp_Ax2d Geom2d_AxisPlacement::Ax2d () const { return axis; }

Dir2d Geom2d_AxisPlacement::Direction () const { return axis.Direction(); }

Pnt2d Geom2d_AxisPlacement::Location () const { return axis.Location(); }

void Geom2d_AxisPlacement::Reverse()        { axis.Reverse(); }

Handle(AxisPlacement) Geom2d_AxisPlacement::Reversed() const {

  gp_Ax2d A = axis;
  A.Reverse();
  Handle(AxisPlacement) Temp = new AxisPlacement (A);
  return Temp;
}

void Geom2d_AxisPlacement::Transform (const Trsf2d& T) { axis.Transform (T); }


void Geom2d_AxisPlacement::SetAxis (const gp_Ax2d& A)  { axis = A; }

void Geom2d_AxisPlacement::SetLocation (const Pnt2d& P) {axis.SetLocation (P);}

void Geom2d_AxisPlacement::SetDirection (const Dir2d& V) {

  axis.SetDirection(V);
}

Standard_Real Geom2d_AxisPlacement::Angle (const Handle(AxisPlacement)& Other) const {

  return axis.Angle (Other->Ax2d());
}
