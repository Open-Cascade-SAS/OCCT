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




#include <BRepPrim_Torus.ixx>

#include <gp.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2d.hxx>
#include <Precision.hxx>

#include <Geom_ToroidalSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom2d_Circle.hxx>

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
