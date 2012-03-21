// Created on: 1993-03-24
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
