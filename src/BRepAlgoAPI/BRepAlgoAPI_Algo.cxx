// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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


#include <BRepAlgoAPI_Algo.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_Algo::BRepAlgoAPI_Algo()
:
  BOPAlgo_Options(NCollection_BaseAllocator::CommonBaseAllocator())
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_Algo::BRepAlgoAPI_Algo
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Options(theAllocator)
{}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BRepAlgoAPI_Algo::~BRepAlgoAPI_Algo()
{
}
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape& BRepAlgoAPI_Algo::Shape()
{
  return myShape;
}
