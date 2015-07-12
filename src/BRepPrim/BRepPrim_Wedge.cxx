// Created on: 1995-01-09
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepPrim_Wedge.hxx>
#include <gp_Ax2.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : BRepPrim_Wedge
//purpose  : 
//=======================================================================
BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz) :
BRepPrim_GWedge(BRepPrim_Builder(),Axes,dx,dy,dz)
{
}

//=======================================================================
//function : BRepPrim_Wedge
//purpose  : 
//=======================================================================

 BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2& Axes, const Standard_Real dx, const Standard_Real dy, const Standard_Real dz, const Standard_Real ltx) :
BRepPrim_GWedge(BRepPrim_Builder(),Axes,dx,dy,dz,ltx)
{
}

//=======================================================================
//function : BRepPrim_Wedge
//purpose  : 
//=======================================================================

 BRepPrim_Wedge::BRepPrim_Wedge(const gp_Ax2& Axes, 
				const Standard_Real xmin, const Standard_Real ymin, const Standard_Real zmin, 
				const Standard_Real z2min, const Standard_Real x2min,
				const Standard_Real xmax, const Standard_Real ymax, const Standard_Real zmax, 
				const Standard_Real z2max, const Standard_Real x2max) :
BRepPrim_GWedge(BRepPrim_Builder(),Axes,xmin,ymin,zmin,z2min,x2min,xmax,ymax,zmax,z2max,x2max)
{
}

