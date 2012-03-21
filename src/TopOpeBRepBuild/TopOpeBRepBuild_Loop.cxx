// Created on: 1995-12-19
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <TopOpeBRepBuild_Loop.ixx>

//=======================================================================
//function : TopOpeBRepBuild_Loop
//purpose  : 
//=======================================================================

TopOpeBRepBuild_Loop::TopOpeBRepBuild_Loop
(const TopoDS_Shape& S) :
myIsShape(Standard_True),myShape(S),myBlockIterator(0,0)
{
}

//=======================================================================
//function : TopOpeBRepBuild_Loop
//purpose  : 
//=======================================================================

TopOpeBRepBuild_Loop::TopOpeBRepBuild_Loop
(const TopOpeBRepBuild_BlockIterator& BI) :
myIsShape(Standard_False),myBlockIterator(BI)
{
}

//=======================================================================
//function : IsShape
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_Loop::IsShape() const
{
  return myIsShape;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_Loop::Shape() const
{
  return myShape;
}

//=======================================================================
//function : BlockIterator
//purpose  : 
//=======================================================================

const TopOpeBRepBuild_BlockIterator& TopOpeBRepBuild_Loop::BlockIterator() const
{
  return myBlockIterator;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Loop::Dump() const 
{
}
