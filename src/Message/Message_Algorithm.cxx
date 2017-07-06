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
#include <TColStd_HSequenceOfHExtendedString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Algorithm,Standard_Transient)

//=======================================================================
//function : Message_Algorithm
//purpose  :
//=======================================================================
Message_Algorithm::Message_Algorithm ()
{
  myMessenger = Message::DefaultMessenger();
}

//=======================================================================
//function : SetMessenger
//purpose  :
//=======================================================================

void Message_Algorithm::SetMessenger (const Handle(Message_Messenger)& theMsgr)
{
  if ( theMsgr.IsNull() )
    myMessenger = Message::DefaultMessenger();
  else
    myMessenger = theMsgr;
}

//=======================================================================
//function : SetStatus
//purpose  :
//=======================================================================

void Message_Algorithm::SetStatus(const Message_Status& theStat)
{
  myStatus.Set( theStat );
}

//=======================================================================
//function : SetStatus
//purpose  :
//=======================================================================

void Message_Algorithm::SetStatus (const Message_Status& theStat, 
				   const Standard_Integer theInt)
{
  // Set status flag
  SetStatus ( theStat );

  // Find index of bit corresponding to that flag
  Standard_Integer aFlagIndex = Message_ExecStatus::StatusIndex(theStat);
  if ( !aFlagIndex ) return;

  // Create map of integer parameters for a given flag, if not yet done
  if ( myReportIntegers.IsNull() )
    myReportIntegers = new TColStd_HArray1OfTransient (Message_ExecStatus::FirstStatus, 
                                                       Message_ExecStatus::LastStatus);
  Handle(Standard_Transient)& aData = 
    myReportIntegers->ChangeValue(aFlagIndex);
  if ( aData.IsNull() )
    aData = new TColStd_HPackedMapOfInteger;

  // add integer parameter for the status
  Handle(TColStd_HPackedMapOfInteger)::DownCast(aData)->ChangeMap().Add(theInt);
}

//=======================================================================
//function : SetStatus
//purpose  :
//=======================================================================

void Message_Algorithm::SetStatus (const Message_Status& theStat, 
				   const Handle(TCollection_HExtendedString) &theStr,
				   const Standard_Boolean noRepetitions)
{
  // Set status flag
  SetStatus ( theStat );
  if ( theStr.IsNull() )
    return;

  // Find index of bit corresponding to that flag
  Standard_Integer aFlagIndex = Message_ExecStatus::StatusIndex(theStat);
  if ( !aFlagIndex ) return;
    
  // Create sequence of string parameters for a given flag, if not yet done
  if ( myReportStrings.IsNull() )
    myReportStrings = new TColStd_HArray1OfTransient (Message_ExecStatus::FirstStatus,
						      Message_ExecStatus::LastStatus);
  Handle(Standard_Transient)& aData = 
    myReportStrings->ChangeValue(aFlagIndex);
  if ( aData.IsNull() )
    aData = new TColStd_HSequenceOfHExtendedString;

  // Add string parameter
  Handle(TColStd_HSequenceOfHExtendedString) aReportSeq = 
    Handle(TColStd_HSequenceOfHExtendedString)::DownCast(aData);
  if ( aReportSeq.IsNull() )
    return;
  if ( noRepetitions )
  {
    // if the provided string has been already registered, just do nothing
    for ( Standard_Integer i=1; i <= aReportSeq->Length(); i++ )
      if ( aReportSeq->Value(i)->String().IsEqual( theStr->String() ) ) 
	return;
  }

  aReportSeq->Append ( theStr );
}

//=======================================================================
//function : SetStatus
//purpose  :
//=======================================================================

void Message_Algorithm::SetStatus (const Message_Status& theStat,
                                   const Message_Msg&    theMsg)
{
  // Set status flag
  SetStatus (theStat);

  // Find index of bit corresponding to that flag
  Standard_Integer aFlagIndex = Message_ExecStatus::StatusIndex (theStat);
  if (aFlagIndex == 0)
  {
    return;
  }

  // Create sequence of messages for a given flag, if not yet done
  if (myReportMessages.IsNull())
  {
    myReportMessages = new Message_ArrayOfMsg (Message_ExecStatus::FirstStatus, Message_ExecStatus::LastStatus);
  }

  myReportMessages->ChangeValue (aFlagIndex) = new Message_Msg (theMsg);
}

//=======================================================================
//function : ClearStatus
//purpose  :
//=======================================================================

void Message_Algorithm::ClearStatus()
{
  myStatus.Clear(); 
  myReportIntegers.Nullify();
  myReportStrings.Nullify();
  myReportMessages.Nullify();
}

//=======================================================================
//function : SendStatusMessages
//purpose  :
//=======================================================================

void Message_Algorithm::SendStatusMessages (const Message_ExecStatus& theStatus,
                                            const Message_Gravity     theTraceLevel,
                                            const Standard_Integer    theMaxCount) const
{
  Handle(Message_Messenger) aMsgr = GetMessenger();
  if (aMsgr.IsNull())
  {
    return;
  }

  // Iterate on all set flags in the specified range
  for ( Standard_Integer i  = Message_ExecStatus::FirstStatus; 
                         i <= Message_ExecStatus::LastStatus; i++ )
  {
    Message_Status stat = Message_ExecStatus::StatusByIndex( i );
    if (!theStatus.IsSet (stat) || !myStatus.IsSet (stat))
    {
      continue;
    }

    NCollection_Handle<Message_Msg> aMsgCustom;
    if (! myReportMessages.IsNull())
      aMsgCustom = myReportMessages->Value (i);
    if (!aMsgCustom.IsNull())
    {
      // print custom message
      aMsgr->Send (*aMsgCustom, theTraceLevel);
      continue;
    }

    // construct message suffix
    TCollection_AsciiString aSuffix;
    switch( Message_ExecStatus::TypeOfStatus( stat ) )
    {
    case Message_DONE:  aSuffix.AssignCat( ".Done" ); break;
    case Message_WARN:  aSuffix.AssignCat( ".Warn" ); break;
    case Message_ALARM: aSuffix.AssignCat( ".Alarm"); break;
    case Message_FAIL:  aSuffix.AssignCat( ".Fail" ); break;
    default:            continue;  
    }
    aSuffix.AssignCat( Message_ExecStatus::LocalStatusIndex( stat ) );

    // find message, prefixed by class type name, iterating by base classes if necessary
    TCollection_AsciiString aMsgName;
    for (Handle(Standard_Type) aType = DynamicType(); ! aType.IsNull(); aType = aType->Parent())
    {
      aMsgName = aType->Name();
      aMsgName += aSuffix;
      if (Message_MsgFile::HasMsg(aMsgName))
        break;
    }

    // create a message
    Message_Msg aMsg ( aMsgName );

    // if additional parameters are defined for a given status flag,
    // try to feed them into the message
    if (!myReportIntegers.IsNull())
    {
      Handle(TColStd_HPackedMapOfInteger) aMapErrors =
        Handle(TColStd_HPackedMapOfInteger)::DownCast(myReportIntegers->Value(i));
      if (!aMapErrors.IsNull())
      {
        aMsg << PrepareReport (aMapErrors, theMaxCount);
      }
    }
    if (!myReportStrings.IsNull()
     && !myReportStrings->Value (i).IsNull())
    {
      Handle(TColStd_HSequenceOfHExtendedString) aReportSeq =
        Handle(TColStd_HSequenceOfHExtendedString)::DownCast (myReportStrings->Value(i));
      if (!aReportSeq.IsNull())
      {
        aMsg << PrepareReport (aReportSeq->Sequence(), theMaxCount);
      }
    }

    // output the message
    aMsgr->Send(aMsg, theTraceLevel);
  }
}

//=======================================================================
//function : SendMessages
//purpose  : 
//=======================================================================

void Message_Algorithm::SendMessages (const Message_Gravity theTraceLevel,
				      const Standard_Integer theMaxCount) const
{
  Message_ExecStatus aStat;
  aStat.SetAllWarn();
  aStat.SetAllAlarm();
  aStat.SetAllFail();
  SendStatusMessages( aStat, theTraceLevel, theMaxCount );
}

//=======================================================================
//function : AddStatus
//purpose  : 
//=======================================================================

void Message_Algorithm::AddStatus
      (const Handle(Message_Algorithm)& theOtherAlgo)
{
  AddStatus( theOtherAlgo->GetStatus(), theOtherAlgo );
}

//=======================================================================
//function : AddStatus
//purpose  : 
//=======================================================================

void Message_Algorithm::AddStatus
      (const Message_ExecStatus& theAllowedStatus,
       const Handle(Message_Algorithm)& theOtherAlgo)
{
  // Iterate on all set flags in the specified range
  const Message_ExecStatus& aStatusOfAlgo = theOtherAlgo->GetStatus();
  for ( Standard_Integer i  = Message_ExecStatus::FirstStatus; 
                         i <= Message_ExecStatus::LastStatus; i++ )
  {
    Message_Status stat = Message_ExecStatus::StatusByIndex( i );
    if ( ! theAllowedStatus.IsSet( stat ) || ! aStatusOfAlgo.IsSet( stat ) )
      continue;

    SetStatus ( stat );

    // if additional parameters are defined for a given status flag,
    // move them to <this> algorithm
    // a) numbers
    Handle(TColStd_HPackedMapOfInteger) aNumsOther = 
      theOtherAlgo->GetMessageNumbers (stat); 
    if ( ! aNumsOther.IsNull() ) 
    {
      // Create sequence of integer parameters for a given flag, if not yet done
      if ( myReportIntegers.IsNull() )
	myReportIntegers =
	  new TColStd_HArray1OfTransient(Message_ExecStatus::FirstStatus, 
                                         Message_ExecStatus::LastStatus);
      Handle(Standard_Transient)& aData = 
	myReportIntegers->ChangeValue(i);
      if ( aData.IsNull() )
	aData = new TColStd_HPackedMapOfInteger;

      // add integer parameter for the status
      Handle(TColStd_HPackedMapOfInteger)::DownCast(aData)
	->ChangeMap().Unite(aNumsOther->Map());
    }
    // b) strings
    Handle(TColStd_HSequenceOfHExtendedString) aStrsOther = 
      theOtherAlgo->GetMessageStrings (stat); 
    if ( ! aStrsOther.IsNull() ) 
    {
      for (Standard_Integer n=1; n < aStrsOther->Length(); n++ )
	SetStatus (stat, aStrsOther->Value(n));
    }
  }
}

//=======================================================================
//function : GetMessageNumbers
//purpose  : 
//=======================================================================

Handle(TColStd_HPackedMapOfInteger) Message_Algorithm::GetMessageNumbers 
       (const Message_Status& theStatus) const
{
  if ( myReportIntegers.IsNull() )
    return 0;

  // Find index of bit corresponding to that flag
  Standard_Integer aFlagIndex = Message_ExecStatus::StatusIndex(theStatus);
  if ( ! aFlagIndex ) return 0;
    
  return Handle(TColStd_HPackedMapOfInteger)::DownCast(myReportIntegers->Value(aFlagIndex));
}

//=======================================================================
//function : GetMessageStrings
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHExtendedString) Message_Algorithm::GetMessageStrings
       (const Message_Status& theStatus) const
{
  if ( myReportStrings.IsNull() )
    return 0;

  // Find index of bit corresponding to that flag
  Standard_Integer aFlagIndex = Message_ExecStatus::StatusIndex(theStatus);
  if ( ! aFlagIndex ) return 0;
    
  return Handle(TColStd_HSequenceOfHExtendedString)::DownCast(myReportStrings->Value(aFlagIndex));
}

//=======================================================================
//function : PrepareReport
//purpose  : static method
//=======================================================================

TCollection_ExtendedString Message_Algorithm::PrepareReport 
  (const Handle(TColStd_HPackedMapOfInteger)& theMapError,
   const Standard_Integer theMaxCount)
{
  TCollection_ExtendedString aNewReport;
  TColStd_MapIteratorOfPackedMapOfInteger anIt(theMapError->Map());
  Standard_Integer nb = 1;
  for (; anIt.More() && nb <= theMaxCount; anIt.Next(), nb++ )
  {
    if ( nb > 1 ) 
      aNewReport += " ";
    aNewReport += anIt.Key();
  }
 
  if ( anIt.More() )
  {
    aNewReport += " ... (total ";
    aNewReport += theMapError->Map().Extent();
    aNewReport += ")";
  }
  return aNewReport;
}

//=======================================================================
//function : PrepareReport
//purpose  : static method
//=======================================================================

TCollection_ExtendedString Message_Algorithm::PrepareReport 
  (const TColStd_SequenceOfHExtendedString& theReportSeq,
   const Standard_Integer theMaxCount)
{
  TCollection_ExtendedString aNewReport;
  Standard_Integer nb = 1;
  for ( ; nb <= theReportSeq.Length() && nb <= theMaxCount; nb++)
  {
    aNewReport += (Standard_CString)( nb > 1 ? ", \'" : "\'" );
    aNewReport += theReportSeq.Value(nb)->String();
    aNewReport += "\'";
  }

  if (theReportSeq.Length() > theMaxCount )
  {
    aNewReport += " ... (total ";
    aNewReport += theReportSeq.Length();
    aNewReport += ") ";
  }
  return aNewReport;
}
