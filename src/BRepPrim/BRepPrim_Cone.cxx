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




#include <BRepPrim_Cone.ixx>
#include <gp.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Line.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const Standard_Real Angle, 
			     const gp_Ax2& Position, 
			     const Standard_Real Height, 
			     const Standard_Real Radius) :
       BRepPrim_Revolution(Position,0,0),
       myHalfAngle(Angle),
       myRadius(Radius)
{
  if (Height < Precision::Confusion())
    Standard_DomainError::Raise("cone with null height");
  if (myHalfAngle*Height < Precision::Confusion())
    Standard_DomainError::Raise("cone with null angle");
  if ((M_PI/2 - myHalfAngle)*Height < Precision::Confusion())
    Standard_DomainError::Raise("cone with angle > PI/2");
  
  // cut at top
  VMax(Height / Cos(myHalfAngle));
  VMin(0.);
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const Standard_Real Angle) :
       BRepPrim_Revolution(gp::XOY(),0,RealLast()),
       myHalfAngle(Angle),
       myRadius(0.)
{
  if ((Angle < 0) || (Angle > M_PI/2)) 
    Standard_DomainError::Raise("cone with angle <0 or > PI/2");
  VMin(0.);
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const Standard_Real Angle, 
			     const gp_Pnt& Apex) :
       BRepPrim_Revolution(gp_Ax2(Apex,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   0,RealLast()),
       myHalfAngle(Angle),
       myRadius(0.)
{
  if ((Angle < 0) || (Angle > M_PI/2)) 
    Standard_DomainError::Raise("cone with angle <0 or > PI/2");
  VMin(0.);
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const Standard_Real Angle,
			     const gp_Ax2& Axes) :
       BRepPrim_Revolution( Axes, 0,RealLast()),
       myHalfAngle(Angle)
{
  if ((Angle < 0) || (Angle > M_PI/2)) 
    Standard_DomainError::Raise("cone with angle <0 or > PI/2");
  VMin(0.);
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const Standard_Real R1,
			     const Standard_Real R2,
			     const Standard_Real H) :
       BRepPrim_Revolution(gp::XOY(),0,0)
{
  SetParameters(R1,R2,H);
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const gp_Pnt& Center,
			     const Standard_Real R1, 
			     const Standard_Real R2,
			     const Standard_Real H) :
       BRepPrim_Revolution(gp_Ax2(Center,gp_Dir(0,0,1),gp_Dir(1,0,0)),
			   0,0)
{
  SetParameters(R1,R2,H);
  SetMeridian();
}

//=======================================================================
//function : BRepPrim_Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone::BRepPrim_Cone(const gp_Ax2& Axes, 
			     const Standard_Real R1, 
			     const Standard_Real R2, 
			     const Standard_Real H) :
       BRepPrim_Revolution(Axes,0,0)
{
  SetParameters(R1,R2,H);
  SetMeridian();
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Cone::MakeEmptyLateralFace()const 
{
  Handle(Geom_ConicalSurface) C =
    new Geom_ConicalSurface(Axes(),myHalfAngle,myRadius);
  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,C,Precision::Confusion());
  return F;
}

//=======================================================================
//function : SetMeridian
//purpose  : 
//=======================================================================

void BRepPrim_Cone::SetMeridian ()
{
  gp_Ax1 A = Axes().Axis();
  A.Rotate(gp_Ax1(Axes().Location(),Axes().YDirection()),myHalfAngle);
  gp_Vec V(Axes().XDirection());
  V *= myRadius;
  A.Translate(V);
  Handle(Geom_Line) L = new Geom_Line(A);
  Handle(Geom2d_Line) L2d = 
    new Geom2d_Line(gp_Pnt2d(myRadius,0),gp_Dir2d(Sin(myHalfAngle),Cos(myHalfAngle)));
  Meridian(L,L2d);
}

//=======================================================================
//function : SetParameters
//purpose  : 
//=======================================================================

void BRepPrim_Cone::SetParameters(const Standard_Real R1,
				  const Standard_Real R2,
				  const Standard_Real H)
{
  if (((R1 != 0) && (R1 < Precision::Confusion())) ||
      ((R2 != 0) && (R2 < Precision::Confusion())))
    Standard_DomainError::Raise("cone with negative or too small radius");
  if (Abs(R1-R2) < Precision::Confusion())
    Standard_DomainError::Raise("cone with two identic radii");
  if (H < Precision::Confusion())
    Standard_DomainError::Raise("cone with negative or null height");

  myRadius = R1;
  myHalfAngle = ATan((R2 - R1) / H);

  // cut top and bottom
  VMin(0.);
  VMax(Sqrt(H*H + (R2-R1)*(R2-R1)));
}  


