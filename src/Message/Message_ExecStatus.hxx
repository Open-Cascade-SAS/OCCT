// Created on: 2003-03-04
// Created by: Pavel TELKOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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

// The original implementation copyright (c) RINA S.p.A.

#ifndef Message_ExecStatus_HeaderFile
#define Message_ExecStatus_HeaderFile

#include <Message_StatusType.hxx>
#include <Message_Status.hxx>

/**
 * Tiny class for extended handling of error / execution
 * status of algorithm in universal way.
 *
 * It is in fact a set of integers represented as a collection of bit flags
 * for each of four types of status; each status flag has its own symbolic 
 * name and can be set/tested individually.
 *
 * The flags are grouped in semantic groups: 
 * - No flags means nothing done
 * - Done flags correspond to some operation succesffuly completed
 * - Warning flags correspond to warning messages on some 
 *   potentially wrong situation, not harming algorithm execution
 * - Alarm flags correspond to more severe warnings about incorrect
 *   user data, while not breaking algorithm execution
 * - Fail flags correspond to cases when algorithm failed to complete
 */

class Standard_EXPORT Message_ExecStatus 
{

 private:

  //! Mask to separate bits indicating status type and index within the type  
  enum StatusMask {
    MType         = 0x0000ff00,
    MIndex        = 0x000000ff
  };
  static inline int getBitFlag (int status)
  {
    return 0x1 << (status & MIndex);
  }
  
 public:
  //!@name Creation and simple operations with statuses
  //!@{

  //! Create empty execution status
  Message_ExecStatus () 
    : myDone( Message_None),  myWarn( Message_None),
      myAlarm( Message_None), myFail( Message_None)
  {}

  //! Initialise the execution status
  Message_ExecStatus ( Message_Status status )
    : myDone( Message_None),  myWarn( Message_None),
      myAlarm( Message_None), myFail( Message_None)
  {
    Set( status );
  }

  //! Sets a status flag
  void Set (Message_Status status)
  { 
    switch( status & MType )
    {
    case Message_DONE: myDone  |= (getBitFlag( status )); break;
    case Message_WARN: myWarn  |= (getBitFlag( status )); break;
    case Message_ALARM:myAlarm |= (getBitFlag( status )); break;
    case Message_FAIL: myFail  |= (getBitFlag( status )); break;
    default: break;
    }
  }

  //! Check status for being set
  Standard_Boolean IsSet (Message_Status status) const
  { 
    switch( status & MType )
    {
    case Message_DONE: return ( myDone  & getBitFlag( status ) ? Standard_True : Standard_False );
    case Message_WARN: return ( myWarn  & getBitFlag( status ) ? Standard_True : Standard_False );
    case Message_ALARM:return ( myAlarm & getBitFlag( status ) ? Standard_True : Standard_False );
    case Message_FAIL: return ( myFail  & getBitFlag( status ) ? Standard_True : Standard_False );
    default:           return Standard_False;
    }
  }
  
  //! Clear one status
  void Clear (Message_Status status)
  {
    switch( status & MType )
    {
    case Message_DONE: myDone  &= ~(getBitFlag( status )); return;
    case Message_WARN: myWarn  &= ~(getBitFlag( status )); return;
    case Message_ALARM:myAlarm &= ~(getBitFlag( status )); return;
    case Message_FAIL: myFail  &= ~(getBitFlag( status )); return;
    default: return;
    }
  }
  
  //!@}

  //!@name Advanced: Group operations (useful for analysis)
  //!@{

  //! Check if at least one status of each type is set
  Standard_Boolean IsDone  () const { return myDone  != Message_None;  }
  Standard_Boolean IsFail  () const { return myFail  != Message_None;  }
  Standard_Boolean IsWarn  () const { return myWarn  != Message_None;  }
  Standard_Boolean IsAlarm () const { return myAlarm != Message_None;  }
  
  //! Set all statuses of each type
  void SetAllDone   () { myDone  = ~0; };
  void SetAllWarn   () { myWarn  = ~0; };
  void SetAllAlarm  () { myAlarm = ~0; };
  void SetAllFail   () { myFail  = ~0; };

  //! Clear all statuses of each type 
  void ClearAllDone () { myDone  = Message_None; };
  void ClearAllWarn () { myWarn  = Message_None; };
  void ClearAllAlarm() { myAlarm = Message_None; };
  void ClearAllFail () { myFail  = Message_None; };

  //! Clear all statuses
  void Clear ()
  { 
    myDone = myWarn = myAlarm = myFail = Message_None;
  }
  
  //! Add statuses to me from theOther execution status
  void Add ( const Message_ExecStatus& theOther )
  {
    myDone  |= theOther.myDone;
    myWarn  |= theOther.myWarn;
    myAlarm |= theOther.myAlarm;
    myFail  |= theOther.myFail;
  }
  const Message_ExecStatus& operator |= ( const Message_ExecStatus& theOther )
  { Add ( theOther ); return *this; }

  //! Leave only the statuses common with theOther
  void And ( const Message_ExecStatus& theOther )
  {
    myDone  &= theOther.myDone;
    myWarn  &= theOther.myWarn;
    myAlarm &= theOther.myAlarm;
    myFail  &= theOther.myFail;
  }
  const Message_ExecStatus& operator &= ( const Message_ExecStatus& theOther )
  { And ( theOther ); return *this; }

  //@}

 public:

  //!@name Advanced: Iteration and analysis of status flags
  //!@{
  
  //! Definitions of range of available statuses
  enum StatusRange
  {
    FirstStatus     = 1,
    StatusesPerType = 32,
    NbStatuses      = 128,
    LastStatus      = 129
  };

  //! Returns index of status in whole range [FirstStatus, LastStatus]
  static Standard_Integer StatusIndex( Message_Status status )
  {
    switch( status & MType )
    {
    case Message_DONE:  return 0 * StatusesPerType + LocalStatusIndex(status);
    case Message_WARN:  return 1 * StatusesPerType + LocalStatusIndex(status);
    case Message_ALARM: return 2 * StatusesPerType + LocalStatusIndex(status);
    case Message_FAIL:  return 3 * StatusesPerType + LocalStatusIndex(status);
    default: return 0;
    }
  }

  //! Returns index of status inside type of status (Done or Warn or, etc) 
  //! in range [1, StatusesPerType]
  static Standard_Integer LocalStatusIndex( Message_Status status )
  {
    return (status & MIndex) + 1;
  }

  //! Returns status type (DONE, WARN, ALARM, or FAIL) 
  static Message_StatusType TypeOfStatus( Message_Status status )
  {
    return (Message_StatusType)(status & MType);
  }

  //! Returns status with index theIndex in whole range [FirstStatus, LastStatus]
  static Message_Status StatusByIndex( const Standard_Integer theIndex )
  {
    Standard_Integer indx = theIndex - 1;
    if ( indx < 32 )
      return (Message_Status)(Message_DONE  + indx);
    else if ( indx < 64 )
      return (Message_Status)(Message_WARN  + ( indx - 32 ));
    else if ( indx < 96 )
      return (Message_Status)(Message_ALARM + ( indx - 64 ));
    else if ( indx < 128 )
      return (Message_Status)(Message_FAIL  + ( indx - 96 ));
    return Message_None;
  }

  //!@}

 private:
  // ---------- PRIVATE FIELDS ----------
  Standard_Integer myDone;
  Standard_Integer myWarn;
  Standard_Integer myAlarm;
  Standard_Integer myFail;
};

#endif
