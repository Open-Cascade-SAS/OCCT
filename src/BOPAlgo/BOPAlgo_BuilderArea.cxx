// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_BuilderArea.ixx>

#include <BOPCol_ListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_BaseAllocator.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderArea::BOPAlgo_BuilderArea()
:
  BOPAlgo_Algo(),
  //myContext(NULL),
  myShapes(myAllocator),
  myLoops(myAllocator),
  myLoopsInternal(myAllocator),
  myAreas(myAllocator),
  myShapesToAvoid(100, myAllocator) 
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderArea::BOPAlgo_BuilderArea(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator),
  //myContext(NULL),
  myShapes(myAllocator),
  myLoops(myAllocator),
  myLoopsInternal(myAllocator),
  myAreas(myAllocator),
  myShapesToAvoid(100, myAllocator) 
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPAlgo_BuilderArea::~BOPAlgo_BuilderArea()
{
}
//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderArea::SetContext(const Handle(BOPInt_Context)& theContext)
{
  myContext=theContext;
}
//=======================================================================
//function : SetShapes
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderArea::SetShapes(const BOPCol_ListOfShape& theLF)
{
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myShapes.Clear();
  aIt.Initialize(theLF);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    myShapes.Append(aF);
  }
}
//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================
  const BOPCol_ListOfShape& BOPAlgo_BuilderArea::Shapes()const
{
  return myShapes;
}
//=======================================================================
//function : Loops
//purpose  : 
//=======================================================================
  const BOPCol_ListOfShape& BOPAlgo_BuilderArea::Loops()const
{
  return myLoops;
}
//=======================================================================
//function : Areas
//purpose  : 
//=======================================================================
  const BOPCol_ListOfShape& BOPAlgo_BuilderArea::Areas()const
{
  return myAreas;
}
/*
//=======================================================================
//function :PerformShapesToAvoid
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderArea::PerformShapesToAvoid()
{
}
//=======================================================================
//function : PerformLoops
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderArea::PerformLoops()
{
}
//=======================================================================
//function : PerformAreas
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderArea::PerformAreas()
{
}
//=======================================================================
//function : PerformInternalShapes
//purpose  : 
//=======================================================================
  void BOPAlgo_BuilderArea::PerformInternalShapes()
{
}
*/
