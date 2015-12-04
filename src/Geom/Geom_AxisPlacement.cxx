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


#include <Geom_AxisPlacement.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_AxisPlacement,Geom_Geometry)

typedef gp_Ax1 Ax1;
typedef gp_Dir Dir;
typedef gp_Pnt Pnt;
typedef gp_Vec Vec;



const gp_Ax1& Geom_AxisPlacement::Axis () const { return axis; }

Dir Geom_AxisPlacement::Direction () const { return axis.Direction(); }

Pnt Geom_AxisPlacement::Location () const  { return axis.Location(); }

void Geom_AxisPlacement::SetAxis (const Ax1& A1) { axis = A1; }

void Geom_AxisPlacement::SetLocation (const Pnt& P) {axis.SetLocation (P);}

Standard_Real Geom_AxisPlacement::Angle (const Handle(Geom_AxisPlacement)& Other) const {
 return axis.Angle (Other->Axis());
}


