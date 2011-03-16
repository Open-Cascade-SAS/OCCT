// File:	ChFiDS_Map.cxx
// Created:	Fri Oct 22 17:25:12 1993
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


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


