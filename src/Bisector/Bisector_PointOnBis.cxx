// Created on: 1994-01-10
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Bisector_PointOnBis.ixx>


//=============================================================================
//function :
// purpose :
//=============================================================================
Bisector_PointOnBis::Bisector_PointOnBis()
{
}

//=============================================================================
//function :
// purpose :
//=============================================================================
Bisector_PointOnBis::Bisector_PointOnBis(const Standard_Real Param1, 
					 const Standard_Real Param2, 
					 const Standard_Real ParamBis,
					 const Standard_Real Distance,
					 const gp_Pnt2d&     P)
: param1   (Param1),
  param2   (Param2),
  paramBis (ParamBis),
  distance (Distance),
  point    (P)
{
  infinite = Standard_False;
}

//=============================================================================
//function : ParamOnC1
// purpose :
//=============================================================================
void  Bisector_PointOnBis::ParamOnC1(const Standard_Real Param)
{
  param1 = Param;
}

//=============================================================================
//function : ParamOnC2
// purpose :
//=============================================================================
void  Bisector_PointOnBis::ParamOnC2(const Standard_Real Param)
{
  param2 = Param;
}

//=============================================================================
//function : ParamOnBis
// purpose :
//=============================================================================
void  Bisector_PointOnBis::ParamOnBis(const Standard_Real Param)
{
  paramBis = Param;
}

//=============================================================================
//function : Distance
// purpose :
//=============================================================================
void  Bisector_PointOnBis::Distance(const Standard_Real Distance)
{
  distance = Distance;
}

//=============================================================================
//function : Point
// purpose :
//=============================================================================
void  Bisector_PointOnBis::Point (const gp_Pnt2d& P)
{
  point = P;
}

//=============================================================================
//function : IsInfinite
// purpose :
//=============================================================================
void  Bisector_PointOnBis::IsInfinite(const Standard_Boolean  Infinite)
{
  infinite = Infinite;
}

//=============================================================================
//function : ParamOnC1
// purpose :
//=============================================================================
Standard_Real  Bisector_PointOnBis::ParamOnC1()const
{
  return param1;
}

//=============================================================================
//function : ParamOnC2
// purpose :
//=============================================================================
Standard_Real  Bisector_PointOnBis::ParamOnC2()const 
{
  return param2;
}

//=============================================================================
//function : ParamOnBis
// purpose :
//=============================================================================
Standard_Real  Bisector_PointOnBis::ParamOnBis()const 
{
  return paramBis;
}

//=============================================================================
//function : Distance
// purpose :
//=============================================================================
Standard_Real  Bisector_PointOnBis::Distance()const 
{
  return distance;
}

//=============================================================================
//function : Point
// purpose :
//=============================================================================
gp_Pnt2d  Bisector_PointOnBis::Point ()const
{
  return point;
}

//=============================================================================
//function : IsInfinite
// purpose :
//=============================================================================
Standard_Boolean Bisector_PointOnBis::IsInfinite() const
{
  return infinite;
}

//=============================================================================
//function : Dump
// purpose :
//=============================================================================
void Bisector_PointOnBis::Dump() const 
{
  cout <<"Param1    :"<<param1<<endl;
  cout <<"Param2    :"<<param2<<endl;
  cout <<"Param Bis :"<<paramBis<<endl;
  cout <<"Distance  :"<<distance<<endl;
}
