// Created on: 2001-04-13
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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
