// Created on: 1992-11-06
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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



#include <BRepPrim_Sphere.ixx>

#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <Precision.hxx>

#include <Geom_SphericalSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom2d_Circle.hxx>

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


