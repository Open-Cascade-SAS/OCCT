// Created on: 1995-01-09
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepPrim_Wedge.ixx>



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

