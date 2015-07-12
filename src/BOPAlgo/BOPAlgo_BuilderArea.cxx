// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_BuilderArea.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BuilderArea::BOPAlgo_BuilderArea()
:
  BOPAlgo_Algo(),
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
BOPAlgo_BuilderArea::BOPAlgo_BuilderArea
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Algo(theAllocator),
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
void BOPAlgo_BuilderArea::SetContext
  (const Handle(IntTools_Context)& theContext)
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
