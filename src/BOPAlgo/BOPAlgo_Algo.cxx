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


#include <BOPAlgo_Algo.hxx>
#include <Message_ProgressIndicator.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_ProgramError.hxx>

namespace
{
  Standard_Boolean myGlobalRunParallel = Standard_False;
}

//=======================================================================
// function: 
// purpose: 
//=======================================================================
void BOPAlgo_Algo::SetParallelMode(Standard_Boolean theNewMode)
{
  myGlobalRunParallel = theNewMode;
}

//=======================================================================
// function: 
// purpose: 
//=======================================================================
Standard_Boolean BOPAlgo_Algo::GetParallelMode()
{
  return myGlobalRunParallel;
}


//=======================================================================
// function: 
// purpose: 
//=======================================================================
BOPAlgo_Algo::BOPAlgo_Algo()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myErrorStatus(1),
  myWarningStatus(0),
  myRunParallel(myGlobalRunParallel)
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
BOPAlgo_Algo::BOPAlgo_Algo
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myErrorStatus(1),
  myWarningStatus(0),
  myRunParallel(myGlobalRunParallel)
{}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BOPAlgo_Algo::~BOPAlgo_Algo()
{
}
//=======================================================================
//function : Allocator
//purpose  : 
//=======================================================================
const Handle(NCollection_BaseAllocator)& BOPAlgo_Algo::Allocator()const
{
  return myAllocator;
}
//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_Algo::CheckData()
{
  myErrorStatus=0;
}
//=======================================================================
// function: CheckResult
// purpose: 
//=======================================================================
void BOPAlgo_Algo::CheckResult()
{
  myErrorStatus=0;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
Standard_Integer BOPAlgo_Algo::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
// function: WarningStatus
// purpose: 
//=======================================================================
Standard_Integer BOPAlgo_Algo::WarningStatus()const
{
  return myWarningStatus;
}
//=======================================================================
//function : SetRunParallel
//purpose  : 
//=======================================================================
void BOPAlgo_Algo::SetRunParallel(const Standard_Boolean theFlag)
{
  myRunParallel=theFlag;
}
//=======================================================================
//function : RunParallel
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_Algo::RunParallel()const
{
  return myRunParallel;
}
//=======================================================================
//function : SetProgressIndicator
//purpose  : 
//=======================================================================
void BOPAlgo_Algo::SetProgressIndicator
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
void BOPAlgo_Algo::UserBreak() const
{
  if (myProgressIndicator.IsNull()) {
    return;
  }
  if (myProgressIndicator->UserBreak()) {
    Standard_NotImplemented::Raise("");
  }
} 
//  myErrorStatus
//
// 1 - object is just initialized
