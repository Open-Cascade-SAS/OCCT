// Created on: 1992-11-06
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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


#include <BRepPrim_Cylinder.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS_Face.hxx>

//=======================================================================
//function : BRepPrim_Cylinder
//purpose  : 
//=======================================================================
BRepPrim_Cylinder::BRepPrim_Cylinder(const gp_Ax2& Position, 
				     const Standard_Real Radius,
				     const Standard_Real Height) : 
       BRepPrim_Revolution(Position,0,Height),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder::BRepPrim_Cylinder(const Standard_Real Radius) :
       BRepPrim_Revolution(gp::XOY(),RealFirst(),RealLast()),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder::BRepPrim_Cylinder(const gp_Pnt& Center, 
				     const Standard_Real Radius) :
       BRepPrim_Revolution(gp_Ax2(Center,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   RealFirst(),RealLast()),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder::BRepPrim_Cylinder(const gp_Ax2& Axes,
				     const Standard_Real Radius) :
       BRepPrim_Revolution(Axes, RealFirst(),RealLast()),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder::BRepPrim_Cylinder(const Standard_Real R,
				     const Standard_Real H) :
       BRepPrim_Revolution(gp::XOY(), 0, H),
       myRadius(R)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder::BRepPrim_Cylinder(const gp_Pnt& Center, 
				     const Standard_Real R, 
				     const Standard_Real H) :
       BRepPrim_Revolution(gp_Ax2(Center,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   0,H),
       myRadius(R)
{
  SetMeridian();
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Cylinder::MakeEmptyLateralFace() const 
{
  Handle(Geom_CylindricalSurface) C =
    new Geom_CylindricalSurface(Axes(),myRadius);
  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,C,Precision::Confusion());
  return F;
}

//=======================================================================
//function : SetMeridian
//purpose  : 
//=======================================================================

void BRepPrim_Cylinder::SetMeridian()
{
  gp_Vec V = Axes().XDirection();
  V.Multiply(myRadius);
  gp_Ax1 A = Axes().Axis();
  A.Translate(V);
  Handle(Geom_Line) L = new Geom_Line(A);
  Handle(Geom2d_Line) L2d = new Geom2d_Line(gp_Pnt2d(myRadius,0),gp_Dir2d(0,1));
  Meridian(L,L2d);
}
