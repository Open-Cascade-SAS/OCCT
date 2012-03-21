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



#include <BRepPrimAPI_MakeRevolution.ixx>
#include <BRepBuilderAPI.hxx>
#include <GeomProjLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Plane.hxx>

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

static Handle(Geom2d_Curve) Project(const Handle(Geom_Curve)& M,
				    const gp_Ax3 Axis)
{
  Handle(Geom2d_Curve) C;
  C = GeomProjLib::Curve2d(M,new Geom_Plane(Axis));
  return C;
}

static Handle(Geom2d_Curve) Project(const Handle(Geom_Curve)& M)
{
  return Project(M,gp_Ax2(gp::Origin(),-gp::DY(),gp::DX()));
}

//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian) :
  myRevolution(gp::XOY(),
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian, 
   const Standard_Real angle) :
  myRevolution(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian, 
   const Standard_Real VMin, 
   const Standard_Real VMax) :
  myRevolution(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const Handle(Geom_Curve)& Meridian,
   const Standard_Real VMin, 
   const Standard_Real VMax, 
   const Standard_Real angle) :
  myRevolution(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1),gp_Dir(1,0,0)),
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian) :
  myRevolution(Axes,
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian, 
   const Standard_Real angle) :
  myRevolution(Axes,
	       Meridian->FirstParameter(),
	       Meridian->LastParameter(),
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian, 
   const Standard_Real VMin, 
   const Standard_Real VMax) :
  myRevolution(Axes,
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
}


//=======================================================================
//function : BRepPrimAPI_MakeRevolution
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevolution::BRepPrimAPI_MakeRevolution
  (const gp_Ax2& Axes, 
   const Handle(Geom_Curve)& Meridian, 
   const Standard_Real VMin, 
   const Standard_Real VMax, 
   const Standard_Real angle) :
  myRevolution(Axes,
	       VMin,
	       VMax,
	       Meridian,
	       Project(Meridian))
{
  myRevolution.Angle(angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeRevolution::OneAxis()
{
  return &myRevolution;
}


//=======================================================================
//function : Revolution
//purpose  : 
//=======================================================================

BRepPrim_Revolution&  BRepPrimAPI_MakeRevolution::Revolution()
{
  return myRevolution;
}


