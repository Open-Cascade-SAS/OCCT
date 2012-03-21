// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepDS_Point.ixx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=======================================================================
//function : TopOpeBRepDS_Point
//purpose  : 
//=======================================================================

TopOpeBRepDS_Point::TopOpeBRepDS_Point()
: myKeep(Standard_True)
{
}


//=======================================================================
//function : TopOpeBRepDS_Point
//purpose  : 
//=======================================================================

TopOpeBRepDS_Point::TopOpeBRepDS_Point(const gp_Pnt& P, 
				       const Standard_Real T)
: myPoint(P),
  myTolerance(T),
  myKeep(Standard_True)
{
}


//=======================================================================
//function : TopOpeBRepDS_Point
//purpose  : 
//=======================================================================

TopOpeBRepDS_Point::TopOpeBRepDS_Point(const TopoDS_Shape& S)
{
  myPoint     = TopOpeBRepTool_ShapeTool::Pnt(S);
  myTolerance = TopOpeBRepTool_ShapeTool::Tolerance(S);
}


//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Point::IsEqual(const TopOpeBRepDS_Point& P) const 
{
  Standard_Real    t = Max(myTolerance,P.Tolerance());
  Standard_Boolean b = myPoint.IsEqual(P.Point(),t);
  return b;
}


//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const gp_Pnt& TopOpeBRepDS_Point::Point()const 
{
  return myPoint;
}

//=======================================================================
//function : ChangePoint
//purpose  : 
//=======================================================================

gp_Pnt& TopOpeBRepDS_Point::ChangePoint() 
{
  return myPoint;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRepDS_Point::Tolerance()const 
{
  return myTolerance;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Point::Tolerance(const Standard_Real Tol)
{
  myTolerance = Tol;
}

//=======================================================================
//function : Keep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Point::Keep() const
{
  return myKeep;
}
//=======================================================================
//function : ChangeKeep
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Point::ChangeKeep(const Standard_Boolean b)
{
  myKeep = b;
}
