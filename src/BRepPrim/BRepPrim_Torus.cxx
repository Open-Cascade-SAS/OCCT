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


#include <BRepPrim_Torus.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS_Face.hxx>

//=======================================================================
//function : BRepPrim_Torus
//purpose  : 
//=======================================================================
BRepPrim_Torus::BRepPrim_Torus(const gp_Ax2& Position, 
			       const Standard_Real Major, 
			       const Standard_Real Minor) :
       BRepPrim_Revolution(Position,0,2*M_PI),
       myMajor(Major),
       myMinor(Minor)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Torus
//purpose  : 
//=======================================================================

BRepPrim_Torus::BRepPrim_Torus(const Standard_Real Major, 
			       const Standard_Real Minor) :
       BRepPrim_Revolution(gp::XOY(),0,2*M_PI),
       myMajor(Major),
       myMinor(Minor)
{
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Torus
//purpose  : 
//=======================================================================

BRepPrim_Torus::BRepPrim_Torus(const gp_Pnt& Center, 
			       const Standard_Real Major, 
			       const Standard_Real Minor) :
       BRepPrim_Revolution(gp_Ax2(Center,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   0,2*M_PI),
       myMajor(Major),
       myMinor(Minor)
{
  SetMeridian();
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Torus::MakeEmptyLateralFace()const 
{
  Handle(Geom_ToroidalSurface) T =
    new Geom_ToroidalSurface(Axes(),myMajor,myMinor);
  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,T,Precision::Confusion());
  return F;
}


//=======================================================================
//function : SetMeridian
//purpose  : 
//=======================================================================

void BRepPrim_Torus::SetMeridian()
{
  gp_Dir D = Axes().YDirection();
  D.Reverse();
  gp_Ax2 A(Axes().Location(),D,Axes().XDirection());
  gp_Vec V = Axes().XDirection();
  V.Multiply(myMajor);
  A.Translate(V);
  Handle(Geom_Circle) C = new Geom_Circle(A,myMinor);
  Handle(Geom2d_Circle) C2d = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(myMajor,0),
							gp_Dir2d(1,0)),
						myMinor);
  Meridian(C,C2d);
}  
