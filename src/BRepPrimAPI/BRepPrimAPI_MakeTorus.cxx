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



#include <BRepPrimAPI_MakeTorus.ixx>
#include <BRepBuilderAPI.hxx>


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2) :
       myTorus(R1, R2)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1, 
				     const Standard_Real R2,
				     const Standard_Real angle) :
       myTorus(R1, R2)
{
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2, 
				     const Standard_Real angle1, 
				     const Standard_Real angle2) :
       myTorus( R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2,
				     const Standard_Real angle1, 
				     const Standard_Real angle2, 
				     const Standard_Real angle) :
       myTorus( R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1,
				     const Standard_Real R2) :
       myTorus(Axes, R1, R2)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1,
				     const Standard_Real R2,
				     const Standard_Real angle) :
       myTorus(Axes, R1, R2)
{
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1, 
				     const Standard_Real R2, 
				     const Standard_Real angle1,
				     const Standard_Real angle2) :
       myTorus(Axes, R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes,
				     const Standard_Real R1, 
				     const Standard_Real R2,
				     const Standard_Real angle1,
				     const Standard_Real angle2, 
				     const Standard_Real angle) :
       myTorus(Axes, R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
  myTorus.Angle(angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeTorus::OneAxis()
{
  return &myTorus;
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

BRepPrim_Torus&  BRepPrimAPI_MakeTorus::Torus()
{
  return myTorus;
}


