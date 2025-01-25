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
#include <Message_ProgressScope.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TCollection_AsciiString.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <BOPAlgo_Alerts.hxx>

namespace
{
Standard_Boolean myGlobalRunParallel = Standard_False;

// Initialize textual messages for errors and warnings defined in BOPAlgo
#include "BOPAlgo_BOPAlgo_msg.pxx"
bool BOPAlgo_InitMessages = false;

void BOPAlgo_LoadMessages()
{
  if (BOPAlgo_InitMessages)
    return;
  BOPAlgo_InitMessages = true;

  if (!Message_MsgFile::HasMsg("BOPAlgo_LOAD_CHECKER"))
  {
    Message_MsgFile::LoadFromString(BOPAlgo_BOPAlgo_msg);
  }
}
} // namespace

//=================================================================================================

BOPAlgo_Options::BOPAlgo_Options()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myReport(new Message_Report),
      myRunParallel(myGlobalRunParallel),
      myFuzzyValue(Precision::Confusion()),
      myUseOBB(Standard_False)
{
  BOPAlgo_LoadMessages();
}

//=================================================================================================

BOPAlgo_Options::BOPAlgo_Options(const Handle(NCollection_BaseAllocator)& theAllocator)
    : myAllocator(theAllocator),
      myReport(new Message_Report),
      myRunParallel(myGlobalRunParallel),
      myFuzzyValue(Precision::Confusion()),
      myUseOBB(Standard_False)
{
  BOPAlgo_LoadMessages();
}

//=================================================================================================

BOPAlgo_Options::~BOPAlgo_Options() {}

//=================================================================================================

void BOPAlgo_Options::DumpErrors(Standard_OStream& theOS) const
{
  myReport->Dump(theOS, Message_Fail);
}

//=================================================================================================

void BOPAlgo_Options::DumpWarnings(Standard_OStream& theOS) const
{
  myReport->Dump(theOS, Message_Warning);
}

//=================================================================================================

void BOPAlgo_Options::SetParallelMode(Standard_Boolean theNewMode)
{
  myGlobalRunParallel = theNewMode;
}

//=================================================================================================

Standard_Boolean BOPAlgo_Options::GetParallelMode()
{
  return myGlobalRunParallel;
}

//=================================================================================================

void BOPAlgo_Options::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = Max(theFuzz, Precision::Confusion());
}

Standard_Boolean BOPAlgo_Options::UserBreak(const Message_ProgressScope& thePS)
{
  if (thePS.UserBreak())
  {
    AddError(new BOPAlgo_AlertUserBreak);
    return Standard_True;
  }
  return Standard_False;
}
