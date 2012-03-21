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



#include <BRepPrim_Cylinder.ixx>

#include <gp.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Line.hxx>

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
