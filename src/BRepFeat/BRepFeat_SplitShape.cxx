// File:	BRepFeat_SplitShape.cxx
// Created:	Mon Sep  4 10:06:40 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepFeat_SplitShape.ixx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Standard_Failure.hxx>

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepFeat_SplitShape::Build ()
{
  mySShape.Perform(myWOnShape);
  if (mySShape.IsDone()) {
    Done();
    myShape = mySShape.ResultingShape();
  }
}



//=======================================================================
//function : DirectLeft
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::DirectLeft() const
{
  return mySShape.DirectLeft();
}


//=======================================================================
//function : DirectLeft
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::Left() const
{
  return mySShape.Left();
}

//=======================================================================
//function : isDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFeat_SplitShape::IsDeleted(const TopoDS_Shape& F) 
{
  TopTools_ListIteratorOfListOfShape itl
    (((LocOpe_Spliter*) &mySShape)->DescendantShapes(F));
  // tout ceci pour truander le const

  return (!itl.More());// a priori impossible

}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::Modified
   (const TopoDS_Shape& F)
{
  return mySShape.DescendantShapes(F);
}




