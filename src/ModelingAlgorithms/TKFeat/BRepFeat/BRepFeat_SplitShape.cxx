// Created on: 1995-09-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepFeat_SplitShape.hxx>
#include <LocOpe_WiresOnShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================

void BRepFeat_SplitShape::Build(const Message_ProgressRange& /*theRange*/)
{
  mySShape.Perform(myWOnShape);
  if (mySShape.IsDone())
  {
    Done();
    myShape = mySShape.ResultingShape();
    myRight.Clear();
  }
}

//=================================================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::DirectLeft() const
{
  return mySShape.DirectLeft();
}

//=================================================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::Left() const
{
  return mySShape.Left();
}

//=================================================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::Right() const
{
  if (myRight.IsEmpty())
  {
    TopTools_MapOfShape                aMapOfLeft;
    TopTools_ListIteratorOfListOfShape anIterator;
    for (anIterator.Initialize(mySShape.Left()); anIterator.More(); anIterator.Next())
    {
      aMapOfLeft.Add(anIterator.Value());
    }
    TopExp_Explorer anExplorer;
    for (anExplorer.Init(myShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
    {
      const TopoDS_Shape& aFace = anExplorer.Current();
      if (!aMapOfLeft.Contains(aFace))
        myRight.Append(aFace);
    }
  }
  return myRight;
}

//=================================================================================================

Standard_Boolean BRepFeat_SplitShape::IsDeleted(const TopoDS_Shape& F)
{
  TopTools_ListIteratorOfListOfShape itl(((LocOpe_Spliter*)&mySShape)->DescendantShapes(F));
  // all that to swindle the constant

  return (!itl.More()); // a priori impossible
}

//=================================================================================================

const TopTools_ListOfShape& BRepFeat_SplitShape::Modified(const TopoDS_Shape& F)
{
  return mySShape.DescendantShapes(F);
}
