// Created on: 2017-06-26
// Created by: Andrey Betenev
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

#ifndef _Message_Report_HeaderFile
#define _Message_Report_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_ListOfAlert.hxx>
#include <Standard_Mutex.hxx>

class Message_Messenger;

class Message_Report;
DEFINE_STANDARD_HANDLE(Message_Report, MMgt_TShared)

//! Container for alert messages, sorted according to their gravity.
//! 
//! For each gravity level, alerts are stored in simple list.
//! If alert being added can be merged with another alert of the same
//! type already in the list, it is merged and not added to the list.
//! 
//! This class is intended to be used as follows:
//!
//! - In the process of execution, algorithm fills report by alert objects
//!   using methods AddAlert()
//!
//! - The result can be queried for presence of particular alert using 
//!   methods HasAlert()
//!
//! - The reports produced by nested or sequentially executed algorithms
//!   can be collected in one using method Merge()
//!
//! - The report can be shown to the user either as plain text with method 
//!   Dump() or in more advanced way, by iterating over lists returned by GetAlerts()
//!
//! - Report can be cleared by methods Clear() (usually after reporting)

class Message_Report : public Standard_Transient
{
public:

  //! Empty constructor
  Standard_EXPORT Message_Report ();
  
  //! Add alert with specified gravity.
  //! This method is thread-safe, i.e. alerts can be added from parallel threads safely.
  Standard_EXPORT void AddAlert (Message_Gravity theGravity, const Handle(Message_Alert)& theAlert);

  //! Returns list of collected alerts with specified gravity
  Standard_EXPORT const Message_ListOfAlert& GetAlerts (Message_Gravity theGravity) const;

  //! Returns true if specific type of alert is recorded
  Standard_EXPORT Standard_Boolean HasAlert (const Handle(Standard_Type)& theType);

  //! Returns true if specific type of alert is recorded with specified gravity
  Standard_EXPORT Standard_Boolean HasAlert (const Handle(Standard_Type)& theType, Message_Gravity theGravity);

  //! Clears all collected alerts
  Standard_EXPORT void Clear ();

  //! Clears collected alerts with specified gravity
  Standard_EXPORT void Clear (Message_Gravity theGravity);

  //! Clears collected alerts with specified type
  Standard_EXPORT void Clear (const Handle(Standard_Type)& theType);

  //! Dumps all collected alerts to stream
  Standard_EXPORT void Dump (Standard_OStream& theOS);

  //! Dumps collected alerts with specified gravity to stream
  Standard_EXPORT void Dump (Standard_OStream& theOS, Message_Gravity theGravity);

  //! Sends all collected alerts to messenger
  Standard_EXPORT void SendMessages (const Handle(Message_Messenger)& theMessenger);

  //! Dumps collected alerts with specified gravity to messenger
  Standard_EXPORT void SendMessages (const Handle(Message_Messenger)& theMessenger, Message_Gravity theGravity);

  //! Merges data from theOther report into this
  Standard_EXPORT void Merge (const Handle(Message_Report)& theOther);

  //! Merges alerts with specified gravity from theOther report into this
  Standard_EXPORT void Merge (const Handle(Message_Report)& theOther, Message_Gravity theGravity);

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(Message_Report,Standard_Transient)

protected:
  Standard_Mutex myMutex;

  // store messages in a lists sorted by gravity;
  // here we rely on knowledge that Message_Fail is the last element of the enum
  Message_ListOfAlert myAlerts[Message_Fail + 1];
};

#endif // _Message_Report_HeaderFile
