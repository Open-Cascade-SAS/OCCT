// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Point.hxx>
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
