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


#include <Geom_Axis1Placement.hxx>
#include <Geom_Geometry.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Axis1Placement,Geom_AxisPlacement)

typedef Geom_Axis1Placement         Axis1Placement;
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

  Handle(Geom_Axis1Placement) A1;
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

Handle(Geom_Axis1Placement) Geom_Axis1Placement::Reversed() const {

   gp_Ax1 A1 = axis;
   A1.Reverse();
   Handle (Axis1Placement) Temp = new Axis1Placement (A1);
   return Temp;
}


