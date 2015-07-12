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


#include <BRepPrim_Sphere.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_SphericalSurface.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS_Face.hxx>

// parameters on the meridian
#define PMIN (-0.5*M_PI)
#define PMAX (0.5*M_PI)

//=======================================================================
//function : BRepPrim_Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere::BRepPrim_Sphere(const Standard_Real Radius) :
       BRepPrim_Revolution(gp::XOY(),PMIN,PMAX),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere::BRepPrim_Sphere(const gp_Pnt& Center, 
				 const Standard_Real Radius) :
       BRepPrim_Revolution(gp_Ax2(Center,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   PMIN,PMAX),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere::BRepPrim_Sphere(const gp_Ax2& Axes, 
				 const Standard_Real Radius) :
       BRepPrim_Revolution(Axes,PMIN,PMAX),
       myRadius(Radius)
{
  SetMeridian();
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Sphere::MakeEmptyLateralFace()const
{
  Handle(Geom_SphericalSurface) S =
    new Geom_SphericalSurface(Axes(),myRadius);
  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,S,Precision::Confusion());
  return F;
}

//=======================================================================
//function : SetMeridian
//purpose  : 
//=======================================================================

void BRepPrim_Sphere::SetMeridian()
{
  // Offset the parameters on the meridian
  // to trim the edge in 3pi/2, 5pi/2

  SetMeridianOffset(2*M_PI);

  gp_Dir D = Axes().YDirection();
  D.Reverse();
  gp_Ax2 A(Axes().Location(),D,Axes().XDirection());
  Handle(Geom_Circle) C = new Geom_Circle(A,myRadius);
  Handle(Geom2d_Circle) C2d = 
    new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)),
		      myRadius);
  Meridian(C,C2d);
}


