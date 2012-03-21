// Created on: 1995-12-07
// Created by: Jacques GOUSSARD
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



#include <BRepCheck_Result.ixx>
#include <BRepCheck.hxx>


//=======================================================================
//function : BRepCheck_Result
//purpose  : 
//=======================================================================

BRepCheck_Result::BRepCheck_Result() :
   myMin(Standard_False),myBlind(Standard_False)
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepCheck_Result::Init(const TopoDS_Shape& S)
{
  myShape = S;
  myMin = Standard_False;
  myBlind = Standard_False;
  myMap.Clear();
  Minimum();
}

//=======================================================================
//function : SetFailStatus
//purpose  : 
//=======================================================================
void BRepCheck_Result::SetFailStatus(const TopoDS_Shape& S) 
{
  if(!myMap.IsBound(S)) {
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(S, thelist);
  }
  BRepCheck::Add(myMap(S), BRepCheck_CheckFail);
}


//=======================================================================
//function : StatusOnShape
//purpose  : 
//=======================================================================

const BRepCheck_ListOfStatus& BRepCheck_Result::StatusOnShape
   (const TopoDS_Shape& S)
{
  if (!myMap.IsBound(S)) {
    InContext(S);
  }
  return myMap(S);
}


//=======================================================================
//function : InitContextIterator
//purpose  : 
//=======================================================================

void BRepCheck_Result::InitContextIterator()
{
  myIter.Initialize(myMap);
  // At least 1 element : the Shape itself
  if (myIter.Key().IsSame(myShape)) {
    myIter.Next();
  }
}


//=======================================================================
//function : NextShapeInContext
//purpose  : 
//=======================================================================

void BRepCheck_Result::NextShapeInContext()
{
  myIter.Next();
  if (myIter.More() && myIter.Key().IsSame(myShape)) {
    myIter.Next();
  }
}  
