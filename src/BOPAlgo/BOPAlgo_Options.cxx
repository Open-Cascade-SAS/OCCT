// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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


#include <BOPAlgo_Options.hxx>
#include <Message_MsgFile.hxx>
#include <Message_ProgressIndicator.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_ProgramError.hxx>

namespace
{
  Standard_Boolean myGlobalRunParallel = Standard_False;

  // Initialize textual messages for errors and warnings defined in BOPAlgo
  #include "BOPAlgo_BOPAlgo_msg.pxx"
  bool BOPAlgo_InitMessages = false;
  void BOPAlgo_LoadMessages ()
  {
    if (BOPAlgo_InitMessages)
      return;
    BOPAlgo_InitMessages = true;

    if (! Message_MsgFile::HasMsg ("BOPAlgo_LOAD_CHECKER"))
    {
      Message_MsgFile::LoadFromString (BOPAlgo_BOPAlgo_msg);
    }
  }
}

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BOPAlgo_Options::BOPAlgo_Options()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myReport(new Message_Report),
  myRunParallel(myGlobalRunParallel),
  myFuzzyValue(Precision::Confusion()),
  myUseOBB(Standard_False)
{
  BOPAlgo_LoadMessages();
}

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BOPAlgo_Options::BOPAlgo_Options
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myReport(new Message_Report),
  myRunParallel(myGlobalRunParallel),
  myFuzzyValue(Precision::Confusion()),
  myUseOBB(Standard_False)
{
  BOPAlgo_LoadMessages();
}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BOPAlgo_Options::~BOPAlgo_Options()
{
}

//=======================================================================
//function : DumpErrors
//purpose  : 
//=======================================================================
void BOPAlgo_Options::DumpErrors(Standard_OStream& theOS) const
{
  myReport->Dump (theOS, Message_Fail);
}

//=======================================================================
//function : DumpWarnings
//purpose  : 
//=======================================================================
void BOPAlgo_Options::DumpWarnings(Standard_OStream& theOS) const
{
  myReport->Dump (theOS, Message_Warning);
}

//=======================================================================
// function: 
// purpose: 
//=======================================================================
void BOPAlgo_Options::SetParallelMode(Standard_Boolean theNewMode)
{
  myGlobalRunParallel = theNewMode;
}

//=======================================================================
// function: 
// purpose: 
//=======================================================================
Standard_Boolean BOPAlgo_Options::GetParallelMode()
{
  return myGlobalRunParallel;
}


//=======================================================================
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void BOPAlgo_Options::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = Max(theFuzz, Precision::Confusion());
}


//=======================================================================
//function : SetProgressIndicator
//purpose  : 
//=======================================================================
void BOPAlgo_Options::SetProgressIndicator
  (const Handle(Message_ProgressIndicator)& theObj)
{
  if (!theObj.IsNull()) {
    myProgressIndicator = theObj;
  }
}
//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================
void BOPAlgo_Options::UserBreak() const
{
  if (myProgressIndicator.IsNull()) {
    return;
  }
  if (myProgressIndicator->UserBreak()) {
    throw Standard_NotImplemented("BOPAlgo_Options::UserBreak(), method is not implemented");
  }
}
