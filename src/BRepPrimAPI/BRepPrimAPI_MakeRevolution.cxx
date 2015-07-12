// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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


#include <BRepBuilderAPI.hxx>
#include <BRepPrim_Revolution.hxx>
#include <BRepPrimAPI_MakeRevolution.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GeomProjLib.hxx>
#include <gp_Ax2.hxx>
#include <Standard_DomainError.hxx>

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


