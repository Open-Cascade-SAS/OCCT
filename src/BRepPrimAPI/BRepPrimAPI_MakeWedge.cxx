// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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



#include <BRepPrimAPI_MakeWedge.ixx>
#include <TopoDS.hxx>


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const Standard_Real dx, 
				     const Standard_Real dy,
				     const Standard_Real dz,
				     const Standard_Real ltx) :
       myWedge(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       dx,dy,dz,ltx)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const gp_Ax2& Axes,
				     const Standard_Real dx, 
				     const Standard_Real dy,
				     const Standard_Real dz, 
				     const Standard_Real ltx) :
       myWedge(Axes,
	       dx,dy,dz,ltx)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const Standard_Real dx, 
				     const Standard_Real dy, 
				     const Standard_Real dz, 
				     const Standard_Real xmin,
				     const Standard_Real zmin,
				     const Standard_Real xmax,
				     const Standard_Real zmax) :
       myWedge(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       0,0,0,zmin,xmin,dx,dy,dz,zmax,xmax)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeWedge
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::BRepPrimAPI_MakeWedge(const gp_Ax2& Axes,
				     const Standard_Real dx,
				     const Standard_Real dy, 
				     const Standard_Real dz,
				     const Standard_Real xmin,
				     const Standard_Real zmin, 
				     const Standard_Real xmax, 
				     const Standard_Real zmax) :
       myWedge(Axes,
	       0,0,0,zmin,xmin,dx,dy,dz,zmax,xmax)
{
}


//=======================================================================
//function : Wedge
//purpose  : 
//=======================================================================

BRepPrim_Wedge&  BRepPrimAPI_MakeWedge::Wedge()
{
  return myWedge;
}


//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

const TopoDS_Shell&  BRepPrimAPI_MakeWedge::Shell()
{
  Build();
  return myWedge.Shell();
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepPrimAPI_MakeWedge::Build()
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,myWedge.Shell());
  myShape.Closed(Standard_True);
  Done();
}

//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepPrimAPI_MakeWedge::Solid()
{
  Build();
  return TopoDS::Solid(myShape);
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::operator TopoDS_Shell()
{
  return Shell();
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeWedge::operator TopoDS_Solid()
{
  return Solid();
}
