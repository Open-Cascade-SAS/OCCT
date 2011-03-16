// File:	BOP_ConnexityBlock.cxx
// Created:	Fri Apr 13 10:26:49 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_ConnexityBlock.ixx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopoDS_Shape.hxx>

//=======================================================================
// function: BOP_ConnexityBlock::BOP_ConnexityBlock
// purpose: 
//=======================================================================
  BOP_ConnexityBlock::BOP_ConnexityBlock()
:
  myRegularity(Standard_False)
{}
//=======================================================================
// function: SetRegularity
// purpose: 
//=======================================================================
  void BOP_ConnexityBlock::SetRegularity (const Standard_Boolean aFlag)
{
  myRegularity=aFlag;
}
//=======================================================================
// function: IsRegular
// purpose: 
//=======================================================================
  Standard_Boolean BOP_ConnexityBlock::IsRegular ()const
{
  return myRegularity;
}
//=======================================================================
// function: SetShapes
// purpose: 
//=======================================================================
  void BOP_ConnexityBlock::SetShapes(const TopTools_ListOfShape& anEdges)
{
  myShapes.Clear();
  TopTools_ListIteratorOfListOfShape anIt(anEdges);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& anE=anIt.Value();
    myShapes.Append(anE);
  }
}

//=======================================================================
// function: SetShapes
// purpose: 
//=======================================================================
  void BOP_ConnexityBlock::SetShapes(const TopTools_IndexedMapOfOrientedShape& anEdges)
{
  myShapes.Clear();
  Standard_Integer i, aNb;

  aNb=anEdges.Extent();
  for (i=1; i<=aNb; i++) {
    const TopoDS_Shape& anE=anEdges(i);
    myShapes.Append(anE);
  }
}

//=======================================================================
// function: Shapes
// purpose: 
//=======================================================================
  const TopTools_ListOfShape& BOP_ConnexityBlock::Shapes()const
{
  return myShapes;
}
