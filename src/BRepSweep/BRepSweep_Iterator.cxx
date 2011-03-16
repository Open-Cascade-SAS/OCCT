// File:	BRepSweep_Iterator.cxx
// Created:	Wed Jun  9 17:54:27 1993
// Author:	Laurent BOURESCHE
//		<lbo@phobox>


#include <BRepSweep_Iterator.ixx>

//=======================================================================
//function : BRepSweep_Iterator
//purpose  : 
//=======================================================================

BRepSweep_Iterator::BRepSweep_Iterator()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepSweep_Iterator::Init(const TopoDS_Shape& aShape)
{
  myIterator.Initialize(aShape);
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  BRepSweep_Iterator::Next()
{
  myIterator.Next();
}


