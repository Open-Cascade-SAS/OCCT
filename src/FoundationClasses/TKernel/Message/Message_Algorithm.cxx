// Created on: 2003-03-04
// Created by: Pavel TELKOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// The original implementation copyright (c) RINA S.p.A.

#include <Message.hxx>
#include <Message_Algorithm.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Message_MsgFile.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <NCollection_PackedMapAlgo.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Algorithm, Standard_Transient)

//=================================================================================================

Message_Algorithm::Message_Algorithm()
{
  myMessenger = Message::DefaultMessenger();
}

//=================================================================================================

void Message_Algorithm::SetMessenger(const occ::handle<Message_Messenger>& theMsgr)
{
  if (theMsgr.IsNull())
    myMessenger = Message::DefaultMessenger();
  else
    myMessenger = theMsgr;
}

//=================================================================================================

void Message_Algorithm::SetStatus(const Message_Status& theStat)
{
  myStatus.Set(theStat);
}

//=================================================================================================

void Message_Algorithm::SetStatus(const Message_Status& theStat, const int theInt)
{
  // Set status flag
  SetStatus(theStat);

  // Find index of bit corresponding to that flag
  int aFlagIndex = Message_ExecStatus::StatusIndex(theStat);
  if (!aFlagIndex)
    return;

  // Create map of integer parameters for a given flag, if not yet done
  if (myReportIntegers.IsNull())
    myReportIntegers =
      new NCollection_HArray1<occ::handle<Standard_Transient>>(Message_ExecStatus::FirstStatus,
                                                               Message_ExecStatus::LastStatus);
  occ::handle<Standard_Transient>& aData = myReportIntegers->ChangeValue(aFlagIndex);
  if (aData.IsNull())
    aData = new TColStd_HPackedMapOfInteger;

  // add integer parameter for the status
  occ::down_cast<TColStd_HPackedMapOfInteger>(aData)->ChangeMap().Add(theInt);
}

//=================================================================================================

void Message_Algorithm::SetStatus(const Message_Status&                           theStat,
                                  const occ::handle<TCollection_HExtendedString>& theStr,
                                  const bool                                      noRepetitions)
{
  // Set status flag
  SetStatus(theStat);
  if (theStr.IsNull())
    return;

  // Find index of bit corresponding to that flag
  int aFlagIndex = Message_ExecStatus::StatusIndex(theStat);
  if (!aFlagIndex)
    return;

  // Create sequence of string parameters for a given flag, if not yet done
  if (myReportStrings.IsNull())
    myReportStrings =
      new NCollection_HArray1<occ::handle<Standard_Transient>>(Message_ExecStatus::FirstStatus,
                                                               Message_ExecStatus::LastStatus);
  occ::handle<Standard_Transient>& aData = myReportStrings->ChangeValue(aFlagIndex);
  if (aData.IsNull())
    aData = new NCollection_HSequence<occ::handle<TCollection_HExtendedString>>;

  // Add string parameter
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>> aReportSeq =
    occ::down_cast<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>>(aData);
  if (aReportSeq.IsNull())
    return;
  if (noRepetitions)
  {
    // if the provided string has been already registered, just do nothing
    for (int i = 1; i <= aReportSeq->Length(); i++)
      if (aReportSeq->Value(i)->String().IsEqual(theStr->String()))
        return;
  }

  aReportSeq->Append(theStr);
}

//=================================================================================================

void Message_Algorithm::SetStatus(const Message_Status& theStat, const Message_Msg& theMsg)
{
  // Set status flag
  SetStatus(theStat);

  // Find index of bit corresponding to that flag
  int aFlagIndex = Message_ExecStatus::StatusIndex(theStat);
  if (aFlagIndex == 0)
  {
    return;
  }

  // Create sequence of messages for a given flag, if not yet done
  if (myReportMessages.IsNull())
  {
    myReportMessages =
      new NCollection_Array1<NCollection_Handle<Message_Msg>>(Message_ExecStatus::FirstStatus,
                                                              Message_ExecStatus::LastStatus);
  }

  myReportMessages->ChangeValue(aFlagIndex) = new Message_Msg(theMsg);
}

//=================================================================================================

void Message_Algorithm::ClearStatus()
{
  myStatus.Clear();
  myReportIntegers.Nullify();
  myReportStrings.Nullify();
  myReportMessages.Nullify();
}

//=================================================================================================

void Message_Algorithm::SendStatusMessages(const Message_ExecStatus& theStatus,
                                           const Message_Gravity     theTraceLevel,
                                           const int                 theMaxCount) const
{
  occ::handle<Message_Messenger> aMsgr = GetMessenger();
  if (aMsgr.IsNull())
  {
    return;
  }

  // Iterate on all set flags in the specified range
  for (int i = Message_ExecStatus::FirstStatus; i <= Message_ExecStatus::LastStatus; i++)
  {
    Message_Status stat = Message_ExecStatus::StatusByIndex(i);
    if (!theStatus.IsSet(stat) || !myStatus.IsSet(stat))
    {
      continue;
    }

    NCollection_Handle<Message_Msg> aMsgCustom;
    if (!myReportMessages.IsNull())
      aMsgCustom = myReportMessages->Value(i);
    if (!aMsgCustom.IsNull())
    {
      // print custom message
      aMsgr->Send(*aMsgCustom, theTraceLevel);
      continue;
    }

    // construct message suffix
    TCollection_AsciiString aSuffix;
    switch (Message_ExecStatus::TypeOfStatus(stat))
    {
      case Message_DONE:
        aSuffix.AssignCat(".Done");
        break;
      case Message_WARN:
        aSuffix.AssignCat(".Warn");
        break;
      case Message_ALARM:
        aSuffix.AssignCat(".Alarm");
        break;
      case Message_FAIL:
        aSuffix.AssignCat(".Fail");
        break;
      default:
        continue;
    }
    aSuffix.AssignCat(Message_ExecStatus::LocalStatusIndex(stat));

    // find message, prefixed by class type name, iterating by base classes if necessary
    TCollection_AsciiString aMsgName;
    for (occ::handle<Standard_Type> aType = DynamicType(); !aType.IsNull(); aType = aType->Parent())
    {
      aMsgName = aType->Name();
      aMsgName += aSuffix;
      if (Message_MsgFile::HasMsg(aMsgName))
        break;
    }

    // create a message
    Message_Msg aMsg(aMsgName);

    // if additional parameters are defined for a given status flag,
    // try to feed them into the message
    if (!myReportIntegers.IsNull())
    {
      occ::handle<TColStd_HPackedMapOfInteger> aMapErrors =
        occ::down_cast<TColStd_HPackedMapOfInteger>(myReportIntegers->Value(i));
      if (!aMapErrors.IsNull())
      {
        aMsg << PrepareReport(aMapErrors, theMaxCount);
      }
    }
    if (!myReportStrings.IsNull() && !myReportStrings->Value(i).IsNull())
    {
      occ::handle<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>> aReportSeq =
        occ::down_cast<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>>(
          myReportStrings->Value(i));
      if (!aReportSeq.IsNull())
      {
        aMsg << PrepareReport(aReportSeq->Sequence(), theMaxCount);
      }
    }

    // output the message
    aMsgr->Send(aMsg, theTraceLevel);
  }
}

//=================================================================================================

void Message_Algorithm::SendMessages(const Message_Gravity theTraceLevel,
                                     const int             theMaxCount) const
{
  Message_ExecStatus aStat;
  aStat.SetAllWarn();
  aStat.SetAllAlarm();
  aStat.SetAllFail();
  SendStatusMessages(aStat, theTraceLevel, theMaxCount);
}

//=================================================================================================

void Message_Algorithm::AddStatus(const occ::handle<Message_Algorithm>& theOtherAlgo)
{
  AddStatus(theOtherAlgo->GetStatus(), theOtherAlgo);
}

//=================================================================================================

void Message_Algorithm::AddStatus(const Message_ExecStatus&             theAllowedStatus,
                                  const occ::handle<Message_Algorithm>& theOtherAlgo)
{
  // Iterate on all set flags in the specified range
  const Message_ExecStatus& aStatusOfAlgo = theOtherAlgo->GetStatus();
  for (int i = Message_ExecStatus::FirstStatus; i <= Message_ExecStatus::LastStatus; i++)
  {
    Message_Status stat = Message_ExecStatus::StatusByIndex(i);
    if (!theAllowedStatus.IsSet(stat) || !aStatusOfAlgo.IsSet(stat))
      continue;

    SetStatus(stat);

    // if additional parameters are defined for a given status flag,
    // move them to <this> algorithm
    // a) numbers
    occ::handle<TColStd_HPackedMapOfInteger> aNumsOther = theOtherAlgo->GetMessageNumbers(stat);
    if (!aNumsOther.IsNull())
    {
      // Create sequence of integer parameters for a given flag, if not yet done
      if (myReportIntegers.IsNull())
        myReportIntegers =
          new NCollection_HArray1<occ::handle<Standard_Transient>>(Message_ExecStatus::FirstStatus,
                                                                   Message_ExecStatus::LastStatus);
      occ::handle<Standard_Transient>& aData = myReportIntegers->ChangeValue(i);
      if (aData.IsNull())
        aData = new TColStd_HPackedMapOfInteger;

      // add integer parameter for the status
      NCollection_PackedMapAlgo::Unite(occ::down_cast<TColStd_HPackedMapOfInteger>(aData)->ChangeMap(),
                                       aNumsOther->Map());
    }
    // b) strings
    occ::handle<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>> aStrsOther =
      theOtherAlgo->GetMessageStrings(stat);
    if (!aStrsOther.IsNull())
    {
      for (int n = 1; n < aStrsOther->Length(); n++)
        SetStatus(stat, aStrsOther->Value(n));
    }
  }
}

//=================================================================================================

occ::handle<TColStd_HPackedMapOfInteger> Message_Algorithm::GetMessageNumbers(
  const Message_Status& theStatus) const
{
  if (myReportIntegers.IsNull())
    return nullptr;

  // Find index of bit corresponding to that flag
  int aFlagIndex = Message_ExecStatus::StatusIndex(theStatus);
  if (!aFlagIndex)
    return nullptr;

  return occ::down_cast<TColStd_HPackedMapOfInteger>(myReportIntegers->Value(aFlagIndex));
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>> Message_Algorithm::
  GetMessageStrings(const Message_Status& theStatus) const
{
  if (myReportStrings.IsNull())
    return nullptr;

  // Find index of bit corresponding to that flag
  int aFlagIndex = Message_ExecStatus::StatusIndex(theStatus);
  if (!aFlagIndex)
    return nullptr;

  return occ::down_cast<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>>(
    myReportStrings->Value(aFlagIndex));
}

//=================================================================================================

TCollection_ExtendedString Message_Algorithm::PrepareReport(
  const occ::handle<TColStd_HPackedMapOfInteger>& theMapError,
  const int                                       theMaxCount)
{
  TCollection_ExtendedString              aNewReport;
  TColStd_PackedMapOfInteger::Iterator anIt(theMapError->Map());
  int                                     nb = 1;
  for (; anIt.More() && nb <= theMaxCount; anIt.Next(), nb++)
  {
    if (nb > 1)
      aNewReport += " ";
    aNewReport += anIt.Key();
  }

  if (anIt.More())
  {
    aNewReport += " ... (total ";
    aNewReport += theMapError->Map().Extent();
    aNewReport += ")";
  }
  return aNewReport;
}

//=================================================================================================

TCollection_ExtendedString Message_Algorithm::PrepareReport(
  const NCollection_Sequence<occ::handle<TCollection_HExtendedString>>& theReportSeq,
  const int                                                             theMaxCount)
{
  TCollection_ExtendedString aNewReport;
  int                        nb = 1;
  for (; nb <= theReportSeq.Length() && nb <= theMaxCount; nb++)
  {
    aNewReport += (const char*)(nb > 1 ? ", \'" : "\'");
    aNewReport += theReportSeq.Value(nb)->String();
    aNewReport += "\'";
  }

  if (theReportSeq.Length() > theMaxCount)
  {
    aNewReport += " ... (total ";
    aNewReport += theReportSeq.Length();
    aNewReport += ") ";
  }
  return aNewReport;
}
