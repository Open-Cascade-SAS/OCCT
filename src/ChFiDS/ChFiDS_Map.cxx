// Created on: 1993-10-22
// Created by: Laurent BOURESCHE
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



#include <ChFiDS_Map.ixx>
#include <TopExp.hxx>

//=======================================================================
//function : ChFiDS_Map
//purpose  : 
//=======================================================================

ChFiDS_Map::ChFiDS_Map()
{
}


//=======================================================================
//function : Fill
//purpose  : 
//=======================================================================

void  ChFiDS_Map::Fill(const TopoDS_Shape& S, 
		       const TopAbs_ShapeEnum T1, 
		       const TopAbs_ShapeEnum T2)
{
  TopExp::MapShapesAndAncestors(S,T1,T2,myMap);
}


//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Map::Contains(const TopoDS_Shape& S)const 
{
  return myMap.Contains(S);
}


//=======================================================================
//function : FindFromKey
//purpose  : 
//=======================================================================

const TopTools_ListOfShape&  ChFiDS_Map::FindFromKey
  (const TopoDS_Shape& S)const 
{
  return myMap.FindFromKey(S);
}


//=======================================================================
//function : FindFromIndex
//purpose  : 
//=======================================================================

const TopTools_ListOfShape&  
  ChFiDS_Map::FindFromIndex(const Standard_Integer I)const 
{
  return myMap.FindFromIndex(I);
}


