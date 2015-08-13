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
#include <Message_ProgressIndicator.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_Algo::BRepAlgoAPI_Algo()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myErrorStatus(1),
  myWarningStatus(0),
  myRunParallel(Standard_False)
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_Algo::BRepAlgoAPI_Algo
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myErrorStatus(1),
  myWarningStatus(0),
  myRunParallel(Standard_False)
{}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BRepAlgoAPI_Algo::~BRepAlgoAPI_Algo()
{
}
//=======================================================================
//function : Allocator
//purpose  : 
//=======================================================================
const Handle(NCollection_BaseAllocator)& BRepAlgoAPI_Algo::Allocator()const
{
  return myAllocator;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
Standard_Integer BRepAlgoAPI_Algo::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
// function: WarningStatus
// purpose: 
//=======================================================================
Standard_Integer BRepAlgoAPI_Algo::WarningStatus()const
{
  return myWarningStatus;
}
//=======================================================================
//function : SetRunParallel
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Algo::SetRunParallel(const Standard_Boolean theFlag)
{
  myRunParallel=theFlag;
}
//=======================================================================
//function : RunParallel
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Algo::RunParallel()const
{
  return myRunParallel;
}
//=======================================================================
//function : SetProgressIndicator
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Algo::SetProgressIndicator
  (const Handle(Message_ProgressIndicator)& theObj)
{
  if (!theObj.IsNull()) {
    myProgressIndicator=theObj;
  }
}
//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Algo::UserBreak() const
{
  if (myProgressIndicator.IsNull()) {
    return;
  }
  if (myProgressIndicator->UserBreak()) {
    Standard_NotImplemented::Raise("");
  }
} 
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape& BRepAlgoAPI_Algo::Shape()
{
  return myShape;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Algo::Clear()
{
}
//  myErrorStatus
//
// 1 - object is just initialized
