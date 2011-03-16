// File:	TopOpeBRepBuild_Loop.cxx
// Created:	Tue Dec 19 19:52:32 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

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
