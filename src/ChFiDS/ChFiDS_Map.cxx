// Created on: 1993-10-22
// Created by: Laurent BOURESCHE
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


#include <ChFiDS_Map.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>

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


