// Created on: 1995-09-04
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
  // all that to swindle the constant

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




