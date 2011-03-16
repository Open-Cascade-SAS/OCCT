// File:	BOP_Loop.cxx
// Created:	Tue Dec 19 19:52:32 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <BOP_Loop.ixx>

//=======================================================================
//function : BOP_Loop
//purpose  : 
//=======================================================================
  BOP_Loop::BOP_Loop (const TopoDS_Shape& S) 
:
  myIsShape(Standard_True),
  myShape(S),
  myBlockIterator(0,0)
{
}

//=======================================================================
//function : BOP_Loop
//purpose  : 
//=======================================================================
  BOP_Loop::BOP_Loop (const BOP_BlockIterator& BI)
:
  myIsShape(Standard_False),
  myBlockIterator(BI)
{
}

//=======================================================================
//function : IsShape
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_Loop::IsShape() const
{
  return myIsShape;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOP_Loop::Shape() const
{
  return myShape;
}

//=======================================================================
//function : BlockIterator
//purpose  : 
//=======================================================================
  const BOP_BlockIterator& BOP_Loop::BlockIterator() const
{
  return myBlockIterator;
}


