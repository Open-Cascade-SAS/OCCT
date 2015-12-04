// Created on: 1993-03-09
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Geom_Axis2Placement.hxx>
#include <Geom_Geometry.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Axis2Placement,Geom_AxisPlacement)

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

  Handle(Geom_Axis2Placement) A2;
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



