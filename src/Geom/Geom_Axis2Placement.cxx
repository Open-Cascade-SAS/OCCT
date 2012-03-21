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



#include <Geom_Axis2Placement.ixx>

typedef Handle(Geom_Axis2Placement) Handle(Axis2Placement);
typedef Geom_Axis2Placement Axis2Placement;
typedef gp_Ax1  Ax1;
typedef gp_Dir  Dir;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_Axis2Placement::Copy() const {

  Handle(Axis2Placement) A2;
  A2 = new Axis2Placement (axis.Location(), axis.Direction(), vxdir, vydir);
  return A2;
}


//=======================================================================
//function : Geom_Axis2Placement
//purpose  : 
//=======================================================================

Geom_Axis2Placement::Geom_Axis2Placement (const gp_Ax2& A2) {

   vxdir = A2. XDirection();
   vydir = A2. YDirection();
   axis  = A2.Axis();
}


//=======================================================================
//function : Geom_Axis2Placement
//purpose  : 
//=======================================================================

Geom_Axis2Placement::Geom_Axis2Placement (

const gp_Pnt& P,
const gp_Dir& N,
const gp_Dir& Vx) {

  axis = gp_Ax1 (P, N);
  vxdir = N.CrossCrossed (Vx, N);
  vydir = N.Crossed (vxdir);
}


//=======================================================================
//function : Geom_Axis2Placement
//purpose  : 
//=======================================================================

Geom_Axis2Placement::Geom_Axis2Placement (

const gp_Pnt& P,
const gp_Dir& Vz,
const gp_Dir& Vx,
const gp_Dir& Vy

) : vxdir (Vx), vydir (Vy) {

  axis.SetLocation (P);
  axis.SetDirection (Vz);
}


//=======================================================================
//function : XDirection
//purpose  : 
//=======================================================================

const gp_Dir& Geom_Axis2Placement::XDirection () const { return vxdir; }


//=======================================================================
//function : YDirection
//purpose  : 
//=======================================================================

const gp_Dir& Geom_Axis2Placement::YDirection () const {  return vydir; }


//=======================================================================
//function : SetAx2
//purpose  : 
//=======================================================================

void Geom_Axis2Placement::SetAx2 (const gp_Ax2& A2) {

   vxdir = A2.XDirection();
   vydir = A2.YDirection();
   axis  = A2.Axis();
}


//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom_Axis2Placement::SetDirection (const gp_Dir& V) {

   axis.SetDirection (V);
   vxdir = V.CrossCrossed (vxdir, V);
   vydir = V.Crossed (vxdir);
}


//=======================================================================
//function : SetXDirection
//purpose  : 
//=======================================================================

void Geom_Axis2Placement::SetXDirection (const gp_Dir& Vx) { 

  vxdir = axis.Direction().CrossCrossed (Vx, axis.Direction());
  vydir = axis.Direction().Crossed      (vxdir);
}



//=======================================================================
//function : SetYDirection
//purpose  : 
//=======================================================================

void Geom_Axis2Placement::SetYDirection (const gp_Dir& Vy) {

  vxdir = Vy.Crossed (axis.Direction());
  vydir = (axis.Direction()).Crossed (vxdir);
}


//=======================================================================
//function : Ax2
//purpose  : 
//=======================================================================

gp_Ax2 Geom_Axis2Placement::Ax2 () const {

  return gp_Ax2 (axis.Location(), axis.Direction(), vxdir);
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_Axis2Placement::Transform (const gp_Trsf& T) {

  //axis.Location().Transform (T);
  axis.SetLocation(axis.Location().Transformed(T)); // 10-03-93
  vxdir.Transform (T);
  vydir.Transform (T);
  axis.SetDirection (vxdir.Crossed (vydir));
}



