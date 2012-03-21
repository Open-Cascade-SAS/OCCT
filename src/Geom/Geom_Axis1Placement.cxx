// Created on: 1993-03-09
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


